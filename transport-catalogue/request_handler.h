#pragma once
#include "svg.h"
#include "transport_router.h"
#include "map_renderer.h"

class RequestHandler
{
private:

    const TransportRouter & m_TransportRouterRef;
    const renderer::MapRenderer & m_RendererRef;

public:

    RequestHandler() = delete;
    RequestHandler(const TransportRouter & TransportRouter, const renderer::MapRenderer & Renderer);

    RequestHandler(const RequestHandler &) = delete;
    RequestHandler & operator=(const RequestHandler &) = delete;
    RequestHandler(RequestHandler &&) noexcept = delete;
    RequestHandler & operator=(RequestHandler &&) noexcept = delete;

    std::optional<const tc_request::Bus*> GetBusData(const std::string_view & BusName) const;
    std::optional<const tc_request::Stop*> GetStopData(const std::string_view & StopData) const;

    svg::Document RenderMap() const;
};
