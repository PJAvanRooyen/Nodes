#ifndef CentralWidgetView_H
#define CentralWidgetView_H

#include "Scene.h"

#include <QPointer>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QUuid>

namespace UI {

class CentralWidgetView : public QGraphicsView
{
    Q_OBJECT

Q_SIGNALS:
    void nodeAdd(QRectF rect, QString text = QString(), QString tooltip = QString());

    void connectionAdd(QUuid nodeId1, QUuid nodeId2, QString text = QString(), QString tooltip = QString());

public:
    CentralWidgetView(QWidget *parent = nullptr);

    void addNode(QUuid id, QRectF rect, QString text = QString(), QString tooltip = QString());

    void addConnection(QUuid nodeId1, QUuid nodeId2,  QString text = QString(), QString tooltip = QString());

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    QPointer<Scene> mScene;

    QGraphicsItem* mDragStartItem;
    QGraphicsLineItem* mTempConnectionLine;
};

}
#endif // CentralWidgetView_H
