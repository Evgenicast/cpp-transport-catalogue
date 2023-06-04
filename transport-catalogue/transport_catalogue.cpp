#include "transport_catalogue.h"
#include <utility>
#include <algorithm>

/*static*/ int tc_request::Bus::s_CountBuses = 0;
/*static*/ int tc_request::Stop::s_CountStops = 0;

namespace transport_catalogue
{
    void TransportCatalogue::AddBus(std::string_view BusNumber, const std::vector<std::string_view> & StopsVec, const bool & IsCircleRoute)
    {
        std::deque<const tc_base::Stop*> StopsDeque;
        std::unordered_set<const tc_base::Stop*> UniqueStopsUnSet;
        for (auto & Stop : StopsVec)
        {
            auto FoundStop = FindStop(Stop);
            StopsDeque.emplace_back(FoundStop);
            UniqueStopsUnSet.emplace(FoundStop);
        }
//      Buses.push_front(domain::BusInputData(BusNumber, IsCircleRoute, StopsPTR, UniqueStopsPTR)); // C++'98 style
//      Buses.push_front({BusNumber, IsCircleRoute, StopsPTR, UniqueStopsPTR}); // modern style, temp obj is created
        m_BusesDeque.emplace_front(BusNumber, IsCircleRoute, StopsDeque, UniqueStopsUnSet); // best version, cos temp obj is not created
        m_BusesUnMap[m_BusesDeque.front().m_BusNumber] = &m_BusesDeque.front();
    }

    void TransportCatalogue::AddStop(std::string_view StopName, const double & LAT, const double & LNG)
    {
        m_StopsDeque.emplace_front(StopName, geo::Coordinates({LAT, LNG}));
        m_StopsUnMap[m_StopsDeque.front().m_Name] = &m_StopsDeque.front();
    }

    const tc_base::Bus * TransportCatalogue::FindBus(const std::string_view & BusNumber) const
    {
        return (m_BusesUnMap.find(BusNumber) == m_BusesUnMap.end()) ? nullptr : m_BusesUnMap.at(BusNumber);
    }

    const tc_base::Stop * TransportCatalogue::FindStop(const std::string_view & StopName) const
    {
        return (m_StopsUnMap.find(StopName) == m_StopsUnMap.end()) ? nullptr : m_StopsUnMap.at(StopName);
    }

    const tc_request::Stop * TransportCatalogue::GetBusesByStop(const std::string_view & StopName) const
    {
        const tc_base::Stop * pStop = FindStop(StopName);
        if (pStop == nullptr)
        {
            return nullptr;
        }

        std::set<std::string_view> FoundBuses;
        for (const auto & Bus : m_BusesDeque)
        {
            if (Bus.m_UniqueStopsUnSet.find(pStop) != Bus.m_UniqueStopsUnSet.end())
            {
                FoundBuses.emplace(Bus.m_BusNumber);
            }
        }
        return new tc_request::Stop(pStop->m_Name, FoundBuses);
    }

    const tc_request::Bus * TransportCatalogue::GetStopsByBus(const std::string_view & BusNumber) const
    {
        const tc_base::Bus* pBus = FindBus(BusNumber);
        if (pBus == nullptr)
        {
            return nullptr;
        }

        unsigned int RouteLength = 0;
        double Curvature = 0.0;

        for (auto it = pBus->m_BusStopsDeque.begin(); it < pBus->m_BusStopsDeque.end()-1; ++it)
        {
            auto FirstStop = *it;
            auto SecondStopIterator = it;
            auto SecondStop = *(++SecondStopIterator);

            Curvature += geo::ComputeDistance(SecondStop->m_Coords, FirstStop->m_Coords);
            RouteLength += GetDistanceBetweenStops(FirstStop,SecondStop);

            if (!pBus->m_IsCircleRoute)
            {
                Curvature += geo::ComputeDistance(SecondStop->m_Coords, FirstStop->m_Coords);
                RouteLength += GetDistanceBetweenStops(SecondStop, FirstStop);
            }
        }

        return new tc_request::Bus(pBus->m_BusNumber, (pBus->m_IsCircleRoute == false) ? (pBus->m_BusStopsDeque.size() * 2) - 1 : pBus->m_BusStopsDeque.size(),
                                         pBus->m_UniqueStopsUnSet.size(), RouteLength, RouteLength / Curvature );
    }

