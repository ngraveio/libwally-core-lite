#ifndef LIBWALLY_CORE_ZCASH_H
#define LIBWALLY_CORE_ZCASH_H

#include "wally_core.h"

/* NGRAVE-ZEC: ZEC_TX_FLAG expands to "| WALLY_TX_FLAG_ZEC_V4" when
 * WALLY_ZCASH is defined (--enable-zcash / -DWALLY_ZCASH), else empty. */
#ifdef WALLY_ZCASH
#define ZEC_TX_FLAG | WALLY_TX_FLAG_ZEC_V4
#else
#define ZEC_TX_FLAG
#endif

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

/* PSBT: pull Zcash CONSENSUS_BRANCH_ID out of the proprietary key (BitGo convention).
 * Sets psbt->branch_id (0 when absent). Always WALLY_OK. */
int zec_psbt_extract_branch_id(struct wally_psbt *psbt);

#ifdef __cplusplus
}
#endif
#endif /* LIBWALLY_CORE_ZCASH_H */