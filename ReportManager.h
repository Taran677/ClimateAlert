#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "Simulation.h"
#include <vector>
#include <string>
#include <fstream>
#include <set>

struct EvacZone
{
    int x, y;
    std::string reason;
    int threatLevel;
};

class ReportManager
{
public:
    ReportManager(std::string filename);
    ~ReportManager();
    void logDay(int day, const std::vector<std::vector<region>> &map);
    void finalizeReport();

private:
    std::ofstream reportFile;
    int totalEvacuations;
    void writeEvacNotice(int day, const std::vector<EvacZone> &zones);
};

#endif