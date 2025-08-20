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

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = wallylite.pc

lib_LTLIBRARIES = libwallylite.la

libwallylite_la_SOURCES = \
    address.c \
    base58.c \
    base64.c \
    bip32.c \
    bech32.c \
    blech32.c \
    hex.c \
    hmac.c \
    internal.c \
    map.c \
    pbkdf2.c \
    psbt.c \
    pullpush.c \
    script.c \
    symmetric.c \
    transaction.c \
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

libwallylite_la_CFLAGS = -I$(top_srcdir) -I$(srcdir)/ccan -DWALLY_CORE_BUILD=1 $(AM_CFLAGS)
libwallylite_la_LIBADD = $(noinst_LTLIBRARIES)

