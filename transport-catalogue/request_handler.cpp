#include "request_handler.h"

std::optional<const domain::BusOutputData*> RequestHandler::GetBusData(const std::string_view & BusName) const
{
    return TC.GetStopsByBus(BusName);
}

std::optional<const domain::StopOutputData*> RequestHandler::GetStopData(const std::string_view & StopData) const
{

    return TC.GetBusesByStop(StopData);
}

svg::Document RequestHandler::RenderMap() const
{
    return Renderer.GetRender();
}
