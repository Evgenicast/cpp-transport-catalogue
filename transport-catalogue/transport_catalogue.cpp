#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

/*static*/ int domain::BusOutputData::Count = 0;
/*static*/ int domain::StopOutputData::cnt = 0;

namespace transport_catalogue
{


    void TransportCatalogue::AddBus(const std::string_view & BusNumber, const std::vector<std::string_view> & Stops, const bool & IsCircleRoute)
    {
        std::deque<const domain::StopInputData*>StopsPTR;
        std::unordered_set<const domain::StopInputData*> UniqueStopsPTR;
        for (auto & stop : Stops)
        {
            auto FoundStop = FindStop(stop);
            StopsPTR.emplace_back(FoundStop);
            UniqueStopsPTR.emplace(FoundStop);
        }
        Buses.emplace_front(domain::BusInputData(BusNumber, IsCircleRoute, StopsPTR, UniqueStopsPTR));
        BusesMap[Buses.front().BusNumber] = &Buses.front();
    }

    void TransportCatalogue::AddStop(const std::string_view & StopName, const double & LAT, const double & LNG)
    {
        Stops.emplace_front(domain::StopInputData(StopName, LAT, LNG));
        StopsMap[Stops.front().Name] = &Stops.front();
    }

    const domain::BusInputData* TransportCatalogue::FindBus(const std::string_view & BusNumber) const
    {
        return (BusesMap.find(BusNumber) == BusesMap.end()) ? nullptr : BusesMap.at(BusNumber);
    }

    const domain::StopInputData* TransportCatalogue::FindStop(const std::string_view & StopName) const
    {
        return (StopsMap.find(StopName) == StopsMap.end()) ? nullptr : StopsMap.at(StopName);
    }

    const domain::StopOutputData* TransportCatalogue::GetBusesByStop(const std::string_view & StopName) const
    {
        const domain::StopInputData* pStop = FindStop(StopName);
        if (pStop == nullptr)
        {
            return nullptr;
        }

        std::set<std::string_view> FoundBuses;
        for (const auto & Bus : Buses)
        {
            if (Bus.UniqueStops.find(pStop) != Bus.UniqueStops.end())
            {
                FoundBuses.emplace(Bus.BusNumber);
            }
        }
        return new domain::StopOutputData(pStop->Name, FoundBuses);
    }

    const domain::BusOutputData* TransportCatalogue::GetStopsByBus(const std::string_view & BusNumber) const
    {
        const domain::BusInputData* pBus = FindBus(BusNumber);
        if (pBus == nullptr)
        {
            return nullptr;
        }

        unsigned int RouteLength = 0;
        double Curvature = 0.0;

        for (auto it = pBus->BusStops.begin(); it < pBus->BusStops.end()-1; ++it)
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

        return new domain::BusOutputData(pBus->BusNumber, (pBus->CIRCLE_ROUTE == false) ? (pBus->BusStops.size() * 2) - 1 : pBus->BusStops.size(),
                                         pBus->UniqueStops.size(), RouteLength, RouteLength / Curvature );
    }

    std::deque<const domain::StopInputData*> TransportCatalogue::GetStops(const std::string_view & BusNumber) const
    {
        std::deque<const domain::StopInputData*>Stops;
        for (const auto & stop : BusesMap.at(BusNumber)->BusStops)
        {
            Stops.emplace_back(stop);
        }
        return Stops;
    }

    std::deque<const domain::BusInputData*> TransportCatalogue::GetBuses() const
    {
        std::deque<const domain::BusInputData*> Buses_;
        for (const auto & Bus : Buses)
        {
            Buses_.emplace_back(&Bus);
        }
        std::sort(Buses_.begin(), Buses_.end(), [](const domain::BusInputData* lhs, const domain::BusInputData* rhs)
        {
            return lhs->BusNumber <= rhs->BusNumber;
        });
        return Buses_;
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::string_view& FromStop, const std::string_view& ToStop, const int& Distance)
    {
        auto lhs = FindStop(FromStop);
        auto rhs = FindStop(ToStop);
        DistanceBetweenStops[std::make_pair(lhs, rhs)] = Distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const domain::StopInputData *lhs, const domain::StopInputData *rhs) const
    {
        return (DistanceBetweenStops.find(std::make_pair(lhs, rhs)) == DistanceBetweenStops.end()) ?
                DistanceBetweenStops.at(std::make_pair(rhs, lhs)) :
                DistanceBetweenStops.at(std::make_pair(lhs, rhs));

    }

    const std::vector<geo::Coordinates> TransportCatalogue::GetStopsCoordinates() const
    {
        std::vector<geo::Coordinates>StopCoordinates;
        for (const auto & bus : Buses)
        {
            for (const auto & stop : bus.BusStops)
            {
                StopCoordinates.emplace_back(stop->Coords);
            }
        }
        return StopCoordinates;
    }
} // namespace transport_catalogue
