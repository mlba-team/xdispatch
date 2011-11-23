
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


#ifndef XDISPATCH_TIMER_H_
#define XDISPATCH_TIMER_H_

#ifndef __XDISPATCH_INDIRECT__
#error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

__XDISPATCH_BEGIN_NAMESPACE

/**
  Provides a timer executing a block or an operation
  on a specific queue when a timeout occurs.

  @remarks Although you can get the dispatch_source_t associated
           with this timer by calling native(), please be aware that as soon as
           you do any changes to this source not by using the timer object
           it is not garanteed, that the timer object will still work as expected.
  */
class XDISPATCH_EXPORT timer : public object {

    public:
        /**
        Constructs a new periodic timer. The timer will be stopped, call start() to execute it

        @param interval The interval in nanoseconds at which the timer will fire after the timeout occured.
        @param target The queue to execute the timer on, defaults to the global_queue
        @param starting The time at which the timer will fire for the first time
        */
        timer(uint64_t interval, const xdispatch::queue& target = global_queue(), dispatch_time_t starting = DISPATCH_TIME_NOW);
        timer(const timer&);
        timer(dispatch_source_t);
        ~timer();

        /**
          Use this to set the interval in nanoseconds.
          When called for the first time on a single-shot timer, the timer
          will be converted to a periodic timer with the given interval.
          */
        void interval(uint64_t);
        /**
          Use this to set the latency in nanoseconds which the timer
          might be early or late. When not set the latency will be kept
          as low as possible.
          */
        void latency(uint64_t);
        /**
          Will start the timer.
          @remarks A new created timer will be stopped and needs to me started first.
                   Once started, ensure balanced calls between start() and stop().
              @see resume();
        */
        void start();
            /**
              @see start();
              */
            void resume();
        /**
          Will stop the timer.
          @remarks A new created timer will be stopped and needs to me started first.
                   Once started, ensure balanced calls between start() and stop().
              @see suspend()
        */
        void stop();
        /**
          @see stop();
          */
        void suspend();
        /**
          Sets the operation that will be executed every time the timer fires.
          As in queues, the timer takes possesion of the operation and will handle deletion.
          To change this behaviour, set the auto_delete flag of the operation.
          @see operation::auto_delete();
          */
        void handler(xdispatch::operation*);
#ifdef XDISPATCH_HAS_BLOCKS
        /**
          Sets the block that will be executed every time the timer fires
          */
        inline void handler(dispatch_block_t b) {
            handler( new block_operation(b) );
        }
#endif
        /**
          Sets the queue the handler will be executed on
          */
        void target_queue(const xdispatch::queue&);
        /**
          @returns the queue the handler will be executed on
          */
        xdispatch::queue target_queue();
        /**
          @returns The native dispatch_source_t associated with the timer
          */
        dispatch_object_t native() const;
        /**
          @returns The timer responsible for the handler to be executed or
                   NULL if the handler was not executed because of a timer firing
          */
        static timer* current();
        /**
          Creates a single shot timer executing the given operation on the given
          queue at the given time. This is quite similar to using xdispatch::queue::after()
          */
        static void single_shot(dispatch_time_t, const xdispatch::queue&, xdispatch::operation*);
        /**
          Creates a single shot timer executing the given operation on the given
          queue at the given time. This is quite similar to using xdispatch::queue::after()
          */
        static void single_shot(struct tm*, const xdispatch::queue&, xdispatch::operation*);
#ifdef XDISPATCH_HAS_BLOCKS
        /**
          Creates a single shot timer executing the given block on the given
          queue at the given time. This is quite similar to using xdispatch::queue::after()
          */
        static void single_shot(dispatch_time_t t, const xdispatch::queue& q, dispatch_block_t b) {
            single_shot( t, q, new block_operation(b) );
        }
        /**
          Creates a single shot timer executing the given block on the given
          queue at the given time. This is quite similar to using xdispatch::queue::after()
          */
        static void single_shot(struct tm* t, const xdispatch::queue& q, dispatch_block_t b) {
            single_shot( t, q, new block_operation(b) );
        }
#endif

        timer& operator =(const timer&);

    private:
        class data;
        data* d;

};

__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_TIMER_H_ */
