#include "domain.h"

tc_base::Stop::Stop(std::string_view Name, geo::Coordinates Coords)
: m_Name(std::move(Name)), m_Coords(std::move(Coords)){}

tc_base::Bus::Bus(std::string_view BusNumber, bool IsCircleRoute, std::deque<const Stop *> BusStopsDeque, std::unordered_set<const Stop *> UniqueStopsUnSet)
: m_BusNumber(std::move(BusNumber)), m_IsCircleRoute(std::move(IsCircleRoute)), m_BusStopsDeque(std::move(BusStopsDeque)), m_UniqueStopsUnSet(std::move(UniqueStopsUnSet)){}

tc_request::Bus::Bus(std::string_view BusNumber, size_t StopsCount, size_t UniqueStopsCount, unsigned int RouteLength, double Curvature)
: m_BusNumber(std::move(BusNumber)), m_StopsCount(std::move(StopsCount)), m_UniqueStopsCount(std::move(UniqueStopsCount)), m_RouteLength(std::move(RouteLength)), m_Curvature(std::move(Curvature))
{
    ++s_CountBuses; // для проверки на утечку памяти. Объект создается динамически.
}

tc_request::Stop::Stop(std::string_view StopName, std::set<std::string_view> BusesForStop)
: m_StopName(std::move(StopName)), m_BusesForStopSet(std::move(BusesForStop))
{
    ++s_CountStops;
}
