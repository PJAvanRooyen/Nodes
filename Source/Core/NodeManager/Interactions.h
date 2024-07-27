#ifndef Interactions_H
#define Interactions_H

#include "StaticTypedefs.h"

#include <QRectF>

namespace Core {

template<class ItemType>
using InteractFn = bool (*)(ItemType* item, std::vector<ItemType*>& neighbours);

namespace InteractFnExample {
template<class ItemType>
bool collidingCircles(ItemType* item, std::vector<ItemType*>& neighbours){
    auto collideOvals = [](const QRectF& itemRect, const QRectF& neighbourRect) -> QPointF {
        QPointF offset = itemRect.center() - neighbourRect.center();
        double distance = std::hypot(offset.x(), offset.y());

        // TODO: improve this to be for ovals, not just circles.
        double desiredDistance = (itemRect.width() + neighbourRect.width())/2.0;
        double moveDistance = 0.0;
        if(desiredDistance > distance){
            moveDistance = desiredDistance - distance;
        }
        QPointF moveVector = offset;
        if(qFuzzyIsNull(distance)){
            // If items are prefectly overlapping, then just move right.
            moveVector = QPointF(moveDistance, 0.0);
        }else{
            moveVector *= moveDistance/distance;
        }

        return moveVector;
    };

    bool isSolved = true;
    QRectF rect = item->rect();

    //! the movement it wants to make to avoid existing collisions.
    QPointF displacementSum;
    for(auto* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        QPointF displacement = collideOvals(rect, neighbourRect);
        displacementSum += displacement;
    }

    if(qFuzzyIsNull(displacementSum.x()) && qFuzzyIsNull(displacementSum.y())){
        return isSolved;
    }

    //! the movement it wants to make afterwards to avoid new collisions.
    QRectF movedRect = rect.translated(displacementSum);
    QPointF postMoveDisplacementSum;
    for(auto* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        QPointF displacement = collideOvals(movedRect, neighbourRect);
        postMoveDisplacementSum += displacement;
    }

    if(qFuzzyIsNull(postMoveDisplacementSum.x()) && qFuzzyIsNull(postMoveDisplacementSum.y())){
        rect.translate(displacementSum);
        isSolved = true;
    }else{
        // Move the rect to:
        // - the movement it wants to make to avoid existing collisions.
        // - half the movement it wants to make afterwards to avoid new collisions.
        QPointF displacement = displacementSum + (postMoveDisplacementSum / 2.0);
        rect.translate(displacement);

        // This final result will result in some minor overlap,
        // therfore the interaction has not yet been fully solved.
        isSolved = false;
    }
    item->setRect(rect);
    return isSolved;
}
}

namespace InteractFnExample {
template<class ItemType>
bool mindmapInteractions(ItemType* item, std::vector<ItemType*>& neighbours){
    auto interactCircles = [](const QRectF& itemRect, const QRectF& neighbourRect, bool isConnected) -> QPointF {
        QPointF offset = itemRect.center() - neighbourRect.center();
        double distance = std::hypot(offset.x(), offset.y());

        double spacing = 0;
        if(isConnected){
            // Add spacing where nodes are separated by a gap equal to the smaller circle's radius.
            spacing = std::min(itemRect.width(), neighbourRect.width()) / 2.0;
        }else{
            // Add spacing where nodes are separated by a gap equal to the larger circle's diameter.
            spacing = std::max(itemRect.width(), neighbourRect.width());
        }

        double desiredDistance = spacing + (itemRect.width() + neighbourRect.width())/2.0;
        double moveDistance = 0.0;
        // NOTE: connected nodes can pull each other closer, not just push.
        if(desiredDistance > distance || isConnected){
            moveDistance = desiredDistance - distance;
        }
        QPointF moveVector = offset;
        if(qFuzzyIsNull(distance)){
            // If items are prefectly overlapping, then just move right.
            moveVector = QPointF(moveDistance, 0.0);
        }else{
            moveVector *= moveDistance/distance;
        }

        return moveVector;
    };

    bool isSolved = true;
    QRectF rect = item->rect();

    //! the movement it wants to make to avoid existing collisions.
    QPointF displacementSum;
    for(auto* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        bool isConnected = item->isConnectedTo(neighbour->index());
        QPointF displacement = interactCircles(rect, neighbourRect, isConnected);
        displacementSum += displacement;
    }

    if(qFuzzyIsNull(displacementSum.x()) && qFuzzyIsNull(displacementSum.y())){
        return isSolved;
    }

    //! the movement it wants to make afterwards to avoid new collisions.
    QRectF movedRect = rect.translated(displacementSum);
    QPointF postMoveDisplacementSum;
    for(auto* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        bool isConnected = item->isConnectedTo(neighbour->index());
        QPointF displacement = interactCircles(movedRect, neighbourRect, isConnected);
        postMoveDisplacementSum += displacement;
    }

    if(qFuzzyIsNull(postMoveDisplacementSum.x()) && qFuzzyIsNull(postMoveDisplacementSum.y())){
        rect.translate(displacementSum);
        isSolved = true;
    }else{
        // Move the rect to:
        // - the movement it wants to make to avoid existing collisions.
        // - half the movement it wants to make afterwards to avoid new collisions.
        QPointF displacement = displacementSum + (postMoveDisplacementSum / 2.0);
        rect.translate(displacement);

        // This final result will result in some minor overlap,
        // therfore the interaction has not yet been fully solved.
        isSolved = false;
    }
    item->setRect(rect);
    return isSolved;
}
}

}
#endif // Interactions_H
