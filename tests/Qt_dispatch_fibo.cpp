/*
* Copyright (c) 2012 Simon Langevin
* Copyright (c) 2012-2013 MLBA-Team. All rights reserved.
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

#include <QtCore/QRunnable>
#include <QtCore/QTime>

#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

class OperationFibo : public QRunnable {

    public:
        OperationFibo(int fiboNumber)
            : _myFibo(fiboNumber) {}

        void run() {
            if( _myFibo < 2 ) {
                // nothing
            } else {
                QDispatchGroup leGroupe = QDispatchGroup();
                QRunnable* fiboA = new OperationFibo( _myFibo - 1 );
                QRunnable* fiboB = new OperationFibo( _myFibo - 2 );
                leGroupe.async( fiboA );
                leGroupe.async( fiboB );

                leGroupe.wait( QTime(0,0) );

                //Dumb calculation to take some CPU time.
                float j = 1.f/3.f;
                for(int i = 1 ; i < 30000;i++)
                    j = j*i;
            }
        }

    private:
        int _myFibo;
};

extern "C" void Qt_dispatch_fibo(){
    MU_BEGIN_TEST(Qt_dispatch_fibo);

    bool ticTac = true;
    QDispatchGroup theGroup;
    QRunnable* theOps[6];
    int cpt = 0;

    while(cpt < 3000) {
        for(int i=0; i<6; i++) {
            theOps[i] = new OperationFibo(8);
            theGroup.async( theOps[i] );
        }
        theGroup.wait( QTime(0,0) );

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
