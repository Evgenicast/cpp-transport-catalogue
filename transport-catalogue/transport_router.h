#ifndef TRANSPORT_ROUTER_H
#define TRANSPORT_ROUTER_H

#pragma once

#include "transport_catalogue.h"
#include "router.h"
#include <memory>

class TransportRouter : public transport_catalogue::TransportCatalogue
{
public:

    using Speed = double; using Wait = size_t;
    struct Item
    {
        Item() = default;
        Item(const Item &) = default;
        Item(Item &&) noexcept = default;
        Item & operator=(const Item &) = delete;
        Item & operator=(Item &&) noexcept = delete;

        std::string Name;
        int SpanCount = 0;
        double Time = 0.0;
        graph::EdgeType Type;
    };

    struct RouteData
    {
        RouteData() = default;
        RouteData(const RouteData &) = default;
        RouteData(RouteData &&) noexcept = default;
        RouteData & operator=(const RouteData &) = delete;
        RouteData & operator=(RouteData &&) noexcept = delete;

        double TotalTime = 0.0;
        std::deque<Item> Items;
        bool Isfound = false;
    };

private:
    graph::Router<double> * RouterPtr_raw = nullptr;
    graph::DirectedWeightedGraph<double> GraphObj_;
    std::unordered_map<std::string_view, size_t> VertexWait;
    std::unordered_map<std::string_view, size_t> VertexDistance;

    Wait BusWaitTime = 0; Speed BusVelocity = 0.0;
public:
    TransportRouter();
    TransportRouter(const TransportRouter &) = delete;
    TransportRouter(TransportRouter &&) noexcept = delete;
    TransportRouter & operator=(const TransportRouter &) = delete;
    TransportRouter & operator=(TransportRouter &&) noexcept = delete;

    void SetRouteData(const int BusWaitTime_, const double BusVelocity_);
    const RouteData GetRoute(const std::string_view From, const std::string_view To);
    void ConstructGraph();
    void ConstructWaitEdge();
    void ConstructBusEdge();
    ~TransportRouter()
    {
         delete RouterPtr_raw;
    }
};

#endif // TRANSPORT_ROUTER_H
