#ifndef TRANSPORT_ROUTER_H
#define TRANSPORT_ROUTER_H

//#define KOAF(a,b) ({a/b})

#include "transport_catalogue.h"
#pragma once

#include "router.h"
#include <memory>
#include <deque>

class TransportRouter
{
public:

    struct Item
    {
        Item() = default;
        Item(const Item &) = default;
        Item(Item &&) noexcept = default;
        Item & operator=(const Item &) = delete;
        Item & operator=(Item &&) noexcept = delete;

        std::string m_Name;
        int m_SpanCount = 0;
        double m_Time = 0.0;
        graph::EdgeType m_Type;
    };

    struct RouteData
    {
        RouteData() = default;
        RouteData(const RouteData &) = default;
        RouteData(RouteData &&) noexcept = default;
        RouteData & operator=(const RouteData &) = delete;
        RouteData & operator=(RouteData &&) noexcept = delete;

        double m_TotalTime = 0.0;
        std::deque<Item> m_ItemsDeque;
        bool m_Isfound = false;
    };

private:

    const transport_catalogue::TransportCatalogue & m_TransportCatalogueRef;

    graph::DirectedWeightedGraph<double> m_Graph;
    std::unordered_map<std::string_view, size_t> m_VertexWaitUnMap;
    std::unordered_map<std::string_view, size_t> m_VertexDistanceUnMap;
    graph::Router<double> * m_RouterPtr = nullptr;

    size_t m_BusWaitTime = 0;
    double m_BusVelocity = 0.0;
    static constexpr double s_SecondsInMinute = 60;
    static constexpr double s_MetersInKm = 1000;

public:

    TransportRouter(const transport_catalogue::TransportCatalogue & m_TransportCatalogue);
    TransportRouter(const TransportRouter &) = delete;
    TransportRouter(TransportRouter &&) noexcept = delete;
    TransportRouter & operator=(const TransportRouter &) = delete;
    TransportRouter & operator=(TransportRouter &&) noexcept = delete;

    void SetRouteData(const int BusWaitTime, const double BusVelocity);
    const RouteData GetRoute(const std::string_view From, const std::string_view To);
    void ConstructGraph();
    void ConstructWaitEdge();
    void ConstructBusEdge();
    ~TransportRouter()
    {
         delete m_RouterPtr;
    }
};

#endif // TRANSPORT_ROUTER_H
