#ifndef CollisionHandler_H
#define CollisionHandler_H

#include "StaticTypedefs.h"

namespace UI {

class CollisionHandler
{
public:
    /*!
     * \brief run, calcualtes the relative positions of items and moves them to reduce overlap.
     * \param items, the items being moved.
     * \param movePercentage, the fraction of the distance by which the items should move based on their overlap.
     * \example for a \p movePercentage of 10,
     *          two items will move apart by 10% of the amount by which their rects overlap.
     * \param neighbourCheckCount, the number of neighbouring items each item should check for collisions.
     * \example for a \p neighbourCheckCount of 4,
     *          each item will check at most 4 of its closest neighbours to see if they are overlapping.
     *          if 1 item overlaps with 5 others, then the 5th will be ignored.
     *
     */
    template <template<typename...> class ListType, typename... Args>
    static void run(ListType<Args*...>& items, int32_t movePercentage = 10, int32_t neighbourCheckCount = 4){
        static_assert(is_list<ListType<Args*...>>());
        static_assert(has_pos<Args...>());
        static_assert(has_size<Args...>());

        const size_t itemCount = items.size();
        std::vector<std::pair<double, size_t>> xPositions;
        std::vector<std::pair<double, size_t>> yPositions;
        xPositions.reserve(itemCount);
        yPositions.reserve(itemCount);

        // Create sorted lists of positions,
        // this is used to efficiently calculate item proximities.
        size_t index = 0;
        for (const auto* item : items) {
            const auto& itemPos = item->pos();
            xPositions.emplace_back(itemPos.x(), index);
            yPositions.emplace_back(itemPos.y(), index);
            ++index;
        }
        std::sort(xPositions.begin(), xPositions.end());
        std::sort(yPositions.begin(), yPositions.end());

        size_t neighboursHalved = std::ceil(neighbourCheckCount/2.0);

        // For each item, get its nearest max(N) neighbours,
        // then solve collisions for those
        size_t itemXIndex = 0;
        for (const auto& xPosItemIndex : xPositions) {
            size_t itemIndex = xPosItemIndex.second;
            auto yPosItemIndexIt =
                std::find_if(yPositions.begin(), yPositions.end(),
                    [itemIndex](const auto& yPosItemIndex){
                return yPosItemIndex.second == itemIndex;
            });
            size_t itemYIndex = yPosItemIndexIt - yPositions.begin();

            static const size_t kSelf = 1;
            size_t lowestXNeighbourIndex = std::max(size_t(0), (itemXIndex - neighboursHalved));
            size_t highestXNeighbourIndex = std::min(itemCount - 1, lowestXNeighbourIndex + neighbourCheckCount + kSelf);
            size_t lowestYNeighbourIndex = std::max(size_t(0), (itemYIndex - neighboursHalved));
            size_t highestYNeighbourIndex = std::min(itemCount - 1, lowestYNeighbourIndex + neighbourCheckCount + kSelf);

            ListType<Args*...> neighbours;
            for(auto xIndex = lowestXNeighbourIndex; xIndex != highestXNeighbourIndex; ++xIndex){
                if(xIndex == itemXIndex){
                    continue;
                }
                size_t neighbourItemIndex = xPositions.at(xIndex).second;
                auto& neighbour = items.at(neighbourItemIndex);
                if(auto itemIt = std::find(neighbours.cbegin(), neighbours.cend(), neighbour); itemIt == neighbours.cend()){
                    neighbours.push_back(neighbour);
                }
            }
            for(auto yIndex = lowestYNeighbourIndex; yIndex != highestYNeighbourIndex; ++yIndex){
                if(yIndex == itemYIndex){
                    continue;
                }
                size_t neighbourItemIndex = yPositions.at(yIndex).second;
                auto& neighbour = items.at(neighbourItemIndex);
                if(auto itemIt = std::find(neighbours.cbegin(), neighbours.cend(), neighbour); itemIt == neighbours.cend()){
                    neighbours.push_back(neighbour);
                }
            }

            auto& item = items.at(itemIndex);
            solveCollisions(item, neighbours);

            ++itemXIndex;
        }
    }

private:
    template <template<typename...> class ListType, typename... Args>
    static void solveCollisions(typename ListType<Args*...>::value_type item, ListType<Args*...>& neighbours){

    }
};

}
#endif // CollisionHandler_H
