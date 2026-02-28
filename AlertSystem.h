#ifndef ALERTSYSTEM_H
#define ALERTSYSTEM_H

#include "Simulation.h"
#include <string>
#include <vector>

struct Alert
{
    std::string type;
    int x, y;
    int severity;
    bool isActive;
};

class AlertManager
{
public:
    void processRegion(int x, int y, region &r);
    void clearAlerts();
    std::vector<std::string> getAlertLines() const;
    bool hasCriticalAlerts() const;

private:
    std::vector<Alert> activeAlerts;
};

#endif