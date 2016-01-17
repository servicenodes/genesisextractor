#include "genesisextractor.h"
#include <QApplication>

//#include <QFileDialog>
//#include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GenesisExtractor w;
    w.show();

    return a.exec();
}
