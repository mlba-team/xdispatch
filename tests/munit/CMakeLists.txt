# munit test framework

# define file groups
file(GLOB_RECURSE MUNIT *.c *.h)

# grouping
source_group(munit FILES ${MUNIT})

# add new target
add_library(munit STATIC ${MUNIT})

# link this target with all needed libraries
if(MZ_IS_VS)
 set(LIB pthreads-w32)
endif()

# link this target with all needed libraries
target_link_libraries(munit ${LIB})
mz_target_props(munit)
