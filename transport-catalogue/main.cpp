#include "json_reader.h"
#include <iostream>
#include <fstream>

using namespace std::literals;
using namespace  std;

void MainTest()
{
    ifstream f("input_simple.json");
    if (!f)
    {
        cout << "file not open" << endl;
        return;
    }

    json_proccessing::JsonReader JS(f);
}

int main()
{

    cout << "BEFORE: TransportCatalogue::GetCountOfBusses() = " << GetCountOfBusses() << endl;
    cout << "BEFORE: TransportCatalogue::GetCountOfStopData() = " << GetCountOfStopData() << endl;

    MainTest();
    cout << endl;
    cout << "AFTER: TransportCatalogue::GetCountOfBusses() = " << GetCountOfBusses() << endl;
    cout << "AFTER: TransportCatalogue::GetCountOfStopData() = " << GetCountOfStopData() << endl;
    return 0;
}
