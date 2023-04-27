#include "transport_router.h"
#include <iostream>

TransportRouter::TransportRouter()
{}

void TransportRouter::SetRouteData(const int BusWaitTime_, const double BusVelocity_)
{
    BusWaitTime = BusWaitTime_;
    BusVelocity = BusVelocity_;
}

void TransportRouter::ConstructGraph()
{
    GraphObj_.SetSize(GetAllStops().size() * 2);
    ConstructWaitEdge();
    ConstructBusEdge();

    RouterPtr_raw = new graph::Router<double>(GraphObj_);
}

const TransportRouter::RouteData TransportRouter::GetRoute(const std::string_view From, const std::string_view To)
{
    if(!RouterPtr_raw)
    {
        ConstructGraph();
    }

    RouteData RouteDataTmp;
    auto RouterTmp = RouterPtr_raw->BuildRoute(VertexWait.at(From), VertexWait.at(To));
    if(RouterTmp)
    {
        RouteDataTmp.Isfound = true;
        for(const auto & Router : RouterTmp->edges)
        {
            auto EdgeTmp = GraphObj_.GetEdge(Router);
            RouteDataTmp.TotalTime += EdgeTmp.weight;
            RouteDataTmp.Items.push_back(Item({EdgeTmp.Name,
                                   (EdgeTmp.Type == graph::EdgeType::BUS) ? EdgeTmp.SpanCount : 0,
                                   EdgeTmp.weight,
                                   EdgeTmp.Type}));

        }
    }
    return RouteDataTmp;
}

void TransportRouter::ConstructWaitEdge()
{
    int VertexID = 0;
    for (const auto & Stop : GetAllStops()) //EDGE WAIT
    {
        VertexWait.insert({Stop->m_Name, VertexID});
        VertexDistance.insert({Stop->m_Name, ++VertexID});
        GraphObj_.AddEdge({VertexWait.at(Stop->m_Name),
                          VertexDistance.at(Stop->m_Name),
                          static_cast<double>(BusWaitTime),
                          Stop->m_Name, graph::EdgeType::WAIT, 0});
        ++VertexID;
    }
}

void TransportRouter::ConstructBusEdge()
{
    for(const auto & Route : GetBuses())
    {
        for(size_t BusFrom_it = 0; BusFrom_it < Route->m_BusStopsDeque.size() - 1; ++BusFrom_it)
        {
            int SpanCount = 0;
            for(size_t BusTo_it = BusFrom_it + 1; BusTo_it < Route->m_BusStopsDeque.size(); ++BusTo_it)
            {
                double RouteDistance = 0.0;
                for(size_t Iter = BusFrom_it + 1; Iter <= BusTo_it; ++Iter)
                {
                    RouteDistance += ComputeDistance(Route->m_BusStopsDeque[Iter - 1], Route->m_BusStopsDeque[Iter]);
                }
                GraphObj_.AddEdge({VertexDistance.at(Route->m_BusStopsDeque[BusFrom_it]->m_Name),
                                 VertexWait.at(Route->m_BusStopsDeque[BusTo_it]->m_Name),
                                 RouteDistance / (BusVelocity * 1000.0 / 60.0),
                                   Route->m_BusNumber, graph::EdgeType::BUS, ++SpanCount});
            }
        }
        if(!Route->m_IsCircleRoute)
        {
            for(int BusFrom_it = Route->m_BusStopsDeque.size() - 1; BusFrom_it > 0; --BusFrom_it)
            {
                int SpanCount = 0;
                for(int BusTo_it = BusFrom_it - 1; BusTo_it >= 0; --BusTo_it)
                {
                    double RouteDistance = 0.0;
                    for(int Iter = BusFrom_it; Iter > BusTo_it; --Iter)
                    {
                        RouteDistance += ComputeDistance(Route->m_BusStopsDeque[Iter], Route->m_BusStopsDeque[Iter - 1]);
                    }
                   GraphObj_.AddEdge({VertexDistance.at(Route->m_BusStopsDeque[BusFrom_it]->m_Name),
                     VertexWait.at(Route->m_BusStopsDeque[BusTo_it]->m_Name),
                     RouteDistance /
                     (BusVelocity * static_cast<Speed>(1000) / static_cast<Speed>(60)), //static constexpr Speed!!
                     Route->m_BusNumber, graph::EdgeType::BUS, ++SpanCount});           // сделать в т.ч. для  60.0
                }
            }
        }
    }
}

