#include "CentralWidget.h"

#include <QApplication>
#include <QScreen>

namespace UI{

void
CentralWidget::onNodeAdd(QRectF rect, QString text, QString tooltip)
{
    // TODO: send event to add node.
    Q_EMIT nodeAdd(std::move(rect), std::move(text), std::move(tooltip));
}

void CentralWidget::onConnectionAdd(QVariant nodeId1, QVariant nodeId2, QString text, QString tooltip)
{
    // TODO: send event to add connection.
    Q_EMIT connectionAdd(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , mView(new CentralWidgetView(this))
{
    connect(mView.get(), &CentralWidgetView::nodeAdd, this, &CentralWidget::onNodeAdd);
    connect(mView.get(), &CentralWidgetView::connectionAdd, this, &CentralWidget::onConnectionAdd);
}

QPointer<CentralWidgetView> CentralWidget::view()
{
    return mView.get();
}

void CentralWidget::addNode(QVariant id, QRectF rect, QString text, QString tooltip)
{
    mView->addNode(std::move(id), std::move(rect), std::move(text), std::move(tooltip));
}

void CentralWidget::addConnection(QVariant nodeId1, QVariant nodeId2,  QString text, QString tooltip)
{
    mView->addConnection(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

}
