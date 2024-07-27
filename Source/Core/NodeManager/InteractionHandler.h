#ifndef InteractionHandler_H
#define InteractionHandler_H

#include "StaticTypedefs.h"
#include "Interactions.h"

#include <QFlags>

#include <set>

namespace Core {

class InteractionHandler
{
public:
    enum ItemFlag{
        ItemsHaveVariableSize = 1,
    };
    Q_DECLARE_FLAGS(ItemFlags, ItemFlag)

    InteractionHandler(ItemFlags flags = ItemFlag::ItemsHaveVariableSize)
        : mItemFlags(flags)
        , mMaxInteractionDistance(0)
    {
    }

    void setMaxInteractionDistance(int32_t interactionDistance){
        mMaxInteractionDistance = interactionDistance;
    }

    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \param items, the items being moved.
     * \param interactFn, the function that dictates how an item should interact with its neighbours.
     * \return true if all interactions resulted in a state where there is no more interaction required.
     */
    template <template<typename...> class ListType, typename ItemType, std::enable_if_t<!std::is_pointer_v<ItemType> && !is_unique_ptr_v<ItemType> && !is_shared_ptr_v<ItemType>>* = Q_NULLPTR>
    bool solve(ListType<ItemType>& items, InteractFn<ItemType> interactFn = &InteractFnExample::collidingCircles) const{
        std::vector<ItemType*> ptrVector;
        ptrVector.reserve(items.size());
        std::transform(items.begin(), items.end(), std::back_inserter(ptrVector),
                       [](ItemType& item) { return std::ref(item); });
        return solve(ptrVector, interactFn);
    }

    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \param items, the items being moved.
     * \param interactFn, the function that dictates how an item should interact with its neighbours.
     * \return true if all interactions resulted in a state where there is no more interaction required.
     */
    template <template<typename...> class ListType, typename ItemType>
    bool solve(ListType<std::unique_ptr<ItemType>>& items, InteractFn<ItemType> interactFn = &InteractFnExample::collidingCircles) const{
        std::vector<ItemType*> ptrVector;
        ptrVector.reserve(items.size());
        std::transform(items.begin(), items.end(), std::back_inserter(ptrVector),
                       [](const std::unique_ptr<ItemType>& ptr) { return ptr.get(); });
        return solve(ptrVector, interactFn);
    }

    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \param items, the items being moved.
     * \param interactFn, the function that dictates how an item should interact with its neighbours.
     * \return true if all interactions resulted in a state where there is no more interaction required.
     */
    template <template<typename...> class ListType, typename ItemType>
    bool solve(ListType<std::shared_ptr<ItemType>>& items, InteractFn<ItemType> interactFn = &InteractFnExample::collidingCircles) const{
        std::vector<ItemType*> ptrVector;
        ptrVector.reserve(items.size());
        std::transform(items.begin(), items.end(), std::back_inserter(ptrVector),
                       [](const std::shared_ptr<ItemType>& ptr) { return ptr.get(); });
        return solve(ptrVector, interactFn);
    }

    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \param items, the items being moved.
     * \param interactFn, the function that dictates how an item should interact with its neighbours.
     * \return true if all interactions resulted in a state where there is no more interaction required.
     */
    template <template<typename...> class ListType, typename ItemType>
    bool solve(ListType<ItemType*>& items, InteractFn<ItemType> interactFn = &InteractFnExample::collidingCircles) const{
        return solveVariableSize(items, interactFn);
    }

private:
    /*!
     * \brief calcualtes the relative positions of items and moves them
     * based on an interaction function if they interact.
     * \details this optimization would work better for small items that are far apart.
     * \note this solution supports variable sized elements.
     * \param items, the items being moved.
     * \param interactFn, the function that dictates how an item should interact with its neighbours.
     * \return true if all interactions resulted in a state where there is no more interaction required.
     */
    template <template<class...> class ListType, class ItemType>
    bool solveVariableSize(ListType<ItemType*>& items, InteractFn<ItemType> interactFn) const{
        static_assert(std::is_same_v<ItemType, ItemType>);
        static_assert(is_list<ListType<std::reference_wrapper<ItemType>>>());
        static_assert(has_rect<ItemType>());
        static_assert(has_setRect<ItemType, QRectF>());

        const size_t itemCount = items.size();
        std::vector<std::pair<double, size_t>> xPositions;
        std::vector<std::pair<double, size_t>> yPositions;
        std::pair<double /*x*/, double /*y*/> largestSize;
        xPositions.reserve(itemCount);
        yPositions.reserve(itemCount);

        bool isSolved = true;

        // Create sorted lists of positions,
        // this is used to efficiently calculate item proximities.
        size_t index = 0;
        for (const auto& item : items) {
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
            auto& item = items.at(itemIndex);

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

            isSolved = interactFn(item, neighbours) && isSolved;

            ++itemXIndex;
        }

        return isSolved;
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
};

Q_DECLARE_OPERATORS_FOR_FLAGS(InteractionHandler::ItemFlags)

}
#endif // InteractionHandler_H
