#include "ReportManager.h"
#include <iostream>
#include <set>

using namespace std;

ReportManager::ReportManager(string filename)
{
    reportFile.open(filename);
    totalEvacuations = 0;
    if (reportFile.is_open())
    {
        reportFile << "!!! EMERGENCY EVACUATION PROTOCOL LOG !!!" << endl;
        reportFile << "==========================================" << endl;
    }
}

ReportManager::~ReportManager()
{
    if (reportFile.is_open())
        reportFile.close();
}

void ReportManager::logDay(int day, const vector<vector<region>> &map)
{
    if (!reportFile.is_open())
        return;
    vector<EvacZone> zones;
    set<pair<int, int>> flagged;
    int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1}, dy[] = {0, 0, -1, 1, -1, 1, -1, 1};

    for (int i = 0; i < GRIDWIDTH; i++)
        for (int j = 0; j < GRIDWIDTH; j++)
            if (map[i][j].severity >= 2)
                for (int k = 0; k < 8; k++)
                {
                    int ni = i + dx[k], nj = j + dy[k];
                    if (ni >= 0 && ni < GRIDWIDTH && nj >= 0 && nj < GRIDWIDTH)
                        if (flagged.find({ni, nj}) == flagged.end() && map[ni][nj].severity < 2)
                        {
                            EvacZone z;
                            z.x = ni;
                            z.y = nj;
                            z.reason = "Proximity to " + getAlertString(map[i][j].alert);
                            zones.push_back(z);
                            flagged.insert({ni, nj});
                        }
                }
    writeEvacNotice(day, zones);
}

void ReportManager::writeEvacNotice(int day, const vector<EvacZone> &zones)
{
    reportFile << "\n[ DAY " << day << " ALERT ]" << endl;
    if (zones.empty())
    {
        reportFile << "Status: All neighboring sectors clear." << endl;
        return;
    }
    for (const auto &z : zones)
    {
        reportFile << "EVACUATE [" << z.x << "," << z.y << "] - Reason: " << z.reason << endl;
        totalEvacuations++;
    }
}

void ReportManager::finalizeReport()
{
    if (reportFile.is_open())
        reportFile << "\nTOTAL EVACUATIONS ISSUED: " << totalEvacuations << endl;
}