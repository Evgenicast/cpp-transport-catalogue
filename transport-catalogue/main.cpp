#include "json_reader.h"
#include <iostream>
#include <fstream>

using namespace std::literals;
using namespace  std;

void MainTest()
{
    ifstream f("input.json");
    if (!f)
    {
        cout << "file not open" << endl;
        return;
    }

    json_proccessing::JsonReader JS(f);
}
int main()
{
    cout << "BEFORE: TransportCatalogue::GetCountOfBusses() = " << domain::GetCountOfBusses() << endl;
    cout << "BEFORE: TransportCatalogue::GetCountOfBusses() = " << domain::GetCountOfStopData() << endl;

    MainTest();
    cout << endl;
    cout << "AFTER: TransportCatalogue::GetCountOfBusses() = " << domain::GetCountOfBusses() << endl;
    cout << "AFTER: TransportCatalogue::GetCountOfBusses() = " << domain::GetCountOfStopData() << endl;
    return 0;
}
