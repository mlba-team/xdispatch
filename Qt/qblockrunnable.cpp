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



#include "../include/QtDispatch/qblockrunnable.h"

#ifdef XDISPATCH_HAS_BLOCKS

QT_BEGIN_NAMESPACE

class QBlockRunnable::Private {
public:
	Private(dispatch_block_t b) : codeBlock(Block_copy(b)){}
	Private(Private* d) : codeBlock(Block_copy(d->codeBlock)){}
	~Private(){
		Block_release(codeBlock);
	}

	dispatch_block_t codeBlock;
};

QBlockRunnable::QBlockRunnable(const QBlockRunnable & b) : QRunnable(), d(new Private(b.d)){

}

QBlockRunnable::QBlockRunnable(dispatch_block_t b) : QRunnable(), d(new Private(b)){

}

QBlockRunnable::~QBlockRunnable(){
	delete d;
}

void QBlockRunnable::run(){
	d->codeBlock();
}

QT_END_NAMESPACE

#endif
