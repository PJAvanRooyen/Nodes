#include "mainwindow.h"
#include "CentralWidget.h"
#include "NodeStore.h"
#include "InteractionModule.h"
#include "Shared/EventSystem/Communicator.h"

#include <QApplication>
#include <QScreen>
#include <QThread>

// #define ManagerDebug

int main(int argc, char *argv[])
{
    QThread coreThread = QThread();
    // COMMUNICATOR
    //=========================================================================
    auto &comm = Shared::EventSystem::Communicator::instance();
    comm.moveToThread(&coreThread);

    // CORE
    //=========================================================================
    using NodeStore = Core::NodeStore;
    NodeStore nodeStore;
    nodeStore.moveToThread(&coreThread);

    using InteractionModule = Core::InteractionModule;
    auto interactionModule = InteractionModule();
    interactionModule.moveToThread(&coreThread);

    // auto visualNodeWrappers = nodeManager.wrappedNodes();

    // static constexpr int kMaxNumIterations = 10;
    // bool solved = false;
    // int noOfSolves = 0;
    // while(!solved && noOfSolves < kMaxNumIterations){
    //     solved = interactionHandler.solve(visualNodeWrappers, &Core::InteractFnExample::mindmapInteractions);
    //     ++noOfSolves;
    // }

    // UI
    //=========================================================================
    QApplication a(argc, argv);

    UI::MainWindow w;
    UI::CentralWidget widget;
    w.setCentralWidget(&widget);

//     { // test
//         auto redraw = [&nodeManager, &widget](){
//             auto visualNodeWrappers = nodeManager.wrappedNodes();

//             // set their names to their index within the manager.
//             for(auto& node : visualNodeWrappers){
//                 node.setText(QString::number(node.index()));
//             }

//             // clear all existing items
//             widget.reset();

//             // Add the nodes to the scene
//             for(auto& node : visualNodeWrappers){
//                 auto nodeId = node.id();
//                 widget.addNode(nodeId, node.rect(), node.text(), QString("TopLeft: (x:%1, y:%2)").arg(node.rect().left()).arg(node.rect().top()));

//                 // Add the node's connections to the scene
//                 for(auto& otherNode : visualNodeWrappers){
//                     if(node.isConnectedTo(otherNode.index())){
//                         widget.addConnection(nodeId, otherNode.id());
//                     }
//                 }
//             }
//         };

//         QObject::connect(&widget, &UI::CentralWidget::connectionAdd,
//              [&widget, &nodeManager, redraw](QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip){
//             Q_UNUSED(text);
//             Q_UNUSED(tooltip);
//             auto wrappedNode1 = nodeManager.wrappedNode(nodeId1);
//             if(!wrappedNode1.has_value()){
//                 return;
//             }
//             auto wrappedNode2 = nodeManager.wrappedNode(nodeId2);
//             if(!wrappedNode2.has_value()){
//                 return;
//             }

//             nodeManager.connect(wrappedNode1.value().index(), wrappedNode2.value().index(), decltype(nodeManager)::ConnectionType::ParentChild);
//             widget.addConnection(nodeId1, nodeId2);

// #ifdef ManagerDebug
//             redraw();
// #endif
//         });
//         QObject::connect(&widget, &UI::CentralWidget::nodeRemove,
//                          [&widget, &nodeManager, redraw](QUuid nodeId){
//             if(size_t nodeIndex = nodeManager.nodeIndex(nodeId); nodeIndex != -1){
//                 nodeManager.removeNode(nodeIndex);
//                 widget.removeNode(nodeId);

// #ifdef ManagerDebug
//                 redraw();
// #endif
//             }
//         });

//         redraw();
//     }

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.setGeometry(screenGeometry);
    widget.view()->setGeometry(screenGeometry);
    w.show();

    coreThread.start();
    auto res =  a.exec();
    coreThread.quit();
    return res;
}
