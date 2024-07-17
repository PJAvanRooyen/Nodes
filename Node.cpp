#include "Node.h"

namespace Shared{

VisualNode::VisualNode()
    : VisualNode(QUuid::createUuid())
{
}

VisualNode::VisualNode(QUuid id)
    : INode()
    , mId(std::move(id))
    , mRect()
    , mText()
{
}

const QUuid&
VisualNode::id() const
{
    return mId;
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
