#include "GraphicsNode.h"

#include <QPainter>
#include <QStyledItemDelegate>

namespace UI{

NodeDescriptive::NodeDescriptive(QString text, QString details)
    : Node()
{
    setData(Qt::ItemDataRole::DisplayRole, text);
    setData(NodeDataRole::DetailedText, details);

    setToolTip(details);
}

void NodeDescriptive::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Node::paint(painter, option, widget);
    auto fm = option->fontMetrics;

    auto text = data(Qt::DisplayRole).toString();
    auto center = option->rect.center();
    auto textCenter = QPoint(fm.horizontalAdvance(text)/2.0, -fm.height()/2.0);
    auto textRelativeCenter = center - textCenter;
    painter->drawText(textRelativeCenter, text);
}


}
