#ifndef NodeAreaView_H
#define NodeAreaView_H

#include "Node/Node.h"
#include "Scene.h"

#include <QPointer>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QUuid>

namespace UI {

class NodeAreaView : public QGraphicsView
{
    Q_OBJECT

Q_SIGNALS:
    void nodeAdd(QRectF rect, QString text = QString(), QString tooltip = QString());

    void nodeRemove(QUuid nodeId);

    void connectionAdd(QUuid nodeId1, QUuid nodeId2, QString text = QString(), QString tooltip = QString());

public:
    NodeAreaView(QWidget *parent = nullptr);

    void reset();

    void addNode(QUuid id, QRectF rect, QString text = QString(), QString tooltip = QString());

    bool removeNode(const QUuid& id);

    void addConnection(QUuid nodeId1, QUuid nodeId2,  QString text = QString(), QString tooltip = QString());

    void update(std::vector<std::shared_ptr<Shared::IVisualNodeWrapper>> nodes);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QPointer<Scene> mScene;

    QGraphicsItem* mDragStartItem;
    QGraphicsLineItem* mTempConnectionLine;
};

}
#endif // NodeAreaView_H
