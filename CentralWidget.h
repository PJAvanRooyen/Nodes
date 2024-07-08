#ifndef CentralWidget_H
#define CentralWidget_H

#include "CentralWidgetView.h"

#include <QPointer>
#include <QWidget>

namespace UI {

class CentralWidget : public QWidget
{
public:
    CentralWidget(QWidget *parent = nullptr);

    QPointer<CentralWidgetView> view();

    void addNode();

private:
    std::unique_ptr<CentralWidgetView> mView;
};

}
#endif // CentralWidget_H
