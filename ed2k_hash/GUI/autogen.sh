#!/bin/sh
#
# autogen.sh for ed2k_hash GUI version
#
# crude little script by Tim Muller
#

for need in install-sh mkinstalldirs missing depcomp
do
  if [ ! -e "../../admin/$need" ]; then
    echo "../../admin/$need not found!";
    exit 1;
  else
    if [ ! -e "./$need" ]; then
      ln -s "../../admin/$need" . || echo ""
    fi
  fi
done

echo "***** Running aclocal..."
aclocal || ( echo "aclocal not found or failed!"; exit 1; )

echo "***** Running autoheader..."
autoheader || ( echo "autoheader not found or failed!"; exit 1; )

echo "***** Running automake..."
automake || ( echo "automake not found or failed!"; exit 1; )

echo "***** Running autoconf..."
autoconf || ( echo "autoconf not found or failed!"; exit 1; )

echo "*****"
echo "***** okay, calling ./configure now..."
echo "*****"

./configure || ( echo "./configure failed!"; exit 1; )

echo "*****"
echo "***** okay. Now type 'make' and after that do 'make install' (as root)"
echo "*****"
