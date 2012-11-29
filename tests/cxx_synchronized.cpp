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


static const size_t ITERATIONS = 10000; // 10 000

static xdispatch::pointer<std::ostringstream>::shared current_string;
static std::ostringstream reference_string;
static xdispatch::synclock lock;
static pthread_mutex_t mutex;

inline void append_str(std::ostringstream& str) {
  str << "aa" << "bb" << std::string("cc") << "dd" << 34.5 << "ee" << 1 << "ff" << std::endl;
}

class MutexRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t index){
      int ret_code = pthread_mutex_lock(&mutex);
      MU_ASSERT_NULL( ret_code );

      append_str (*current_string);

      ret_code = pthread_mutex_unlock(&mutex);
      MU_ASSERT_NULL( ret_code );
    }

};

class SynchronizedRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t index){

      synchronized {
        append_str (*current_string);
      }

    }

};

class SynchronizeRun : public xdispatch::iteration_operation {

  public:
    void operator()(size_t){

      synchronize(lock) {
        append_str (*current_string);
      }
    }

};

class SynchronizedGroupRun : public xdispatch::operation {
  public:
    SynchronizedGroupRun(xdispatch::group& g)
      : _group(g){}

    void operator () (){
      synchronized {
        append_str (*current_string);
      }

      for(size_t i = 0; i < 8; ++i) {
        if( dispatch_atomic_inc(&finished_workers) < ITERATIONS )
          _group.async (new SynchronizedGroupRun(_group));
      }
    }

    static uintptr_t finished_workers;

    xdispatch::group& _group;
};

uintptr_t SynchronizedGroupRun::finished_workers = 0;

/**
  Test the 'synchronized' statements
  */
extern "C" void cxx_synchronized() {
  Stopwatch watch;
  double duration_synchronized = 0, duration_synchronize = 0, duration_mutex = 0;
  xdispatch::queue q = xdispatch::global_queue();

  MU_BEGIN_TEST(cxx_synchronized);

  pthread_mutex_init( &mutex, NULL );

  // create reference string
  for(size_t i = 0; i < ITERATIONS; ++i)
    append_str (reference_string);

  // reset
  current_string.reset( new std::ostringstream() );

  // test 1 1/2
  {
    MU_MESSAGE("Testing 'synchronized' keyword (apply)");
    xdispatch::iteration_operation* test11 = new SynchronizedRun;
    watch.reset ();
    xdispatch::global_queue().apply(test11, ITERATIONS);
    duration_synchronized = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", duration_synchronized);
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
    duration_synchronize = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", duration_synchronize);
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
    duration_mutex = watch.elapsed() / (double)ITERATIONS;
    MU_ASSERT_EQUAL_STR(current_string->str(), reference_string.str());
    MU_MESSAGE("%f us per Iteration", duration_mutex);
  }

  // our keyword may not be slower than a normal mutex
  MU_ASSERT_LESS_THAN_EQUAL_DOUBLE(duration_synchronized, 2*duration_mutex);
  MU_ASSERT_LESS_THAN_EQUAL_DOUBLE(duration_synchronize, 2*duration_mutex);

  MU_PASS("Yay");

  MU_END_TEST
}

