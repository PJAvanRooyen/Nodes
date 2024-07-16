#ifndef CentralWidget_H
#define CentralWidget_H

#include "CentralWidgetView.h"

#include <QPointer>
#include <QWidget>

namespace UI {

class CentralWidget : public QWidget
{
    Q_OBJECT

Q_SIGNALS:
    void nodeAdd(QRectF rect, QString text, QString tooltip);

    void connectionAdd(QVariant nodeId1, QVariant nodeId2, QString text, QString tooltip);

private Q_SLOTS:
    void onNodeAdd(QRectF rect, QString text, QString tooltip);

    void onConnectionAdd(QVariant nodeId1, QVariant nodeId2, QString text, QString tooltip);

public:
    CentralWidget(QWidget *parent = nullptr);

    QPointer<CentralWidgetView> view();

    void addNode(QVariant id, QRectF rect, QString text = QString(), QString tooltip = QString());

    void addConnection(QVariant nodeId1, QVariant nodeId2,  QString text = QString(), QString tooltip = QString());

private:
    std::unique_ptr<CentralWidgetView> mView;
};

}
#endif // CentralWidget_H
