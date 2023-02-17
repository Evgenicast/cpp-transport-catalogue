#pragma once
#include "domain.h"
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>
#include <set>

namespace transport_catalogue
{
    class TransportCatalogue
    {
    private:

        std::deque<domain::BusInputData> Buses;
        std::deque<domain::StopInputData> Stops;
        std::unordered_map<std::string_view, const domain::BusInputData*> BusesMap;
        std::unordered_map<std::string_view, const domain::StopInputData*> StopsMap;
        std::unordered_map<std::pair<const domain::StopInputData*, const domain::StopInputData*>, int, domain::StopsDistPtrPairHasher> DistanceBetweenStops;

    public:

        TransportCatalogue() = default;
        TransportCatalogue(TransportCatalogue &) = delete;
        TransportCatalogue & operator=(TransportCatalogue &) noexcept = delete;
        TransportCatalogue(TransportCatalogue &&) = delete;
        TransportCatalogue & operator=(TransportCatalogue &&) noexcept = delete;

        void AddBus(const std::string_view & BusNumber, const std::vector<std::string_view> & Stops, const bool & IsCircleRoute);
        void AddStop(const std::string_view & StopName, const double & LAT, const double & LNG);

        const domain::BusInputData* FindBus(const std::string_view & BusNumber) const;
        const domain::StopInputData* FindStop(const std::string_view & StopName) const;

        const domain::StopOutputData* GetBusesByStop(const std::string_view & StopName) const;
        const domain::BusOutputData* GetStopsByBus(const std::string_view & BusNumber) const;

        std::deque<const domain::StopInputData*> GetStops(const std::string_view & BusNumber) const;
        std::deque<const domain::BusInputData*> GetBuses() const;

        void SetDistanceBetweenStops(const std::string_view & FromStop, const std::string_view & ToStop, const int & Distance);
        int GetDistanceBetweenStops(const domain::StopInputData * lhs, const domain::StopInputData * rhs) const;
        const std::vector<geo::Coordinates> GetStopsCoordinates() const;

    };

} // namespace transport_catalogue
