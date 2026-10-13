#ifndef LIBWALLY_CORE_ZCASH_H
#define LIBWALLY_CORE_ZCASH_H

#include "wally_core.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wally_tx;
struct wally_psbt;

#define ZEC_V4_VERSION_GROUP_ID 0x892F2085u

/* Fixed extra bytes a ZEC V4 tx contributes beyond the base BTC layout:
 * nVersionGroupId(4) + nExpiryHeight(4) + valueBalanceSapling(8) + 3 zero-varints. */
size_t zec_v4_extra_length(void);

/* Serialization. Each returns the advanced write cursor. */
unsigned char *zec_v4_write_header(unsigned char *p, const struct wally_tx *tx);  /* after version  */
unsigned char *zec_v4_write_trailer(unsigned char *p, const struct wally_tx *tx); /* after locktime */

/* Parsing. */
bool zec_v4_detect_header(const unsigned char *bytes, size_t len); /* leading 0x04 00 00 80 */
int  zec_v4_validate_trailer(const unsigned char *p, size_t avail); /* p at locktime; WALLY_OK/EINVAL */
const unsigned char *zec_v4_read_header(const unsigned char *p, struct wally_tx *tx);  /* after version  */
const unsigned char *zec_v4_read_trailer(const unsigned char *p, struct wally_tx *tx); /* p at locktime  */

/* ZEC V6 (ZIP-229), transparent-only. Unlike V4, the branch ID, locktime and expiry
 * height sit in the header, before the inputs, and the trailer holds 4 zero counts:
 * nSpendsSapling, nOutputsSapling, nActionsOrchard, nActionsIronwood. */
#define ZEC_V6_VERSION_GROUP_ID 0xD884B698u

/* Fixed extra bytes a ZEC V6 tx contributes beyond the base BTC layout:
 * nVersionGroupId(4) + nConsensusBranchId(4) + nExpiryHeight(4) + 4 zero-varints.
 * The locktime is counted by the base layout, it only moves position. */
size_t zec_v6_extra_length(void);

/* Serialization. Each returns the advanced write cursor. */
unsigned char *zec_v6_write_header(unsigned char *p, const struct wally_tx *tx); /* after version */
unsigned char *zec_v6_write_trailer(unsigned char *p);                           /* after outputs */

/* Parsing. */
bool zec_v6_detect_header(const unsigned char *bytes, size_t len); /* 0x06 00 00 80 + V6 group ID */
size_t zec_v6_header_length(void);                                 /* bytes after version, before inputs */
int  zec_v6_validate_trailer(const unsigned char *p, size_t avail); /* p after outputs; WALLY_OK/EINVAL */
const unsigned char *zec_v6_read_header(const unsigned char *p, struct wally_tx *tx); /* after version */

/* PSBT: pull Zcash CONSENSUS_BRANCH_ID out of the proprietary key (BitGo convention).
 * Sets psbt->branch_id (0 when absent). Always WALLY_OK. */
int zec_psbt_extract_branch_id(struct wally_psbt *psbt);

#ifdef __cplusplus
}
#endif
#endif /* LIBWALLY_CORE_ZCASH_H */