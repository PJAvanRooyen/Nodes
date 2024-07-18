#ifndef GraphicsNode_H
#define GraphicsNode_H

#include "GraphicsItem.h"

#include <QUuid>
#include <QGraphicsItem>
#include <QPainter>

namespace UI{

class GraphicsNode : public QGraphicsEllipseItem, public GraphicsItemData{
public:
    GraphicsNode(QGraphicsItem *parent = nullptr)
        : GraphicsNode(QUuid::createUuid(), parent)
    {
    }

    GraphicsNode(QUuid id, QGraphicsItem *parent = nullptr)
        : QGraphicsEllipseItem(parent)
    {
        QGraphicsEllipseItem::setData(GraphicsItemData::DataRole::Type, QVariant::fromValue<ItemType>(ItemType::Node));
        QGraphicsEllipseItem::setData(GraphicsItemData::DataRole::ID, std::move(id));
    }

    QString text() const{
        return QGraphicsEllipseItem::data(DataRole::Text).toString();
    }
    void setText(QString text){
        QGraphicsEllipseItem::setData(DataRole::Text, std::move(text));
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        QGraphicsEllipseItem::paint(painter, option, widget);

        auto text = QGraphicsEllipseItem::data(DataRole::Text).toString();
        if(!text.isEmpty()){
            painter->drawText(this->rect(), text, QTextOption(Qt::AlignCenter));
        }
    }

private:
};

}
#endif // GraphicsNode_H
