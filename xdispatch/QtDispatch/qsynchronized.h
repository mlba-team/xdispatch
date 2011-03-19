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



#ifndef QSYNCHRONIZED_H_
#define QSYNCHRONIZED_H_

#include "../dispatch.h"
#include "qdispatchglobal.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Dispatch)

class QDispatchSynclock;

/**
  Returns QDispatchSynclock used to protect the area
  identified by the given key.

  @see synchronized
  */
QDispatchSynclock qGetLockForKey(uint key);

/**
  Provides an easy locking mechanism used to
  ensure the threadsafety of a code area

  @see synchronized
  @see synchronize
  */
class QDispatchSynclock {

public:
    QDispatchSynclock();
    QDispatchSynclock(const QDispatchSynclock&);
    ~QDispatchSynclock();

    operator bool() const;

    void unlock();

private:
    dispatch_semaphore_t sem;
    bool lock;

    QDispatchSynclock& operator= (QDispatchSynclock);

    friend QDispatchSynclock qGetLockForKey(uint);
    QDispatchSynclock(dispatch_semaphore_t);
};

# define QT_DISPATCH_CONCAT(A,B) A ## B
# define QT_DISPATCH_LOCK_VAR(X) QT_DISPATCH_CONCAT(qt_synclock_, X)

/**
   Same as synchronize( lock )

   This macro is available even when no_keywords is specified using
   the .pro file's CONFIG variable.

   @see synchronize( lock )
   */
#  define Q_SYNCHRONIZE( lock ) for(QDispatchSynclock QT_DISPATCH_LOCK_VAR(__LINE__)(lock) ; QT_DISPATCH_LOCK_VAR(__LINE__) ; QT_DISPATCH_LOCK_VAR(__LINE__).unlock() )

/**
   Same as synchronized( lock )

   This macro is available even when no_keywords is specified using
   the .pro file's CONFIG variable.

   @see synchronized( lock )
   */
#  define Q_SYNCHRONIZED Q_SYNCHRONIZE(qGetLockForKey(__COUNTER__))

/**
  @macro synchronize( lock )

  This macro is used to implement Qt's synchronize keyword. The lock
  parameter is an object is a variable of type QDispatchSynclock.
   @codeline CONFIG += no_keywords

  @see Q_SYNCHRONIZE()

  Provides an easy way to make certain areas within your
  code threadsafe. To do so, simply declare a QDispatchSynclock object
  at some point in your code and pass it every time you need to make sure
  some area in your code can only be accessed exactly by one thread. Pass
  the same object to several synchronize() {} areas to ensure that a thread
  can only be within exactly one of those areas at a time.

  @code
    // somewhere declare your lock
    QDispatchSynclock age_lock;

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
            qDebug() << "Age is" << age;
        }
    }

    // another method that can safely be called from multiple threads
    void check_age(){

        // by re-using the age_lock we ensure that exactly one
        // thread can either be within the synchronize(){} block
        // in increase_age() or within this block
        synchronize(age_lock) {
            if(age > 18 && age < 19)
                qDebug() << "Hey wonderful";
        }
    }
  @endcode

  Tests have shown that marking your code this way is in no
  way slower than using a QMutex and calling QMutex.lock()
  or QMutex.unlock() every time.

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
  @macro synchronized

  This macro is used to implement Qt's synchronized.
  If you're worried about namespace pollution, you can disable this macro by adding the
  following line to your .pro file:
   @codeline CONFIG += no_keywords

  @see Q_SYNCHRONIZED( lock )


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
            qDebug() << "Age is" << age;
        }
    }

    // another method that can safely be called from multiple threads
    void check_age(){

        synchronized {
            // everything done in here is threadsafe
            // as well, but can be called at the same
            // time as the synchronized block of increase_age
            if(age > 18 && age < 19)
                qDebug() << "Hey wonderful";
        }
    }
  @endcode

  Tests have shown that marking your code this way is in no
  way slower than using a QMutex and calling QMutex.lock()
  or QMutex.unlock() every time. However please note that
  locking is a bit slower than when using synchronize(){} and
  managing the synclock by yourself. Additionally the internally
  used QDispatchSynclock objects will exist as long as the entire
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
#ifndef QT_NO_KEYWORDS
# ifndef synchronize
#  define synchronize( lock ) Q_SYNCHRONIZE(lock)
# endif

# ifndef synchronized
#  define synchronized Q_SYNCHRONIZED
# endif
#endif

QT_END_NAMESPACE
QT_END_HEADER

#endif /* SYNCHRONIZED_H_ */
