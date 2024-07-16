#ifndef Node_H
#define Node_H

#include <QUuid>
#include <QRectF>

#include <memory>
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

template<class NodeType>
class AbstractConnectedNodeWrapper
{
public:
    AbstractConnectedNodeWrapper(std::reference_wrapper<NodeType> node)
        : mNode(node)
    {
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

    QPointF center() const
    {
        return mNode.get().center();
    }

    QSizeF size() const
    {
        return mNode.get().size();
    }

    virtual bool isConnected() const = 0;

    virtual bool isConnectedTo(size_t id) const = 0;

protected:
    std::reference_wrapper<NodeType> _node() const{
        return mNode;
    }

private:
    std::reference_wrapper<NodeType> mNode;
};

namespace InterconnectedMemory {
template<class NodeType, int32_t MaxSize>
class ConnectedNodeWrapper : public AbstractConnectedNodeWrapper<NodeType>
{
public:
    using Base = AbstractConnectedNodeWrapper<NodeType>;
    using ConnectionsContainerType = std::pair<std::reference_wrapper<const std::bitset<MaxSize>>, std::reference_wrapper<const std::bitset<MaxSize>>>;

    ConnectedNodeWrapper(
        size_t nodeIndex,
        std::reference_wrapper<NodeType> node,
        ConnectionsContainerType& connections)
        : Base(node)
        , mNodeIndex(nodeIndex)
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

private:
    size_t mNodeIndex;
    ConnectionsContainerType mConnections;
};
}

}

#endif // Node_H
