#ifndef NodeManager_H
#define NodeManager_H

#include "ConnectionHandler.h"
#include "Node.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace Core{

template<class ConnectionHandlerType,
         std::enable_if_t<std::is_base_of_v<IConnectionHanlder, ConnectionHandlerType>>* = Q_NULLPTR>
class NodeManager
{
public:
    using ConnectionHandler = ConnectionHandlerType;
    using ConnectionType = typename ConnectionHandlerType::ConnectionType;
    using INode = Shared::INode;

    NodeManager()
        : mNodes()
        , mConnectionHanlder()
    {
    }

    const std::vector<std::unique_ptr<INode>>& nodes() const{
        return mNodes;
    }

    template<class NodeType>
    std::vector<std::reference_wrapper<NodeType>> nodes() const{
        std::vector<std::reference_wrapper<NodeType>> concreteNodes;
        concreteNodes.reserve(mNodes.size());
        for(const auto& node : mNodes){
            if(NodeType* concreteNode = dynamic_cast<NodeType*>(node.get())){
                concreteNodes.push_back(std::ref(*concreteNode));
            }
        }
        return concreteNodes;
    }

    template<class NodeType, std::enable_if_t<std::is_base_of_v<INode, NodeType>>* = Q_NULLPTR>
    void addNode(std::unique_ptr<NodeType> node)
    {
        mNodes.push_back(std::move(node));
        mConnectionHanlder.addNode();
    }

    template<class NodeType, std::enable_if_t<std::is_base_of_v<INode, NodeType>>* = Q_NULLPTR>
    void addNode(size_t sourceIndex,
                 std::unique_ptr<NodeType> node,
                 ConnectionType connectionType)
    {
        size_t targetIndex = mNodes.size();
        mNodes.push_back(std::move(node));
        mConnectionHanlder.addNode(sourceIndex, targetIndex, connectionType);
    }

protected:
    const std::vector<std::unique_ptr<INode>>& _nodes() const{
        return mNodes;
    }

    const ConnectionHandlerType& _connectionHandler() const{
        return mConnectionHanlder;
    }

private:
    std::vector<std::unique_ptr<INode>> mNodes;
    ConnectionHandlerType mConnectionHanlder;
};

namespace InterconnectedMemory {
template<int32_t MaxSize>
class NodeManager : public Core::NodeManager<Core::InterconnectedMemory::ConnectionHanlder<MaxSize>>
{
    using Base = Core::NodeManager<Core::InterconnectedMemory::ConnectionHanlder<MaxSize>>;

public:
    template<class NodeType>
    std::vector<Shared::InterconnectedMemory::ConnectedNodeWrapper<NodeType, MaxSize>> wrappedNodes() const{
        using NodeWrapperType = Shared::InterconnectedMemory::ConnectedNodeWrapper<NodeType, MaxSize>;

        std::vector<NodeWrapperType> concreteNodes;
        const auto& nodes = Base::_nodes();
        const auto& connectionHandler = Base::_connectionHandler();
        concreteNodes.reserve(nodes.size());

        for(auto nodeIt = nodes.cbegin(); nodeIt != nodes.cend(); ++nodeIt){
            const auto& node = *nodeIt;
            if(NodeType* concreteNode = dynamic_cast<NodeType*>(node.get())){
                size_t nodeIndex = nodeIt - nodes.cbegin();
                auto connections = connectionHandler.nodeConnections(nodeIndex);
                auto wrapper = NodeWrapperType(std::ref(*concreteNode), connections);
                concreteNodes.push_back(std::move(wrapper));
            }
        }

        return concreteNodes;
    }
};
}

}

#endif // NodeManager_H
