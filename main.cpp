#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cmath>
#define STEPS 20
#define GRIDWIDTH 15
using namespace std;

struct region;
void simulateSensors(vector<vector<region>> &map);
void evaluateAlerts(vector<vector<region>> &map);
int randomNum(int low, int high);
void print(vector<vector<region>> &map);
void printGrid(vector<vector<region>> &map);
static mt19937 gen(std::random_device{}());
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

int randomNum(int low, int high)
{
    uniform_int_distribution<> dist(low, high);
    return dist(gen);
}

string printAlerts(alertType &a)
{
    if (a == NONE)
        return "NONE";
    if (a & FLOOD)
        return "FLOOD ";
    if (a & HEATWAVE)
        return "HEATWAVE ";
    if (a & EARTHQUAKE)
        return "EARTHQUAKE ";
    return "NONE";
}

void printGrid(vector<vector<region>> &map)
{
    for (vector<region> &i : map)
    {
        for (region &j : i)
        {
            if (j.alert == NONE)
                cout << setw(4) << '.';
            else
            {
                alertType alrt = j.alert;
                string printValue = "";
                if (alrt & FLOOD)
                    printValue += "F";
                if (alrt & HEATWAVE)
                    printValue += "H";
                if (alrt & EARTHQUAKE)
                    printValue += "E";
                cout << setw(4) << printValue;
            }
        }
        cout << endl;
    }
}

void simulateSensors(vector<vector<region>> &map)
{
    int m = map.size();
    int n = map[0].size();
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            region &currElement = map[i][j];
            region &rowNeighbor = map[i][max(j - 1, 0)];
            region &colNeighbor = map[max(i - 1, 0)][j];
            double tempLimit = 60.0;
            currElement.alert = NONE;
            if (i > 0 && j > 0)
            {
                double rainCalc = randomNum(-40, 40) + (rowNeighbor.rainfall * 0.475 + colNeighbor.rainfall * 0.475);
                currElement.rainfall = clamp(rainCalc, 0.0, 500.0);

                double tempCalc = randomNum(-4, 4) + (rowNeighbor.temperature * 0.475 + colNeighbor.temperature * 0.475);
                if (map[i][j].alert & FLOOD)
                    tempLimit = 25.0;
                currElement.temperature = clamp(tempCalc, 0.0, tempLimit);

                double tremorCalc = randomNum(-2, 2) + (rowNeighbor.tremor * 0.475 + colNeighbor.tremor * 0.475);
                currElement.tremor = clamp(tremorCalc, 0.0, 10.0);
            }
            else if (i > 0)
            {
                double rainCalc = randomNum(-40, 40) + (colNeighbor.rainfall);
                currElement.rainfall = clamp(rainCalc, 0.0, 500.0);
                if (currElement.alert & FLOOD)
                    tempLimit = 25.0;
                double tempCalc = randomNum(-4, 4) + (colNeighbor.temperature);
                currElement.temperature = clamp(tempCalc, 0.0, tempLimit);

                double tremorCalc = randomNum(-2, 2) + (colNeighbor.tremor);
                currElement.tremor = clamp(tremorCalc, 0.0, 10.0);
            }
            else if (j > 0)
            {
                double rainCalc = randomNum(-40, 40) + (rowNeighbor.rainfall);
                currElement.rainfall = clamp(rainCalc, 0.0, 500.0);
                if (currElement.alert & FLOOD)
                    tempLimit = 25.0;

                double tempCalc = randomNum(-4, 4) + (rowNeighbor.temperature);
                currElement.temperature = clamp(tempCalc, 0.0, tempLimit);

                double tremorCalc = randomNum(-2, 2) + (rowNeighbor.tremor);
                currElement.tremor = clamp(tremorCalc, 0.0, 10.0);
            }
            else
            {
                currElement.rainfall = randomNum(0, 500);
                currElement.temperature = randomNum(0, 60);
                currElement.tremor = randomNum(0, 10);
            }
        }
    }
}

