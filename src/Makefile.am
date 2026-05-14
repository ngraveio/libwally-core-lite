SUBDIRS = secp256k1

LIBSECP256K1 = secp256k1/libsecp256k1.la

$(LIBSECP256K1): $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

noinst_LTLIBRARIES =

include_HEADERS =
include_HEADERS += $(top_srcdir)/include/wally.hpp
include_HEADERS += $(top_srcdir)/include/wally_address.h
include_HEADERS += $(top_srcdir)/include/wally_bip32.h
include_HEADERS += $(top_srcdir)/include/wally_core.h
include_HEADERS += $(top_srcdir)/include/wally_crypto.h
include_HEADERS += $(top_srcdir)/include/wally_descriptor.h
include_HEADERS += $(top_srcdir)/include/wally_map.h
include_HEADERS += $(top_srcdir)/include/wally_psbt.h
include_HEADERS += $(top_srcdir)/include/wally_psbt_members.h
include_HEADERS += $(top_srcdir)/include/wally_script.h
include_HEADERS += $(top_srcdir)/include/wally_symmetric.h
include_HEADERS += $(top_srcdir)/include/wally_transaction.h
include_HEADERS += $(top_srcdir)/include/wally_transaction_members.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wallylite.pc

lib_LTLIBRARIES = libwallylite.la

libwallylite_la_SOURCES = \
    address.c \
    base_58.c \
    base_64.c \
    bip32.c \
    bech32.c \
    blech32.c \
    hex_.c \
    hmac.c \
    internal.c \
    map.c \
    pbkdf2.c \
    psbt.c \
    pullpush.c \
    script.c \
    sign.c \
    symmetric.c \
    transaction.c \
    tx_io.c \
    wif.c \
    wordlist.c \
    ccan/ccan/base64/base64.c \
    ccan/ccan/crypto/ripemd160/ripemd160.c \
    ccan/ccan/crypto/sha256/sha256.c \
    ccan/ccan/crypto/sha512/sha512.c \
    ccan/ccan/str/hex/hex.c

libwallylite_la_INCLUDES = \
    include/wally.hpp \
    include/wally_address.h \
    include/wally_bip32.h \
    include/wally_core.h \
    include/wally_crypto.h \
    include/wally_map.h \
    include/wally_psbt.h \
    include/wally_script.h \
    include/wally_symmetric.h \
    include/wally_transaction.h

libwallylite_la_LDFLAGS = -no-undefined

libwallylite_la_CFLAGS = -I$(top_srcdir) -I$(srcdir)/ccan -I$(top_srcdir)/src/secp256k1/include -DWALLY_CORE_BUILD=1 $(AM_CFLAGS)
libwallylite_la_LIBADD = $(LIBSECP256K1) $(noinst_LTLIBRARIES)

