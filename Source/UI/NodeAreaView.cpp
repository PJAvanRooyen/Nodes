#include "NodeAreaView.h"
#include "GraphicsNode.h"
#include "GraphicsConnection.h"

#include <QMouseEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

namespace UI{

NodeAreaView::NodeAreaView(QWidget *parent)
    : QGraphicsView(parent)
    , mScene(new Scene(this))
    , mDragStartItem(Q_NULLPTR)
    , mTempConnectionLine(Q_NULLPTR)
{
    setMouseTracking(true);
    setScene(mScene);
    setRenderHint(QPainter::Antialiasing);
}

void
NodeAreaView::reset()
{
    mScene->clear();
    mDragStartItem = nullptr;
    mTempConnectionLine= nullptr;
}

void
NodeAreaView::addNode(QUuid id, QRectF rect, QString text, QString tooltip)
{
    auto* node = new GraphicsNode(id);
    node->setText(std::move(text));
    node->setToolTip(std::move(tooltip));
    node->setRect(rect);

    node->setPen(QPen(Qt::black));
    node->setBrush(Qt::red);

    mScene->addItem(node);
}

bool
NodeAreaView::removeNode(const QUuid &id)
{
    bool removed = false;
    const auto items = mScene->items();
    for(auto* item : items){
        if(!item){
            continue;
        }
        auto itemType = item->data(GraphicsItemData::DataRole::Type).value<ItemType>();
        auto itemDataVar = item->data(GraphicsItemData::DataRole::ID);
        if(itemType == ItemType::Node){
            if(itemDataVar.canConvert<QUuid>() && itemDataVar.value<QUuid>() == id){
                mScene->removeItem(item);
                removed = true;
            }
        } else if(itemType == ItemType::Connection){
            if(itemDataVar.canConvert<std::pair<QUuid, QUuid>>()){
                // Remove connections to/from the node
                auto connectionId = itemDataVar.value<std::pair<QUuid, QUuid>>();
                if(connectionId.first == id || connectionId.second == id){
                    mScene->removeItem(item);
                }
            }
        }
    }

    return removed;
}

void
NodeAreaView::addConnection(QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip)
{
    QVariant id = QVariant::fromValue<QPair<QUuid, QUuid>>(qMakePair(nodeId1, nodeId2));
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
            auto* line = new GraphicsConnection(id);
            line->setPen(QPen(Qt::red, 2));
            line->setLine(QLineF(startPos, endPos));
            mScene->addItem(line);
        }
    }
}

void
NodeAreaView::update(std::vector<std::shared_ptr<Shared::IVisualNodeWrapper>> nodes)
{
    const auto items = mScene->items();
    for(auto* item : items){
        if(!item){
            continue;
        }
        auto itemType = item->data(GraphicsItemData::DataRole::Type).value<ItemType>();
        if(itemType != ItemType::Node){
            continue;
        }
        auto itemDataVar = item->data(GraphicsItemData::DataRole::ID);
        if(!itemDataVar.canConvert<QUuid>()){
            continue;
        }
        auto id = itemDataVar.value<QUuid>();
        if(auto updatedNodeIt = std::find_if(nodes.cbegin(), nodes.cend(),
            [&id](const std::shared_ptr<Shared::IVisualNodeWrapper>& node){
                return node && node->id() == id;
            }); updatedNodeIt != nodes.cend()){
            auto& node = static_cast<GraphicsNode&>(*item);
            const auto& updatedItem = *updatedNodeIt;
            node.setRect(updatedItem->rect());
            // TODO: update other parts e.g. text if it changed.
        }
    }
}

void
NodeAreaView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if(event->button() == Qt::MouseButton::LeftButton){
        auto startItems = mScene->items(mapToScene(event->pos()));
        for(auto* startItem : startItems){
            if(startItem->data(GraphicsItemData::DataRole::Type).value<ItemType>() == ItemType::Node){
                mDragStartItem = startItem;
                if (mDragStartItem) {
                    const auto startItemId = mDragStartItem->data(GraphicsItemData::DataRole::ID).value<QUuid>();
                    QVariant id = QVariant::fromValue<QPair<QUuid, QUuid>>(qMakePair(startItemId, QUuid()));
                    mTempConnectionLine = new GraphicsConnection(id);
                    mTempConnectionLine->setPen(QPen(Qt::red, 2));
                    mTempConnectionLine->setLine(QLineF(mDragStartItem->boundingRect().center(), event->pos()));
                    mScene->addItem(mTempConnectionLine);
                    break;
                }
            }
        }
    }
}

void
NodeAreaView::mouseMoveEvent(QMouseEvent *event)
{
    if (mTempConnectionLine && mDragStartItem) {
        mTempConnectionLine->setLine(QLineF(mDragStartItem->boundingRect().center(), event->pos()));
    }
    QGraphicsView::mouseMoveEvent(event);
}

void
NodeAreaView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if(event->button() == Qt::MouseButton::LeftButton){
        if (mTempConnectionLine) {
            auto endItems = mScene->items(mapToScene(event->pos()));
            bool endedOnItem = false;
            for(const auto* endItem : endItems){
                if(endItem->data(GraphicsItemData::DataRole::Type).value<ItemType>() == ItemType::Node){
                    if (mDragStartItem && endItem && endItem != mDragStartItem) {
                        const auto startItemId = mDragStartItem->data(GraphicsItemData::DataRole::ID).value<QUuid>();
                        const auto endItemId = endItem->data(GraphicsItemData::DataRole::ID).value<QUuid>();
                        QVariant id = QVariant::fromValue<QPair<QUuid, QUuid>>(qMakePair(startItemId, endItemId));
                        mTempConnectionLine->setData(GraphicsItemData::DataRole::ID, id);
                        mTempConnectionLine->setLine(QLineF(mDragStartItem->boundingRect().center(), endItem->boundingRect().center()));
                        endedOnItem = true;

                        Q_EMIT connectionAdd(startItemId, endItemId);
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
    }
}

void
NodeAreaView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    QPointF scenePos = mapToScene(event->pos());

    static const QSizeF kNodeSize = QSizeF(20, 20);
    static const qreal kNodeRadius = kNodeSize.width()/2.0;

    QRectF rect = QRectF(QPointF(scenePos.x() - kNodeRadius, scenePos.y() - kNodeRadius), kNodeSize);
    Q_EMIT nodeAdd(rect);
}

void
NodeAreaView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    mScene->setSceneRect(rect());
}

void
NodeAreaView::contextMenuEvent(QContextMenuEvent *event)
{
    auto scenePos = mapToScene(event->pos());
    auto items = scene()->items(scenePos);
    for(auto* item : items){
        auto idVar = item->data(GraphicsItemData::DataRole::ID);
        if (item->data(GraphicsItemData::DataRole::Type).value<ItemType>() == ItemType::Node &&  idVar.canConvert<QUuid>()){
            QMenu menu;
            QAction *deleteAction = menu.addAction("Delete Node");
            QAction *selectedAction = menu.exec(event->pos());
            if (selectedAction == deleteAction) {
                Q_EMIT nodeRemove(idVar.value<QUuid>());
            }
        }
    }
}




}
