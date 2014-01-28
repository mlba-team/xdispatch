#
# package.sh
# 
# Copyright (c) 2011-2014 MLBA-Team
# All rights reserved.
# 
# @LICENSE_HEADER_START@
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# @LICENSE_HEADER_END@
#

#!/bin/sh

CONFIG=package/ubuntu
VERSION=$1
BUILDDIR=/tmp/xdispatch

# copy sources to clean target
echo -- create $BUILDDIR
mkdir -p $BUILDDIR
echo -- copy sources to $BUILDDIR/$VERSION
cp -vrf . $BUILDDIR/$VERSION
CURRENT=`pwd`
cd $BUILDDIR/$VERSION
# remove .svn directories (if any)
echo -- clean svn information
rm -vrf `find . -type d -name .svn`
# delete old build files
echo -- cleaning build directories
rm -vrf build/QtCreator_ProjectFiles build/*MakeFiles build/Docs build/VS10_ProjectFiles*
# copy debian packaging information
echo -- copy packaging information
mkdir -p debian
cp -vrf $CONFIG/* debian

# build src package
echo -- build source package
dpkg-buildpackage -us -uc -S -rfakeroot

# build binary packages
echo -- build binaries

sudo pbuilder build --buildresult $BUILDDIR ../*.dsc 

# copy binaries back to source dir
echo -- retrieve packages
mv *.deb *.dsc *.tar.gz *.changes $CURRENT/../
cd ..
lintian *.deb
mv *.deb *.dsc *.tar.gz *.changes $CURRENT/../

# cleanup
echo -- cleanup
cd $CURRENT
#rm -rf $BUILDDIR

echo -- DONE

