#ifndef EvInteraction_H
#define EvInteraction_H

#include "AbstractEvent.h"
#include "Node/Node.h"

#include <vector>
#include <memory>

namespace Shared::EventSystem {

struct EvInteractionsSolveReq : public EventSystem::Event<EvInteractionsSolveReq>
{
    using NodeType = Shared::IVisualNodeWrapper;

    EvInteractionsSolveReq(
        std::vector<QUuid> nodeIdSubset = {},
        std::vector<QUuid> immovableNodeIds = {},
        std::vector<QUuid> quiescentNodeIds = {})
        : Event<EvInteractionsSolveReq>()
        , nodeIdSubset(std::move(nodeIdSubset))
        , immovableNodeIds(std::move(immovableNodeIds))
        , quiescentNodeIds(std::move(quiescentNodeIds)){}

    /*!
     * \brief nodes that should participate in interactions.
     * \note this is the opposite of quiescentNodeIds.
     * \note if empty, then all nodes should be interacting,
     * excluding the quiescentNodeIds.
     */
    const std::vector<QUuid> nodeIdSubset;

    /*!
     * \brief nodes that will not change their position while interacting.
     * \details these nodes can still be interacted with e.g. by causing other nodes to move,
     * but these nodes will not move.
     */
    const std::vector<QUuid> immovableNodeIds;

    /*!
     * \brief nodes that will not interact with or be interacted with.
     */
    const std::vector<QUuid> quiescentNodeIds;
};

struct EvInteractionsSolveResp : public EventSystem::Event<EvInteractionsSolveResp>
{
    using NodeType = Shared::IVisualNodeWrapper;

    EvInteractionsSolveResp(std::vector<std::shared_ptr<NodeType>> nodes)
        : Event<EvInteractionsSolveResp>()
        , nodes(std::move(nodes)){};

    std::vector<std::shared_ptr<NodeType>> nodes;
};

}

#endif // EvInteraction_H
