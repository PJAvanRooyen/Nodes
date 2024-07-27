#include "NodeManager.h"
#include "EventSystem/Communicator.h"
#include "EventSystem/Events/EvNode.h"
#include "EventSystem/Events/EvConnection.h"
#include "EventSystem/Events/EvInteraction.h"

namespace Core{

// The maximum number of nodes supported.
// NOTE: in future, a "memory" manager should be created where:
// if the user adds more nodes than this, then a new nodeManager can be created with a bigger size,
// and the exisiting nodes and connections can be moved to the new manager.
static constexpr uint32_t kMaxNodeCount = 100;

NodeManager::NodeManager()
    : mNodeHandler()
    , mInteractionHandler()
{
    auto &communicator = Shared::EventSystem::Communicator::instance();
    communicator.connect(this,
                         {Shared::EventSystem::EvNodeAddReq::staticType(),
                          Shared::EventSystem::EvNodeRemoveReq::staticType(),
                          Shared::EventSystem::EvConnectionAddReq::staticType(),
                          Shared::EventSystem::EvInteractionsSolveReq::staticType(),
                          });
}

void NodeManager::init()
{
    { // Overlapping
        addNode(QRectF(QPointF(100,100), QSizeF(20,20)));
        addNode(QRectF(QPointF(90,90), QSizeF(20,20)));
        connect(mNodeHandler.nodes().front()->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(110,90), QSizeF(20,20)));
        connect(mNodeHandler.nodes().front()->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(90,110), QSizeF(20,20)));
        connect(mNodeHandler.nodes().front()->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(110,110), QSizeF(20,20)));
        connect(mNodeHandler.nodes().front()->id(), mNodeHandler.nodes().back()->id());
    }

    { // Touching
        addNode(QRectF(QPointF(500,500), QSizeF(20,20)));
        addNode(QRectF(QPointF(485.857864376,485.857864376), QSizeF(20,20)));
        connect(mNodeHandler.nodeAt(5)->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(514.14213562373095,485.857864376), QSizeF(20,20)));
        connect(mNodeHandler.nodeAt(5)->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(485.857864376,514.14213562373095), QSizeF(20,20)));
        connect(mNodeHandler.nodeAt(5)->id(), mNodeHandler.nodes().back()->id());
        addNode(QRectF(QPointF(514.14213562373095,514.14213562373095), QSizeF(20,20)));
        connect(mNodeHandler.nodeAt(5)->id(), mNodeHandler.nodes().back()->id());
    }
}

void
NodeManager::customEvent(QEvent* event) {
    Q_ASSERT(event);

    const auto eventType = event->type();
    if (eventType == Shared::EventSystem::EvNodeAddReq::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvNodeAddReq&>(*event);
        addNode(ev.rect, ev.text, ev.tooltip);
    } else if(eventType == Shared::EventSystem::EvNodeRemoveReq::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvNodeRemoveReq&>(*event);
        removeNode(ev.nodeId);
    } else if(eventType == Shared::EventSystem::EvConnectionAddReq::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvConnectionAddReq&>(*event);
        connect(ev.node1Id, ev.node2Id, ev.text, ev.tooltip);
    } else if(eventType == Shared::EventSystem::EvInteractionsSolveReq::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvInteractionsSolveReq&>(*event);

        std::vector<std::shared_ptr<Shared::IVisualNodeWrapper>> wrappedNodes;
        mNodeHandler.wrappedNodes(wrappedNodes);
        static constexpr int kMaxNumIterations = 10;
        bool solved = false;
        int noOfSolves = 0;
        while(!solved && noOfSolves < kMaxNumIterations){
            solved = mInteractionHandler.solve(wrappedNodes, &Core::InteractFnExample::mindmapInteractions);
            ++noOfSolves;
        }

        auto resp = Shared::EventSystem::EvInteractionsSolveResp(wrappedNodes);
        Shared::EventSystem::Communicator::instance().postEvent(resp);
    }
}

void NodeManager::addNode(QRectF rect, QString text, QString tooltip)
{
    auto node = std::make_unique<Shared::VisualNode>();
    node->setRect(std::move(rect));
    if(!text.isEmpty()){
        node->setText(std::move(text));
    }
    auto nodeRef = mNodeHandler.addNode(std::move(node));
    auto& addedNode = nodeRef.get();

    const auto& nodeId = addedNode.id();
    { //test
        if(text.isEmpty()){
            auto wrappedNode = mNodeHandler.wrappedNode(nodeId);
            if(!wrappedNode){
                return;
            }
            wrappedNode->setText(QString::number(wrappedNode->index()));
        }
    }

    const auto nodeRect = addedNode.rect();
    const auto& nodeText = addedNode.text();

    auto resp = Shared::EventSystem::EvNodeAddResp(
        nodeId,
        nodeRect,
        nodeText,
        QString("TopLeft: (x:%1, y:%2)").arg(nodeRect.left()).arg(nodeRect.top()));
    Shared::EventSystem::Communicator::instance().postEvent(resp);
}

void NodeManager::removeNode(QUuid nodeId)
{
    if(size_t nodeIndex = mNodeHandler.nodeIndex(nodeId); nodeIndex != -1){
       mNodeHandler.removeNode(nodeIndex);
    }

    auto resp = Shared::EventSystem::EvNodeRemoveResp(nodeId);
    Shared::EventSystem::Communicator::instance().postEvent(resp);
}

void NodeManager::connect(QUuid node1Id, QUuid node2Id, QString text, QString tooltip)
{
    Q_UNUSED(text);
    Q_UNUSED(tooltip);
    auto wrappedNode1 = mNodeHandler.wrappedNode(node1Id);
    if(!wrappedNode1){
        return;
    }
    auto wrappedNode2 = mNodeHandler.wrappedNode(node2Id);
    if(!wrappedNode2){
        return;
    }

    mNodeHandler.connect(
        wrappedNode1->index(),
        wrappedNode2->index(),
        decltype(mNodeHandler)::ConnectionType::ParentChild);

    auto resp = Shared::EventSystem::EvConnectionAddResp(node1Id, node2Id);
    Shared::EventSystem::Communicator::instance().postEvent(resp);
}

}