    std::deque<const tc_base::Stop*> TransportCatalogue::GetStops(const std::string_view & BusNumber) const
    {
        std::deque<const tc_base::Stop*>Stops;
        for (const auto & Stop : m_BusesUnMap.at(BusNumber)->m_BusStopsDeque)
        {
            Stops.emplace_back(Stop);
        }
        return Stops;
    }

    std::deque<const tc_base::Bus*> TransportCatalogue::GetBuses() const
    {
        std::deque<const tc_base::Bus*> Buses;
        for (const auto & Bus : m_BusesDeque)
        {
            Buses.emplace_back(&Bus);
        }
        std::sort(Buses.begin(), Buses.end(), [](const tc_base::Bus * lhs, const tc_base::Bus * rhs)
        {
            return lhs->m_BusNumber <= rhs->m_BusNumber;
        });
        return Buses;
    }

    const std::vector<const tc_base::Stop *> TransportCatalogue::GetAllStops() const
    {
        std::vector<const tc_base::Stop*> Stops;
        for(const auto & Iter : m_StopsUnMap)
        {
            Stops.push_back(Iter.second);
        }
        return Stops;
    }

    double TransportCatalogue::ComputeDistance(const tc_base::Stop * From, const tc_base::Stop * To) const
    {
        double RouteDistance = 0.0;
        const auto it_DistanceFromTo = m_DistanceBetweenStopsUnMap.find({From, To});
        if(it_DistanceFromTo == m_DistanceBetweenStopsUnMap.end())
        {
            const auto it_DistanceToFrom = m_DistanceBetweenStopsUnMap.find({To, From});
            if(it_DistanceToFrom != m_DistanceBetweenStopsUnMap.end())
            {
                RouteDistance += it_DistanceToFrom->second;
            }
            else
            {
                RouteDistance = -1.0;
            }
        }
        else
        {
            RouteDistance = it_DistanceFromTo->second;
        }
        return RouteDistance;
    }

    void TransportCatalogue::SetDistanceBetweenStops(const std::string_view & FromStop, const std::string_view & ToStop, const int & Distance)
    {
        auto lhs = FindStop(FromStop);
        auto rhs = FindStop(ToStop);
        m_DistanceBetweenStopsUnMap[std::make_pair(lhs, rhs)] = Distance;
    }

    int TransportCatalogue::GetDistanceBetweenStops(const tc_base::Stop * From, const tc_base::Stop * To) const
    {
        return (m_DistanceBetweenStopsUnMap.find(std::make_pair(From, To)) == m_DistanceBetweenStopsUnMap.end()) ?
                m_DistanceBetweenStopsUnMap.at(std::make_pair(To, From)) :
                m_DistanceBetweenStopsUnMap.at(std::make_pair(From, To));
    }

    const std::vector<geo::Coordinates> TransportCatalogue::GetStopsCoordinates() const
    {
        std::vector<geo::Coordinates> StopCoordinates;
        for (const auto & bus : m_BusesDeque)
        {
            for (const auto & stop : bus.m_BusStopsDeque)
            {
                StopCoordinates.emplace_back(stop->m_Coords);
            }
        }
        return StopCoordinates;
    }

    TransportCatalogue::hashed_distances TransportCatalogue::GetAllDistances() const
    {
        return hashed_distances(m_DistanceBetweenStopsUnMap);
    }

} // namespace transport_catalogue

//----N.B.----//
/*
m_Name - class member's name of variable, object;
m_NamePtr - class member's name of pointer;
m_Name<Container's type> - class member's name of container depending on its type; ex. std::deque<const StopInputData*> m_BusStopsDeque;
m_NameRef - class member's name of reference;
Name - local Name, funcion parameter's name;
ABV - abbreviation name of var, con and etc;
Name<Container's type> - local name of container depending on its type or funtion parameter; ex. std::deque<const StopInputData*> BusStopsDeque;
*/


