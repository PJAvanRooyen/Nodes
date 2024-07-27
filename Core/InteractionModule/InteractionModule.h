#ifndef InteractionModule_H
#define InteractionModule_H

#include "InteractionHandler.h"

#include <QObject>

namespace Core {
class InteractionModule : public QObject
{
    Q_OBJECT

public:
    InteractionModule();
    virtual ~InteractionModule() = default;

private:
    InteractionHandler mInteractionHandler;
};
}
#endif // InteractionModule_H
