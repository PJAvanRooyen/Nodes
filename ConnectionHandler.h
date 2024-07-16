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

    virtual void addNode(std::size_t sourceIndex,
                         std::size_t targetIndex,
                         ConnectionType connectionType) = 0;

    virtual void connect(std::size_t sourceIndex,
                         std::size_t targetIndex,
                         ConnectionType connectionType) = 0;

protected:
    IConnectionHanlder() = default;
};

namespace InterconnectedMemory {

template<int32_t MaxSize>
class ConnectionHanlder : public IConnectionHanlder
{
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

    void addNode(){
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;
        parentChildConnections.push_back({});
        childParentConnections.push_back({});
    }

    void addNode(std::size_t sourceIndex,
                 std::size_t targetIndex,
                 ConnectionType connectionType){
        auto& parentChildConnections = mConnections.first;
        auto& childParentConnections = mConnections.second;
        assert(parentChildConnections.size() >= sourceIndex);
        assert(childParentConnections.size() >= sourceIndex);
        assert(targetIndex == parentChildConnections.size());
        assert(targetIndex == childParentConnections.size());

        addNode();
        connect(sourceIndex, targetIndex, connectionType);
    }

    void connect(std::size_t sourceIndex,
                 std::size_t targetIndex,
                 ConnectionType connectionType)
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
