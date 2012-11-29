/*
* Copyright (c) 2012 Simon Langevin
* Copyright (c) 2012 MLBA-Team. All rights reserved.
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
#include "cxx_tests.h"

class operation_fibo : public xdispatch::operation {

    public:
        operation_fibo(int fiboNumber)
            : _myFibo(fiboNumber) {}

        void operator()() {
            if( _myFibo < 2 ) {
                // nothing
            } else {
                xdispatch::group leGroupe;
                xdispatch::operation* fiboA = new operation_fibo( _myFibo - 1 );
                xdispatch::operation* fiboB = new operation_fibo( _myFibo - 2 );
                leGroupe.async( fiboA );
                leGroupe.async( fiboB );

                leGroupe.wait( DISPATCH_TIME_FOREVER );

                //Dumb calculation to take some CPU time.
                float j = 1.f/3.f;
                for(int i = 1 ; i < 30000;i++)
                    j = j*i;
            }
        }

    private:
        int _myFibo;
};

extern "C" void cxx_dispatch_fibo(){
    MU_BEGIN_TEST(cxx_dispatch_fibo);

    bool ticTac = true;
    xdispatch::group theGroup;
    xdispatch::operation* theOps[6];
    int cpt = 0;
    while(cpt < 3000) {
        for(int i=0; i<6; i++) {
            theOps[i] = new operation_fibo( 8 );
            theGroup.async( theOps[i] );
        }
        theGroup.wait( DISPATCH_TIME_FOREVER );

#if 0
        if(ticTac)
            MU_MESSAGE("Tic");
        else
            MU_MESSAGE("Tac");
#endif

        ticTac = !ticTac;
        cpt++;
    }

    MU_PASS("");
    MU_END_TEST;
}
