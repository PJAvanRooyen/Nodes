#include "mainwindow.h"
#include "CentralWidget.h"
#include "NodeManager.h"
#include "InteractionHandler.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    using NodeManager = Core::NodeManager<Core::InterconnectedMemory::ConnectionHanlder<100>>;
    NodeManager nodeManager;
    { // Overlapping
    auto node0 = std::make_unique<Shared::VisualNode>();
    node0->setRect(QRectF(QPointF(100,100), QSizeF(20,20))); //! test
    node0->setText(QString::number(0)); //! test
    nodeManager.addNode(std::move(node0));
    auto node1 = std::make_unique<Shared::VisualNode>();
    node1->setRect(QRectF(QPointF(90,90), QSizeF(20,20))); //! test
    node1->setText(QString::number(1)); //! test
    nodeManager.addNode(0, std::move(node1), NodeManager::ConnectionType::ParentChild);
    auto node2 = std::make_unique<Shared::VisualNode>();
    node2->setRect(QRectF(QPointF(110,90), QSizeF(20,20))); //! test
    node2->setText(QString::number(2)); //! test
    nodeManager.addNode(0, std::move(node2), NodeManager::ConnectionType::ParentChild);
    auto node3 = std::make_unique<Shared::VisualNode>();
    node3->setRect(QRectF(QPointF(90,110), QSizeF(20,20))); //! test
    node3->setText(QString::number(3)); //! test
    nodeManager.addNode(0, std::move(node3), NodeManager::ConnectionType::ParentChild);
    auto node4 = std::make_unique<Shared::VisualNode>();
    node4->setRect(QRectF(QPointF(110,110), QSizeF(20,20))); //! test
    node4->setText(QString::number(4)); //! test
    nodeManager.addNode(0, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }

    { // Touching
        auto node0 = std::make_unique<Shared::VisualNode>();
        node0->setRect(QRectF(QPointF(500,500), QSizeF(20,20))); //! test
        node0->setText(QString::number(0)); //! test
        nodeManager.addNode(std::move(node0));
        auto node1 = std::make_unique<Shared::VisualNode>();
        node1->setRect(QRectF(QPointF(485.857864376,485.857864376), QSizeF(20,20))); //! test
        node1->setText(QString::number(1)); //! test
        nodeManager.addNode(0, std::move(node1), NodeManager::ConnectionType::ParentChild);
        auto node2 = std::make_unique<Shared::VisualNode>();
        node2->setRect(QRectF(QPointF(514.14213562373095,485.857864376), QSizeF(20,20))); //! test
        node2->setText(QString::number(2)); //! test
        nodeManager.addNode(0, std::move(node2), NodeManager::ConnectionType::ParentChild);
        auto node3 = std::make_unique<Shared::VisualNode>();
        node3->setRect(QRectF(QPointF(485.857864376,514.14213562373095), QSizeF(20,20))); //! test
        node3->setText(QString::number(3)); //! test
        nodeManager.addNode(0, std::move(node3), NodeManager::ConnectionType::ParentChild);
        auto node4 = std::make_unique<Shared::VisualNode>();
        node4->setRect(QRectF(QPointF(514.14213562373095,514.14213562373095), QSizeF(20,20))); //! test
        node4->setText(QString::number(4)); //! test
        nodeManager.addNode(0, std::move(node4), NodeManager::ConnectionType::ParentChild);
    }


    using InteractionHandler = Core::InteractionHandler;
    auto interactionHandler = InteractionHandler(Core::InteractionHandler::ItemsHaveVariableSize);
    interactionHandler.setMaxInteractionDistance(0);
    auto visualNodes = nodeManager.nodes<Shared::VisualNode>();

    static constexpr int kMaxNumIterations = 10;
    bool solved = false;
    int noOfSolves = 0;
    while(!solved && noOfSolves < kMaxNumIterations){
        solved = interactionHandler.solve(visualNodes);
        ++noOfSolves;
    }

    UI::MainWindow w;
    UI::CentralWidget widget;
    w.setCentralWidget(&widget);
    for(auto* node : visualNodes){
        widget.addNode(node->rect(), node->text(), QString("TopLeft: (x:%1, y:%2)").arg(node->rect().left()).arg(node->rect().top()));
    }

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    w.setGeometry(screenGeometry);
    widget.view()->setGeometry(screenGeometry);
    w.show();
    return a.exec();
}
