#include "NodeStore.h"
#include "Shared/EventSystem/Communicator.h"
#include "Shared/EventSystem/Events/EvNode.h"

namespace Core{

// The maximum number of nodes supported.
// NOTE: in future, a "memory" manager should be created where:
// if the user adds more nodes than this, then a new nodeManager can be created with a bigger size,
// and the exisiting nodes and connections can be moved to the new manager.
static constexpr uint32_t kMaxNodeCount = 100;

NodeStore::NodeStore()
    : mNodeManager()
{
    auto &communicator = Shared::EventSystem::Communicator::instance();
    communicator.connect(this,
                         {Shared::EventSystem::EvNodeAddReq::staticType()});

    using NodeManager = decltype(mNodeManager);

    { // Overlapping
        auto node0 = std::make_unique<Shared::VisualNode>();
        node0->setRect(QRectF(QPointF(100,100), QSizeF(20,20))); //! test
        mNodeManager.addNode(std::move(node0));
        auto node1 = std::make_unique<Shared::VisualNode>();
        node1->setRect(QRectF(QPointF(90,90), QSizeF(20,20))); //! test
        mNodeManager.addNode(0, std::move(node1), NodeManager::ConnectionType::ParentChild);
        auto node2 = std::make_unique<Shared::VisualNode>();
        node2->setRect(QRectF(QPointF(110,90), QSizeF(20,20))); //! test
        mNodeManager.addNode(0, std::move(node2), NodeManager::ConnectionType::ParentChild);
        auto node3 = std::make_unique<Shared::VisualNode>();
        node3->setRect(QRectF(QPointF(90,110), QSizeF(20,20))); //! test
        mNodeManager.addNode(0, std::move(node3), NodeManager::ConnectionType::ParentChild);
        auto node4 = std::make_unique<Shared::VisualNode>();
        node4->setRect(QRectF(QPointF(110,110), QSizeF(20,20))); //! test
        mNodeManager.addNode(0, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }

    { // Touching
        auto node0 = std::make_unique<Shared::VisualNode>();
        node0->setRect(QRectF(QPointF(500,500), QSizeF(20,20))); //! test
        mNodeManager.addNode(std::move(node0));
        auto node1 = std::make_unique<Shared::VisualNode>();
        node1->setRect(QRectF(QPointF(485.857864376,485.857864376), QSizeF(20,20))); //! test
        mNodeManager.addNode(5, std::move(node1), NodeManager::ConnectionType::ParentChild);
        auto node2 = std::make_unique<Shared::VisualNode>();
        node2->setRect(QRectF(QPointF(514.14213562373095,485.857864376), QSizeF(20,20))); //! test
        mNodeManager.addNode(5, std::move(node2), NodeManager::ConnectionType::ParentChild);
        auto node3 = std::make_unique<Shared::VisualNode>();
        node3->setRect(QRectF(QPointF(485.857864376,514.14213562373095), QSizeF(20,20))); //! test
        mNodeManager.addNode(5, std::move(node3), NodeManager::ConnectionType::ParentChild);
        auto node4 = std::make_unique<Shared::VisualNode>();
        node4->setRect(QRectF(QPointF(514.14213562373095,514.14213562373095), QSizeF(20,20))); //! test
        mNodeManager.addNode(5, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }
}

void
NodeStore::customEvent(QEvent* event) {
    Q_ASSERT(event);

    const auto eventType = event->type();
    if (eventType == Shared::EventSystem::EvNodeAddReq::staticType()) {
        const auto& ev = static_cast<Shared::EventSystem::EvNodeAddReq&>(*event);

        const auto& rect = ev.rect;
        const auto& text = ev.text;

        auto node = std::make_unique<Shared::VisualNode>();
        node->setRect(std::move(rect));
        if(!text.isEmpty()){
            node->setText(std::move(text));
        }
        auto nodeRef = mNodeManager.addNode(std::move(node));
        auto& addedNode = nodeRef.get();

        const auto& nodeId = addedNode.id();
        { //test
            if(text.isEmpty()){
                auto wrappedNode = mNodeManager.wrappedNode(nodeId);
                if(!wrappedNode.has_value()){
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
}

}
