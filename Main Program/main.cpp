#include "Jamming.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Jamming w;
    w.show();
    return a.exec();
}
