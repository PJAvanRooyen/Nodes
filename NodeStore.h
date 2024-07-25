#ifndef NodeStore_H
#define NodeStore_H

#include "NodeManager.h"

#include <QObject>

namespace Core {
class NodeStore : public QObject
{
    Q_OBJECT

public:
    NodeStore();
    virtual ~NodeStore() = default;

protected:
    void customEvent(QEvent* event) override;

private:
    InterconnectedMemory::VisualNodeManager<100> mNodeManager;
};
}
#endif // NodeStore_H
