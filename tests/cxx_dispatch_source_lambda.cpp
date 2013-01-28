/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
* Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
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

#include "tests.h"


struct TestTypeLambda : public xdispatch::sourcetype {

    static TestTypeLambda* instance;
    TestTypeLambda() : xdispatch::sourcetype() {
		instance = this;
	}

	void signalFinished(const xdispatch::any& v){
		ready(v);
	}
};

TestTypeLambda* TestTypeLambda::instance = NULL;

extern "C" void cxx_dispatch_source_lambda() {
    MU_BEGIN_TEST(cxx_dispatch_source_lambda);

    xdispatch::source src(new TestTypeLambda);
    MU_ASSERT_NULL( src.native() );
    src.target_queue(xdispatch::main_queue());
    src.handler([=]{
		MU_ASSERT_TRUE(xdispatch::source::data<std::string>() == "any working");
		MU_PASS("");
	});

    src.resume();

    TestTypeLambda::instance->signalFinished(std::string("any working"));
	xdispatch::exec();

    MU_END_TEST
}

