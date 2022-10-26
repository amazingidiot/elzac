#include "osc_action.h"

Osc::Action::Action(
    QString address,
    std::function<bool(Osc::Server* server, Osc::Message* message)> run)
{
    this->address = address;
    this->run = run;
}
