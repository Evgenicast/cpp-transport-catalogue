#include "transport_router.h"
#include <iostream>

TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue & m_TransportCatalogue)
: m_TransportCatalogueRef(m_TransportCatalogue){}

void TransportRouter::SetRouteData(const int BusWaitTime, const double BusVelocity)
{
    m_BusWaitTime = BusWaitTime;
    m_BusVelocity = BusVelocity;
}

std::pair<size_t, double> TransportRouter::GetRoutingSettings() const
{
    return {m_BusWaitTime, m_BusVelocity};
}

void TransportRouter::ConstructGraph()
{
    m_Graph.SetSize(m_TransportCatalogueRef.GetAllStops().size() * 2);
    ConstructWaitEdge();
    ConstructBusEdge();

    m_RouterPtr = new graph::Router<double>(m_Graph);
}

const TransportRouter::RouteData TransportRouter::GetRoute(const std::string_view From, const std::string_view To)
{
    if(!m_RouterPtr)
    {
        ConstructGraph();
    }

    RouteData RouteDataTmp;
    auto RouterTmp = m_RouterPtr->BuildRoute(m_VertexWaitUnMap.at(From), m_VertexWaitUnMap.at(To));
    if(RouterTmp)
    {
        RouteDataTmp.m_Isfound = true;
        for(const auto & Router : RouterTmp->edges)
        {
            auto EdgeTmp = m_Graph.GetEdge(Router);
            RouteDataTmp.m_TotalTime += EdgeTmp.weight;
            RouteDataTmp.m_ItemsDeque.push_back(Item({EdgeTmp.Name, (EdgeTmp.Type == graph::EdgeType::BUS) ?
                                               EdgeTmp.SpanCount : 0, EdgeTmp.weight, EdgeTmp.Type}));

        }
    }
    return RouteDataTmp;
}

void TransportRouter::ConstructWaitEdge()
{
    int VertexID = 0;
    for (const auto & Stop : m_TransportCatalogueRef.GetAllStops()) //EDGE WAIT
    {
        m_VertexWaitUnMap.insert({Stop->m_Name, VertexID});
        m_VertexDistanceUnMap.insert({Stop->m_Name, ++VertexID});
        m_Graph.AddEdge({m_VertexWaitUnMap.at(Stop->m_Name), m_VertexDistanceUnMap.at(Stop->m_Name),
                        static_cast<double>(m_BusWaitTime), Stop->m_Name, graph::EdgeType::WAIT, 0});
        ++VertexID;
    }
}

void TransportRouter::ConstructBusEdge()
{
    for(const auto & Route : m_TransportCatalogueRef.GetBuses())
    {
        for(size_t BusFrom_it = 0; BusFrom_it < Route->m_BusStopsDeque.size() - 1; ++BusFrom_it)
        {
            int SpanCount = 0;
            for(size_t BusTo_it = BusFrom_it + 1; BusTo_it < Route->m_BusStopsDeque.size(); ++BusTo_it)
            {
                double RouteDistance = 0.0;
                for(size_t Iter = BusFrom_it + 1; Iter <= BusTo_it; ++Iter)
                {
                    RouteDistance += m_TransportCatalogueRef.ComputeDistance(Route->m_BusStopsDeque[Iter - 1], Route->m_BusStopsDeque[Iter]);
                }
                m_Graph.AddEdge({m_VertexDistanceUnMap.at(Route->m_BusStopsDeque[BusFrom_it]->m_Name),
                                 m_VertexWaitUnMap.at(Route->m_BusStopsDeque[BusTo_it]->m_Name),
                                 RouteDistance / (m_BusVelocity * s_MetersInKm / s_SecondsInMinute),
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
                        RouteDistance += m_TransportCatalogueRef.ComputeDistance(Route->m_BusStopsDeque[Iter], Route->m_BusStopsDeque[Iter - 1]);
                    }
                    m_Graph.AddEdge({m_VertexDistanceUnMap.at(Route->m_BusStopsDeque[BusFrom_it]->m_Name),
                                     m_VertexWaitUnMap.at(Route->m_BusStopsDeque[BusTo_it]->m_Name),
                                     RouteDistance / (m_BusVelocity * s_MetersInKm / s_SecondsInMinute),
                                     Route->m_BusNumber, graph::EdgeType::BUS, ++SpanCount});
                }
            }
        }
    }
}

