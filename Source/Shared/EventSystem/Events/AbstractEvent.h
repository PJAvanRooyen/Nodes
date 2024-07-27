#ifndef AbstractEvent_H
#define AbstractEvent_H

#include <QEvent>
#include <QObject>

namespace Shared ::EventSystem {

class IEvent : public QEvent {
public:
  virtual IEvent* copy() const = 0;

protected:
  IEvent(const QEvent::Type eventType)
    : QEvent(eventType) {}
};

template<class EventType>
class Event : public IEvent {
public:
  virtual ~Event() {}

  static QEvent::Type staticType() {
    static QEvent::Type sType =
      static_cast<QEvent::Type>(QEvent::registerEventType());
    return sType;
  }

  IEvent* copy() const {
    return new EventType(*static_cast<const EventType*>(this));
  }

protected:
  Event()
    : IEvent(staticType()) {}
};

}

#endif
