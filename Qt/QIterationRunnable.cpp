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



#include "../xdispatch/QtDispatch/qiterationrunnable.h"


class QIterationRunnable::Private {
public:
        Private(int index) : index(index) {}
        Private(Private* d) : index(d->index) {}

        int index;
};

QIterationRunnable::QIterationRunnable(const QIterationRunnable & b) : QRunnable(), d(new Private(b.d)){

}

QIterationRunnable::QIterationRunnable() : QRunnable(), d(new Private(0)){

}

QIterationRunnable::QIterationRunnable(int index) : QRunnable(), d(new Private(index)){

}

QIterationRunnable::~QIterationRunnable(){
	delete d;
}

void QIterationRunnable::run(){
    run(d->index);
}
