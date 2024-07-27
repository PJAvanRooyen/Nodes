#include "CentralWidget.h"
#include "Shared/EventSystem/Communicator.h"
#include "Shared/EventSystem/Events/EvNode.h"
#include "Shared/EventSystem/Events/EvConnection.h"
#include "Shared/EventSystem/Events/EvInteraction.h"

#include <QApplication>
#include <QScreen>
#include <QUuid>

namespace UI{

void
CentralWidget::onNodeAdd(QRectF rect, QString text, QString tooltip)
{
    auto req = Shared::EventSystem::EvNodeAddReq(std::move(rect), std::move(text), std::move(tooltip));
    Shared::EventSystem::Communicator::instance().postEvent(req);
}

void CentralWidget::onNodeRemove(QUuid nodeId)
{
    auto req = Shared::EventSystem::EvNodeRemoveReq(nodeId);
    Shared::EventSystem::Communicator::instance().postEvent(req);
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
    auto &communicator = Shared::EventSystem::Communicator::instance();
    communicator.connect(this,
                         {Shared::EventSystem::EvNodeAddResp::staticType(),
                          Shared::EventSystem::EvNodeRemoveResp::staticType(),
                          Shared::EventSystem::EvConnectionAddResp::staticType(),
                          });

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

void CentralWidget::customEvent(QEvent *event)
{
    Q_ASSERT(event);
    const auto eventType = event->type();
    if (eventType == Shared::EventSystem::EvNodeAddResp::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvNodeAddResp&>(*event);
        addNode(ev.nodeId, ev.rect, ev.text, ev.tooltip);
    } else if (eventType == Shared::EventSystem::EvNodeRemoveResp::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvNodeRemoveResp&>(*event);
        removeNode(ev.nodeId);
    } else if (eventType == Shared::EventSystem::EvConnectionAddResp::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvConnectionAddResp&>(*event);
        addConnection(ev.node1Id, ev.node2Id);
    }
}

}
