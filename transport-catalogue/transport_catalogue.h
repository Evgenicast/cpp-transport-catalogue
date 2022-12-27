#ifndef TRANSPORT_CATALOGUE_H
#define TRANSPORT_CATALOGUE_H

#include "geo.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <set>

namespace transport_catalogue
{

    using StopToStop = std::pair<std::string, std::string>;

    struct StopInputData
    {
        StopInputData(const std::string_view & Name_, const double & Lat, const double & Lng)
        : Name(Name_), Coords({Lat, Lng}){};

        std::string Name;
        geo::Coordinates Coords;
    };

    struct BusInputData
    {
        BusInputData() = default;
        explicit BusInputData(const std::string_view & RouteName_, const bool & CIRCLE_ROUTE_, const std::deque<const StopInputData*> & BusRoutes_,
        std::unordered_set<const StopInputData*> & UniqueStops_, const int & Length_, const double & Curvature_ )
        : RouteName(RouteName_),
          CIRCLE_ROUTE(CIRCLE_ROUTE_),
          BusRoutes(BusRoutes_),
          UniqueStops(std::move(UniqueStops_)),
          Length(Length_), Curvature(Curvature_)
        {};

        std::string RouteName;
        bool CIRCLE_ROUTE = false;
        std::deque<const StopInputData*> BusRoutes;
        std::unordered_set<const StopInputData*> UniqueStops;
        int Length;
        double Curvature;
    };

    class TransportCatalogue
    {
    private:

        class BusOutputData
        {
        private:

            static int Count; // для проверки на утечку памяти. Объект создается динамически.

        public:

            BusOutputData() = delete;
            explicit BusOutputData( const std::string_view & BusName_, const double & Length_, const bool & IsRouteCircle_,
            const size_t & StopsCount_, const size_t & UniqueStopCount_ )
            {
                ++Count;

                BusName = BusName_;
                Length = Length_;
                IsRouteCircle = IsRouteCircle_;
                StopsCount = StopsCount_;
                UniqueStopCount = UniqueStopCount_;
            }

            explicit BusOutputData( const std::string_view & BusName_, const double & Length_, const bool & IsRouteCircle_,
            const size_t & StopsCount_, const size_t & UniqueStopCount_, double & Curvature_)
            :   BusName(BusName_), Length(Length_), IsRouteCircle(IsRouteCircle_),
                StopsCount(StopsCount_), UniqueStopCount(UniqueStopCount_), Curvature(Curvature_)
            {
                ++Count;
            }

            BusOutputData(const BusOutputData &) = delete;
            BusOutputData(BusOutputData &&) noexcept = delete;

            BusOutputData & operator=(const BusOutputData &) = delete;
            BusOutputData & operator=(BusOutputData &&) noexcept = delete;

            ~BusOutputData() { --Count; }
            static int GetCount() {return Count;}

            std::string BusName;
            double Length;
            bool IsRouteCircle;
            size_t StopsCount;
            size_t UniqueStopCount;
            double Curvature;
        };

        class StopOutputData
        {
        private:

            static int cnt;

        public:

            StopOutputData() = delete;
            explicit StopOutputData( const std::string_view & StopName_, const std::set<std::string_view> & BusesForStop_, const bool IsBus_ )
                : StopName(StopName_), BusesForStop(/*std::move(BusesForStop_)*/BusesForStop_), IsBus(IsBus_) // Внимательно! Сам же запретил перемещиение!
            {
                ++cnt;
            }

            StopOutputData(const StopOutputData &) = delete;
            StopOutputData(StopOutputData &&) noexcept = delete;

            StopOutputData & operator=(const StopOutputData &) = delete;
            StopOutputData & operator=(StopOutputData &&) noexcept = delete;

            ~StopOutputData() {--cnt;};

            std::string StopName;
            std::set<std::string_view> BusesForStop;
            bool IsBus;
            static int GetCount() {return cnt;}
        };

        class StopsDistPtrPairHasher
        {
        private:

            std::hash<const void*> PairHasher;

        public:
            size_t operator() (const std::pair<const StopInputData *, const StopInputData *> & Stop) const
            {
                return PairHasher(Stop.first) + (PairHasher(Stop.second) * 29);
            }
        };

        std::deque<BusInputData> Buses;
        std::deque<StopInputData> Stops;
        std::deque <StopOutputData> BusesForStop;
        std::unordered_map<std::string_view, BusInputData*> BusesMap;
        std::unordered_map<std::string_view, StopInputData*> StopsMap;
        std::unordered_map<std::string_view, StopOutputData*> BusesForStopMap;

        std::unordered_map<std::pair<const StopInputData*, const StopInputData *>, size_t, StopsDistPtrPairHasher> DistanceBetweenStops;

        public:

        TransportCatalogue();

        void AddStop(const std::string_view & StopName , double & LAT, const double & LNG);
        void AddBus(const std::string_view & BusName, const bool & IsCircleRoute, const std::deque<std::string> & Stops);

        void CalcAndAddLengthToBusData(const std::string_view & BusName);
        void FindAndAddBusesForStop(const std::string_view & Stop);

        TransportCatalogue::BusOutputData * FindBusRoute(const std::string_view & BusName);
        TransportCatalogue::StopOutputData * FindStopData(const std::string_view & Stop);

        const StopInputData * CheckIsStop(const std::string_view & Stop) const;
        const BusInputData * CheckIsBus(const std::string_view & Bus) const;
        const StopOutputData * CheckIsStopForBus(const std::string_view & Stop) const;

        void SetDistanceBetweenStops(const std::string_view & StopFrom_, const std::string_view & StopTo_, const int & Distance);
        int GetDistanceBetweenStops(const StopInputData * StopFrom, const StopInputData *StopTo ) const;

        static int GetCountOfBusses() {return BusOutputData::GetCount();}
        static int GetCountOfStopData() {return StopOutputData::GetCount();}

    };
}//transport_catalogue
#endif // TRANSPORT_CATALOGUE_H
