#!/bin/bash
#
# autogen.sh for ed2k_hash version 0.4.x
#
# Run this to create configure script etc.
#
# crude little script by Tim Muller, with some stuff
# from gnome
#

AUTOCONF_BIN=autoconf
AUTOHEADER_BIN=autoheader
AUTOMAKE_BIN=automake
ACLOCAL_BIN=aclocal

DIE=0

($AUTOCONF_BIN --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to compile ed2k-hash."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
  DIE=1
}

($AUTOMAKE_BIN --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed to compile ed2k-hash."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake/automake-1.7.6.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}


# if no automake, don't bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
  echo "installed doesn't appear recent enough."
  echo "Get ftp://ftp.gnu.org/pub/gnu/automake/automake-1.7.6.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
}

if test "$DIE" -eq 1; then
  exit 1
fi

#-----------------------------------------------------------------------------#
# Test automake version (by Julian Foad, found on flightgear-devel 17/08/2002)
#-----------------------------------------------------------------------------#

AUTOMAKE_VERSION_REAL=`automake --version | head -n 1 | awk '{print $4}'`
AUTOMAKE_VERSION=`echo $AUTOMAKE_VERSION_REAL | sed -e 's/\.\([0-9]\).*/\1/'`

echo "***** Checking automake version: $AUTOMAKE_VERSION_REAL"

if [ "x$AUTOMAKE_VERSION" == "x14" ] || [ "x$AUTOMAKE_VERSION" == "x15" ]
then
  echo "***** Checking for newer automake versions...";

  FOUND_AM=0

  for ver in 1.7 1.6
  do
    if automake-$ver --version </dev/null &>/dev/null
    then
      AUTOMAKE_BIN=automake-$ver;
      ACLOCAL_BIN=aclocal-$ver;
      echo "***** Found $AUTOMAKE_BIN. Using that instead.";
      FOUND_AM=1
      break
    fi
  done

  if [ "x$FOUND_AM" = "x0" ]; then
    echo "***** Your automake version (1.4 or 1.5) is too old.";
    echo "***** Please install a newer version (1.7 or 1.6)";
  fi

fi

#-----------------------------------------------------------------------------#
# ... and the same for autoconf ...
#-----------------------------------------------------------------------------#

# This only seems not to work with autoconf2.13, but does work with 2.5x
AUTOCONF_VERSION_REAL=`$AUTOCONF_BIN --version | head -n 1 | awk '{print $4}'`

if [ -z $AUTOCONF_VERSION_REAL ]; then
  AUTOCONF_VERSION_REAL=`$AUTOCONF_BIN --version | head -n 1 | awk '{print $3}'`
fi

echo "***** Checking autoconf version: $AUTOCONF_VERSION_REAL"

# Check whether the autoconf version detected is too old.
FOUND_OLD_AC=0
for ver in 2.10 2.11 2.12 2.13
do
  if [ "x$AUTOCONF_VERSION_REAL" = "x$ver" ]
  then
    FOUND_OLD_AC=1
    break
  fi
done

if [ "x$FOUND_OLD_AC" = "x1" ];
then
  echo "***** Checking for newer autoconf versions..."

  FOUND_NEW_AC=0

  for ver in 2.53 2.54 2.55 2.56 2.57 2.58 2.60
  do
    if autoconf$ver --version </dev/null &>/dev/null
    then
      AUTOCONF_BIN=autoconf$ver;
      AUTOHEADER_BIN=autoheader$ver;
      echo "***** Found $AUTOCONF_BIN. Using that instead.";
      FOUND_NEW_AC=1
      break
    fi
  done

  if [ "x$FOUND_NEW_AC" = "x0" ]; then
    echo "***** Your autoconf version (<=2.52) is too old.";
    echo "***** Please install a newer version (>=2.53)";
  fi
fi





if [ -d macros ]; then
  cat ./macros/*m4 > acinclude.m4
else
  if [ -e acinclude.m4 ]; then
    rm acinclude.m4
  fi
fi

echo "***** Running $ACLOCAL_BIN..."
$ACLOCAL_BIN || exit 1;

echo "***** Running $AUTOHEADER_BIN..."
$AUTOHEADER_BIN || exit 1;

echo "***** Running $AUTOMAKE_BIN..."
$AUTOMAKE_BIN --add-missing || exit 1;

echo "***** Running $AUTOCONF_BIN..."
$AUTOCONF_BIN || exit 1;

echo "*****"
echo "***** okay, calling ./configure now..."
echo "*****"

./configure "$@" || exit 1;

echo "*****"
echo "***** okay. Now type 'make' and after that do 'make install' (as root)"
echo "*****"

