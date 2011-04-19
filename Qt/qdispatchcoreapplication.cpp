
#include <QThread>
#include <QDebug>

#include "../include/QtDispatch/qdispatchcoreapplication.h"

QT_BEGIN_NAMESPACE


QDispatchCoreApplication::QDispatchCoreApplication(int& argc, char** argv) : QDispatchApplication(argc, argv) {
    qCritical("QDispatchCoreApplication is deprecated. Please use QDispatchApplication instead");
}


int QDispatchCoreApplication::exec() {

    return QDispatchApplication::exec();

}

QT_END_NAMESPACE
