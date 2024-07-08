#include "CentralWidgetView.h"
#include "GraphicsNode.h"

#include <QMouseEvent>

namespace UI{

CentralWidgetView::CentralWidgetView(QWidget *parent)
    : QGraphicsView(parent)
    , mScene(new Scene(this))
{
    setMouseTracking(true);
    setScene(mScene);
    setRenderHint(QPainter::Antialiasing);
}

void CentralWidgetView::addNode(){
    auto* node = new Node();
    node->setRect(QRectF(QPointF(0, 0), QSizeF(100, 100)));

    mScene->addItem(node);
}

void CentralWidgetView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void CentralWidgetView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QGraphicsView::mouseDoubleClickEvent(event);

    QPointF scenePos = mapToScene(event->pos());

    static const QSizeF kNodeSize = QSizeF(100, 100);
    static const qreal kNodeRadius = kNodeSize.width()/2.0;
    auto* node = new NodeDescriptive("text", "details");
    node->setRect(QRectF(QPointF(scenePos.x() - kNodeRadius, scenePos.y() - kNodeRadius), kNodeSize));

    node->setPen(QPen(Qt::black));
    node->setBrush(Qt::red);

    scene()->addItem(node);
}

void CentralWidgetView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    mScene->setSceneRect(rect());
}


}
