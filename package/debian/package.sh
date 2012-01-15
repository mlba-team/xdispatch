#!/bin/sh

VERSION=$1
BUILDDIR=/tmp/xdispatch

# copy sources to clean target
echo -- create $BUILDDIR
mkdir -p $BUILDDIR
echo -- copy sources to $BUILDDIR/$VERSION
cp -rf . $BUILDDIR/$VERSION
CURRENT=`pwd`
cd $BUILDDIR/$VERSION
# remove .svn directories (if any)
echo -- clean svn information
rm -rf `find . -type d -name .svn`
# delete old build files
echo -- cleaning build directories
rm -rf Build/QtCreator_ProjectFiles Build/*MakeFiles Build/Docs Build/VS10_ProjectFiles*
# copy debian packaging information
echo -- copy packaging information
cp -rf package/debian debian

# build src package
echo -- build source package
dpkg-buildpackage -us -uc -S -rfakeroot

# build binary packages
echo -- build binaries

sudo pbuilder build --buildresult $BUILDDIR ../*.dsc 

# copy binaries back to source dir
echo -- retrieve packages
cd ..
lintian *.deb
cp *.deb *.dsc *.tar.gz *.changes $CURRENT/../

# cleanup
echo -- cleanup
cd $CURRENT
rm -rf $BUILDDIR

echo -- DONE

