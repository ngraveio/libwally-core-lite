#include "internal.h"
#include "script_int.h" /* uint32_to_le_bytes / uint32_from_le_bytes (alignment-safe) */

#include <include/wally_transaction.h>
#include <include/wally_psbt.h>
#include <include/wally_map.h>
#include <include/wally_zcash.h>

#include <stdbool.h>
#include <string.h>

/* ZEC V4 (Sapling) constants */
static const uint8_t  ZEC_V4_HEADER[] = {0x04, 0x00, 0x00, 0x80}; /* 0x80000004 LE */
#define ZEC_V4_HEADER_LEN (sizeof(ZEC_V4_HEADER))

/* Trailer = nExpiryHeight(4) + valueBalanceSapling(8) + 3 shielded-count varints (all 0). */
#define ZEC_V4_TRAILER_ZEROS (sizeof(uint64_t) + 3) /* valueBalanceSapling + 3 zero-varints */

/* PSBT proprietary key carrying CONSENSUS_BRANCH_ID (BitGo convention). */
static const uint8_t ZEC_PROP_KEY_BRANCH_ID[] = {0xFC, 0x05, 0x42, 0x49, 0x54, 0x47, 0x4F, 0x00};
#define ZEC_PROP_KEY_BRANCH_ID_LEN (sizeof(ZEC_PROP_KEY_BRANCH_ID))

size_t zec_v4_extra_length(void)
{
    /* nVersionGroupId + nExpiryHeight + valueBalanceSapling + 3 zero-varints */
    return sizeof(uint32_t) + sizeof(uint32_t) + ZEC_V4_TRAILER_ZEROS;
}

unsigned char *zec_v4_write_header(unsigned char *p, const struct wally_tx *tx)
{
    /* Written immediately after the 4-byte version. */
    p += uint32_to_le_bytes(tx->version_group_id, p);
    return p;
}

unsigned char *zec_v4_write_trailer(unsigned char *p, const struct wally_tx *tx)
{
    /* Written immediately after the 4-byte locktime. */
    p += uint32_to_le_bytes(tx->expiry_height, p);
    memset(p, 0, ZEC_V4_TRAILER_ZEROS); /* valueBalanceSapling(0) + 3 shielded counts(0) */
    p += ZEC_V4_TRAILER_ZEROS;
    return p;
}

bool zec_v4_detect_header(const unsigned char *bytes, size_t len)
{
    return len >= ZEC_V4_HEADER_LEN && !memcmp(bytes, ZEC_V4_HEADER, ZEC_V4_HEADER_LEN);
}

int zec_v4_validate_trailer(const unsigned char *p, size_t avail)
{
    size_t i;
    /* p points at the locktime. Need locktime(4) + expiry(4) + valueBalance(8) + 3 = 19. */
    const size_t need = sizeof(uint32_t) + sizeof(uint32_t) + ZEC_V4_TRAILER_ZEROS;
    if (avail < need)
        return WALLY_EINVAL;

    /* valueBalanceSapling (bytes 8..15, relative to locktime) must be 0 (transparent only). */
    for (i = 8; i < 8 + sizeof(uint64_t); ++i)
        if (p[i])
            return WALLY_EINVAL;

    /* nShieldedSpend + nShieldedOutput + nJoinSplit counts (bytes 16..18) must be 0.
     * Drop this check when/if shielded transactions are supported. */
    if (p[16] || p[17] || p[18])
        return WALLY_EINVAL;

    return WALLY_OK;
}

const unsigned char *zec_v4_read_header(const unsigned char *p, struct wally_tx *tx)
{
    /* Read immediately after the 4-byte version. */
    p += uint32_from_le_bytes(p, &tx->version_group_id);
    return p;
}

const unsigned char *zec_v4_read_trailer(const unsigned char *p, struct wally_tx *tx)
{
    /* p points at the locktime (already parsed by the caller without advancing). */
    p += sizeof(uint32_t);                       /* advance past locktime          */
    p += uint32_from_le_bytes(p, &tx->expiry_height);
    p += ZEC_V4_TRAILER_ZEROS;                   /* skip valueBalance + 3 varints  */
    return p;
}

