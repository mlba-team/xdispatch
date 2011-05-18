/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>

#include "../core/src/shims/atomic.h"
#include "tests.h"

#ifdef XDISPATCH_HAS_BLOCKS

struct TestType : public xdispatch::sourcetype {

	static TestType* instance;
	TestType(){
		instance = this;
	}

	void signalFinished(const xdispatch::any& v){
		ready(v);
	}
};

TestType* TestType::instance = NULL;

extern "C" void cxx_dispatch_source() {
    MU_BEGIN_TEST(cxx_dispatch_source);

    xdispatch::source src(new TestType);
	src.set_queue(xdispatch::main_queue());
	src.handler(${
		MU_ASSERT_TRUE(xdispatch::source::data<std::string>() == "any working");
		MU_PASS("");
	});

	TestType::instance->signalFinished(std::string("any working"));
	xdispatch::exec();

    MU_END_TEST
}

#endif /* XDISPATCH_HAS_BLOCKS */