#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cmath>
#define STEPS 200
#define GRIDWIDTH 15
using namespace std;

struct region;
void simulateSensors(vector<vector<region>> &map);
void propagateFlood(vector<vector<region>> &map);
void updateHeatwave(vector<vector<region>> &map);
void updateQuakes(vector<vector<region>> &map);
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
            if (currElement.rainfall > 475 || currElement.tremor == 10 || currElement.temperature > 50)
                currElement.severity = 3;
            else if (currElement.rainfall > 425 || currElement.tremor > 9 || currElement.temperature > 45)
                currElement.severity = 2;
            else if (currElement.rainfall > 300 || currElement.tremor > 8 || currElement.temperature > 40)
                currElement.severity = 1;
            else
                currElement.severity = 0;

            if (currElement.rainfall > 300)
                currElement.alert = (alertType)(currElement.alert | FLOOD);
            if (currElement.temperature > 40)
                currElement.alert = (alertType)(currElement.alert | HEATWAVE);
            if (currElement.tremor > 8)
                currElement.alert = (alertType)(currElement.alert | EARTHQUAKE);
        }
    }
}

void propagateFlood(vector<vector<region>> &map)
{
    int dx[8] = {1, -1, 0, 0, 1, -1, -1, 1};
    int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};

    auto newMap = map;
    int n = map.size();
    int m = map[0].size();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (map[i][j].alert & FLOOD)
            {
                int bias = randomNum(-100, 70);
                newMap[i][j].rainfall = clamp(map[i][j].rainfall + bias, 0, 500);
                if (newMap[i][j].rainfall < 275)
                    newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~FLOOD);
                for (int k = 0; k < 8; k++)
                {
                    int newX = i + dx[k];
                    int newY = j + dy[k];

                    if (newX >= 0 && newX < n &&
                        newY >= 0 && newY < m)
                    {
                        if (map[newX][newY].rainfall > 200)
                        {
                            newMap[newX][newY].alert =
                                (alertType)(newMap[newX][newY].alert | FLOOD);
                        }
                    }
                }
            }
        }
    }
    map = newMap;
}

void updateHeatwave(vector<vector<region>> &map)
{
    int dx[8] = {1, -1, 0, 0, 1, -1, -1, 1};
    int dy[8] = {0, 0, 1, -1, 1, -1, 1, -1};
    auto newMap = map;
    int m = map.size();
    int n = map[0].size();
    for (int i = m - 1; i >= 0; i--)
    {
        for (int j = n - 1; j >= 0; j--)
        {
            int tempLimit = 50;
            if (newMap[i][j].alert & FLOOD)
            {
                newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~HEATWAVE);
            }
            if (newMap[i][j].temperature > 40 &&
                !(newMap[i][j].alert & FLOOD))
                newMap[i][j].alert = (alertType)(newMap[i][j].alert | HEATWAVE);
            if (newMap[i][j].rainfall < 200)
                newMap[i][j].alert = (alertType)(newMap[i][j].alert & ~HEATWAVE);
            int bias = randomNum(-10, 20);
            if (map[i][j].rainfall > 200)
                tempLimit = 25;
            if (map[i][j].alert & HEATWAVE)
            {

                for (int k = 0; k < 8; k++)
                {
                    int newX = i + dx[k];
                    int newY = j + dy[k];
                    if (newX >= 0 && newX < m &&
                        newY >= 0 && newY < n)
                    {
                        if ((map[newX][newY].temperature > 38) && !(map[newX][newY].alert & FLOOD))
                        {
                            newMap[newX][newY].alert = (alertType)(newMap[newX][newY].alert | HEATWAVE);
                        }
                    }
                }
            }
            newMap[i][j].temperature = clamp(newMap[i][j].temperature + bias, 0, tempLimit);
        }
    }
    map = newMap;
}

void updateQuakes(vector<vector<region>> &map)
{
    for (vector<region> &i : map)
        for (region &j : i)
        {
            j.tremor = clamp(j.tremor + randomNum(-10, 5), 0, 10);
            if ((j.alert & EARTHQUAKE) && j.tremor < 8)
                j.alert = (alertType)(j.alert & ~EARTHQUAKE);
            if (j.tremor > 8)
                j.alert = (alertType)(j.alert | EARTHQUAKE);
        }
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
        propagateFlood(map);
        updateHeatwave(map);
        updateQuakes(map);
    }
}