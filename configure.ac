AC_PREREQ([2.60])
AC_INIT([libwallycore-lite],[0.9.0])
AC_CONFIG_AUX_DIR([tools/build-aux])
AC_CONFIG_MACRO_DIR([tools/build-aux/m4])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_HOST
AH_TOP([#ifndef LIBWALLYCORE_CONFIG_H
#define LIBWALLYCORE_CONFIG_H])
AH_BOTTOM([#include "ccan_config.h"
#endif /*LIBWALLYCORE_CONFIG_H*/])

LDPATH_VAR=LD_LIBRARY_PATH
case $host_os in
*darwin*)
    is_osx="yes"
    LDPATH_VAR=DYLD_LIBRARY_PATH
    ;;
esac
AM_CONDITIONAL([IS_OSX], [test "x$is_osx" == "xyes"])
AC_SUBST([LDPATH_VAR])

case $host in
*mingw*)
    is_mingw="yes"
    ;;
esac
AM_CONDITIONAL([IS_MINGW], [test "x$is_mingw" == "xyes"])

# Require Automake 1.11.2 for AM_PROG_AR
AM_INIT_AUTOMAKE([1.11.2 foreign subdir-objects])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_AR
#saved_cflags="$CFLAGS"
LT_INIT([enable-static, enable-shared])
#CFLAGS="$saved_cflags"

AC_SUBST([AR])
AC_SUBST([RANLIB])

GNU_SED=sed
AC_CHECK_PROG(HAVE_GSED,gsed,yes,no)
if test "x$HAVE_GSED" == "xyes"; then
  GNU_SED=gsed
else
    if test "x$is_osx" == "xyes"; then
        AC_MSG_ERROR([gsed must be available to build this library])
    fi
fi
AC_SUBST([GNU_SED])

#
# C facilities
#
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[enable debugging (default: no)]),
    [debug=$enableval], [debug=no])
AC_ARG_ENABLE(coverage,
    AS_HELP_STRING([--enable-coverage],[enable code coverage (default: no)]),
    [coverage=$enableval], [coverage=no])
AC_ARG_ENABLE(tests,
    AS_HELP_STRING([--enable-tests],[enable code tests (default: yes)]),
    [tests=$enableval], [tests=yes])
AC_ARG_ENABLE(elements,
    AS_HELP_STRING([--enable-elements],[enable elements tx code (default: no)]),
    [elements=$enableval], [elements=no])
AC_ARG_ENABLE(builtin-memset,
    AS_HELP_STRING([--enable-builtin-memset],[disable to add -fno-builtin-memset to compiler flags. helps with explicit_bzero/memset being elided on Linux clang 7.0.1 and up (default: yes)]),
    [builtin_memset=$enableval], [builtin_memset=yes])
AC_ARG_ENABLE(minimal,
    AS_HELP_STRING([--enable-minimal],[enable minimal size/memory footprint build (default: no)]),
    [minimal=$enableval], [minimal=no])
AC_ARG_ENABLE(mbed-tls,
    AS_HELP_STRING([--enable-mbed-tls],[enable minimal size/memory footprint build (default: no)]),
    [mbedtls=$enableval], [mbedtls=no])
AC_ARG_ENABLE(asm,
    AS_HELP_STRING([--enable-asm],[enable assembly language implementations (default: yes)]),
    [asm=$enableval], [asm=yes])
AM_CONDITIONAL([RUN_TESTS], [test "x$tests" == "xyes"])
AM_CONDITIONAL([BUILD_ELEMENTS], [test "x$elements" == "xyes"])
AM_CONDITIONAL([BUILD_STANDARD_SECP], [test "x$standard_secp" == "xyes"])
AM_CONDITIONAL([BUILD_MINIMAL], [test "x$minimal" == "xyes"])

AC_C_BIGENDIAN()
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_TYPE_UINT32_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

AM_CFLAGS=
AX_CHECK_COMPILE_FLAG([-O0], [NOOPT_CFLAGS="-O0"])

if test "x$debug" == "xyes"; then
    # Make debugging easier, leave assertions in
    AM_CFLAGS="$AM_CFLAGS $NOOPT_CFLAGS"
    AX_CHECK_COMPILE_FLAG([-ggdb], [AM_CFLAGS="$AM_CFLAGS -ggdb"])
    AX_CHECK_LINK_FLAG([-O0], [LDFLAGS="$LDFLAGS -O0"])
    AX_CHECK_LINK_FLAG([-ggdb], [LDFLAGS="$LDFLAGS -ggdb"])
    if test "x$coverage" == "xyes"; then
        AX_CHECK_COMPILE_FLAG([-fprofile-arcs -ftest-coverage], [AM_CFLAGS="$AM_CFLAGS -fprofile-arcs -ftest-coverage"])
        AX_CHECK_LINK_FLAG([-lgcov], [LDFLAGS="$LDFLAGS -lgcov"])
        AX_CHECK_LINK_FLAG([--coverage], [LDFLAGS="$LDFLAGS --coverage"])
    fi
