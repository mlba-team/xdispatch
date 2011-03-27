#ifndef QRUNNABLE_OPERATIONS_H_
#define QRUNNABLE_OPERATIONS_H_

#include "../include/QtDispatch/qiterationrunnable.h"
#include "../include/xdispatch/dispatch"

class RunnableOperation : public xdispatch::operation {
public:
    RunnableOperation(QRunnable* r) : runnable(r) {
        Q_ASSERT(r);
    }
    ~RunnableOperation(){
        if(runnable->autoDelete())
            delete runnable;
    }

    void operator ()(){
        runnable->run();
    }

private:
    QRunnable* runnable;
};

class IterationRunnableOperation : public xdispatch::iteration_operation {
public:
    IterationRunnableOperation(QIterationRunnable* r) : runnable(r) {
        Q_ASSERT(r);
    }
    ~IterationRunnableOperation(){
        if(runnable->autoDelete())
            delete runnable;
    }

    void operator ()(size_t index){
        runnable->run(index);
    }

private:
    QIterationRunnable* runnable;
};

#endif /* QRUNNABLE_OPERATIONS_H_ */
