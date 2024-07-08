#ifndef Scene_H
#define Scene_H

#include <QGraphicsScene>

namespace UI {

class Scene : public QGraphicsScene
{
public:
    Scene(QObject *parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

}
#endif // Scene_H