void evaluateAlerts(vector<vector<region>> &map)
{
    static int callCount = 0; 
    callCount++;

    int m = map.size();
    int n = map[0].size();
    int dx[3] = {-1, 0, -1};
    int dy[3] = {0, 1, 1};
    auto newMap = map;

    bool anyFlood = false, anyHeat = false;

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (map[i][j].alert & FLOOD)
                anyFlood = true;
            if (map[i][j].alert & HEATWAVE)
                anyHeat = true;

            newMap[i][j].tremor = clamp((int)map[i][j].tremor + randomNum(-10, 5), 0, 10);

            if (map[i][j].alert & FLOOD)
            {
                newMap[i][j].rainfall = clamp((int)map[i][j].rainfall + randomNum(-100, 70), 0, 500);
                for (int k = 0; k < 3; k++)
                {
                    int nX = i + dx[k], nY = j + dy[k];
                    if (nX >= 0 && nX < m && nY >= 0 && nY < n && map[nX][nY].rainfall > 200)
                        newMap[nX][nY].alert = (alertType)(newMap[nX][nY].alert | FLOOD);
                }
            }

            int tempLimit = (map[i][j].rainfall > 200) ? 25 : 50;
            newMap[i][j].temperature = clamp((int)map[i][j].temperature + randomNum(-10, 20), 0, tempLimit);
            if (map[i][j].alert & HEATWAVE)
            {
                for (int k = 0; k < 3; k++)
                {
                    int nX = i + dx[k], nY = j + dy[k];
                    if (nX >= 0 && nX < m && nY >= 0 && nY < n && map[nX][nY].temperature > 38 && !(map[nX][nY].alert & FLOOD))
                        newMap[nX][nY].alert = (alertType)(newMap[nX][nY].alert | HEATWAVE);
                }
            }

            if (newMap[i][j].rainfall > 475 || newMap[i][j].tremor == 10 || newMap[i][j].temperature > 50)
                newMap[i][j].severity = 3;
            else if (newMap[i][j].rainfall > 425 || newMap[i][j].tremor > 9 || newMap[i][j].temperature > 45)
                newMap[i][j].severity = 2;
            else if (newMap[i][j].rainfall > 300 || newMap[i][j].tremor > 8 || newMap[i][j].temperature > 40)
                newMap[i][j].severity = 1;
            else
                newMap[i][j].severity = 0;

            if (newMap[i][j].rainfall > 300)
                newMap[i][j].alert = (alertType)(newMap[i][j].alert | FLOOD);
            else if (newMap[i][j].rainfall < 275)
                newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~FLOOD);

            if (newMap[i][j].temperature > 40 && !(newMap[i][j].alert & FLOOD))
                newMap[i][j].alert = (alertType)(newMap[i][j].alert | HEATWAVE);
            if (newMap[i][j].rainfall > 200 || newMap[i][j].temperature < 38)
                newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~HEATWAVE);

            if (newMap[i][j].tremor > 8)
                newMap[i][j].alert = (alertType)(newMap[i][j].alert | EARTHQUAKE);
            else
                newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~EARTHQUAKE);
        }
    }

    if (callCount % 10 == 0)
    {
        if (!anyFlood)
        {
            int rI = randomNum(0, m - 1), rJ = randomNum(0, n - 1);
            newMap[rI][rJ].rainfall = 400; // Force spawn
            newMap[rI][rJ].alert = (alertType)(newMap[rI][rJ].alert | FLOOD);
        }
        if (!anyHeat)
        {
            int rI = randomNum(0, m - 1), rJ = randomNum(0, n - 1);
            newMap[rI][rJ].temperature = 45; // Force spawn
            newMap[rI][rJ].alert = (alertType)(newMap[rI][rJ].alert | HEATWAVE);
        }
    }

    map = newMap;
}
void print(vector<vector<region>> &map)
{
    for (vector<region> i : map)
    {
        for (region j : i)
        {
            cout << std::setfill('-') << std::setw(10) << "" << std::endl;
            cout << "Rainfall: " << j.rainfall << endl;
            cout << "Temperature: " << j.temperature << endl;
            cout << "tremor: " << j.tremor << endl;
            cout << "alert: " << printAlerts(j.alert) << endl;
            cout << "severity: " << j.severity << endl;
        }
    }
}

int main()
{
    vector<vector<region>> map(GRIDWIDTH, vector<region>(GRIDWIDTH));
    simulateSensors(map);
    for (int i = 0; i < STEPS; i++)
    {
        cout << "STEP :" << i + 1 << endl;
        printGrid(map);
        evaluateAlerts(map);
    }
}