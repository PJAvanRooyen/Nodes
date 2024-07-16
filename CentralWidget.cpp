#include "CentralWidget.h"

#include <QApplication>
#include <QScreen>

namespace UI{

void
CentralWidget::onNodeAdd(QRectF rect, QString text, QString tooltip)
{
    // TODO: send event to add node.
    //(std::move(rect), std::move(text), std::move(tooltip));
}

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , mView(new CentralWidgetView(this))
{
    connect(mView.get(), &CentralWidgetView::nodeAdd, this, &CentralWidget::onNodeAdd);
}

QPointer<CentralWidgetView> CentralWidget::view()
{
    return mView.get();
}

void CentralWidget::addNode(QVariant id, QRectF rect, QString text, QString tooltip)
{
    mView->addNode(std::move(id), std::move(rect), std::move(text), std::move(tooltip));
}

}
