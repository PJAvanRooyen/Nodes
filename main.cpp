#include "mainwindow.h"
#include "CentralWidget.h"
#include "NodeManager.h"
#include "InteractionHandler.h"
#include "Interactions.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // The maximum number of nodes supported.
    // NOTE: in future, a "memory" manager should be created where:
    // if the user adds more nodes than this, then a new nodeManager can be created with a bigger size,
    // and the exisiting nodes and connections can be moved to the new manager.
    static constexpr uint32_t kMaxNodeCount = 100;

    using NodeManager = Core::InterconnectedMemory::VisualNodeManager<kMaxNodeCount>;
    NodeManager nodeManager;
    { // Overlapping
        auto node0 = std::make_unique<Shared::VisualNode>();
        node0->setRect(QRectF(QPointF(100,100), QSizeF(20,20))); //! test
        nodeManager.addNode(std::move(node0));
        auto node1 = std::make_unique<Shared::VisualNode>();
        node1->setRect(QRectF(QPointF(90,90), QSizeF(20,20))); //! test
        nodeManager.addNode(0, std::move(node1), NodeManager::ConnectionType::ParentChild);
        auto node2 = std::make_unique<Shared::VisualNode>();
        node2->setRect(QRectF(QPointF(110,90), QSizeF(20,20))); //! test
        nodeManager.addNode(0, std::move(node2), NodeManager::ConnectionType::ParentChild);
        auto node3 = std::make_unique<Shared::VisualNode>();
        node3->setRect(QRectF(QPointF(90,110), QSizeF(20,20))); //! test
        nodeManager.addNode(0, std::move(node3), NodeManager::ConnectionType::ParentChild);
        auto node4 = std::make_unique<Shared::VisualNode>();
        node4->setRect(QRectF(QPointF(110,110), QSizeF(20,20))); //! test
        nodeManager.addNode(0, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }

    { // Touching
        auto node0 = std::make_unique<Shared::VisualNode>();
        node0->setRect(QRectF(QPointF(500,500), QSizeF(20,20))); //! test
        nodeManager.addNode(std::move(node0));
        auto node1 = std::make_unique<Shared::VisualNode>();
        node1->setRect(QRectF(QPointF(485.857864376,485.857864376), QSizeF(20,20))); //! test
        nodeManager.addNode(5, std::move(node1), NodeManager::ConnectionType::ParentChild);
        auto node2 = std::make_unique<Shared::VisualNode>();
        node2->setRect(QRectF(QPointF(514.14213562373095,485.857864376), QSizeF(20,20))); //! test
        nodeManager.addNode(5, std::move(node2), NodeManager::ConnectionType::ParentChild);
        auto node3 = std::make_unique<Shared::VisualNode>();
        node3->setRect(QRectF(QPointF(485.857864376,514.14213562373095), QSizeF(20,20))); //! test
        nodeManager.addNode(5, std::move(node3), NodeManager::ConnectionType::ParentChild);
        auto node4 = std::make_unique<Shared::VisualNode>();
        node4->setRect(QRectF(QPointF(514.14213562373095,514.14213562373095), QSizeF(20,20))); //! test
        nodeManager.addNode(5, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }

    using InteractionHandler = Core::InteractionHandler;
    auto interactionHandler = InteractionHandler(Core::InteractionHandler::ItemsHaveVariableSize);
    interactionHandler.setMaxInteractionDistance(100);
    auto visualNodeWrappers = nodeManager.wrappedNodes();

    static constexpr int kMaxNumIterations = 10;
    bool solved = false;
    int noOfSolves = 0;
    while(!solved && noOfSolves < kMaxNumIterations){
        solved = interactionHandler.solve(visualNodeWrappers, &Core::InteractFnExample::mindmapInteractions);
        ++noOfSolves;
    }

    UI::MainWindow w;
    UI::CentralWidget widget;
    w.setCentralWidget(&widget);
    QObject::connect(&widget, &UI::CentralWidget::nodeAdd,
                     [&widget, &nodeManager](QRectF rect, QString text, QString tooltip){
        Q_UNUSED(tooltip);

        auto node = std::make_unique<Shared::VisualNode>();
        node->setRect(std::move(rect));
        if(!text.isEmpty()){
            node->setText(std::move(text));
        }
        auto nodeRef = nodeManager.addNode(std::move(node));
        auto& addedNode = nodeRef.get();

        const auto& nodeId = addedNode.id();
        { //test
            if(text.isEmpty()){
                auto wrappedNode = nodeManager.wrappedNode(nodeId);
                if(!wrappedNode.has_value()){
                    return;
                }
                wrappedNode->setText(QString::number(wrappedNode->index()));
            }
        }

        const auto nodeRect = addedNode.rect();
        const auto& nodeText = addedNode.text();
        widget.addNode(nodeId, nodeRect, nodeText, QString("TopLeft: (x:%1, y:%2)").arg(nodeRect.left()).arg(nodeRect.top()));
    });
    QObject::connect(&widget, &UI::CentralWidget::connectionAdd,
         [&widget, &nodeManager](QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip){
        Q_UNUSED(text);
        Q_UNUSED(tooltip);
        auto wrappedNode1 = nodeManager.wrappedNode(nodeId1);
        if(!wrappedNode1.has_value()){
            return;
        }
        auto wrappedNode2 = nodeManager.wrappedNode(nodeId2);
        if(!wrappedNode2.has_value()){
            return;
        }

        nodeManager.connect(wrappedNode1.value().index(), wrappedNode2.value().index(), decltype(nodeManager)::ConnectionType::ParentChild);
        widget.addConnection(nodeId1, nodeId2);
    });

    // set their names to their index within the manager.
    for(auto& node : visualNodeWrappers){
        node.setText(QString::number(node.index()));
    }


    // Add the nodes to the scene
    for(auto& node : visualNodeWrappers){
        auto nodeId = node.id();
        widget.addNode(nodeId, node.rect(), node.text(), QString("TopLeft: (x:%1, y:%2)").arg(node.rect().left()).arg(node.rect().top()));

        // Add the node's connections to the scene
        for(auto& otherNode : visualNodeWrappers){
            if(node.isConnectedTo(otherNode.index())){
                widget.addConnection(nodeId, otherNode.id());
            }
        }
    }

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.setGeometry(screenGeometry);
    widget.view()->setGeometry(screenGeometry);
    w.show();
    return a.exec();
}
