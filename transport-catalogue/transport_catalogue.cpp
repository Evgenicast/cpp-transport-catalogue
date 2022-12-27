#include "transport_catalogue.h"
#include <algorithm>
#include <numeric>

namespace transport_catalogue
{

    /*static*/ int TransportCatalogue::BusOutputData::Count = 0;
    /*static*/ int TransportCatalogue::StopOutputData::cnt = 0;

    TransportCatalogue::TransportCatalogue()
    {
        //to do !
    }

    void TransportCatalogue::AddStop(const std::string_view & StopName, double & LAT, const double & LNG)
    {
        Stops.emplace_front(StopName, LAT, LNG);
        StopsMap[Stops.front().Name] = &Stops.front();
    }

    void TransportCatalogue::AddBus(const std::string_view & BusName, const bool & IsCircleRoute, const std::deque<std::string> & Stops)
    {
        std::unordered_set<const StopInputData*> pUniqueStops;
        std::deque<const StopInputData*> pBusRoutes;

        for (auto & Stop : Stops )
        {
            auto pStop = CheckIsStop(Stop);
            pBusRoutes.emplace_back(pStop);
            pUniqueStops.emplace(pStop);
        }

        Buses.emplace_front(BusName, IsCircleRoute, pBusRoutes, pUniqueStops, 0, 0.0);
        BusesMap[Buses.front().RouteName] = &Buses.front();
    }

    void TransportCatalogue::CalcAndAddLengthToBusData(const std::string_view & BusName)
    {
        double Curvature = 0.0;
        const BusInputData * pBus = CheckIsBus(BusName);
        if (pBus == nullptr)
        {
            return;
        }

        for (auto it = pBus->BusRoutes.begin(); it < pBus->BusRoutes.end()-1; ++it)
        {
            auto FirstStop = *it;
            auto SecondStopIterator = it;
            auto SecondStop = *(++SecondStopIterator);

            Curvature += ComputeDistance(SecondStop->Coords, FirstStop->Coords);
            BusesMap[BusName]->Length += GetDistanceBetweenStops(FirstStop, SecondStop);

            if (!pBus->CIRCLE_ROUTE)
            {
                Curvature += ComputeDistance(SecondStop->Coords, FirstStop->Coords);
                BusesMap[BusName]->Length += GetDistanceBetweenStops(SecondStop, FirstStop);
            }
        }

        BusesMap[BusName]->Curvature = BusesMap[BusName]->Length / Curvature;
    }

    void TransportCatalogue::FindAndAddBusesForStop(const std::string_view & Stop)
    {
        auto StopName = std::move(Stop);

        const StopInputData * pStop =  StopsMap.at(Stop);

        std::set<std::string_view> FoundBuses;
        for (const auto & Bus : Buses)
        {
            if (Bus.UniqueStops.find(pStop) != Bus.UniqueStops.end())
            {

               FoundBuses.emplace(Bus.RouteName);;
            }
        }
        BusesForStop.emplace_front(StopName, FoundBuses, true);
        BusesForStopMap[BusesForStop.front().StopName] = &BusesForStop.front();
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
        auto RouteLength = pBus->Length;
        auto IsCirleRoute = pBus->CIRCLE_ROUTE;
        auto RouteStopsQty = pBus->BusRoutes.size();
        auto UniqueStopsQty = pBus->UniqueStops.size();
        auto CurvatureKoaf  = pBus->Curvature;

        return new TransportCatalogue::BusOutputData(BusName, RouteLength, IsCirleRoute, RouteStopsQty, UniqueStopsQty, CurvatureKoaf);
    }

    TransportCatalogue::StopOutputData * TransportCatalogue::FindStopData(const std::string_view & Stop)
    {
        auto Dummy = Stop.find(' ', 0);
        auto StopName_ = Stop.substr(Dummy + 1, Stop.back());
        const StopOutputData * pStop = CheckIsStopForBus(StopName_);
        if (pStop == nullptr)
        {
            return nullptr;
        }
        auto StopName = static_cast<std::string>(pStop->StopName);
        auto BusesForStop = pStop->BusesForStop;
        auto IsBus = pStop->IsBus;

        return new StopOutputData(StopName, BusesForStop, IsBus);
    }

    const StopInputData * TransportCatalogue::CheckIsStop(const std::string_view & Stop) const
    {
        return (StopsMap.find(Stop) == StopsMap.end()) ? nullptr : StopsMap.at(Stop);
    }

    const TransportCatalogue::StopOutputData * TransportCatalogue::CheckIsStopForBus(const std::string_view & Stop) const
    {
        return (BusesForStopMap.find(Stop) == BusesForStopMap.end()) ? nullptr : BusesForStopMap.at(Stop);
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
