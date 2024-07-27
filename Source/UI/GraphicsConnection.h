#ifndef GraphicsConnection_H
#define GraphicsConnection_H

#include "GraphicsItem.h"

#include <QGraphicsItem>

namespace UI{

class GraphicsConnection : public QGraphicsLineItem, public GraphicsItemData{
public:
    GraphicsConnection(QVariant id, QGraphicsItem *parent = nullptr)
        : QGraphicsLineItem(parent)
    {
        QGraphicsLineItem::setData(GraphicsItemData::DataRole::Type, QVariant::fromValue<ItemType>(ItemType::Connection));
        QGraphicsLineItem::setData(GraphicsItemData::DataRole::ID, std::move(id));
    }

private:
};

}
#endif // GraphicsConnection_H
