#
# pkg.cmake
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

set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf")
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.rtf")
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.rtf")
set( CPACK_PACKAGING_INSTALL_PREFIX "/")
set( CPACK_OSX_PACKAGE_VERSION "10.6")


message("-- Configured generator 'PackageMaker'")
set( XDISPATCH_CPACK ${XDISPATCH_CPACK} PackageMaker )
