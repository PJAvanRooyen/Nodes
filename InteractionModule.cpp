#include "InteractionModule.h"

namespace Core{

InteractionModule::InteractionModule()
    : mInteractionHandler(Core::InteractionHandler::ItemsHaveVariableSize)
{
    mInteractionHandler.setMaxInteractionDistance(100);
}


}
