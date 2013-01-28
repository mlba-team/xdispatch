#
# Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
#
# @MLBA_OPEN_LICENSE_HEADER_START@
#
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
#
# @MLBA_OPEN_LICENSE_HEADER_END@
#

set( CPACK_DEBIAN_PACKAGE_NAME "libxdispatch")
set( CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>=2.13-1) , libgcc1 (>=4.5.1), libstdc++6 (>=4.5.1)")
set( CPACK_DEBIAN_PACKAGE_MAINTAINER "${XDISPATCH_MAINTAINER}")
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${XDISPATCH_SUMMARY}")
set( CPACK_PACKAGE_DESCRIPTION "${XDISPATCH_DESCRIPTION}")
set( CPACK_DEBIAN_PACKAGE_SECTION "Devel")
set( CPACK_DEBIAN_PACKAGE_VERSION "${XDISPATCH_VERSION}-${PACKAGE_COMPILER}")
set( CPACK_DEBIAN_COMPONENT_INSTALL ON) # enable me as soon as component based deb files work with cmake

add_custom_target(
	debian ./package/debian/package.sh xdispatch-${XDISPATCH_VERSION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/package/debian/changelog.in
    ${CMAKE_CURRENT_SOURCE_DIR}/package/debian/changelog
)
	
message("-- Configured generator 'DEB (Debian)'")
set( XDISPATCH_CPACK ${XDISPATCH_CPACK} DEB )
