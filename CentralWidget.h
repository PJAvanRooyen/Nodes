#ifndef CentralWidget_H
#define CentralWidget_H

#include "CentralWidgetView.h"

#include <QPointer>
#include <QWidget>
#include <QUuid>

namespace UI {

class CentralWidget : public QWidget
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
    CentralWidget(QWidget *parent = nullptr);

    QPointer<CentralWidgetView> view();

    void reset();

    void addNode(QUuid id, QRectF rect, QString text = QString(), QString tooltip = QString());

    void removeNode(const QUuid& id);

    void addConnection(QUuid nodeId1, QUuid nodeId2,  QString text = QString(), QString tooltip = QString());

private:
    std::unique_ptr<CentralWidgetView> mView;
};

}
#endif // CentralWidget_H
