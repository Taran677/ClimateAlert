#include "Simulation.h"
#include "ReportManager.h"
#include "AlertSystem.h"
#include <iostream>
#include <algorithm>

using namespace std;

int main()
{
    vector<vector<region>> map(GRIDWIDTH, vector<region>(GRIDWIDTH));
    ReportManager reporter("evacuation_orders.txt");
    AlertManager alertMgr;

    simulateSensors(map);

    for (int day = 1; day <= STEPS; day++)
    {
        cout << "\n================ [ DAY " << day << " ] ================" << endl;

        evaluateAlerts(map);
        alertMgr.clearAlerts();
        reporter.logDay(day, map);
        for (int i = 0; i < GRIDWIDTH; i++)
            for (int j = 0; j < GRIDWIDTH; j++)
                alertMgr.processRegion(i, j, map[i][j]);

        vector<string> gridLines = getGridLines(map);
        for (const string &line : gridLines)
        {
            cout << line << endl;
        }

        cout << "============================================" << endl;
    }

    reporter.finalizeReport();
    cout << "\nSimulation Cycle Complete." << endl;

    return 0;
}