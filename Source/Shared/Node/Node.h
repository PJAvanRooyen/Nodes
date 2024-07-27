#ifndef Node_H
#define Node_H

#include <QUuid>
#include <QRectF>

#include <bitset>

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

    VisualNode(QUuid id);

    const QUuid& id() const;

    QRectF rect() const;

    void setRect(QRectF rect);

    const QString& text() const;

    void setText(QString text);

    QPointF center() const;

    QSizeF size() const;

private:
    QUuid mId;
    QRectF mRect;
    QString mText;
};

template<class NodeType>
class INodeWrapper
{
public:
    virtual bool isConnected() const = 0;

    virtual bool isConnectedTo(size_t id) const = 0;

    virtual std::reference_wrapper<NodeType> node() const = 0;
};

class IVisualNodeWrapper : public INodeWrapper<Shared::VisualNode>
{
public:
    using NodeType = Shared::VisualNode;

    virtual size_t index() const = 0;

    virtual const QUuid& id() const= 0;

    virtual QRectF rect() const = 0;

    virtual void setRect(QRectF rect)= 0;

    virtual const QString& text() const = 0;

    virtual void setText(QString text) = 0;
};

namespace InterconnectedMemory {
template<int32_t MaxSize>
class VisualNodeWrapper : public IVisualNodeWrapper
{
    using NodeType = IVisualNodeWrapper::NodeType;
    using ConnectionsContainerType = std::pair<std::reference_wrapper<const std::bitset<MaxSize>>, std::reference_wrapper<const std::bitset<MaxSize>>>;

public:
    VisualNodeWrapper(
        size_t nodeIndex,
        std::reference_wrapper<NodeType> node,
        ConnectionsContainerType& connections)
        : mNodeIndex(nodeIndex)
        , mNode(node)
        , mConnections(connections)
    {
    }

    size_t index() const{
        return mNodeIndex;
    }

    bool isConnected() const override
    {
        return mConnections.first.get().any() || mConnections.second.get().any();
    }

    bool isConnectedTo(size_t id) const override
    {
        return mConnections.first.get().test(id) || mConnections.second.get().test(id);
    }

    std::reference_wrapper<NodeType> node() const override{
        return mNode;
    }

    const QUuid& id() const
    {
        return mNode.get().id();
    }

    QRectF rect() const
    {
        return mNode.get().rect();
    }

    void setRect(QRectF rect)
    {
        mNode.get().setRect(std::move(rect));
    }

    const QString& text() const
    {
        return mNode.get().text();
    }

    void setText(QString text)
    {
        mNode.get().setText(std::move(text));
    }

private:
    size_t mNodeIndex;
    std::reference_wrapper<NodeType> mNode;
    ConnectionsContainerType mConnections;
};

}

}

#endif // Node_H
