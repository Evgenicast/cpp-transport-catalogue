#pragma once

#include "geo.h"
#include <deque>
#include <string>
#include <set>
#include <unordered_set>

namespace domain
{
    using StopToStop = std::pair<std::string, std::string>;

    struct StopInputData
    {
        StopInputData(const std::string_view & Name_, const double & LAT, const double & LNG)
        : Name(Name_), Coords({LAT, LNG}){};

        std::string Name;
        geo::Coordinates Coords;
    };

    struct BusInputData
    {
        BusInputData(const std::string_view & BusNumber_, const bool & CIRCLE_ROUTE_, std::deque<const StopInputData*> & BusStops_,
        std::unordered_set<const StopInputData*> & UniqueStops_)
        : BusNumber(BusNumber_), CIRCLE_ROUTE(CIRCLE_ROUTE_), BusStops(BusStops_), UniqueStops(UniqueStops_){};

        std::string BusNumber;
        bool CIRCLE_ROUTE;
        std::deque<const StopInputData*> BusStops;
        std::unordered_set<const StopInputData*> UniqueStops;
    };

    class BusOutputData
    {
    private:
        static int Count; // для проверки на утечку памяти. Объект создается динамически.

        std::string BusNumber;
        int StopsCount;
        size_t UniqueStopsCount;
        unsigned int RouteLength;
        double Curvature;
    public:
        BusOutputData(const std::string_view & BusNumber_, const size_t & StopsCount_, const size_t & UniqueStopsCount_, const unsigned int & RouteLength_,
        const double & Curvature_)
        : BusNumber(BusNumber_), StopsCount(StopsCount_), UniqueStopsCount(UniqueStopsCount_), RouteLength(RouteLength_), Curvature(Curvature_)
        {
            ++Count; // для проверки на утечку памяти. Объект создается динамически.
        }
        ~BusOutputData() { --Count; } // для проверки на утечку памяти. Объект создается динамически.
        static int GetCount() { return Count; } // для проверки на утечку памяти. Объект создается динамически.

        BusOutputData(const BusOutputData &) = delete;
        BusOutputData(BusOutputData &&) noexcept = delete;
        BusOutputData & operator=(const BusOutputData &) = delete;
        BusOutputData & operator=(BusOutputData &&) noexcept = delete;

        const std::string & GetBusNumber() const { return BusNumber; }
        const int & GetStopsCount() const { return StopsCount; }
        const size_t & GetUniqueStopsCount() const { return UniqueStopsCount; }
        const unsigned int & GetRouteLength() const { return RouteLength; }
        const double & GetCurvature() const { return Curvature; }
    };

    class StopOutputData
    {
    private:
         static int cnt;
         std::string StopName;
         std::set<std::string_view> BusesForStop;
    public:
        StopOutputData(const std::string_view & StopName_, const std::set<std::string_view> & BusesForStop_)
        : StopName(StopName_), BusesForStop(BusesForStop_)
        {
            ++cnt;
        }
        ~StopOutputData() { --cnt; };
        static int GetCount() { return cnt; }

        StopOutputData(const StopOutputData &) = delete;
        StopOutputData(StopOutputData &&) noexcept = delete;

        StopOutputData & operator=(const StopOutputData &) = delete;
        StopOutputData & operator=(StopOutputData &&) noexcept = delete;

        const std::string & GetStopName() const { return StopName; }
        const std::set<std::string_view> & GetBusesForStop () const { return BusesForStop; }
    };

    class StopsDistPtrPairHasher
    {
    private:
        std::hash<const void*> PairHasher;
    public:
        size_t operator() (const std::pair<const StopInputData*, const StopInputData*> & Stop) const
        {
            return PairHasher(Stop.first) + (PairHasher(Stop.second) * 29);
        }
    };

    inline int GetCountOfBusses() {return BusOutputData::GetCount();}
    inline int GetCountOfStopData() {return StopOutputData::GetCount();}
}// namespace domain
