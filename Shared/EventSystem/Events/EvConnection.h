#ifndef EvConnection_H
#define EvConnection_H

#include "AbstractEvent.h"

#include <QUuid>

namespace Shared::EventSystem {

struct EvConnectionAddReq : public EventSystem::Event<EvConnectionAddReq>
{
    EvConnectionAddReq(QUuid node1Id, QUuid node2Id, QString text, QString tooltip)
      : Event<EvConnectionAddReq>()
      , node1Id(std::move(node1Id))
      , node2Id(std::move(node2Id))
      , text(std::move(text))
      , tooltip(std::move(tooltip)){}

    const QUuid node1Id;
    const QUuid node2Id;
    const QString text;
    const QString tooltip;
};

struct EvConnectionAddResp : public EventSystem::Event<EvConnectionAddResp>
{
    EvConnectionAddResp(QUuid node1Id, QUuid node2Id)
        : Event<EvConnectionAddResp>()
        , node1Id(std::move(node1Id))
        , node2Id(std::move(node2Id)){}

    const QUuid node1Id;
    const QUuid node2Id;
};

}

#endif // EvConnection_H