else
    # Optimise and harden if we can
    AX_CHECK_COMPILE_FLAG([-O2], [AM_CFLAGS="-O2 $AM_CFLAGS"])
    AC_CHECK_DEFINE([_FORTIFY_SOURCE], [], [
      AX_CHECK_COMPILE_FLAG([-D_FORTIFY_SOURCE=2],
        [CPPFLAGS="$CPPFLAGS -D_FORTIFY_SOURCE=2"])
    ])
    AX_CHECK_COMPILE_FLAG([-fstack-protector-strong], [AM_CFLAGS="$AM_CFLAGS -fstack-protector-strong"])
    AX_CHECK_COMPILE_FLAG([-DNDEBUG=1], [AM_CFLAGS="$AM_CFLAGS -DNDEBUG=1"])
    AX_CHECK_LINK_FLAG([-O2], [LDFLAGS="-O2 $LDFLAGS"])
    AX_CHECK_LINK_FLAG([-Wl,-z,relro], [LDFLAGS="$LDFLAGS -Wl,-z,relro"])
fi

if test "x$elements" == "xyes"; then
    AX_CHECK_COMPILE_FLAG([-DBUILD_ELEMENTS=1], [AM_CFLAGS="$AM_CFLAGS -DBUILD_ELEMENTS=1"])
fi

if test "x$minimal" == "xyes"; then
    AX_CHECK_COMPILE_FLAG([-DBUILD_MINIMAL=1], [AM_CFLAGS="$AM_CFLAGS -DBUILD_MINIMAL=1"])
fi

if test "x$builtin_memset" == "xno"; then
    AX_CHECK_COMPILE_FLAG([-fno-builtin-memset], [AM_CFLAGS="$AM_CFLAGS -fno-builtin"])
fi

# -flax-vector-conversions is needed for our arm assembly
AX_CHECK_COMPILE_FLAG([-flax-vector-conversions], [AM_CFLAGS="$AM_CFLAGS -flax-vector-conversions"])
AX_CHECK_COMPILE_FLAG([-fno-strict-aliasing], [NOALIAS_CFLAGS="-fno-strict-aliasing"])
AX_CHECK_COMPILE_FLAG([-fno-builtin], [NOBUILTIN_CFLAGS="-fno-builtin"])

AX_CHECK_COMPILE_FLAG([-Wformat-nonliteral], [AM_CFLAGS="-Wformat-nonliteral $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wformat-security], [AM_CFLAGS="-Wformat-security $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wformat], [AM_CFLAGS="-Wformat $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wstrict-prototypes], [AM_CFLAGS="-Wstrict-prototypes $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wshadow], [AM_CFLAGS="-Wshadow $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wnested-externs], [AM_CFLAGS="-Wnested-externs $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wcast-align], [AM_CFLAGS="-Wcast-align $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wpedantic], [AM_CFLAGS="-Wpedantic $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wextra], [AM_CFLAGS="-Wextra $AM_CFLAGS"])
AX_CHECK_COMPILE_FLAG([-Wall], [AM_CFLAGS="-Wall $AM_CFLAGS"])

# Needed for Ubuntu 20.04LTS when compiling as a library
AX_CHECK_COMPILE_FLAG([-D_DEFAULT_SOURCE=1], [AM_CFLAGS="-D_DEFAULT_SOURCE=1 $AM_CFLAGS"])

# Disable some unhelpful errors from those enabled above
AX_CHECK_COMPILE_FLAG([-Wno-unused-function], [AM_CFLAGS="$AM_CFLAGS -Wno-unused-function"])
AX_CHECK_COMPILE_FLAG([-Wno-long-long], [AM_CFLAGS="$AM_CFLAGS -Wno-long-long"])
AX_CHECK_COMPILE_FLAG([-Wno-overlength-strings], [AM_CFLAGS="$AM_CFLAGS -Wno-overlength-strings"])
AX_CHECK_COMPILE_FLAG([-Wno-variadic-macros], [AM_CFLAGS="$AM_CFLAGS -Wno-variadic-macros"])

if echo | "$CC" -dM -E - | grep __clang__ >/dev/null; then
    AX_CHECK_COMPILE_FLAG([-Wno-gnu-statement-expression], [AM_CFLAGS="$AM_CFLAGS -Wno-gnu-statement-expression"])
    AX_CHECK_COMPILE_FLAG([-Wno-zero-length-array], [AM_CFLAGS="$AM_CFLAGS -Wno-zero-length-array"])
    AX_CHECK_COMPILE_FLAG([-Wno-language-extension-token], [AM_CFLAGS="$AM_CFLAGS -Wno-language-extension-token"])
    AX_CHECK_COMPILE_FLAG([-Wno-unknown-attributes], [AM_CFLAGS="$AM_CFLAGS -Wno-unknown-attributes"])
fi
AC_SUBST([NOOPT_CFLAGS])
AC_SUBST([NOALIAS_CFLAGS])
AC_SUBST([NOBUILTIN_CFLAGS])

