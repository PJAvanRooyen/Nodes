#ifndef CollisionHandler_H
#define CollisionHandler_H

#include "StaticTypedefs.h"

#include <QFlags>
#include <QRectF>

#include <set>

namespace Core {

class ICollisionHanlder
{
public:
    enum ItemFlag{
        ItemsHaveVariableSize = 1,
    };
    Q_DECLARE_FLAGS(ItemFlags, ItemFlag)

    virtual ~ICollisionHanlder() = default;

protected:
    ICollisionHanlder(){

    }
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ICollisionHanlder::ItemFlags)

template<class ItemType>
using InteractFn = void (*)(ItemType* item, std::vector<ItemType*>& neighbours);

namespace InteractFnExample {
template<class ItemType>
void collidingOvals(ItemType* item, std::vector<ItemType*>& neighbours){
    auto collideOvals = [](const QRectF& itemRect, const QRectF& neighbourRect) -> QPointF {
        QPointF offset = itemRect.center() - neighbourRect.center();
        double distance = std::hypot(offset.x(), offset.y());

        // TODO: improve this to be for ovals, not just circles.
        double desiredDistance = (itemRect.width() + neighbourRect.width())/2.0;
        double moveDistance = 0.0;
        if(desiredDistance > distance){
            moveDistance = desiredDistance - distance;
        }
        QPointF moveVector = offset * moveDistance/distance;

        return moveVector;
    };

    QRectF rect = item->rect();

    //! the movement it wants to make to avoid existing collisions.
    QPointF displacementSum;
    for(ItemType* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        QPointF displacement = collideOvals(rect, neighbourRect);
        displacementSum += displacement;
    }

    if(qFuzzyIsNull(displacementSum.x()) && qFuzzyIsNull(displacementSum.y())){
        return;
    }

    //! the movement it wants to make afterwards to avoid new collisions.
    QRectF movedRect = rect.translated(displacementSum);
    QPointF postMoveDisplacementSum;
    for(ItemType* neighbour : neighbours){
        QRectF neighbourRect = neighbour->rect();
        QPointF displacement = collideOvals(movedRect, neighbourRect);
        postMoveDisplacementSum += displacement;
    }

    if(qFuzzyIsNull(postMoveDisplacementSum.x()) && qFuzzyIsNull(postMoveDisplacementSum.y())){
        rect.translate(displacementSum);
    }else{
        // Move the rect to the average position between:
        // - the movement it wants to make to avoid existing collisions.
        // - the movement it wants to make afterwards to avoid new collisions.
        QPointF avgMove = (displacementSum + postMoveDisplacementSum) / 2.0;
        rect.translate(avgMove);
    }
    item->setRect(rect);
}
}

template <class ItemType>
class CollisionHandler : public ICollisionHanlder
{
public:
    CollisionHandler(ItemFlags flags = ItemFlag::ItemsHaveVariableSize)
        : mItemFlags(flags)
        , mMaxInteractionDistance(0)
        , mInteractFn(&InteractFnExample::collidingOvals)
    {
    }

