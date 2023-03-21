#pragma once
#include "svg.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

class RequestHandler
{
private:
    const transport_catalogue::TransportCatalogue & TC;  
    const renderer::MapRenderer & Renderer;
public:
    RequestHandler() = delete;
    RequestHandler(const transport_catalogue::TransportCatalogue & TC, const renderer::MapRenderer & Renderer_)
    : TC(TC), Renderer(Renderer_) {};

    RequestHandler(const RequestHandler &) = delete;
    RequestHandler & operator=(const RequestHandler &) = delete;
    RequestHandler(RequestHandler &&) noexcept = delete;
    RequestHandler & operator=(RequestHandler &&) noexcept = delete;

    std::optional<const domain::BusOutputData*> GetBusData(const std::string_view & BusName) const;
    std::optional<const domain::StopOutputData*> GetStopData(const std::string_view & StopData) const;

    svg::Document RenderMap() const;
};
