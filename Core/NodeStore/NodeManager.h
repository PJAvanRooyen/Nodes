#ifndef NodeManager_H
#define NodeManager_H

#include "ConnectionHandler.h"
#include "Node/Node.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace Core{

template<class NodeBaseType, class ConnectionHandlerType,
         std::enable_if_t<std::is_base_of_v<Shared::INode, NodeBaseType> && std::is_base_of_v<IConnectionHanlder, ConnectionHandlerType>>* = Q_NULLPTR>
class NodeManager
{
public:
    using ConnectionHandler = ConnectionHandlerType;
    using ConnectionType = typename ConnectionHandlerType::ConnectionType;

    NodeManager()
        : mNodes()
        , mConnectionHanlder()
    {
    }

    const std::vector<std::unique_ptr<NodeBaseType>>& nodes() const{
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

    template<class NodeType, std::enable_if_t<std::is_base_of_v<NodeBaseType, NodeType>>* = Q_NULLPTR>
    std::reference_wrapper<NodeType> addNode(std::unique_ptr<NodeType> node)
    {
        mNodes.push_back(std::move(node));
        mConnectionHanlder.addNode();
        return static_cast<NodeType&>(*mNodes.back());
    }

    template<class NodeType, std::enable_if_t<std::is_base_of_v<NodeBaseType, NodeType>>* = Q_NULLPTR>
    std::reference_wrapper<NodeType> addNode(size_t sourceIndex,
                 std::unique_ptr<NodeType> node,
                 ConnectionType connectionType)
    {
        size_t targetIndex = mNodes.size();
        mNodes.push_back(std::move(node));
        mConnectionHanlder.addNode(sourceIndex, targetIndex, connectionType);
        return static_cast<NodeType&>(*mNodes.back());
    }

    void removeNode(size_t nodeIndex)
    {
        assert(nodeIndex < mNodes.size());
        mNodes.erase(mNodes.begin() + nodeIndex);
        mConnectionHanlder.removeNode(nodeIndex);
    }

    void connect(size_t sourceIndex,
                 size_t targetIndex,
                 ConnectionType connectionType)
    {
        mConnectionHanlder.connect(sourceIndex, targetIndex, connectionType);
    }

    template<class NodeType = NodeBaseType, std::enable_if_t<std::is_base_of_v<NodeBaseType, NodeType>>* = Q_NULLPTR>
    NodeType* nodeAt(size_t nodeIndex)
    {
        const auto& nodes = this->nodes();
        assert(nodeIndex < nodes.size());
        auto nodeIt = nodes.cbegin() += nodeIndex;
        if(nodeIt != nodes.cend()){
            const auto& node = *nodeIt;
            if(node){
                return node.get();
            }
        }
        return Q_NULLPTR;
    }

protected:
    const ConnectionHandlerType& _connectionHandler() const{
        return mConnectionHanlder;
    }

private:
    std::vector<std::unique_ptr<NodeBaseType>> mNodes;
    ConnectionHandlerType mConnectionHanlder;
};

namespace InterconnectedMemory {
template<int32_t MaxSize>
class VisualNodeManager :  public Core::NodeManager<Shared::VisualNode, Core::InterconnectedMemory::ConnectionHanlder<MaxSize>>
{
    using Base = Core::NodeManager<Shared::VisualNode, Core::InterconnectedMemory::ConnectionHanlder<MaxSize>>;
    using NodeWrapperType = Shared::InterconnectedMemory::ConnectedVisualNodeWrapper<MaxSize>;

public:
    size_t nodeIndex(const QUuid& nodeId) const{
        const auto& nodes = Base::nodes();
        auto nodeIt = std::find_if(nodes.cbegin(), nodes.cend(), [&nodeId](const std::unique_ptr<Shared::VisualNode>& node){
            return node->id() == nodeId;
        });
        if(nodeIt == nodes.cend()){
            return -1;
        }

        return nodeIt - nodes.cbegin();
    }

    NodeWrapperType wrappedNode(std::size_t nodeIndex) const{
        assert(nodeIndex < MaxSize);

        const auto& nodes = Base::nodes();
        assert(nodeIndex < nodes.size());
        const auto& connectionHandler = Base::_connectionHandler();

        auto nodeIt = nodes.cbegin() += nodeIndex;
        if(nodeIt != nodes.cend()){
            const std::unique_ptr<Shared::VisualNode>& node = *nodeIt;
            if(node){
                size_t nodeIndex = nodeIt - nodes.cbegin();
                auto connections = connectionHandler.nodeConnections(nodeIndex);
                return NodeWrapperType(nodeIndex, std::ref(*node.get()), connections);
            }
        }

        return NodeWrapperType();
    }

    std::vector<NodeWrapperType> wrappedNodes() const{
        std::vector<NodeWrapperType> concreteNodes;
        const auto& nodes = Base::nodes();
        const auto& connectionHandler = Base::_connectionHandler();
        concreteNodes.reserve(nodes.size());

        for(auto nodeIt = nodes.cbegin(); nodeIt != nodes.cend(); ++nodeIt){
            const std::unique_ptr<Shared::VisualNode>& node = *nodeIt;
            if(node){
                size_t nodeIndex = nodeIt - nodes.cbegin();
                auto connections = connectionHandler.nodeConnections(nodeIndex);
                auto wrapper = NodeWrapperType(nodeIndex, std::ref(*node.get()), connections);
                concreteNodes.push_back(std::move(wrapper));
            }
        }

        return concreteNodes;
    }

    std::optional<NodeWrapperType> wrappedNode(const QUuid& nodeId) const{
        const auto& nodes = Base::nodes();
        auto nodeIt = std::find_if(nodes.cbegin(), nodes.cend(), [&nodeId](const std::unique_ptr<Shared::VisualNode>& node){
            return node->id() == nodeId;
        });

        const auto& connectionHandler = Base::_connectionHandler();

        if(nodeIt != nodes.cend()){
            const std::unique_ptr<Shared::VisualNode>& node = *nodeIt;
            if(node){
                size_t nodeIndex = nodeIt - nodes.cbegin();
                auto connections = connectionHandler.nodeConnections(nodeIndex);
                return NodeWrapperType(nodeIndex, std::ref(*node.get()), connections);
            }
        }

        assert(false);
        return std::nullopt;
    }

};
}

}

#endif // NodeManager_H
