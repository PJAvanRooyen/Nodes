#ifndef CentralWidgetView_H
#define CentralWidgetView_H

#include "Scene.h"

#include <QPointer>
#include <QGraphicsView>

namespace UI {

class CentralWidgetView : public QGraphicsView
{
    Q_OBJECT

Q_SIGNALS:


public:
    CentralWidgetView(QWidget *parent = nullptr);

    void addNode();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    QPointer<Scene> mScene;

};

}
#endif // CentralWidgetView_H
