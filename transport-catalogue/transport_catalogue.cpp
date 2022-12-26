#include "transport_catalogue.h"
#include <algorithm>
#include <numeric>

namespace transport_catalogue
{

    /*static*/ int TransportCatalogue::BusOutputData::Count = 0;
    /*static*/ int TransportCatalogue::StopData::cnt = 0;

    TransportCatalogue::TransportCatalogue()
    {
        //to do !
    }

    void TransportCatalogue::AddStop(const std::string_view & StopName, double & LAT, const double & LNG)
    {
        Stops.emplace_front(StopName, LAT, LNG);
        StopsMap[Stops.front().Name] = &Stops.front();
    }

    void TransportCatalogue::AddBus(const std::string_view & bus_num, const bool & cicle_type, const std::deque<std::string> & Stops)
    {
        std::unordered_set<const StopInputData*> pUniqueStops;
        std::deque<const StopInputData*> pBusRoutes;

        for (auto & Stop : Stops )
        {
            auto pIsStop = CheckIsStop(Stop);
            pBusRoutes.emplace_back(pIsStop);
            pUniqueStops.emplace(pIsStop);
        }

        Buses.emplace_front(bus_num, cicle_type, pBusRoutes, pUniqueStops);
        BusesMap[Buses.front().RouteName] = &Buses.front();
    }

    TransportCatalogue::BusOutputData * TransportCatalogue::FindBusRoute(const std::string_view & Bus)
    {
        auto Dummy = Bus.find(' ', 0);
        auto BusName = Bus.substr(Dummy + 1, Bus.back());
        std::vector<StopInputData*> FoundStops;

        const BusInputData * pBus = CheckIsBus(BusName);
        if (pBus == nullptr)
        {
            return nullptr;
        }

        double RouteLength = 0.0;
        double Curvature = 0.0;
        double CurvatureKoaf = 0.0;

        for (auto it = pBus->BusRoutes.begin(); it < pBus->BusRoutes.end()-1; ++it)
        {
            auto FirstStop = *it;
            auto SecondStopIterator = it;
            auto SecondStop = *(++SecondStopIterator);

            Curvature += ComputeDistance(SecondStop->Coords, FirstStop->Coords);
            RouteLength += GetDistanceBetweenStops(FirstStop,SecondStop);

            if (!pBus->CIRCLE_ROUTE)
            {
                Curvature += ComputeDistance(SecondStop->Coords, FirstStop->Coords);
                RouteLength += GetDistanceBetweenStops(SecondStop, FirstStop);
            }
        }

        CurvatureKoaf = RouteLength / Curvature;
        auto IsCirleRoute = pBus->CIRCLE_ROUTE;
        auto RouteStopsQty = pBus->BusRoutes.size();
        auto UniqueStopsQty = pBus->UniqueStops.size();

        return new TransportCatalogue::BusOutputData(BusName, RouteLength, IsCirleRoute, RouteStopsQty, UniqueStopsQty, CurvatureKoaf);
    }

    TransportCatalogue::StopData * TransportCatalogue::FindStopData(const std::string_view & Stop)
    {
        auto Dummy = Stop.find(' ', 0);
        auto StopName = Stop.substr(Dummy + 1, Stop.back());
        const StopInputData * pStop = CheckIsStop(StopName);
        if (pStop == nullptr)
        {
            return nullptr;
        }

        std::set<std::string_view> FoundBuses;
        for (const auto & Bus : Buses)
        {
            if (Bus.UniqueStops.find(pStop) != Bus.UniqueStops.end())
            {
                FoundBuses.emplace(Bus.RouteName);
            }
        }
        return new TransportCatalogue::StopData(StopName, FoundBuses);
    }

    const StopInputData * TransportCatalogue::CheckIsStop(const std::string_view & Stop) const
    {
        return (StopsMap.find(Stop) == StopsMap.end()) ? nullptr : StopsMap.at(Stop);
    }

    const BusInputData * TransportCatalogue::CheckIsBus(const std::string_view & Bus) const
    {
        return (BusesMap.find(Bus) == BusesMap.end()) ? nullptr : BusesMap.at(Bus);
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::string_view &StopFrom_, const std::string_view &StopTo_, const int & Distance)
    {
        auto pStopFrom = CheckIsStop(StopFrom_);
        auto pStopTo = CheckIsStop(StopTo_);
        DistanceBetweenStops[std::make_pair(pStopFrom, pStopTo)] = Distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const StopInputData * StopFrom, const StopInputData * StopTo) const
    {
        return (DistanceBetweenStops.find(std::make_pair(StopFrom, StopTo)) == DistanceBetweenStops.end()) ?
                DistanceBetweenStops.at(std::make_pair(StopTo, StopFrom)) :
                DistanceBetweenStops.at(std::make_pair(StopFrom, StopTo));
    }
}//transport_catalogue
