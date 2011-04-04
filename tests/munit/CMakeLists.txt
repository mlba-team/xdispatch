# munit test framework

# define file groups
file(GLOB_RECURSE MUNIT *.c *.h)

# grouping
source_group(munit FILES ${MUNIT})

# add new target
add_library(munit STATIC ${MUNIT})

mz_target_props(munit)
