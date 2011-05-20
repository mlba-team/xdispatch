#ifndef DISPATCH_COMPILER_H_
#define DISPATCH_COMPILER_H_

#ifndef DISPATCH_NOINLINE
# if __GNUC__
#  define DISPATCH_NOINLINE	__attribute__((noinline))
# else
#  define DISPATCH_NOINLINE
# endif
#endif

#ifndef DISPATCH_INLINE
# if __GNUC__
#  define DISPATCH_INLINE __attribute__((always_inline))
# else
#  define DISPATCH_INLINE
# endif
#endif

#ifndef DISPATCH_UNUSED
# if __GNUC__
#  define DISPATCH_UNUSED __attribute__((unused))
# else
#  define DISPATCH_UNUSED
# endif
#endif

#ifndef DISPATCH_NORETURN
# if __GNUC__
#  define DISPATCH_NORETURN __attribute__((__noreturn__))
# else
#  define DISPATCH_NORETURN
# endif
#endif

#endif /* DISPATCH_COMPILER_H_ */