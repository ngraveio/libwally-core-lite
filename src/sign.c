#include "internal.h"
#include <include/wally_crypto.h>

static const char TAPTWEAK_BTC[] = "TapTweak";

int wally_ec_private_key_verify(const unsigned char *priv_key, size_t priv_key_len)
{
    if (!priv_key || priv_key_len != EC_PRIVATE_KEY_LEN)
        return WALLY_EINVAL;
    return seckey_verify(priv_key) ? WALLY_OK : WALLY_EINVAL;
}

int wally_ec_public_key_verify(const unsigned char *pub_key, size_t pub_key_len)
{
    secp256k1_pubkey pub;

    if (!pub_key ||
        !(pub_key_len == EC_PUBLIC_KEY_LEN || pub_key_len == EC_PUBLIC_KEY_UNCOMPRESSED_LEN) ||
        !pubkey_parse(&pub, pub_key, pub_key_len))
        return WALLY_EINVAL;

    wally_clear(&pub, sizeof(pub));
    return WALLY_OK;
}

int wally_ec_public_key_from_private_key(const unsigned char *priv_key, size_t priv_key_len,
                                         unsigned char *bytes_out, size_t len)
{
    secp256k1_pubkey pub;
    size_t len_in_out = EC_PUBLIC_KEY_LEN;
    const secp256k1_context *ctx = secp_ctx();
    bool ok;

    ok = priv_key && priv_key_len == EC_PRIVATE_KEY_LEN &&
         bytes_out && len == EC_PUBLIC_KEY_LEN &&
         pubkey_create(ctx, &pub, priv_key) &&
         pubkey_serialize(bytes_out, &len_in_out, &pub, PUBKEY_COMPRESSED) &&
         len_in_out == EC_PUBLIC_KEY_LEN;

    if (!ok && bytes_out)
        wally_clear(bytes_out, len);
    wally_clear(&pub, sizeof(pub));
    return ok ? WALLY_OK : WALLY_EINVAL;
}

int wally_ec_public_key_decompress(const unsigned char *pub_key, size_t pub_key_len,
                                   unsigned char *bytes_out, size_t len)
{
    secp256k1_pubkey pub;
    size_t len_in_out = EC_PUBLIC_KEY_UNCOMPRESSED_LEN;
    bool ok;

    ok = pub_key && pub_key_len == EC_PUBLIC_KEY_LEN &&
         bytes_out && len == EC_PUBLIC_KEY_UNCOMPRESSED_LEN &&
         pubkey_parse(&pub, pub_key, pub_key_len) &&
         pubkey_serialize(bytes_out, &len_in_out, &pub, PUBKEY_UNCOMPRESSED) &&
         len_in_out == EC_PUBLIC_KEY_UNCOMPRESSED_LEN;

    if (!ok && bytes_out)
        wally_clear(bytes_out, len);
    wally_clear(&pub, sizeof(pub));
    return ok ? WALLY_OK : WALLY_EINVAL;
}

int wally_ec_public_key_tweak(const unsigned char *pub_key, size_t pub_key_len,
                              const unsigned char *tweak, size_t tweak_len,
                              unsigned char *bytes_out, size_t len)
{
    const secp256k1_context *ctx = secp256k1_context_static;
    secp256k1_pubkey pk;

    if (!pub_key || pub_key_len != EC_PUBLIC_KEY_LEN ||
        !tweak || tweak_len != EC_PRIVATE_KEY_LEN ||
        !bytes_out || len != EC_PUBLIC_KEY_LEN)
        return WALLY_EINVAL;

    if (!pubkey_parse(&pk, pub_key, pub_key_len) ||
        !pubkey_tweak_add(ctx, &pk, tweak) ||
        !pubkey_serialize(bytes_out, &len, &pk, PUBKEY_COMPRESSED) ||
        len != EC_PUBLIC_KEY_LEN)
        return WALLY_ERROR;
    return WALLY_OK;
}