# SWIG versions vary in generated code quality; skip warnings
SWIG_WARN_CFLAGS="-fno-strict-aliasing"
AX_CHECK_COMPILE_FLAG([-Wno-shadow], [SWIG_WARN_CFLAGS="$SWIG_WARN_CFLAGS -Wno-shadow"])
AX_CHECK_COMPILE_FLAG([-Wno-missing-field-initializers], [SWIG_WARN_CFLAGS="$SWIG_WARN_CFLAGS -Wno-missing-field-initializers"])
if echo | "$CC" -dM -E - | grep __clang__ >/dev/null; then
    AX_CHECK_COMPILE_FLAG([-Wno-self-assign], [SWIG_WARN_CFLAGS="$SWIG_WARN_CFLAGS -Wno-self-assign"])
fi
AC_SUBST([SWIG_WARN_CFLAGS])

AC_ARG_ENABLE(export-all,
    AS_HELP_STRING([--enable-export-all],[export all functions (for testing, default: no)]),
    [export_all=$enableval], [export_all=no])
AM_CONDITIONAL([EXPORT_ALL], [test "x$export_all" == "xyes"])

if test "x$export_all" != "xyes"; then
    AX_CHECK_COMPILE_FLAG([-fvisibility=hidden], [AM_CFLAGS="$AM_CFLAGS -fvisibility=hidden"])
fi

# Assume we have no unaligned access if cross-compiling
AC_RUN_IFELSE([AC_LANG_SOURCE([[int main(void){static int a[2];return *((int*)(((char*)a)+1)) != 0;}]])],
              have_unaligned=1, have_unaligned=0, have_unaligned=0)
AC_DEFINE_UNQUOTED([HAVE_UNALIGNED_ACCESS], [$have_unaligned], [Define if we have unaligned access])

if test "x$is_osx" != "xyes"; then
    # Assume we are using gcc (i.e. have this attribute) if cross-compiling
    AC_COMPILE_IFELSE([AC_LANG_SOURCE([extern int foo(int) __attribute__((weak)); int main(void){return 0;}])],
                      [AC_DEFINE([HAVE_ATTRIBUTE_WEAK], 1, [Define if we have __attribute__((weak))])])
fi

AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <sys/mman.h>]],[[mmap(0,0,0,0,0,0)]])],
               [AC_DEFINE(HAVE_MMAP, 1, [Define if we have mmap])])

AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdlib.h>]],[[return posix_memalign(NULL,0,0)]])],
               [AC_DEFINE(HAVE_POSIX_MEMALIGN, 1, [Define if we have posix_memalign])])

AC_CHECK_FUNCS([memset_s explicit_bzero explicit_memset])

if test "x$asm" == "xyes"; then
    AC_MSG_CHECKING(whether we can use inline asm code)
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    ]], [[
    int a = 42;
    int *pnt = &a;
    __asm__ __volatile__ ("" : : "r"(pnt) : "memory");
    ]])],
      [AC_MSG_RESULT(yes)
       AC_DEFINE([HAVE_INLINE_ASM], [1], [inline asm code can be used])],
      [AC_MSG_RESULT(no)]
    )
fi

AC_CHECK_HEADERS([byteswap.h sys/mman.h])
if test "x$mbedtls" == "xyes"; then
    AC_CHECK_HEADERS([mbedtls/sha256.h mbedtls/sha512.h])
fi

AC_ARG_ENABLE(clear-tests,
    AS_HELP_STRING([--enable-clear-tests],[enable tests for memory clearing (default: yes)]),
    [clear_tests=$enableval], [clear_tests=yes])

AX_PTHREAD([ac_have_pthread=yes], [ac_have_pthread=no])
AM_CONDITIONAL([USE_PTHREAD], [test "x$ac_have_pthread" == "xyes" -a "x$clear_tests" == "xyes"])
if test "x$ac_have_pthread" == "xyes"; then
    AC_DEFINE([HAVE_PTHREAD], 1, [Define if we have pthread support])
    AC_CHECK_HEADERS([asm/page.h])
fi

AC_SUBST([AM_CFLAGS])

AM_CONDITIONAL([SHARED_BUILD_ENABLED], [test "x$enable_shared" == "xyes"])

AC_CONFIG_FILES([
 Makefile
 src/Makefile
 src/wallylite.pc
])

AC_CONFIG_SUBDIRS([src/secp256k1])

export CC
export CFLAGS
export AR
export ARFLAGS
export AR_FLAGS
export LD
export LDFLAGS
ac_configure_args="${ac_configure_args} --disable-shared --with-pic --with-bignum=no --enable-experimental --enable-module-ecdh --enable-module-recovery --enable-module-ecdsa-s2c --enable-module-rangeproof --enable-module-surjectionproof --enable-module-whitelist --enable-module-generator --enable-module-extrakeys --enable-module-schnorrsig --enable-openssl-tests=no --enable-exhaustive-tests=no --enable-benchmark=no --disable-dependency-tracking ${secp_asm}"

AC_OUTPUT