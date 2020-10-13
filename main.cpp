#include "labelprint.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LabelPrint w;
    w.show();
    return a.exec();
}
