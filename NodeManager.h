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
    std::vector<NodeType*> nodes() const{
        std::vector<NodeType*> concreteNodes;
        concreteNodes.reserve(mNodes.size());
        for(const auto& node : mNodes){
            if(NodeType* concreteNode = dynamic_cast<NodeType*>(node.get())){
                concreteNodes.push_back(concreteNode);
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

private:
    std::vector<std::unique_ptr<INode>> mNodes;
    ConnectionHandlerType mConnectionHanlder;
};

}

#endif // NodeManager_H
