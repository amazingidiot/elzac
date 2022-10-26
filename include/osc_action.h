#pragma once

#include <QRegularExpression>
#include <functional>

#include "osc_message.h"
#include "osc_server.h"

namespace Osc {
class Server;

class Action : public QObject {
    Q_OBJECT

public:
    Action(
        QString address,
        std::function<bool(Osc::Server* server, Osc::Message* message)> run);

    QString address;
    std::function<bool(Osc::Server* server, Osc::Message* message)> run;
};
} // namespace Osc
