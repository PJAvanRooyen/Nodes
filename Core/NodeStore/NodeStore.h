#ifndef NodeStore_H
#define NodeStore_H

#include "NodeManager.h"

#include <QObject>
#include <QRectF>
#include <QString>
#include <QUuid>

namespace Core {
class NodeStore : public QObject
{
    Q_OBJECT

public:
    NodeStore();
    virtual ~NodeStore() = default;

    void init();

protected:
    void customEvent(QEvent* event) override;

private:
    void addNode(QRectF rect, QString text = QString(), QString tooltip = QString());

    void removeNode(QUuid nodeId);

    void connect(QUuid node1Id, QUuid node2Id, QString text = QString(), QString tooltip = QString());

private:
    InterconnectedMemory::VisualNodeManager<100> mNodeManager;
};
}
#endif // NodeStore_H
