#include "CentralWidgetView.h"
#include "GraphicsNode.h"

#include <QMouseEvent>

namespace UI{

CentralWidgetView::CentralWidgetView(QWidget *parent)
    : QGraphicsView(parent)
    , mScene(new Scene(this))
    , mDragStartItem(Q_NULLPTR)
    , mTempConnectionLine(Q_NULLPTR)
{
    setMouseTracking(true);
    setScene(mScene);
    setRenderHint(QPainter::Antialiasing);
}

void CentralWidgetView::addNode(QVariant id, QRectF rect, QString text, QString tooltip)
{
    auto* node = new GraphicsItem<QGraphicsEllipseItem, QVariant>(id);
    node->setText(std::move(text));
    node->setToolTip(std::move(tooltip));
    node->setRect(rect);

    node->setPen(QPen(Qt::black));
    node->setBrush(Qt::red);

    mScene->addItem(node);
}

void CentralWidgetView::addConnection(QVariant nodeId1, QVariant nodeId2, QString text, QString tooltip)
{
    QVariant id = QVariant::fromValue<QPair<QVariant, QVariant>>(qMakePair(nodeId1, nodeId2));
    if (mTempConnectionLine && mDragStartItem && mTempConnectionLine->data(GraphicsItemData::DataRole::ID) == id) {
        // Already added.
        return;
    }else{
        if (mTempConnectionLine){
            mScene->removeItem(mTempConnectionLine);
            delete mTempConnectionLine;
            mTempConnectionLine = nullptr;
        }
        if(mDragStartItem){
            mDragStartItem = nullptr;
        }

        QPointF startPos;
        QPointF endPos;
        bool startFound = false;
        bool endFound = false;
        const auto items = mScene->items();
        for(const auto* item : items){
            auto id = item->data(GraphicsItemData::DataRole::ID);
            if(id == nodeId1){
                startPos = item->boundingRect().center();
                startFound = true;
                if(endFound){
                    break;
                }
            } else if(id == nodeId2){
                endPos = item->boundingRect().center();
                endFound = true;
                if(startFound){
                    break;
                }
            }
        }

        if(startFound && endFound){
            auto* line = new GraphicsItem<QGraphicsLineItem, QVariant>(id);
            line->setPen(QPen(Qt::red, 2));
            line->setLine(QLineF(startPos, endPos));
            mScene->addItem(line);
        }
    }
}

void CentralWidgetView::mousePressEvent(QMouseEvent *event)
{
    auto startItems = mScene->items(mapToScene(event->pos()));
    for(auto* startItem : startItems){
        if (dynamic_cast<const GraphicsItem<QGraphicsLineItem, QVariant>*>(startItem)){
            continue;
        } else{
            mDragStartItem = startItem;
            if (mDragStartItem) {
                QVariant id = QVariant::fromValue<QPair<QVariant, QVariant>>(qMakePair(mDragStartItem->data(GraphicsItemData::DataRole::ID), QVariant()));
                mTempConnectionLine = new GraphicsItem<QGraphicsLineItem, QVariant>(id);
                mTempConnectionLine->setPen(QPen(Qt::red, 2));
                mScene->addItem(mTempConnectionLine);
                break;
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void CentralWidgetView::mouseMoveEvent(QMouseEvent *event)
{
    if (mTempConnectionLine && mDragStartItem) {
        mTempConnectionLine->setLine(QLineF(mDragStartItem->boundingRect().center(), event->pos()));
    }
    QGraphicsView::mouseMoveEvent(event);
}

void CentralWidgetView::mouseReleaseEvent(QMouseEvent *event)
{
    if (mTempConnectionLine) {
        auto endItems = mScene->items(mapToScene(event->pos()));
        bool endedOnItem = false;
        for(const auto* endItem : endItems){
            if (dynamic_cast<const GraphicsItem<QGraphicsLineItem, QVariant>*>(endItem)){
                continue;
            } else{
                if (mDragStartItem && endItem && endItem != mDragStartItem) {
                    // TODO:
                    // - send signal to add connection in the node manager.
                    // - in response addConnection will be called for 2 node indices.
                    // - if the mTempConnectionLine's id matches the node ids, then keep it, else find the nodes and draw a line between them.
                    QVariant id = QVariant::fromValue<QPair<QVariant, QVariant>>(qMakePair(mDragStartItem->data(GraphicsItemData::DataRole::ID), endItem->data(GraphicsItemData::DataRole::ID)));
                    mTempConnectionLine->setData(GraphicsItemData::DataRole::ID, id);
                    mTempConnectionLine->setLine(QLineF(mDragStartItem->boundingRect().center(), endItem->boundingRect().center()));
                    endedOnItem = true;
                    break;
                }
            }
        }
        if(!endedOnItem){
            mScene->removeItem(mTempConnectionLine);
            delete mTempConnectionLine;
        }
        mTempConnectionLine = nullptr;
        mDragStartItem = nullptr;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CentralWidgetView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    QPointF scenePos = mapToScene(event->pos());

    static const QSizeF kNodeSize = QSizeF(100, 100);
    static const qreal kNodeRadius = kNodeSize.width()/2.0;

    QRectF rect = QRectF(QPointF(scenePos.x() - kNodeRadius, scenePos.y() - kNodeRadius), kNodeSize);
    emit nodeAdd(rect);
}

void CentralWidgetView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    mScene->setSceneRect(rect());
}


}
