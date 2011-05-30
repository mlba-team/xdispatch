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


#include "../include/QtDispatch/qiterationblockrunnable.h"

#ifdef XDISPATCH_HAS_BLOCKS

QT_BEGIN_NAMESPACE

class QIterationBlockRunnable::Private {
public:
    Private(dispatch_iteration_block_t b) : codeBlock(XDISPATCH_BLOCK_PERSIST(b)){}
    Private(Private* d) : codeBlock(XDISPATCH_BLOCK_COPY(d->codeBlock)){}
	~Private(){
        XDISPATCH_BLOCK_DELETE(codeBlock);
	}

    dispatch_iteration_block_store codeBlock;
};

QIterationBlockRunnable::QIterationBlockRunnable(const QIterationBlockRunnable & b) : QIterationRunnable(b), d(new Private(b.d)){

}

QIterationBlockRunnable::QIterationBlockRunnable(dispatch_iteration_block_t b, size_t index) : QIterationRunnable(index), d(new Private(b)){

}

QIterationBlockRunnable::QIterationBlockRunnable(dispatch_iteration_block_t b) : QIterationRunnable(), d(new Private(b)){

}

QIterationBlockRunnable::~QIterationBlockRunnable(){
	delete d;
}

void QIterationBlockRunnable::run(size_t index){
    XDISPATCH_BLOCK_EXEC(d->codeBlock)(index);
}

QT_END_NAMESPACE

#endif
