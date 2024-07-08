#include "Scene.h"

namespace UI{

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
{
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseDoubleClickEvent(event);
}

}
