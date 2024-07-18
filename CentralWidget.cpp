#include "CentralWidget.h"

#include <QApplication>
#include <QScreen>
#include <QUuid>

namespace UI{

void
CentralWidget::onNodeAdd(QRectF rect, QString text, QString tooltip)
{
    // TODO: send event to add node instead.
    Q_EMIT nodeAdd(std::move(rect), std::move(text), std::move(tooltip));
}

void CentralWidget::onNodeRemove(QUuid nodeId)
{
    // TODO: send event to remove node instead.
    Q_EMIT nodeRemove(std::move(nodeId));
}

void CentralWidget::onConnectionAdd(QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip)
{
    // TODO: send event to add connection instead.
    Q_EMIT connectionAdd(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
    , mView(new CentralWidgetView(this))
{
    connect(mView.get(), &CentralWidgetView::nodeAdd, this, &CentralWidget::onNodeAdd);
    connect(mView.get(), &CentralWidgetView::nodeRemove, this, &CentralWidget::onNodeRemove);
    connect(mView.get(), &CentralWidgetView::connectionAdd, this, &CentralWidget::onConnectionAdd);
}

QPointer<CentralWidgetView> CentralWidget::view()
{
    return mView.get();
}

void CentralWidget::reset()
{
    mView->reset();
}

void CentralWidget::addNode(QUuid id, QRectF rect, QString text, QString tooltip)
{
    mView->addNode(std::move(id), std::move(rect), std::move(text), std::move(tooltip));
}

void CentralWidget::removeNode(const QUuid &id)
{
    mView->removeNode(std::move(id));
}

void CentralWidget::addConnection(QUuid nodeId1, QUuid nodeId2,  QString text, QString tooltip)
{
    mView->addConnection(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

}
