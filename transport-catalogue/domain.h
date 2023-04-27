#pragma once

#include "geo.h"
#include <deque>
#include <string>
#include <set>
#include <unordered_set>

namespace tc_base
{
    using StopToStop = std::pair<std::string, std::string>;
    struct Stop
    {
        std::string m_Name;
        geo::Coordinates m_Coords;

        Stop(std::string_view Name, geo::Coordinates Coords);
    };

    struct Bus
    {
        std::string m_BusNumber;
        bool m_IsCircleRoute;
        std::deque<const Stop*> m_BusStopsDeque;
        std::unordered_set<const Stop*> m_UniqueStopsUnSet;

        Bus(std::string_view BusNumber, bool IsCircleRoute, std::deque<const Stop*> BusStopsDeque,
        std::unordered_set<const Stop*> UniqueStopsUnSet);
    };

}//namespace tc_base

namespace tc_request
{
    class Bus
    {
    private:

        static int s_CountBuses; // для проверки на утечку памяти. Объект создается динамически.
                                //constexpr - нельзя, ибо определяется в другом месте.
        std::string m_BusNumber;
        int m_StopsCount;
        size_t m_UniqueStopsCount;
        unsigned int m_RouteLength;
        double m_Curvature;

    public:

        Bus(std::string_view BusNumber, size_t StopsCount, size_t UniqueStopsCount, unsigned int RouteLength, double Curvature);

        Bus(const Bus &) = delete;
        Bus(Bus &&) noexcept = delete;
        Bus & operator=(const Bus &) = delete;
        Bus & operator=(Bus &&) noexcept = delete;

        const std::string & GetBusNumber() const { return m_BusNumber; }
        const int & GetStopsCount() const { return m_StopsCount; }
        const size_t & GetUniqueStopsCount() const { return m_UniqueStopsCount; }
        const unsigned int & GetRouteLength() const { return m_RouteLength; }
        const double & GetCurvature() const { return m_Curvature; }

        ~Bus() { --s_CountBuses; } // для проверки на утечку памяти. Объект создается динамически.
        static int GetCount() { return s_CountBuses; } // для проверки на утечку памяти. Объект создается динамически.
    };

    class Stop
    {
    private:

         static int s_CountStops;
         std::string m_StopName;
         std::set<std::string_view> m_BusesForStopSet;

    public:

        Stop(std::string_view StopName, std::set<std::string_view> BusesForStop);

        Stop(const Stop &) = delete;
        Stop(Stop &&) noexcept = delete;
        Stop & operator=(const Stop &) = delete;
        Stop & operator=(Stop &&) noexcept = delete;

        const std::string & GetStopName() const { return m_StopName; }
        const std::set<std::string_view> & GetBusesForStop () const { return m_BusesForStopSet; }

        ~Stop() { --s_CountStops; }
        static int GetCount() { return s_CountStops; }
    };

}//namespace tc_request

namespace tc_utilities
{
    class StopsDistPtrPairHasher
    {
    private:

        std::hash<const void*> PairHasher;

    public:

        size_t operator() (const std::pair<const tc_base::Stop*, const tc_base::Stop*> & Stop) const
        {
            return PairHasher(Stop.first) + (PairHasher(Stop.second) * 29);
        }
    };

}//namespace tc_utilities

    inline int GetCountOfBusses() {return tc_request::Bus::GetCount();}
    inline int GetCountOfStopData() {return tc_request::Stop::GetCount();}

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
