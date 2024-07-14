#ifndef Node_H
#define Node_H

#include <QUuid>
#include <QRectF>

namespace Shared{

class INode
{
public:
    virtual ~INode() = default;

protected:
    INode(){

    }
};

class VisualNode : public INode
{
public:
    VisualNode();

    QRectF rect() const;

    void setRect(QRectF rect);

    const QString& text() const;

    void setText(QString text);

    QPointF center() const;

    QSizeF size() const;

private:
    QRectF mRect;
    QString mText;
};

}

#endif // Node_H
