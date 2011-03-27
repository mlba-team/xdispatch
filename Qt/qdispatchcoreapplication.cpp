
#include <QThread>

#include "../include/QtDispatch/qdispatchcoreapplication.h"
#include "qdispatchapplicationprivate.h"

QT_BEGIN_NAMESPACE

QDispatchCoreApplication* QDispatchCoreApplication::self = NULL;

QDispatchCoreApplication::QDispatchCoreApplication(int& argc, char** argv) : QCoreApplication(argc, argv), d(new QDispatchApplicationPrivate) {
    Q_CHECK_PTR(d);
    self = this;
    d->self = this;
}

QDispatchCoreApplication::~QDispatchCoreApplication() {
    delete d;
}

int QDispatchCoreApplication::exec() {

    if(QThread::currentThread() != self->thread()) {
        qWarning("%s::exec: Must be called from the main thread", self->metaObject()->className());
        return -1;
    }

    return self->d->exec();

}

QT_END_NAMESPACE