    void setMaxInteractionDistance(int32_t interactionDistance){
        mMaxInteractionDistance = interactionDistance;
    }

    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \param items, the items being moved.
     */
    template <template<typename...> class ListType, typename Arg>
    void solve(ListType<Arg*>& items) const{
        if(mItemFlags & ItemFlag::ItemsHaveVariableSize){
            solveVariableSize(items);
        }else{
            // TODO: solve without needing to measure every item.
            solveVariableSize(items);
        }
    }

private:
    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \details this optimization would work better for small items that are far apart.
     * \note this solution supports variable sized elements.
     * \param items, the items being moved.
     */
    template <template<typename...> class ListType>
    void solveVariableSize(ListType<ItemType*>& items) const{
        static_assert(std::is_same_v<ItemType, ItemType>);
        static_assert(is_list<ListType<ItemType*>>());
        static_assert(has_rect<ItemType>());
        //static_assert(has_setRect<Arg>());

        const size_t itemCount = items.size();
        std::vector<std::pair<double, size_t>> xPositions;
        std::vector<std::pair<double, size_t>> yPositions;
        std::pair<double /*x*/, double /*y*/> largestSize;
        xPositions.reserve(itemCount);
        yPositions.reserve(itemCount);

        // Create sorted lists of positions,
        // this is used to efficiently calculate item proximities.
        size_t index = 0;
        for (const auto* item : items) {
            const auto& rect = item->rect();
            const auto& itemPos = rect.topLeft();
            xPositions.emplace_back(itemPos.x(), index);
            yPositions.emplace_back(itemPos.y(), index);

            const auto width = rect.width();
            if(width > largestSize.first){
                largestSize.first = width;
            }
            const auto height = rect.height();
            if(height > largestSize.second){
                largestSize.second = height;
            }

            ++index;
        }
        std::sort(xPositions.begin(), xPositions.end());
        std::sort(yPositions.begin(), yPositions.end());

        // For each item, get its neighbours that are within its range of influence.
        size_t itemXIndex = 0;
        for (auto xPosItemIndexIt = xPositions.cbegin(); xPosItemIndexIt != xPositions.cend(); ++xPosItemIndexIt) {
            const auto& xPosItemIndex = *xPosItemIndexIt;
            size_t itemIndex = xPosItemIndex.second;
            auto* item = items.at(itemIndex);

            const auto& itemRect = item->rect();
            const auto& itemPos = itemRect.topLeft();
            const double itemXPos = itemPos.x();
            const double itemYPos = itemPos.y();
            const auto& itemWidth = itemRect.width();
            const auto& itemHeight = itemRect.height();

            // get the item's index within the sorted list of y positions.
            auto yPosItemIndexIt =
                std::find_if(yPositions.begin(), yPositions.end(),
                             [itemIndex](const auto& yPosItemIndex){
                                 return yPosItemIndex.second == itemIndex;
                             });
            size_t itemYIndex = yPosItemIndexIt - yPositions.begin();

            double lowestXPos = itemXPos - mMaxInteractionDistance - largestSize.first;
            double highestXPos = itemXPos + itemWidth + mMaxInteractionDistance + largestSize.first;
            double lowestYPos = itemYPos - itemHeight - mMaxInteractionDistance - largestSize.second;
            double highestYPos = itemYPos + mMaxInteractionDistance + largestSize.second;

            std::set<size_t> checkedIndices;
            std::vector<ItemType*> neighbours;
            //! from itemXIndex in xPositions, iterate backwards until the iterated item's right boarder is < lowestXPos
            if(xPosItemIndexIt != xPositions.cbegin()){
                auto prevXPosItemIndexIt = xPosItemIndexIt - 1;
                while(prevXPosItemIndexIt >= xPositions.cbegin()){
                    const auto& earlerXPosItemIndex = *prevXPosItemIndexIt;
                    const size_t earlierItemIndex = earlerXPosItemIndex.second;
                    if(checkedIndices.find(earlierItemIndex) != checkedIndices.end()){
                        --prevXPosItemIndexIt;
                        continue;
                    }
                    checkedIndices.insert(earlierItemIndex);
                    auto* earlierItem = items.at(earlierItemIndex);
                    const auto& rect = earlierItem->rect();
                    const auto& pos = rect.topLeft();
                    auto x = pos.x();
                    auto y = pos.y();
                    const auto& width = rect.width();
                    const auto& height = rect.height();

                    if(x + width >= lowestXPos &&
                        y >= lowestYPos &&
                        y - height <= highestYPos){
                        neighbours.push_back(earlierItem);
                    }else{
                        break;
                    }
                    --prevXPosItemIndexIt;
                }
            }
            //! from itemXIndex in xPositions, iterate forward until the iterated item's left boarder is > highestXPos
            if(xPosItemIndexIt != xPositions.cend()){
                auto laterXPosItemIndexIt = xPosItemIndexIt + 1;
                while(laterXPosItemIndexIt != xPositions.cend()){
                    const auto& laterXPosItemIndex = *laterXPosItemIndexIt;
                    const size_t laterItemIndex = laterXPosItemIndex.second;
                    if(checkedIndices.find(laterItemIndex) != checkedIndices.end()){
                        ++laterXPosItemIndexIt;
                        continue;
                    }
                    checkedIndices.insert(laterItemIndex);
                    auto* laterItem = items.at(laterItemIndex);
                    const auto& rect = laterItem->rect();
                    const auto& pos = rect.topLeft();
                    auto x = pos.x();
                    auto y = pos.y();
                    const auto& height = rect.height();

                    if(x <= highestXPos &&
                        y >= lowestYPos &&
                        y - height <= highestYPos){
                        neighbours.push_back(laterItem);
                    }else{
                        break;
                    }
                    ++laterXPosItemIndexIt;
                }
            }
            //! from itemYIndex in yPositions, iterate backwards until the iterated item's top is < lowestYPos
            if(yPosItemIndexIt != yPositions.cbegin()){
                auto prevYPosItemIndexIt = yPosItemIndexIt - 1;
                while(prevYPosItemIndexIt >= yPositions.cbegin()){
                    const auto& earlerYPosItemIndex = *prevYPosItemIndexIt;
                    const size_t earlierItemIndex = earlerYPosItemIndex.second;
                    if(checkedIndices.find(earlierItemIndex) != checkedIndices.end()){
                        --prevYPosItemIndexIt;
                        continue;
                    }
                    checkedIndices.insert(earlierItemIndex);
                    auto* earlierItem = items.at(earlierItemIndex);
                    const auto& rect = earlierItem->rect();
                    const auto& pos = rect.topLeft();
                    auto x = pos.x();
                    auto y = pos.y();
                    const auto& width = rect.width();

                    if(y >= lowestYPos &&
                        x + width >= lowestXPos &&
                        x <= highestXPos){
                        neighbours.push_back(earlierItem);
                    }else{
                        break;
                    }
                    --prevYPosItemIndexIt;
                }
            }
            //! from itemYIndex in yPositions, iterate forward until the iterated item's bottom is > highestYPos
            if(yPosItemIndexIt != yPositions.cend()){
                auto laterYPosItemIndexIt = yPosItemIndexIt + 1;
                while(laterYPosItemIndexIt != yPositions.cend()){
                    const auto& laterYPosItemIndex = *laterYPosItemIndexIt;
                    const size_t laterItemIndex = laterYPosItemIndex.second;
                    if(checkedIndices.find(laterItemIndex) != checkedIndices.end()){
                        ++laterYPosItemIndexIt;
                        continue;
                    }
                    checkedIndices.insert(laterItemIndex);
                    auto* laterItem = items.at(laterItemIndex);
                    const auto& rect = laterItem->rect();
                    const auto& pos = rect.topLeft();
                    auto x = pos.x();
                    auto y = pos.y();
                    const auto& width = rect.width();
                    const auto& height = rect.height();

                    if(y - height <= highestYPos &&
                        x + width >= lowestXPos &&
                        x <= highestXPos){
                        neighbours.push_back(laterItem);
                    }else{
                        break;
                    }
                }
                ++laterYPosItemIndexIt;
            }

            mInteractFn(item, neighbours);

            ++itemXIndex;
        }
    }

private:
    /*!
     * \brief flags to indicate the type of items being handled.
     * \details if e.g. we know that all items will be the same size,
     * then collision handling calculations can be simplified.
     */
    ItemFlags mItemFlags;

    /*!
     * \brief the distance the item checks around itself to look for colliding items.
     * \example for a distance of 50,
     *          an item will check 50px away from its own corners for overlap with other items' corners.
     */
    int32_t mMaxInteractionDistance;

    /*!
     * \brief the function that dictates how an item should interact with its neighbours.
     */
    InteractFn<ItemType> mInteractFn;
};

}
#endif // CollisionHandler_H
