#
# rpm.cmake
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

# set( CPACK_RPM_PACKAGE_NAME "xdispatch")
set( CPACK_RPM_PACKAGE_VERSION "${XDISPATCH_VERSION}.${PACKAGE_COMPILER}")
set( CPACK_RPM_PACKAGE_LICENSE "${XDISPATCH_LICENSE}")
set( CPACK_RPM_PACKAGE_REQUIRES "glibc")
set( CPACK_RPM_PACKAGE_VENDOR "mlba-team.de" )
if(MZ_IS_64BIT)
    set( CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
else()
    set( CPACK_RPM_PACKAGE_ARCHITECTURE "x86")
endif()
set( CPACK_RPM_COMPONENT_INSTALL ON)

message("-- Configured generator 'RPM'")
set( XDISPATCH_CPACK ${XDISPATCH_CPACK} RPM )
