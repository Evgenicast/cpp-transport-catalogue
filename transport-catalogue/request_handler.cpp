#include "request_handler.h"

RequestHandler::RequestHandler(const transport_catalogue::TransportCatalogue & TransportCatalogue, const renderer::MapRenderer & Renderer)
: m_TransportCatalogueRef(TransportCatalogue), m_RendererRef(Renderer){}

std::optional<const tc_request::Bus*> RequestHandler::GetBusData(const std::string_view & BusName) const
{
    return m_TransportCatalogueRef.GetStopsByBus(BusName);
}

std::optional<const tc_request::Stop *> RequestHandler::GetStopData(const std::string_view & StopData) const
{

    return m_TransportCatalogueRef.GetBusesByStop(StopData);
}

svg::Document RequestHandler::RenderMap() const
{
    return m_RendererRef.GetRender();
}
