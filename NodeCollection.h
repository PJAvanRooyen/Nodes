#ifndef NodeCollection_H
#define NodeCollection_H

#include "NodeGroup.h"

namespace Shared{

template<class INodeGroupType = Shared::INodeGroup<Shared::INode<uint64_t>>>
class NodeCollection
{
    using INodeGroup = INodeGroupType;
    using INode = typename INodeGroup::INodeType;
    using ConnectionType = typename INodeGroup::ConnectionType;

public:
    NodeCollection();

    void addNode(std::unique_ptr<INode> node)
    {
        addNodeGroup(std::move(node));
    }

protected:
    void addNodeGroup(std::unique_ptr<INode> node)
    {
        mNodeGroups.push_back(std::make_unique<INodeGroupType>(std::move(node)));
    }

private:
    std::vector<std::unique_ptr<INodeGroupType>> mNodeGroups;
};

}

#endif // NodeCollection_H
