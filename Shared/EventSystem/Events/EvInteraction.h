#ifndef EvInteraction_H
#define EvInteraction_H

#include "AbstractEvent.h"

#include <QRectF>
#include <QString>
#include <QUuid>

namespace Shared::EventSystem {

struct EvInteractionsSolveReq : public EventSystem::Event<EvInteractionsSolveReq>
{
    EvInteractionsSolveReq()
      : Event<EvInteractionsSolveReq>(){}
};

}

#endif // EvInteraction_H
