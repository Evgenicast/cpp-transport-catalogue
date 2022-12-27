#include "stat_reader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace transport_catalogue
{
    namespace output
    {
        std::string_view StatReader::ExtractName(const std::string_view& Data)
        {
            int Dummy = Data.find(' ', 0);
            std::string_view BusName = Data.substr(Dummy + 1, Data.back());

            return BusName;
        }

        void StatReader::PrintRouteInfo(TransportCatalogue & TC, std::string Query, std::ostream & out)
        {
            auto BusData = TC.FindBusRoute(Query);
            Query = ExtractName(Query);
            if(BusData == nullptr)
            {
                out << "Bus " << Query << ": not found" << std::endl;
            }
            else
            {
                out << "Bus " << BusData->BusName << ": ";
                out << (BusData->IsRouteCircle ? BusData->StopsCount : ((BusData->StopsCount * 2) - 1)) << " stops on route, " << BusData->UniqueStopCount << " unique stops, "
                    << std::setprecision(6) << BusData->Length << " route length, "
                    << BusData->Curvature << " curvature" << std::endl;
            }
            delete BusData;
        }

        void StatReader::PrintStopInfo(TransportCatalogue & TC, std::string Query, std::ostream & out)
        {
            auto StopData = TC.FindStopData(Query);
            Query = ExtractName(Query);

            if(StopData == nullptr)
            {
                out << "Stop " << Query << ": not found" << std::endl;
            }
            else if (StopData->BusesForStop.empty())
            {
                out << "Stop " << Query << ": no buses" << std::endl;
            }
            else
            {
                out << "Stop " << StopData->StopName << ": buses ";
                for (const auto & Bus : StopData->BusesForStop)
                {
                    out << Bus << ' ';
                }
                out << std::endl;
            }
            delete StopData;
        }

        void StatReader::RequestAndOutput(TransportCatalogue & TC, std::ostream& out)
        {

            int QueryCount;
            std::cin >> QueryCount;
            constexpr auto max_size = std::numeric_limits<std::streamsize>::max();
            std::cin.ignore(max_size, '\n');
            std::string Data;

            for (int i = 0; i < QueryCount; ++i)
            {
                getline(std::cin, Data);
                if (Data[0] == 'B')
                {
                    PrintRouteInfo(TC, Data, out);
                }
                else if (Data[0] == 'S')
                {
                    PrintStopInfo(TC, Data, out);
                }
                else
                    throw std::invalid_argument( " Output request error" +Data+ " No matching item for request ");
            }
        }
    }//output
}//transport_catalogue
