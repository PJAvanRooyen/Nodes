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
class AbstractConnectedNodeWrapper
{
public:
    AbstractConnectedNodeWrapper(std::reference_wrapper<NodeType> node)
        : mNode(node)
    {
    }

    virtual bool isConnected() const = 0;

    virtual bool isConnectedTo(size_t id) const = 0;

    std::reference_wrapper<NodeType> node() const{
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

template<int32_t MaxSize>
class ConnectedVisualNodeWrapper : public ConnectedNodeWrapper<Shared::VisualNode, MaxSize>
{
    using Base = ConnectedNodeWrapper<Shared::VisualNode, MaxSize>;

public:
    ConnectedVisualNodeWrapper(
        size_t nodeIndex,
        std::reference_wrapper<Shared::VisualNode> node,
        typename Base::ConnectionsContainerType& connections)
        : Base(nodeIndex, node, connections)
    {
    }

    const QUuid& id() const
    {
        return Base::node().get().id();
    }

    QRectF rect() const
    {
        return Base::node().get().rect();
    }

    void setRect(QRectF rect)
    {
        Base::node().get().setRect(std::move(rect));
    }

    const QString& text() const
    {
        return Base::node().get().text();
    }

    void setText(QString text)
    {
        Base::node().get().setText(std::move(text));
    }

    QPointF center() const
    {
        return Base::node().get().center();
    }

    QSizeF size() const
    {
        return Base::node().get().size();
    }
};
}

}

#endif // Node_H
