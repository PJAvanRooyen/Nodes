#include "mainwindow.h"
#include "CentralWidget.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UI::MainWindow w;
    UI::CentralWidget widget;
    w.setCentralWidget(&widget);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.setGeometry(screenGeometry);
    widget.view()->setGeometry(screenGeometry);
    w.show();
    return a.exec();
}
