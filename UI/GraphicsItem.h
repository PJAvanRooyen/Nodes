#ifndef GraphicsItem_H
#define GraphicsItem_H

#include <qnamespace.h>

namespace UI{
Q_NAMESPACE

class GraphicsItemData
{
public:
    enum DataRole : qint32
    {
        Text = Qt::DisplayRole,
        Type = Qt::UserRole + 1,
        ID,
        TooltipText,
    };
};

enum class ItemType : qint32
{
    Node,
    Connection
};
Q_ENUM_NS(ItemType)

}
#endif // GraphicsItem_H
