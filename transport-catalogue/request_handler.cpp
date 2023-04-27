#include "request_handler.h"

RequestHandler::RequestHandler(const TransportRouter & TransportRouter, const renderer::MapRenderer & Renderer)
: m_TransportRouterRef(TransportRouter), m_RendererRef(Renderer){}

std::optional<const tc_request::Bus*> RequestHandler::GetBusData(const std::string_view & BusName) const
{
    return m_TransportRouterRef.GetStopsByBus(BusName);
}

std::optional<const tc_request::Stop *> RequestHandler::GetStopData(const std::string_view & StopData) const
{

    return m_TransportRouterRef.GetBusesByStop(StopData);
}

svg::Document RequestHandler::RenderMap() const
{
    return m_RendererRef.GetRender();
}
