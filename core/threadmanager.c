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



#define _CRT_SECURE_NO_WARNINGS

#ifdef Darwin
#	include <CoreServices/CoreServices.h>
#endif

#include "queue_internal.h"

/* Once-only initialisation of the key */
static pthread_once_t _dispatch_buffer_key_once = PTHREAD_ONCE_INIT;
/* Key for the thread-specific buffer */
static pthread_key_t _dispatch_buffer_key;

// run once to allocate the thread keys
static void _buffer_key_alloc(void){
	pthread_key_create(&_dispatch_buffer_key, NULL);
}

void _set_thread_queue(dispatch_queue_t q){
	pthread_setspecific(_dispatch_buffer_key, q);
}

dispatch_queue_t _get_thread_queue(){
	return (dispatch_queue_t)pthread_getspecific(_dispatch_buffer_key);
}

void* _thread(void* q){
	_thread_t me = (_thread_t)q;

	pthread_once(&_dispatch_buffer_key_once, _buffer_key_alloc);

        while(!me->loop){
#ifdef WIN32
            Sleep(1);
#else
            usleep(10);
#endif
        }

	_evt_run(me->loop, 0);

	return NULL;
}

void* _timer_thread(void* q){
	_evt_loop_t loop = (_evt_loop_t)q;

	_evt_run(loop,0);

	return NULL;
}

void _spawn_thread(_thread_t t){

	int rc = pthread_create(&t->id, NULL, _thread, t);
#ifdef DEBUG
	if(rc){
#		ifdef WIN32
		char error[100];
		strerror_s(error,99,rc);
		printf("ERROR: Could not spawn needed thread: %s\n",error);
#		else
		printf("ERROR: Could not spawn needed thread: %s\n",strerror(rc));
#		endif
	}
#else
	rc = 0;
#endif
}

void _spawn_timer_thread(void* q){
	pthread_t newThread;
	int rc = pthread_create(&newThread, NULL, _timer_thread, q);
#ifdef DEBUG
	if(rc){
#		ifdef WIN32
		char error[100];
		strerror_s(error,99,rc);
		printf("ERROR: Could not spawn needed thread: %s\n",error);
#		else
		printf("ERROR: Could not spawn needed thread: %s\n",strerror(rc));
#		endif
	}
#else
	rc = 0;
#endif
}

void _check_threads(_threadpool_t p){
	// TODO Make this more sophisticated
	if(_tp_count(p) < MAX_THREADS) {
		//printf("Spawn\n");
		_tp_add(p);
		if(_tp_count(p)<2){
			//printf("Spawn\n");
			_tp_add(p);
		}
	}
}

unsigned int _ideal_thread_count(){

	static unsigned int ct = 0;
	if ( ct == 0) {
#ifdef WIN32
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		ct = sysinfo.dwNumberOfProcessors;
#else
#	ifdef Darwin
		ct = MPProcessorsScheduled();
#	else
		ct = (int)sysconf(_SC_NPROCESSORS_ONLN);
#	endif
#endif
		if (ct < 3) ct = 3;
	}

	return ct;

}

#ifdef WIN32

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// Definition of a gettimeofday function

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	// Define a structure to receive the current Windows filetime
	FILETIME ft;

	// Initialize the present time to 0 and the timezone to UTC
	unsigned __int64 tmpres = 0;
	static int tzflag = 0;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		// The GetSystemTimeAsFileTime returns the number of 100 nanosecond 
		// intervals since Jan 1, 1601 in a structure. Copy the high bits to 
		// the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		// Convert to microseconds by dividing by 10
		tmpres /= 10;

		// The Unix epoch starts on Jan 1 1970.  Need to subtract the difference 
		// in seconds from Jan 1 1601.
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

		// Finally change microseconds to seconds and place in the seconds value. 
		// The modulus picks up the microseconds.
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}

		// Adjust for the timezone west of Greenwich
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

#endif

