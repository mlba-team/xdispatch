#
# CMakeLists.txt
# 
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

# munit test framework

# define file groups
file(GLOB_RECURSE MUNIT *.c *.h)

# grouping
source_group(munit FILES ${MUNIT})

# add new target
add_library(munit STATIC ${MUNIT})

# link this target with all needed libraries
target_link_libraries(munit)
mz_target_props(munit)
mz_auto_format_c(munit)
