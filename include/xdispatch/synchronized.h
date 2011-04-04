/*
* Copyright (c) 2011 MLBA. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @MLBA_OPEN_LICENSE_HEADER_END@
*/



#ifndef SYNCHRONIZED_H_
#define SYNCHRONIZED_H_

#ifndef __XDISPATCH_INDIRECT__
#error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

#include "dispatch.h"

__XDISPATCH_BEGIN_NAMESPACE

/**
  @file
  */

class XDISPATCH_EXPORT synclock;

/**
  Returns the synclock used to protect the area
  identified by the given key.

  @see synchronized
  */
XDISPATCH_EXPORT synclock get_lock_for_key(unsigned int key);

/**
  Provides an easy locking mechanism used to
  ensure the threadsafety of a code area

  @see synchronized
  @see synchronize
  */
class XDISPATCH_EXPORT synclock {

public:
    synclock();
    synclock(const synclock&);
    ~synclock();

    operator bool() const;

    void unlock();

private:
    dispatch_semaphore_t sem;
    bool lock;

    synclock& operator= (synclock);

    friend synclock get_lock_for_key(unsigned int);
    synclock(dispatch_semaphore_t);
};

# define XDISPATCH_CONCAT(A,B) A ## B
# define XDISPATCH_LOCK_VAR(X) XDISPATCH_CONCAT(xd_synclock_, X)

/**
   Same as synchronize( lock )

   This macro is available even when XDISPATCH_NO_KEYWORDS was
   specified.

   @see synchronize( lock )
   */
#  define XDISPATCH_SYNCHRONIZE( lock ) for(xdispatch::synclock XDISPATCH_LOCK_VAR(__LINE__)(lock) ; XDISPATCH_LOCK_VAR(__LINE__) ; XDISPATCH_LOCK_VAR(__LINE__).unlock() )

/**
   Same as synchronized( lock )

   This macro is available even when XDISPATCH_NO_KEYWORDS was
   specified.

   @see synchronized( lock )
   */
#  define XDISPATCH_SYNCHRONIZED XDISPATCH_SYNCHRONIZE(xdispatch::get_lock_for_key(__COUNTER__))

/**
  @def synchronize( lock )

  This macro is used to implement Qt's synchronize keyword. The lock
  parameter is an object is a variable of type synclock.

  If you're worried about namespace pollution, you can disable this macro by defining
  the following macro before including xdispatch/dispatch.h:

  @code
   #define XDISPATCH_NO_KEYWORDS
   #include <xdispatch/dispatch.h>
  @endcode

  @see XDISPATCH_SYNCHRONIZE()

  Provides an easy way to make certain areas within your
  code threadsafe. To do so, simply declare a synclock object
  at some point in your code and pass it every time you need to make sure
  some area in your code can only be accessed exactly by one thread. Pass
  the same object to several synchronize() {} areas to ensure that a thread
  can only be within exactly one of those areas at a time.

  @code
    // somewhere declare your lock
    synclock age_lock;

    // the variable that is protected by this lock
    int age = 0;

    // a method that can be called from multiple threads safely
    void increase_age(){

        // this area is completely thread safe. We could
        // use the QDispatchSyncLock object within another
        // area marked the same way and only one of them
        // can be accessed at the same time
        synchronize(age_lock) {
            // everything done in here is threadsafe
            age++;
            std::cout << "Age is" << age;
        }
    }

    // another method that can safely be called from multiple threads
    void check_age(){

        // by re-using the age_lock we ensure that exactly one
        // thread can either be within the synchronize(){} block
        // in increase_age() or within this block
        synchronize(age_lock) {
            if(age > 18 && age < 19)
                std::cout << "Hey wonderful";
        }
    }
  @endcode

  Tests have shown that marking your code this way is in no
  way slower than using a mutex and calling lock()
  or unlock() every time.

  The advantage of using the synchronized keyword over a classical
  mutex is that even if you throw an exception within your code or
  have several ways to return from the function it is always ensured
  that the synchronized area will be unlocked as soon as your program
  leaves the piece of code within the brackets.

  In contrast to synchronized you need to
  manage the lifetime of the lock object yourself.

  @see synchronized
  */

/**
  @def synchronized

  This macro is used to implement Qt's synchronized keyword.

  If you're worried about namespace pollution, you can disable this macro by defining
  the following macro before including xdispatch/dispatch.h:

  @code
   #define XDISPATCH_NO_KEYWORDS
   #include <xdispatch/dispatch.h>
  @endcode

  @see XDISPATCH_SYNCHRONIZED( lock )


  Provides an easy way to mark an area within your
  code threadsafe. To do so, simply enclose the used
  area with a 'synchronized{ }' statement.

  In constrast to synchronize() you can mark individual
  areas threadsafe only.

  @code
    // the variable that is protected by this lock
    int age = 0;

    // a method that can be called from multiple threads safely
    void increase_age(){

        // this area is completely thread safe
        synchronized {
            // everything done in here is threadsafe
            age++;
            std::cout << "Age is" << age;
        }
    }

    // another method that can safely be called from multiple threads
    void check_age(){

        synchronized {
            // everything done in here is threadsafe
            // as well, but can be called at the same
            // time as the synchronized block of increase_age
            if(age > 18 && age < 19)
                std::cout << "Hey wonderful";
        }
    }
  @endcode

  Tests have shown that marking your code this way is in no
  way slower than using a mutex and calling lock()
  or unlock() every time. However please note that
  locking is a bit slower than when using synchronize(){} and
  managing the synclock by yourself. Additionally the internally
  used synclock objects will exist as long as the entire
  program is executed and cannot be deleted.

  The advantage of using the synchronized keyword over a classical
  mutex is that even if you throw an exception within your code or
  have several ways to return from the function it is always ensured
  that the synchronized area will be unlocked as soon as your program
  leaves the piece of code within the brackets.

  In contrast to synchronized you need to
  manage the lifetime of the lock object yourself.

  @see synchronize
  */
#ifndef XDISPATCH_NO_KEYWORDS
# ifndef synchronize
#  define synchronize( lock ) XDISPATCH_SYNCHRONIZE(lock)
# endif

# ifndef synchronized
#  define synchronized XDISPATCH_SYNCHRONIZED
# endif
#endif

__XDISPATCH_END_NAMESPACE

#endif /* SYNCHRONIZED_H_ */
