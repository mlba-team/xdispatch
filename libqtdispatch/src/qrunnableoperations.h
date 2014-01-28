/*
* qrunnableoperations.h
* 
* Copyright (c) 2011-2014 MLBA-Team
* All rights reserved.
* 
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/

#ifndef QRUNNABLE_OPERATIONS_H_
#define QRUNNABLE_OPERATIONS_H_

#include "../include/QtDispatch/qiterationrunnable.h"
#include "../include/xdispatch/dispatch"

class RunnableOperation
    : public xdispatch::operation
{
public:
    RunnableOperation (
        QRunnable *r
    )
        : runnable( r )
    {
        Q_ASSERT( r );
    }

    ~RunnableOperation ()
    {
        if( runnable->autoDelete() )
            delete runnable;
    }

    void operator () ()
    {
        runnable->run();
    }

private:
    RunnableOperation (
        const RunnableOperation &
    ) { }


    QRunnable *runnable;
};


class IterationRunnableOperation
    : public xdispatch::iteration_operation
{
public:
    IterationRunnableOperation (
        QIterationRunnable *r
    )
        : runnable( r )
    {
        Q_ASSERT( r );
    }

    ~IterationRunnableOperation ()
    {
        if( runnable->autoDelete() )
            delete runnable;
    }

    void operator () (
        size_t index
    )
    {
        runnable->run( index );
    }

private:
    IterationRunnableOperation (
        const IterationRunnableOperation &
    ) { }


    QIterationRunnable *runnable;
};


#endif /* QRUNNABLE_OPERATIONS_H_ */