#include "Simulation.h"
#include <iostream>
#include <random>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

static mt19937 gen(std::random_device{}());

int randomNum(int low, int high)
{
    uniform_int_distribution<> dist(low, high);
    return dist(gen);
}

string getAlertString(alertType a)
{
    if (a == NONE)
        return "NONE";
    string res = "";
    if (a & FLOOD)
        res += "FLOOD ";
    if (a & HEATWAVE)
        res += "HEAT ";
    if (a & EARTHQUAKE)
        res += "QUAKE ";
    return res;
}

vector<string> getGridLines(vector<vector<region>> &map)
{
    vector<string> lines;
    string header = "   ";
    for (int x = 0; x < GRIDWIDTH; x++)
        header += (x < 10 ? "  " : " ") + to_string(x);
    lines.push_back(header);

    for (int i = 0; i < GRIDWIDTH; i++)
    {
        string line = (i < 10 ? " " : "") + to_string(i) + " ";
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            if (map[i][j].alert == NONE)
                line += "  .";
            else
            {
                if (map[i][j].alert & FLOOD)
                    line += "  \033[1;34mF\033[0m";
                else if (map[i][j].alert & HEATWAVE)
                    line += "  \033[1;31mH\033[0m";
                else if (map[i][j].alert & EARTHQUAKE)
                    line += "  \033[1;33mE\033[0m";
            }
        }
        lines.push_back(line);
    }
    return lines;
}

void simulateSensors(vector<vector<region>> &map)
{
    for (int i = 0; i < GRIDWIDTH; i++)
    {
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            map[i][j].rainfall = randomNum(0, 500);
            map[i][j].temperature = randomNum(0, 60);
            map[i][j].tremor = randomNum(0, 10);
            map[i][j].alert = NONE;
            map[i][j].severity = 0;
        }
    }
}

void evaluateAlerts(vector<vector<region>> &map)
{
    for (int i = 0; i < GRIDWIDTH; i++)
    {
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            map[i][j].rainfall = clamp(map[i][j].rainfall + randomNum(-40, 40), 0, 500);
            map[i][j].temperature = clamp(map[i][j].temperature + randomNum(-4, 4), 0, 60);
            map[i][j].tremor = clamp(map[i][j].tremor + randomNum(-1, 1), 0, 10);
            map[i][j].alert = NONE;
            map[i][j].severity = 0;

            if (map[i][j].rainfall > 380)
            {
                map[i][j].alert = (alertType)(map[i][j].alert | FLOOD);
                map[i][j].severity = 3;
            }
            if (map[i][j].temperature > 50)
            {
                map[i][j].alert = (alertType)(map[i][j].alert | HEATWAVE);
                map[i][j].severity = 3;
            }
            if (map[i][j].tremor > 8)
            {
                map[i][j].alert = (alertType)(map[i][j].alert | EARTHQUAKE);
                map[i][j].severity = 3;
            }
        }
    }
}