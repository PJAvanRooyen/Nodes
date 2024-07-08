#include "CentralWidget.h"

#include <QApplication>
#include <QScreen>

namespace UI{

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , mView(new CentralWidgetView(this))
{
}

QPointer<CentralWidgetView> CentralWidget::view()
{
    return mView.get();
}

void CentralWidget::addNode(){
    mView->addNode();
}

}
