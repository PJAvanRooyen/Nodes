#ifndef Node_H
#define Node_H

#include <QUuid>

namespace Shared{

template<class IdType>
class INode
{
public:
    using NodeIdType = IdType;

    virtual const IdType& id() const = 0;

protected:
    INode();
};

template<class IdType>
class Node : public INode<IdType>
{
public:
    using NodeIdType = IdType;

    template<class NodeIdType = IdType, std::enable_if_t<std::is_same_v<NodeIdType, QUuid>>* = Q_NULLPTR>
    Node(QUuid id = QUuid::createUuid())
        : INode<IdType>()
        , mId(std::move(id))
    {

    }

    template<class NodeId = IdType, std::enable_if_t<!std::is_same_v<IdType, QUuid>>* = Q_NULLPTR>
    Node(NodeId id)
        : INode<IdType>()
        , mId(std::move(id))
    {

    }

    const IdType& id() const final;

private:
    IdType mId;
};

template<class ConsumedDataType, class ProducedDataType>
class ExecutableNode : public Node<int64_t>
{
public:
    ExecutableNode();

    ProducedDataType execute(ConsumedDataType data);
};

}

#endif // Node_H
