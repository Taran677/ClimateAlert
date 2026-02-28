#include "Simulation.h"
#include <iostream>
#include <random>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>

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
            else if (map[i][j].alert & FLOOD)
                line += "  \033[1;34mF\033[0m";
            else if (map[i][j].alert & HEATWAVE)
                line += "  \033[1;31mH\033[0m";
            else if (map[i][j].alert & EARTHQUAKE)
                line += "  \033[1;33mE\033[0m";
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
    int countF = 0, countH = 0;
    int totalCells = GRIDWIDTH * GRIDWIDTH;

    for (int i = 0; i < GRIDWIDTH; i++)
    {
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            if (map[i][j].alert & FLOOD)
                countF++;
            if (map[i][j].alert & HEATWAVE)
                countH++;
        }
    }

    for (int i = 0; i < GRIDWIDTH; i++)
    {
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            int sparkF = (countF < totalCells / 8) ? 60 : 35;
            int sparkH = (countH < totalCells / 8) ? 7 : 3;

            map[i][j].rainfall += randomNum(-sparkF, sparkF + 3);
            map[i][j].temperature += randomNum(-sparkH, sparkH);
            map[i][j].tremor = randomNum(0, 10);

            float decayF = (countF > totalCells / 4) ? 0.12f : 0.04f;
            if (map[i][j].rainfall > 100)
                map[i][j].rainfall -= (map[i][j].rainfall * decayF);
            if (map[i][j].temperature > 25)
                map[i][j].temperature -= 1;

            map[i][j].rainfall = clamp(map[i][j].rainfall, 0, 500);
            map[i][j].temperature = clamp(map[i][j].temperature, 0, 60);

            map[i][j].alert = NONE;
            map[i][j].severity = 0;
        }
    }

    vector<vector<region>> nextMap = map;

    for (int i = 0; i < GRIDWIDTH; i++)
    {
        for (int j = 0; j < GRIDWIDTH; j++)
        {
            int fN = 0, hN = 0;
            bool neighborHasQuake = false;

            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < GRIDWIDTH && nj >= 0 && nj < GRIDWIDTH && (di != 0 || dj != 0))
                    {
                        if (map[ni][nj].alert & FLOOD)
                            fN++;
                        if (map[ni][nj].alert & HEATWAVE)
                            hN++;
                        if (map[ni][nj].alert & EARTHQUAKE)
                            neighborHasQuake = true;
                    }
                }
            }

            if (fN >= 3 && hN < 2)
                nextMap[i][j].rainfall += 28;
            if (hN >= 3 && fN < 2)
                nextMap[i][j].temperature += 5;

            if (neighborHasQuake && randomNum(0, 10) > 8)
                nextMap[i][j].tremor += 3;

            nextMap[i][j].rainfall = clamp(nextMap[i][j].rainfall, 0, 500);
            nextMap[i][j].temperature = clamp(nextMap[i][j].temperature, 0, 60);
            nextMap[i][j].tremor = clamp(nextMap[i][j].tremor, 0, 10);

            if (nextMap[i][j].rainfall > 380)
            {
                nextMap[i][j].alert = (alertType)(nextMap[i][j].alert | FLOOD);
                nextMap[i][j].severity = 3;
            }
            if (nextMap[i][j].temperature > 50)
            {
                nextMap[i][j].alert = (alertType)(nextMap[i][j].alert | HEATWAVE);
                nextMap[i][j].severity = 3;
            }
            if (nextMap[i][j].tremor > 8)
            {
                nextMap[i][j].alert = (alertType)(nextMap[i][j].alert | EARTHQUAKE);
                nextMap[i][j].severity = 3;
            }
        }
    }
    map = nextMap;
}