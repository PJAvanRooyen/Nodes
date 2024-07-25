#ifndef EvNode_H
#define EvNode_H

#include "AbstractEvent.h"

#include <QRectF>
#include <QString>
#include <QUuid>

namespace Shared::EventSystem {

struct EvNodeAddReq : public EventSystem::Event<EvNodeAddReq>
{
    EvNodeAddReq(QRectF rect, QString text, QString tooltip)
      : Event<EvNodeAddReq>()
      , rect(std::move(rect))
      , text(std::move(text))
      , tooltip(std::move(tooltip)){}

    const QRectF rect;
    const QString text;
    const QString tooltip;
};

struct EvNodeAddResp : public EventSystem::Event<EvNodeAddResp>
{
    EvNodeAddResp(QUuid nodeId, QRectF rect, QString text, QString tooltip)
        : Event<EvNodeAddResp>()
        , nodeId(std::move(nodeId))
        , rect(std::move(rect))
        , text(std::move(text))
        , tooltip(std::move(tooltip)){}

    const QUuid nodeId;
    const QRectF rect;
    const QString text;
    const QString tooltip;
};

}

#endif // EvNode_H