/* ZEC V6 (ZIP-229) constants */
static const uint8_t ZEC_V6_HEADER[] = {0x06, 0x00, 0x00, 0x80}; /* 0x80000006 LE */
#define ZEC_V6_HEADER_LEN (sizeof(ZEC_V6_HEADER))

/* After the version: nVersionGroupId + nConsensusBranchId + nLockTime + nExpiryHeight. */
#define ZEC_V6_HEADER_FIELDS_LEN (4 * sizeof(uint32_t))

/* Trailer = nSpendsSapling + nOutputsSapling + nActionsOrchard + nActionsIronwood (all 0). */
#define ZEC_V6_TRAILER_ZEROS 4

size_t zec_v6_extra_length(void)
{
    /* nVersionGroupId + nConsensusBranchId + nExpiryHeight + 4 zero-varints */
    return 3 * sizeof(uint32_t) + ZEC_V6_TRAILER_ZEROS;
}

size_t zec_v6_header_length(void)
{
    return ZEC_V6_HEADER_FIELDS_LEN;
}

unsigned char *zec_v6_write_header(unsigned char *p, const struct wally_tx *tx)
{
    /* Written immediately after the 4-byte version. */
    p += uint32_to_le_bytes(tx->version_group_id, p);
    p += uint32_to_le_bytes(tx->consensus_branch_id, p);
    p += uint32_to_le_bytes(tx->locktime, p);
    p += uint32_to_le_bytes(tx->expiry_height, p);
    return p;
}

unsigned char *zec_v6_write_trailer(unsigned char *p)
{
    /* Written immediately after the last output. */
    memset(p, 0, ZEC_V6_TRAILER_ZEROS);
    return p + ZEC_V6_TRAILER_ZEROS;
}

bool zec_v6_detect_header(const unsigned char *bytes, size_t len)
{
    uint32_t version_group_id;

    /* The version and the group ID are only valid as a pair. */
    if (len < ZEC_V6_HEADER_LEN + sizeof(uint32_t) ||
        memcmp(bytes, ZEC_V6_HEADER, ZEC_V6_HEADER_LEN))
        return false;
    uint32_from_le_bytes(bytes + ZEC_V6_HEADER_LEN, &version_group_id);
    return version_group_id == ZEC_V6_VERSION_GROUP_ID;
}

int zec_v6_validate_trailer(const unsigned char *p, size_t avail)
{
    /* Exactly 4 zero counts: any shielded data or trailing bytes are rejected.
     * Drop this check when/if shielded transactions are supported. */
    if (avail != ZEC_V6_TRAILER_ZEROS || p[0] || p[1] || p[2] || p[3])
        return WALLY_EINVAL;
    return WALLY_OK;
}

const unsigned char *zec_v6_read_header(const unsigned char *p, struct wally_tx *tx)
{
    /* Read immediately after the 4-byte version. */
    p += uint32_from_le_bytes(p, &tx->version_group_id);
    p += uint32_from_le_bytes(p, &tx->consensus_branch_id);
    p += uint32_from_le_bytes(p, &tx->locktime);
    p += uint32_from_le_bytes(p, &tx->expiry_height);
    return p;
}

int zec_psbt_extract_branch_id(struct wally_psbt *psbt)
{
    size_t i;
    bool found = false;
    uint32_t prop_branch_id = 0;

    psbt->branch_id = 0;
    for (i = 0; i < psbt->unknowns.num_items; ++i) {
        const struct wally_map_item *item = &psbt->unknowns.items[i];
        if (item->key_len == ZEC_PROP_KEY_BRANCH_ID_LEN &&
            !memcmp(item->key, ZEC_PROP_KEY_BRANCH_ID, ZEC_PROP_KEY_BRANCH_ID_LEN) &&
            item->value_len == sizeof(uint32_t)) {
            uint32_from_le_bytes(item->value, &prop_branch_id); /* alignment-safe */
            found = true;
            break;
        }
    }

    if (psbt->tx && psbt->tx->version_group_id == ZEC_V6_VERSION_GROUP_ID) {
        /* V6 commits to its branch ID in the tx; a conflicting key is an error. */
        if (found && prop_branch_id != psbt->tx->consensus_branch_id)
            return WALLY_EINVAL;
        psbt->branch_id = psbt->tx->consensus_branch_id;
    } else if (found) {
        psbt->branch_id = prop_branch_id;
    }
    return WALLY_OK;
}
