#include "transport_catalogue.h"
#include "stat_reader.h"
#include "input_reader.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

void MainTest()
{
//    ifstream f("./../YPTransportCatalagueBegining/DataSet/Data.txt");
//    ifstream f("Data.txt");
//    if (!f)
//    {
//        cout << "file not open" << endl;
//        return;
//    }

    transport_catalogue::TransportCatalogue tc;
    transport_catalogue::input::LoadData(tc, std::cin);
    transport_catalogue::output::StatReader sr;
    sr.RequestAndOutput(tc, cout);
}


int main()
{
    cout << "BEFORE: TransportCatalogue::GetCountOfBusses() = " << transport_catalogue::TransportCatalogue::GetCountOfBusses() << endl;
    cout << "BEFORE: TransportCatalogue::GetCountOfBusses() = " << transport_catalogue::TransportCatalogue::GetCountOfStopData() << endl;

    MainTest();

    cout << "AFTER: TransportCatalogue::GetCountOfBusses() = " << transport_catalogue::TransportCatalogue::GetCountOfBusses() << endl;
    cout << "AFTER: TransportCatalogue::GetCountOfBusses() = " << transport_catalogue::TransportCatalogue::GetCountOfStopData() << endl;

    return 0;
}

