/*
' Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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



#include "internal.h"

dispatch_time_t
dispatch_time(dispatch_time_t when, int64_t delta){

	if(when != 0)
		return when + delta;

#ifdef _WIN32
	return GetTickCount()*NSEC_PER_MSEC + delta;
#else
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_sec*NSEC_PER_SEC + now.tv_usec*NSEC_PER_USEC + delta;
#endif
}

dispatch_time_t
dispatch_walltime(const struct timespec *when, int64_t delta){
	struct timeval now;

	gettimeofday(&now, NULL);

	if(when)
        return _dispatch_spec_to_time(when) + delta;
	else
		return now.tv_sec*NSEC_PER_SEC + now.tv_usec*NSEC_PER_USEC + delta;
}

struct timespec _dispatch_time_to_spec(dispatch_time_t t){
    struct timespec res;
    res.tv_sec = (long)t / NSEC_PER_SEC;
    res.tv_nsec = (long)t % NSEC_PER_SEC;
    return res;
}

dispatch_time_t _dispatch_spec_to_time(const struct timespec* s){
    return s->tv_sec*NSEC_PER_SEC + s->tv_nsec;
}