int wally_ec_sig_to_der(const unsigned char *sig, size_t sig_len,
                        unsigned char *bytes_out, size_t len, size_t *written)
{
    secp256k1_ecdsa_signature sig_secp;
    size_t len_in_out = len;
    const secp256k1_context *ctx = secp256k1_context_static;
    bool ok;

    if (written)
        *written = 0;

    ok = sig && sig_len == EC_SIGNATURE_LEN &&
         bytes_out && len >= EC_SIGNATURE_DER_MAX_LEN && written &&
         secp256k1_ecdsa_signature_parse_compact(ctx, &sig_secp, sig) &&
         secp256k1_ecdsa_signature_serialize_der(ctx, bytes_out,
                                                 &len_in_out, &sig_secp);

    if (!ok && bytes_out)
        wally_clear(bytes_out, len);
    if (ok)
        *written = len_in_out;
    wally_clear(&sig_secp, sizeof(sig_secp));
    return ok ? WALLY_OK : WALLY_EINVAL;
}

int wally_ec_xonly_public_key_verify(const unsigned char *pub_key, size_t pub_key_len)
{
    secp256k1_xonly_pubkey pub;

    if (!pub_key || pub_key_len != EC_XONLY_PUBLIC_KEY_LEN ||
        !xpubkey_parse(&pub, pub_key, pub_key_len))
        return WALLY_EINVAL;

    wally_clear(&pub, sizeof(pub));
    return WALLY_OK;
}

int wally_ec_sig_from_der(const unsigned char *bytes, size_t bytes_len,
                          unsigned char *bytes_out, size_t len)
{
    secp256k1_ecdsa_signature sig_secp;
    const secp256k1_context *ctx = secp256k1_context_static;
    bool ok;

    ok = bytes && bytes_len && bytes_out && len == EC_SIGNATURE_LEN &&
         secp256k1_ecdsa_signature_parse_der(ctx, &sig_secp, bytes, bytes_len) &&
         secp256k1_ecdsa_signature_serialize_compact(ctx, bytes_out, &sig_secp);

    if (!ok && bytes_out)
        wally_clear(bytes_out, len);
    wally_clear(&sig_secp, sizeof(sig_secp));
    return ok ? WALLY_OK : WALLY_EINVAL;
}

static int get_bip341_tweak(const unsigned char *pub_key, size_t pub_key_len,
                            const unsigned char *merkle_root, uint32_t flags,
                            unsigned char *tweak, size_t tweak_len)
{
    unsigned char preimage[EC_XONLY_PUBLIC_KEY_LEN + SHA256_LEN];
    const size_t offset = pub_key_len == EC_PUBLIC_KEY_LEN ? 1 : 0;
    const size_t preimage_len = merkle_root ? sizeof(preimage) : EC_XONLY_PUBLIC_KEY_LEN;

    if (flags)
        return WALLY_EINVAL;

    memcpy(preimage, pub_key + offset, EC_XONLY_PUBLIC_KEY_LEN);
    if (merkle_root)
        memcpy(preimage + EC_XONLY_PUBLIC_KEY_LEN, merkle_root, SHA256_LEN);
    return wally_bip340_tagged_hash(preimage, preimage_len,
                                    TAPTWEAK_BTC,
                                    tweak, tweak_len);
}

int wally_ec_public_key_bip341_tweak(
    const unsigned char *pub_key, size_t pub_key_len,
    const unsigned char *merkle_root, size_t merkle_root_len,
    uint32_t flags, unsigned char *bytes_out, size_t len)
{
    secp256k1_xonly_pubkey xonly;
    int ret;

    if (!pub_key || BYTES_INVALID_N(merkle_root, merkle_root_len, SHA256_LEN) ||
        !bytes_out || len != EC_PUBLIC_KEY_LEN)
        return WALLY_EINVAL;

    ret = xpubkey_parse(&xonly, pub_key, pub_key_len) ? WALLY_OK : WALLY_EINVAL;
    if (ret == WALLY_OK) {
        unsigned char tweak[SHA256_LEN];
        secp256k1_pubkey tweaked;
        size_t len_in_out = EC_PUBLIC_KEY_LEN;
        ret = get_bip341_tweak(pub_key, pub_key_len, merkle_root,
                               flags, tweak, sizeof(tweak));
        if (ret == WALLY_OK && !xpubkey_tweak_add(&tweaked, &xonly, tweak))
            ret = WALLY_ERROR;
        if (ret == WALLY_OK)
            pubkey_serialize(bytes_out, &len_in_out,
                             &tweaked, SECP256K1_EC_COMPRESSED);
    }
    return ret;
}
