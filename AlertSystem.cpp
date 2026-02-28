#include "AlertSystem.h"
#include <iostream>

using namespace std;

void AlertManager::processRegion(int x, int y, region &r)
{
    if (r.alert == NONE || r.severity < 2)
        return;
    Alert a;
    a.x = x;
    a.y = y;
    a.severity = r.severity;
    a.isActive = true;
    a.type = getAlertString(r.alert);
    activeAlerts.push_back(a);
}

void AlertManager::clearAlerts() { activeAlerts.clear(); }

vector<string> AlertManager::getAlertLines() const
{
    vector<string> lines;
    lines.push_back("\033[1;35m[!] ACTIVE DISASTERS [!]\033[0m");
    if (activeAlerts.empty())
    {
        lines.push_back("No active alerts.");
        return lines;
    }
    for (const auto &a : activeAlerts)
    {
        string color = (a.type.find("FLOOD") != string::npos) ? "\033[34m" : (a.type.find("HEAT") != string::npos) ? "\033[31m"
                                                                                                                   : "\033[33m";
        lines.push_back("-> " + color + a.type + "\033[0m at [" + to_string(a.x) + "," + to_string(a.y) + "]");
    }
    return lines;
}

bool AlertManager::hasCriticalAlerts() const
{
    for (const auto &a : activeAlerts)
        if (a.severity == 3)
            return true;
    return false;
}