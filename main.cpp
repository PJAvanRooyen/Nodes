#include "mainwindow.h"
#include "CentralWidget.h"
#include "Core/NodeStore/NodeStore.h"
#include "Core/InteractionModule/InteractionModule.h"
#include "Shared/EventSystem/Communicator.h"

#include <QApplication>
#include <QScreen>
#include <QThread>

// auto visualNodeWrappers = nodeManager.wrappedNodes();
// static constexpr int kMaxNumIterations = 10;
// bool solved = false;
// int noOfSolves = 0;
// while(!solved && noOfSolves < kMaxNumIterations){
//     solved = interactionHandler.solve(visualNodeWrappers, &Core::InteractFnExample::mindmapInteractions);
//     ++noOfSolves;
// }

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
//     }

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

    // UI
    //=========================================================================
    QApplication a(argc, argv);

    UI::MainWindow w;
    UI::CentralWidget widget;
    w.setCentralWidget(&widget);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.setGeometry(screenGeometry);
    widget.view()->setGeometry(screenGeometry);
    w.show();

    coreThread.start();
    nodeStore.init();
    auto res =  a.exec();
    coreThread.quit();
    return res;
}
