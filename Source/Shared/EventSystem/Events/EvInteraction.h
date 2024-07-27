#ifndef EvInteraction_H
#define EvInteraction_H

#include "AbstractEvent.h"
#include "Node/Node.h"

#include <vector>

namespace Shared::EventSystem {

struct EvInteractionsSolveReq : public EventSystem::Event<EvInteractionsSolveReq>
{
    using NodeType = Shared::InterconnectedMemory::ConnectedVisualNodeWrapper<100>;

    EvInteractionsSolveReq(std::vector<NodeType> nodes)
        : Event<EvInteractionsSolveReq>()
        , nodes(std::move(nodes)){}

    const std::vector<NodeType> nodes;
};

}

#endif // EvInteraction_H
