dnl ==============================================================
dnl Process this file with autoconf to produce a configure script.
dnl (or run ./autogen.sh if it exists)
dnl ==============================================================

AC_INIT(ed2k_hash/processfile.h)

AM_INIT_AUTOMAKE(ed2k_hash,0.4.0)

dnl This ksh/zsh feature conflicts with `cd blah ; pwd`
unset CDPATH

AC_PROG_CC
AC_PROG_CXX

AC_PREFIX_DEFAULT(/usr/local)

AM_CONFIG_HEADER(config.h)

AC_OUTPUT([
./Makefile
ed2k_hash/Makefile
ed2k_hash/docs/Makefile
ed2k_hash/docs/en/Makefile
ed2k_hash/GUI/Makefile
])


