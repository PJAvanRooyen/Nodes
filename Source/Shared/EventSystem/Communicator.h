#ifndef Communicator_H
#define Communicator_H

#include "Events/AbstractEvent.h"

#include <QEvent>
#include <QObject>

#include <set>

namespace Shared ::EventSystem {

class Communicator : public QObject {
  Q_OBJECT

public:
  ~Communicator();

  static Communicator &instance();

  void connect(QObject *const receiver,
               const std::vector<IEvent::Type> &events);

  void postEvent(const IEvent& event) const;

private:
  Communicator();

  std::unordered_map<const IEvent::Type, std::set<QObject*>> mReceivers;
};

}

#endif
