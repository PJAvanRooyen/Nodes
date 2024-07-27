#ifndef NodeArea_H
#define NodeArea_H

#include "NodeAreaView.h"

#include <QPointer>
#include <QWidget>
#include <QUuid>

namespace UI {

class NodeArea : public QWidget
{
    Q_OBJECT

Q_SIGNALS:
    void nodeAdd(QRectF rect, QString text, QString tooltip);

    void nodeRemove(QUuid nodeId);

    void connectionAdd(QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip);

private Q_SLOTS:
    void onNodeAdd(QRectF rect, QString text, QString tooltip);

    void onNodeRemove(QUuid nodeId);

    void onConnectionAdd(QUuid nodeId1, QUuid nodeId2, QString text, QString tooltip);

public:
    NodeArea(QWidget *parent = nullptr);

    QPointer<NodeAreaView> view();

    void reset();

    void addNode(QUuid id, QRectF rect, QString text = QString(), QString tooltip = QString());

    void removeNode(const QUuid& id);

    void addConnection(QUuid nodeId1, QUuid nodeId2,  QString text = QString(), QString tooltip = QString());

protected:
    void customEvent(QEvent* event) override;

private:
    std::unique_ptr<NodeAreaView> mView;
};

}
#endif // NodeArea_H
