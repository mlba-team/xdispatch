/*
 * Copyright (c) 2008-2009 Apple Inc. All rights reserved.
 * Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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


#include <xdispatch/dispatch>

#include <string>
#include <sstream>

#include "cxx_tests.h"
#include "stopwatch.h"

#include "../core/platform/atomic.h"

static const size_t ITERATIONS = 10000; // 10 000

static xdispatch::pointer<std::ostringstream>::shared current_string;
static std::ostringstream reference_string;
static xdispatch::synclock lock;
static pthread_mutex_t mutex;

class MutexRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t index){

      MU_ASSERT_NULL( pthread_mutex_lock(&mutex) );
      (*current_string) << "a" << "b" << "c" << "d" << "e" << "f" << std::endl;
      MU_ASSERT_NULL( pthread_mutex_unlock(&mutex) );

    }

};

class SynchronizedRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t index){

      synchronized {
        (*current_string) << "a" << "b" << "c" << "d" << "e" << "f" << std::endl;
      }

    }

};

class SynchronizeRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t){

      synchronize(lock) {
        (*current_string) << "a" << "b" << "c" << "d" << "e" << "f" << std::endl;
      }
    }

};

class SynchronizedGroupRun : public xdispatch::operation {
  public:
    SynchronizedGroupRun(xdispatch::group& g)
      : _group(g){}

    void operator () (){
      synchronized {
        (*current_string) << "a" << "b" << "c" << "d" << "e" << "f" << std::endl;
      }

      for(size_t i = 0; i < 8; ++i) {
        if( dispatch_atomic_inc(&finished_workers) < ITERATIONS )
          _group.async (new SynchronizedGroupRun(_group));
      }
    }

    static intptr_t finished_workers;

    xdispatch::group& _group;
};

intptr_t SynchronizedGroupRun::finished_workers = 0;

/**
  Test the 'synchronized' statements
  */
extern "C" void cxx_synchronized() {
  Stopwatch watch;
  uint64_t dur_synchronized = 0, dur_synchronize = 0, dur_mutex = 0;
  xdispatch::queue q = xdispatch::global_queue();

  MU_BEGIN_TEST(cxx_synchronized);

  pthread_mutex_init( &mutex, NULL );

  // create reference string
  for(size_t i = 0; i < ITERATIONS; ++i)
    reference_string << "a" << "b" << "c" << "d" << "e" << "f" << std::endl;

  // reset
  current_string.reset( new std::ostringstream() );

  // test 1 1/2
  {
    MU_MESSAGE("Testing 'synchronized' keyword (apply)");
    xdispatch::iteration_operation* test11 = new SynchronizedRun;
    watch.reset ();
    xdispatch::global_queue().apply(test11, ITERATIONS);
    dur_synchronized = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", dur_synchronized);
  }


  // reset
  current_string.reset( new std::ostringstream() );


  // test 1 2/2
  {
    MU_MESSAGE("Testing 'synchronized' keyword (for / group)");
    SynchronizedGroupRun::finished_workers = 0;
    xdispatch::group group1;
    xdispatch::operation* test12 = new SynchronizedGroupRun(group1);
    q.suspend();
    group1.async(test12, q);
    watch.reset();
    q.resume();
    group1.wait();
    MU_MESSAGE("%f us per Iteration", watch.elapsed() / (double)ITERATIONS);
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
  }

  // reset
  current_string.reset( new std::ostringstream() );


  // test 2 1/2
  {
    MU_MESSAGE("Testing 'synchronize' keyword (apply)");
    xdispatch::iteration_operation* test21 = new SynchronizeRun;
    watch.reset();
    xdispatch::global_queue().apply(test21, ITERATIONS);
    dur_synchronize = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", dur_synchronize);
  }


#if 0
  // reset
  current_string.reset( new std::ostringstream() );


  // test 2 2/2
  {
    MU_MESSAGE("Testing 'synchronize' keyword (for / group)");
    xdispatch::operation* test22 = new PartialRun1;
    xdispatch::group group2;
    q.suspend();
    for(int i = 0; i < ITERATIONS; i++){
      group2.async(test22, q);
    }
    watch.reset();
    q.resume();
    group2.wait();
    MU_MESSAGE("%f us per Iteration", watch.elapsed() / (double)ITERATIONS);
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
  }
#endif


  // reset
  current_string.reset( new std::ostringstream() );


  // test 3
  {
    MU_MESSAGE("Testing with normal mutex (apply)");
    xdispatch::iteration_operation* mutexRun = new MutexRun;
    watch.reset();
    xdispatch::global_queue().apply(mutexRun, ITERATIONS);
    dur_mutex = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", dur_mutex);
  }

  // our keyword may not be slower than a normal mutex
  MU_ASSERT_LESS_THAN_EQUAL_DOUBLE(dur_synchronized, dur_mutex);
  MU_ASSERT_LESS_THAN_EQUAL_DOUBLE(dur_synchronize, dur_mutex);

  MU_PASS("Yay");

  MU_END_TEST
}

