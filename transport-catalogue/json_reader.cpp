#include "json_reader.h"
#include <sstream>
#include <fstream>
#include <map>

namespace json_proccessing
{
JsonReader::JsonReader(std::istream & Input)
{
    Object = json::Load(Input).GetRoot().AsDict(); //можно локально, но данные файла могут понадобитсья.
    ReadBaseRequest(TC);
    ReadRenderSettings(TC, MapRenderer);
    const auto DocumentOutput = ReadStatRequest(RequestHandler(TC, MapRenderer));
    json::Print(json::Document(DocumentOutput), std::cout);
}

void JsonReader::ReadBaseRequest(transport_catalogue::TransportCatalogue & TC)
{
    const auto ObjectBase = Object.find("base_requests");
    if (ObjectBase != Object.end())
    {
        for (const auto & Request : ObjectBase->second.AsArray())
        {
            const auto RequestType = Request.AsDict().find("type");
            if (RequestType != Request.AsDict().end())
            {
                if (RequestType->second.AsString() == "Stop")
                {
                    ReadStopData(TC, Request.AsDict());
                }
            }
        }

        for (const auto & Request : ObjectBase->second.AsArray())
        {
            const auto RequestType = Request.AsDict().find("type");
            if (RequestType != Request.AsDict().end())
            {
                if (RequestType->second.AsString() == "Stop")
                {
                    ReadStopDistance(TC, Request.AsDict());
                }
            }
        }

        for (const auto & Request : ObjectBase->second.AsArray())
        {
            const auto RequestType = Request.AsDict().find("type");
            if (RequestType != Request.AsDict().end())
            {
                if (RequestType->second.AsString() == "Bus")
                {
                    ReadBusData(TC, Request.AsDict());
                }
            }
        }
    }
}

const svg::Color JsonReader::GetColor(const json::Node & Color)
{
    if (Color.IsString())
    {
        return svg::Color{ Color.AsString() };
    }
    else if (Color.IsArray())
    {
        if (Color.AsArray().size() == 3)
        {
            return svg::Rgb
            {
                static_cast<uint8_t>(Color.AsArray()[0].AsInt()),
                static_cast<uint8_t>(Color.AsArray()[1].AsInt()),
                static_cast<uint8_t>(Color.AsArray()[2].AsInt())
            };
        }
        else if (Color.AsArray().size() == 4)
        {
            return svg::Rgba
            {
                static_cast<uint8_t>(Color.AsArray()[0].AsInt()),
                static_cast<uint8_t>(Color.AsArray()[1].AsInt()),
                static_cast<uint8_t>(Color.AsArray()[2].AsInt()),
                Color.AsArray()[3].AsDouble()
            };
        }
    }
    return svg::Color();
}

const json::Dict JsonReader::GetMapRender(const RequestHandler & RequestHandler, const json::Dict & Dict)
{
    svg::Document Render = RequestHandler.RenderMap();
    std::ostringstream strm;
    Render.Render(strm);
    json::Dict MapRender;
    MapRender.emplace("map", strm.str());
    MapRender.emplace("request_id", Dict.at("id").AsInt());
    return MapRender;
}

void JsonReader::ReadRenderSettings(transport_catalogue::TransportCatalogue & TC, renderer::MapRenderer & MapRenderer)
{
    const auto RenderSettings = Object.find("render_settings");
    if (RenderSettings != Object.end())
    {
        renderer::RenderSettings Settings;
        Settings.Width = RenderSettings->second.AsDict().at("width").AsDouble();
        Settings.Height = RenderSettings->second.AsDict().at("height").AsDouble();
        Settings.Padding = RenderSettings->second.AsDict().at("padding").AsDouble();
        Settings.LineWidth = RenderSettings->second.AsDict().at("line_width").AsDouble();
        Settings.StopRadius = RenderSettings->second.AsDict().at("stop_radius").AsDouble();
        Settings.BusLabelFontSize = RenderSettings->second.AsDict().at("bus_label_font_size").AsInt();
        Settings.BusLabelOffset = { RenderSettings->second.AsDict().at("bus_label_offset").AsArray()[0].AsDouble(), RenderSettings->second.AsDict().at("bus_label_offset").AsArray()[1].AsDouble() };
        Settings.StopLabelFontSize = RenderSettings->second.AsDict().at("stop_label_font_size").AsInt();
        Settings.StopLabelOffset = { RenderSettings->second.AsDict().at("stop_label_offset").AsArray()[0].AsDouble(), RenderSettings->second.AsDict().at("stop_label_offset").AsArray()[1].AsDouble() };
        Settings.UnderLayerColor = GetColor(RenderSettings->second.AsDict().at("underlayer_color"));
        Settings.UnderlayerWidth = RenderSettings->second.AsDict().at("underlayer_width").AsDouble();
        for (const auto& color : RenderSettings->second.AsDict().at("color_palette").AsArray())
        {
            Settings.ColorPalette.emplace_back(GetColor(color));
        }
        MapRenderer.SetRenderSettings(Settings);

        const auto StopsCoordinates = TC.GetStopsCoordinates();
        renderer::utilites::SphereProjector Projector(StopsCoordinates.begin(), StopsCoordinates.end(), Settings.Width, Settings.Height, Settings.Padding);
        const std::vector<svg::Color> ColorPallete = MapRenderer.GetColorPallete();
        size_t ColorNumber = 0;
        std::map<std::string, svg::Point> Stops;
        for (const auto & it : TC.GetBuses())
        {
            std::vector<svg::Point> StopsPoints;
            for (const auto& stop : TC.GetStops(it->BusNumber))
            {
                StopsPoints.emplace_back(Projector(stop->Coords));
                Stops[stop->Name] = StopsPoints.back();
            }
            if (it->CIRCLE_ROUTE == true)
            {
                MapRenderer.AddTextRender(*StopsPoints.begin(), it->BusNumber, ColorPallete[ColorNumber], false);
                MapRenderer.AddRoutRender(StopsPoints, ColorPallete[ColorNumber]);
            }
            else if (it->CIRCLE_ROUTE == false)
            {
                MapRenderer.AddTextRender(*StopsPoints.begin(), it->BusNumber, ColorPallete[ColorNumber], false);
                if (*it->BusStops.begin() != it->BusStops.back())MapRenderer.AddTextRender(StopsPoints.back(), it->BusNumber, ColorPallete[ColorNumber], false);
                StopsPoints.insert(StopsPoints.end(), StopsPoints.rbegin() + 1, StopsPoints.rend());
                MapRenderer.AddRoutRender(StopsPoints, ColorPallete[ColorNumber]);
            }
            (ColorNumber == ColorPallete.size() - 1) ? ColorNumber = 0 : ++ColorNumber;
        }
        for (const auto & [Name, Coordinates] : Stops)
        {
            MapRenderer.AddStopPointRender(Coordinates);
            MapRenderer.AddTextRender(Coordinates, Name, ColorPallete[ColorNumber], true);
        }
    }
}

json::Array JsonReader::ReadStatRequest(const RequestHandler & RequestHandler)
{
    const auto ObjectBase = Object.find("stat_requests");
    json::Array OutputData;
    if (ObjectBase != Object.end())
    {
        for (const auto& request : ObjectBase->second.AsArray())
        {
            const auto request_type= request.AsDict().find("type");
            if (request_type!= request.AsDict().end())
            {
                if (request_type->second.AsString() == "Stop")
                {
                    OutputData.emplace_back(GetStopInfo(RequestHandler, request.AsDict()));
                }
                else if (request_type->second.AsString() == "Bus")
                {
                    OutputData.emplace_back(GetBusInfo(RequestHandler, request.AsDict()));
                }
                else if (request_type->second.AsString() == "Map")
                {
                    OutputData.emplace_back(GetMapRender(RequestHandler, request.AsDict()));
                }
            }
        }
    }
    return OutputData;
}

void JsonReader::ReadStopData(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict)
{
    const auto Name = Dict.at("name").AsString();
    const auto Latitude = Dict.at("latitude").AsDouble();
    const auto Longitude = Dict.at("longitude").AsDouble();
    TC.AddStop(Name, Latitude, Longitude);
}

void JsonReader::ReadStopDistance(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict)
{
    const auto StopNameFrom = Dict.at("name").AsString();
    const auto Stops = Dict.at("road_distances").AsDict();
    for (const auto & [StopNameTo, Distance] : Stops)
    {
        TC.SetDistanceBetweenStops(StopNameFrom, StopNameTo, Distance.AsInt());
    }
}

void JsonReader::ReadBusData(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict)
{
    const auto BusName = Dict.at("name").AsString();
    std::vector<std::string_view> Stops;
    for (const auto & Stop : Dict.at("stops").AsArray())
    {
        Stops.emplace_back(Stop.AsString());
    }
    const auto IsCircleRoute = Dict.at("is_roundtrip").AsBool();
    TC.AddBus(BusName, Stops, IsCircleRoute);
}

const json::Dict JsonReader::GetBusInfo(const RequestHandler & RequestHandler, const json::Dict & Dict)
{
    const auto BusName = Dict.at("name").AsString();
    const auto BusData = RequestHandler.GetBusData(BusName);
    json::Dict BusInfo;
    if (BusData == nullptr)
    {
        BusInfo.emplace("request_id", Dict.at("id").AsInt());
        BusInfo.emplace("error_message", "not found");
    }
    else
    {
        BusInfo.emplace("curvature", BusData.value()->GetCurvature());
        BusInfo.emplace("request_id", Dict.at("id").AsInt());
        BusInfo.emplace("route_length", static_cast<int>(BusData.value()->GetRouteLength()));
        BusInfo.emplace("stop_count", BusData.value()->GetStopsCount());
        BusInfo.emplace("unique_stop_count", static_cast<int>(BusData.value()->GetUniqueStopsCount()));
    }

    if (BusData.has_value())
    {
        delete BusData.value(); //если тут не удалить, то - утечка памяти. См. Тесты в Мейн.
    }

    return BusInfo;
}

const json::Dict JsonReader::GetStopInfo(const RequestHandler & RequestHandler, const json::Dict & Dict)
{

    const auto StopName = Dict.at("name").AsString();
    const auto StopData = RequestHandler.GetStopData(StopName);
    json::Dict StopInfo;

    if(StopData == nullptr)
    {
        StopInfo.emplace("request_id", Dict.at("id").AsInt());
        StopInfo.emplace("error_message", "not found");
    }
    else
    {
        json::Array Buses;
        for (auto & bus : StopData.value()->GetBusesForStop())
        {
            Buses.push_back(static_cast<std::string>(bus));
        }
        StopInfo.emplace("buses", Buses);
        StopInfo.emplace("request_id", Dict.at("id").AsInt());
    }

    if (StopData.has_value())
    {
        delete StopData.value(); //если тут не удалить, то - утечка памяти. См. Тесты в Мейн.
    }
    return StopInfo;
}
}//json_processing
