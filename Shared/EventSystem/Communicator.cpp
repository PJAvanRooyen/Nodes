#include "Communicator.h"

#include <QCoreApplication>

namespace Shared ::EventSystem {

Communicator::~Communicator() {}

Communicator &Communicator::instance() {
  static Communicator instance;
  return instance;
}

void Communicator::connect(QObject *const receiver,
                           const std::vector<QEvent::Type> &events) {
  for (IEvent::Type eventType : events) {
    auto &eventReceivers = mReceivers[eventType];
    eventReceivers.insert(receiver);
  }
}

void
Communicator::postEvent(const IEvent& event) const {
  const IEvent::Type eventType = event.type();
  if (eventType < QEvent::Type::User) {
    return;
  }

  if (mReceivers.find(eventType) == mReceivers.end()) {
    return;
  }

  const std::set<QObject *> &eventReceivers = mReceivers.at(eventType);
  for (QObject *const receiver : eventReceivers) {
    QCoreApplication::postEvent(receiver, event.copy());
  }
}

Communicator::Communicator() : mReceivers() {}

}
