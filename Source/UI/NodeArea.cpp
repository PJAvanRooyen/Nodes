#include "NodeArea.h"
#include "EventSystem/Communicator.h"
#include "EventSystem/Events/EvNode.h"
#include "EventSystem/Events/EvConnection.h"
#include "EventSystem/Events/EvInteraction.h"

#include <QApplication>
#include <QScreen>
#include <QUuid>

namespace UI{

void
NodeArea::onNodeAdd(QRectF rect, QString text, QString tooltip)
{
    auto req = Shared::EventSystem::EvNodeAddReq(std::move(rect), std::move(text), std::move(tooltip));
    Shared::EventSystem::Communicator::instance().postEvent(req);
}

void NodeArea::onNodeRemove(QUuid nodeId)
{
    auto req = Shared::EventSystem::EvNodeRemoveReq(nodeId);
    Shared::EventSystem::Communicator::instance().postEvent(req);
}

void NodeArea::onConnectionAdd(QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip)
{
    // TODO: send event to add connection instead.
    Q_EMIT connectionAdd(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

NodeArea::NodeArea(QWidget *parent)
    : QWidget(parent)
    , mView(new NodeAreaView(this))
{
    auto &communicator = Shared::EventSystem::Communicator::instance();
    communicator.connect(this,
                         {Shared::EventSystem::EvNodeAddResp::staticType(),
                          Shared::EventSystem::EvNodeRemoveResp::staticType(),
                          Shared::EventSystem::EvConnectionAddResp::staticType(),
                          Shared::EventSystem::EvInteractionsSolveResp::staticType(),
                          });

    connect(mView.get(), &NodeAreaView::nodeAdd, this, &NodeArea::onNodeAdd);
    connect(mView.get(), &NodeAreaView::nodeRemove, this, &NodeArea::onNodeRemove);
    connect(mView.get(), &NodeAreaView::connectionAdd, this, &NodeArea::onConnectionAdd);
}

QPointer<NodeAreaView> NodeArea::view()
{
    return mView.get();
}

void NodeArea::reset()
{
    mView->reset();
}

void NodeArea::addNode(QUuid id, QRectF rect, QString text, QString tooltip)
{
    mView->addNode(id, std::move(rect), std::move(text), std::move(tooltip));

    auto req = Shared::EventSystem::EvInteractionsSolveReq({}, {std::move(id)});
    Shared::EventSystem::Communicator::instance().postEvent(req);
}

void NodeArea::removeNode(const QUuid &id)
{
    mView->removeNode(std::move(id));
}

void NodeArea::addConnection(QUuid nodeId1, QUuid nodeId2,  QString text, QString tooltip)
{
    mView->addConnection(std::move(nodeId1), std::move(nodeId2), std::move(text), std::move(tooltip));
}

void NodeArea::update(std::vector<std::shared_ptr<Shared::IVisualNodeWrapper>> nodes)
{
    mView->update(nodes);
}

void NodeArea::customEvent(QEvent *event)
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
    } else if (eventType == Shared::EventSystem::EvInteractionsSolveResp::staticType()) {
        auto& ev = static_cast<Shared::EventSystem::EvInteractionsSolveResp&>(*event);
        update(ev.nodes);
    }
}

}
