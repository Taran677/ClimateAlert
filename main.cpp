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

        for (int i = 0; i < GRIDWIDTH; i++)
            for (int j = 0; j < GRIDWIDTH; j++)
                alertMgr.processRegion(i, j, map[i][j]);

        vector<string> gridLines = getGridLines(map);
        vector<string> alertLines = alertMgr.getAlertLines();
        size_t maxLines = max(gridLines.size(), alertLines.size());

        for (size_t i = 0; i < maxLines; i++)
        {
            if (i < gridLines.size())
                cout << gridLines[i];
            else
                cout << string(48, ' ');
            cout << "  |  ";
            if (i < alertLines.size())
                cout << alertLines[i];
            cout << endl;
        }

        reporter.logDay(day, map);
        if (alertMgr.hasCriticalAlerts())
            cout << "\033[1;31mSYSTEM: CRITICAL THREATS DETECTED!\033[0m" << endl;
    }

    reporter.finalizeReport();
    cout << "\nSimulation Cycle Complete." << endl;
    return 0;
}