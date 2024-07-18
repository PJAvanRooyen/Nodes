#ifndef ConnectionHanlder_H
#define ConnectionHanlder_H

#include <vector>
#include <cassert>
#include <bitset>

namespace Core{
class IConnectionHanlder{
public:
    enum class ConnectionType
    {
        ParentChild,
        Partner,
    };

    virtual void addNode() = 0;

    virtual void addNode(size_t sourceIndex,
                         size_t targetIndex,
                         ConnectionType connectionType) = 0;

    virtual void removeNode(size_t nodeIndex) = 0;

    virtual void connect(size_t sourceIndex,
                         size_t targetIndex,
                         ConnectionType connectionType) = 0;

protected:
    IConnectionHanlder() = default;
};

namespace InterconnectedMemory {

template<int32_t MaxSize>
class ConnectionHanlder : public IConnectionHanlder
{
    static const std::string kOnes;

public:
    using ConnectionType = typename IConnectionHanlder::ConnectionType;
    using ConnectionsContainerUnidirectional = std::vector<std::bitset<MaxSize>>;
    using ParentChildConnections = ConnectionsContainerUnidirectional;
    using ChildParentConnections = ConnectionsContainerUnidirectional;
    using ConnectionsContainer = std::pair<ParentChildConnections, ChildParentConnections>;
    using NodeConnectionsContainerRef = std::pair<std::reference_wrapper<const std::bitset<MaxSize>>, std::reference_wrapper<const std::bitset<MaxSize>>>;

    ConnectionHanlder()
        : IConnectionHanlder()
        , mConnections()
    {
    }

    const ConnectionsContainer& connections() const
    {
        return mConnections;
    }

    NodeConnectionsContainerRef nodeConnections(std::size_t nodeIndex) const
    {
        assert(nodeIndex < MaxSize);
        const std::bitset<MaxSize>& parentChildConnections = mConnections.first[nodeIndex];
        const std::bitset<MaxSize>& childParentConnections = mConnections.second[nodeIndex];
        return std::make_pair(std::ref(parentChildConnections), std::ref(childParentConnections));
    }

    void addNode() override{
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;
        parentChildConnections.push_back({});
        childParentConnections.push_back({});
    }

    void addNode(size_t sourceIndex,
                 size_t targetIndex,
                 ConnectionType connectionType) override{
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;
        assert(parentChildConnections.size() >= sourceIndex);
        assert(childParentConnections.size() >= sourceIndex);
        assert(targetIndex == parentChildConnections.size());
        assert(targetIndex == childParentConnections.size());

        addNode();
        connect(sourceIndex, targetIndex, connectionType);
    }

    void removeNode(size_t nodeIndex) override
    {
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;

        // Remove the node's set of connections.
        parentChildConnections.erase(parentChildConnections.begin() + nodeIndex);
        childParentConnections.erase(childParentConnections.begin() + nodeIndex);

        // Update all nodes' connections to remove this node's index, and re-index all nodes with a greater index.
        // NOTE: while it is tempting to use masks, creating a '1's mask for
        // MaxSize bits could take up a lot of memory when using std::string,
        // or would limit MaxSize to 64 when using ULL.
        for(auto nodeConnIt = childParentConnections.begin(); nodeConnIt != childParentConnections.end(); ++nodeConnIt){
            auto& nodeConns = *nodeConnIt;
            for(size_t i = nodeIndex; i < MaxSize - 1; ++i){
                size_t next = i + 1;
                nodeConns[i] = nodeConns[next];
            }
            nodeConns.reset(MaxSize - 1);
        }
        for(auto nodeConnIt = parentChildConnections.begin(); nodeConnIt != parentChildConnections.end(); ++nodeConnIt){
            auto& nodeConns = *nodeConnIt;
            for(size_t i = nodeIndex; i < MaxSize - 1; ++i){
                size_t next = i + 1;
                nodeConns[i] = nodeConns[next];
            }
            nodeConns.reset(MaxSize - 1);
        }
    }

    void connect(size_t sourceIndex,
                 size_t targetIndex,
                 ConnectionType connectionType) override
    {
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;
        assert(parentChildConnections.size() >= sourceIndex);
        assert(childParentConnections.size() >= sourceIndex);
        assert(parentChildConnections.size() >= targetIndex);
        assert(childParentConnections.size() >= targetIndex);

        switch (connectionType) {
            case ConnectionType::Partner:{
                auto& nodeChildren = parentChildConnections[targetIndex];
                nodeChildren.set(sourceIndex);

                auto& sourceParents = childParentConnections[sourceIndex];
                sourceParents.set(targetIndex);
                [[fallthrough]];
            }
            case ConnectionType::ParentChild:{
                auto& sourceChildren = parentChildConnections[sourceIndex];
                sourceChildren.set(targetIndex);

                auto& nodeParents = childParentConnections[targetIndex];
                nodeParents.set(sourceIndex);
            }break;
        }
    }

private:
    ConnectionsContainer mConnections;
};

}

}

#endif // ConnectionHanlder_H
