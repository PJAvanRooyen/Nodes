#ifndef GraphicsNode_H
#define GraphicsNode_H

#include <QGraphicsItem>

namespace UI{

class Node : public QGraphicsEllipseItem{

};

class NodeDescriptive : public Node{
enum NodeDataRole : qint32
    {
        DetailedText = Qt::UserRole + 1
    };

public:
    NodeDescriptive(QString text, QString details);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:

};

}
#endif // GraphicsNode_H
