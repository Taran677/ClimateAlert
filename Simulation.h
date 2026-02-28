#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <string>

#define STEPS 20
#define GRIDWIDTH 15

enum alertType
{
    NONE = 0,
    FLOOD = 1 << 0,
    HEATWAVE = 1 << 1,
    EARTHQUAKE = 1 << 2
};

struct region
{
    int rainfall;
    int temperature;
    int tremor;
    alertType alert;
    int severity;
};

void simulateSensors(std::vector<std::vector<region>> &map);
void evaluateAlerts(std::vector<std::vector<region>> &map);
int randomNum(int low, int high);
std::string getAlertString(alertType a);
std::vector<std::string> getGridLines(std::vector<std::vector<region>> &map);

#endif