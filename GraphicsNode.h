#ifndef GraphicsNode_H
#define GraphicsNode_H

#include "StaticTypedefs.h"

#include <QGraphicsItem>
#include <QUuid>
#include <QPainter>
#include <QStyledItemDelegate>

namespace UI{

class GraphicsItemData
{
public:
    enum DataRole : qint32
    {
        Text = Qt::DisplayRole,
        ID = Qt::UserRole + 1,
        TooltipText,
    };
};

template<class GraphicsItemBaseType,
         class IdType,
         std::enable_if_t<std::is_base_of_v<QGraphicsItem, GraphicsItemBaseType>>* = Q_NULLPTR>
class GraphicsItem : public GraphicsItemBaseType, public GraphicsItemData{
public:
    using Base = GraphicsItemBaseType;

    template<class IdT = IdType, std::enable_if_t<std::is_same_v<QUuid, IdT>>* = Q_NULLPTR>
    GraphicsItem(QGraphicsItem *parent = nullptr)
        : Base(parent)
    {
        Base::setData(GraphicsItemData::DataRole::ID, QUuid::createUuid());
    }

    GraphicsItem(IdType id, QGraphicsItem *parent = nullptr)
        : Base(parent)
    {
        Base::setData(GraphicsItemData::DataRole::ID, std::move(id));
    }

    QString text() const{
        return Base::data(DataRole::Text).toString();
    }
    void setText(QString text){
        Base::setData(DataRole::Text, std::move(text));
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Base::paint(painter, option, widget);

        if constexpr(has_rect<decltype(this)>()){
            auto text = Base::data(DataRole::Text).toString();
            if(!text.isEmpty()){
                painter->drawText(this->rect(), text, QTextOption(Qt::AlignCenter));
            }
        }else if constexpr(has_rect<Base>()){
            auto text = Base::data(DataRole::Text).toString();
            if(!text.isEmpty()){
                painter->drawText(Base::rect(), text, QTextOption(Qt::AlignCenter));
            }
        }
    }

private:
};

}
#endif // GraphicsNode_H
