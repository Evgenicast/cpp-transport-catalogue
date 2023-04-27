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

        std::deque<tc_base::Bus> m_BusesDeque;
        std::deque<tc_base::Stop> m_StopsDeque;
        std::unordered_map<std::string_view, const tc_base::Bus*> m_BusesUnMap;
        std::unordered_map<std::string_view, const tc_base::Stop*> m_StopsUnMap;
        std::unordered_map<std::pair<const tc_base::Stop*, const tc_base::Stop*>, int, tc_utilities::StopsDistPtrPairHasher> m_DistanceBetweenStopsUnMap;

    public:

        TransportCatalogue() = default;
        TransportCatalogue(TransportCatalogue &) = delete;
        TransportCatalogue & operator=(TransportCatalogue &) noexcept = delete;
        TransportCatalogue(TransportCatalogue &&) = delete;
        TransportCatalogue & operator=(TransportCatalogue &&) noexcept = delete;

        void AddBus(std::string_view BusNumber, const std::vector<std::string_view> & StopsVec, const bool & IsCircleRoute);
        void AddStop(std::string_view StopName, const double & LAT, const double & LNG);

        const tc_base::Bus * FindBus(const std::string_view & BusNumber) const;
        const tc_base::Stop * FindStop(const std::string_view & StopName) const;

        const tc_request::Stop * GetBusesByStop(const std::string_view & StopName) const;
        const tc_request::Bus * GetStopsByBus(const std::string_view & BusNumber) const;

        std::deque<const tc_base::Stop*> GetStops(const std::string_view & BusNumber) const;
        std::deque<const tc_base::Bus*> GetBuses() const;
        const std::vector<const tc_base::Stop*> GetAllStops() const;

        double ComputeDistance(const tc_base::Stop* From, const tc_base::Stop * To) const;
        void SetDistanceBetweenStops(const std::string_view & FromStop, const std::string_view & ToStop, const int & Distance);
        int GetDistanceBetweenStops(const tc_base::Stop * From, const tc_base::Stop * To) const;
        const std::vector<geo::Coordinates> GetStopsCoordinates() const;

        ~TransportCatalogue() = default;
    };
} // namespace transport_catalogue
