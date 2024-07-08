#ifndef NodeGroup_H
#define NodeGroup_H

#include "Node.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include <bitset>

namespace Shared{
template<class INodeType = Shared::INode<uint64_t>>
class INodeGroup{
public:
    using INode = INodeType;

    enum class ConnectionType
    {
        ParentChild,
        Partner,
    };

    virtual void addNode(std::unique_ptr<INode>& sourceNode,
                         std::unique_ptr<INode> node,
                         ConnectionType connectionType) = 0;
};

namespace NodeGroupInterconnectedMemory {

template<int32_t MaxSize, class INodeType = Shared::INode<uint64_t>>
class NodeGroup : public INodeGroup<INodeType>
{
public:
    using INode = typename INodeGroup<INodeType>::INode;
    using ConnectionType = typename INodeGroup<INodeType>::ConnectionType;

    NodeGroup(std::unique_ptr<INode> initialNode)
        : mNodes()
        , mParentChildConnections()
        , mChildParentConnections()
    {
        mNodes.push_back(std::move(initialNode));
        mParentChildConnections.push_back();
        mChildParentConnections.push_back();
    }

    void addNode(std::unique_ptr<INode>& sourceNode,
                 std::unique_ptr<INode> node,
                 ConnectionType connectionType)
    {
        mNodes.push_back(std::move(node));
        mParentChildConnections.push_back();
        mChildParentConnections.push_back();
        connect(sourceNode, mNodes.back(), connectionType);
    }

    void connect(std::unique_ptr<INode>& sourceNode,
                 std::unique_ptr<INode>& node,
                 ConnectionType connectionType)
    {
        assert((!mParentChildConnections.empty()));
        assert((!mChildParentConnections.empty()));

        auto sourceIndex = indexOf(sourceNode);
        auto nodeIndex = indexOf(node);

        assert((mParentChildConnections.size() >= sourceIndex));
        assert((mChildParentConnections.size() >= sourceIndex));

        switch (connectionType) {
        case ConnectionType::Partner:{
            auto& nodeChildren = mParentChildConnections[nodeIndex];
            nodeChildren.set(sourceIndex);

            auto& sourceParents = mChildParentConnections[sourceIndex];
            sourceParents.set(nodeIndex);
            [[fallthrough]];
        }
        case ConnectionType::ParentChild:{
            auto& sourceChildren = mParentChildConnections[sourceIndex];
            sourceChildren.set(nodeIndex);

            auto& nodeParents = mChildParentConnections[nodeIndex];
            nodeParents.set(sourceIndex);
        }break;
        }
    }

    int32_t indexOf(const std::unique_ptr<INode>& node) const
    {
        assert((!mNodes.empty()));

        // Assume most common case first, then check remaining.
        const auto& nodeId = node->id();
        if(mNodes.back()->id() == nodeId){
            return mNodes.size() - 1;
        }

        auto nodeIt = std::find_if(mNodes.cbegin(), mNodes.cend(), [&nodeId](const std::unique_ptr<INode>& storedNode){
            return storedNode->id() == nodeId;
        } );
        if(nodeIt == mNodes.cend()){
            return -1;
        }
        return nodeIt - mNodes.cbegin();
    }

private:
    std::vector<std::unique_ptr<INode>> mNodes;

    std::vector<std::bitset<MaxSize>> mParentChildConnections;
    std::vector<std::bitset<MaxSize>> mChildParentConnections;
};

}

}

#endif // NodeGroup_H
