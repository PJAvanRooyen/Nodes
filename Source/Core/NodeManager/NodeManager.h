#ifndef NodeManager_H
#define NodeManager_H

#include "InteractionHandler.h"
#include "NodeHandler.h"

#include <QObject>
#include <QRectF>
#include <QString>
#include <QUuid>

namespace Core {
class NodeManager : public QObject
{
    Q_OBJECT

public:
    NodeManager();
    virtual ~NodeManager() = default;

    void init();

protected:
    void customEvent(QEvent* event) override;

private:
    void addNode(QRectF rect, QString text = QString(), QString tooltip = QString());

    void removeNode(QUuid nodeId);

    void connect(QUuid node1Id, QUuid node2Id, QString text = QString(), QString tooltip = QString());

private:
    InterconnectedMemory::VisualNodeManager<100> mNodeHandler;
    InteractionHandler mInteractionHandler;
};
}
#endif // NodeManager_H
