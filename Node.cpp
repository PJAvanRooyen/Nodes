#include "Node.h"

namespace Shared{

VisualNode::VisualNode()
    : INode()
    , mRect()
{
}

QRectF
VisualNode::rect() const
{
    return mRect;
}

void
VisualNode::setRect(QRectF rect)
{
    mRect = std::move(rect);
}

const QString&
VisualNode::text() const
{
    return mText;
}

void
VisualNode::setText(QString text)
{
    mText = std::move(text);
}

QPointF
VisualNode::center() const
{
    return mRect.center();
}


QSizeF
VisualNode::size() const
{
    return mRect.size();
}

}
