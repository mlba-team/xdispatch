
#include <QThread>

#include "../include/QtDispatch/qdispatchapplication.h"

#ifdef Q_OS_MAC

#ifndef QT_MAC_USE_COCOA
# error "QtDispatch is supporting Qt built on Cocoa only"
#endif

QT_BEGIN_NAMESPACE

QDispatchApplication* QDispatchApplication::self = NULL;

QDispatchApplication::QDispatchApplication(int& argc, char** argv) : QApplication(argc, argv), d(NULL) {
    self = this;
}

QDispatchApplication::~QDispatchApplication() {

}

int QDispatchApplication::exec() {

    if(QThread::currentThread() != self->thread()) {
        qWarning("%s::exec: Must be called from the main thread", self->metaObject()->className());
        return -1;
    }

    return QApplication::exec();

}

QT_END_NAMESPACE

#endif
