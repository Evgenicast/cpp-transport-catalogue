#include "json_reader.h"
#include "json_builder.h"
#include <sstream>
#include <fstream>
#include <map>

namespace json_proccessing
{
    JsonReader::JsonReader(std::istream & Input)
    {
        m_JsonObject = json::Load(Input).GetRoot().AsDict();
        ReadBaseRequest(m_TransportCatalogue);
        TransportRouter TransportRouter(m_TransportCatalogue);
        ReadRouteSettings(TransportRouter);
        ReadRenderSettings(m_TransportCatalogue, m_MapRenderer);
        const auto DocumentOutput = ReadStatRequest(RequestHandler(m_TransportCatalogue, m_MapRenderer), TransportRouter);
        json::Print(json::Document(json::Builder{}.Value(DocumentOutput).Build()), std::cout);
    }

    void JsonReader::ReadBaseRequest(transport_catalogue::TransportCatalogue & m_TransportCatalogue)
    {
        const auto ObjectBase = m_JsonObject.find("base_requests");
        if (ObjectBase != m_JsonObject.end())
        {
            for (const auto & Request : ObjectBase->second.AsArray())
            {
                const auto RequestType = Request.AsDict().find("type");
                if (RequestType != Request.AsDict().end())
                {
                    if (RequestType->second.AsString() == "Stop")
                    {
                        ReadStopData(m_TransportCatalogue, Request.AsDict());
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
                        ReadStopDistance(m_TransportCatalogue, Request.AsDict());
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
                        ReadBusData(m_TransportCatalogue, Request.AsDict());
                    }
                }
            }
        }
    }

    void JsonReader::ReadRouteSettings(TransportRouter & TransportRouter)
    {
        const auto RouteSettings = m_JsonObject.find("routing_settings");
        if (RouteSettings != m_JsonObject.end())
        {
            TransportRouter.SetRouteData(RouteSettings->second.AsDict().at("bus_wait_time").AsInt(),
                            RouteSettings->second.AsDict().at("bus_velocity").AsDouble());
        }
    }

    void JsonReader::ReadRenderSettings(transport_catalogue::TransportCatalogue & m_TransportCatalogue, renderer::MapRenderer & MapRenderer)
    {
        const auto RenderSettings = m_JsonObject.find("render_settings");
        if (RenderSettings != m_JsonObject.end())
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

            const auto StopsCoordinates = m_TransportCatalogue.GetStopsCoordinates();
            renderer::utilites::SphereProjector Projector(StopsCoordinates.begin(), StopsCoordinates.end(), Settings.Width, Settings.Height, Settings.Padding);
            const std::vector<svg::Color> ColorPallete = MapRenderer.GetColorPallete();
            size_t ColorNumber = 0;
            std::map<std::string, svg::Point> Stops;

            for (const auto & it : m_TransportCatalogue.GetBuses())
            {
                std::vector<svg::Point> StopsPoints;
                for (const auto & stop : m_TransportCatalogue.GetStops(it->m_BusNumber))
                {
                    StopsPoints.emplace_back(Projector(stop->m_Coords));
                    Stops[stop->m_Name] = StopsPoints.back();
                }
                if (it->m_IsCircleRoute == true)
                {
                    MapRenderer.AddTextRender(*StopsPoints.begin(), it->m_BusNumber, ColorPallete[ColorNumber], false);
                    MapRenderer.AddRoutRender(StopsPoints, ColorPallete[ColorNumber]);
                }
                else if (it->m_IsCircleRoute == false)
                {
                    MapRenderer.AddTextRender(*StopsPoints.begin(), it->m_BusNumber, ColorPallete[ColorNumber], false);
                    if (*it->m_BusStopsDeque.begin() != it->m_BusStopsDeque.back())MapRenderer.AddTextRender(StopsPoints.back(), it->m_BusNumber, ColorPallete[ColorNumber], false);
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

    json::Array JsonReader::ReadStatRequest(const RequestHandler & RequestHandler, TransportRouter & TransportRouter)
    {
        const auto ObjectBase = m_JsonObject.find("stat_requests");
        if (ObjectBase != m_JsonObject.end())
        {
            for (const auto & request : ObjectBase->second.AsArray())
            {
                const auto request_type= request.AsDict().find("type");
                if (request_type!= request.AsDict().end())
                {
                    if (request_type->second.AsString() == "Stop")
                    {
                        m_OutputData.emplace_back(GetStopInfo(RequestHandler, request.AsDict()));
                    }
                    else if (request_type->second.AsString() == "Bus")
                    {
                        m_OutputData.emplace_back(GetBusInfo(RequestHandler, request.AsDict()));
                    }
                    else if (request_type->second.AsString() == "Map")
                    {
                        m_OutputData.emplace_back(GetMapRender(RequestHandler, request.AsDict()));
                    }
                    else if (request_type->second.AsString() == "Route")
                    {
                        m_OutputData.emplace_back(GetRoute(TransportRouter, request.AsDict()));
                    }
                }
            }
        }

        return m_OutputData;
    }

    json::Node JsonReader::GetRoute(TransportRouter & TransportRouter, const json::Dict & Dict)
    {
        auto RouteData = TransportRouter.GetRoute(Dict.at("from").AsString(), Dict.at("to").AsString());
        if (!RouteData.m_Isfound)
        {
            return json::Builder{}.StartDict().Key("request_id").Value(Dict.at("id").AsInt())
            .Key("error_message").Value("not found").EndDict().Build().AsDict();
        }

        json::Array Items;
        for (const auto & Item : RouteData.m_ItemsDeque)
        {
            json::Dict ItemsMap;
            if(Item.m_Type == graph::EdgeType::BUS)
            {
                ItemsMap["type"] = "Bus";
                ItemsMap["bus"] = Item.m_Name;
                ItemsMap["span_count"] = Item.m_SpanCount;
            }
            else if(Item.m_Type == graph::EdgeType::WAIT)
            {
                ItemsMap["type"] = "Wait";
                ItemsMap["stop_name"] = Item.m_Name;
            }
            ItemsMap["time"] = Item.m_Time;
            Items.push_back(ItemsMap);
        }
        return json::Builder{}.StartDict()
        .Key("request_id").Value(Dict.at("id").AsInt())
        .Key("total_time").Value(RouteData.m_TotalTime)
        .Key("items").Value(Items)
        .EndDict().Build().AsDict();
    }

    void JsonReader::ReadStopData(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict)
    {
        const auto Name = Dict.at("name").AsString();
        const auto Latitude = Dict.at("latitude").AsDouble();
        const auto Longitude = Dict.at("longitude").AsDouble();
        m_TransportCatalogue.AddStop(Name, Latitude, Longitude);
    }

    void JsonReader::ReadStopDistance(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict)
    {
        const auto StopNameFrom = Dict.at("name").AsString();
        const auto Stops = Dict.at("road_distances").AsDict();
        for (const auto & [StopNameTo, Distance] : Stops)
        {
            m_TransportCatalogue.SetDistanceBetweenStops(StopNameFrom, StopNameTo, Distance.AsInt());
        }
    }

    void JsonReader::ReadBusData(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict)
    {
        const auto BusName = Dict.at("name").AsString();
        std::vector<std::string_view> Stops;
        for (const auto & Stop : Dict.at("stops").AsArray())
        {
            Stops.emplace_back(Stop.AsString());
        }
        const auto IsCircleRoute = Dict.at("is_roundtrip").AsBool();
        m_TransportCatalogue.AddBus(BusName, Stops, IsCircleRoute);
    }

    const json::Node JsonReader::GetBusInfo(const RequestHandler & RequestHandler, const json::Dict & Dict)
    {
        const std::string BusName = Dict.at("name").AsString();
        m_BusDataPtr = RequestHandler.GetBusData(BusName);

        return (m_BusDataPtr == nullptr) ? json::Builder{}.StartDict()
            .Key("request_id").Value(Dict.at("id").AsInt())
            .Key("error_message").Value("not found")
            .EndDict().Build().AsDict()
            : json::Builder{}.StartDict()
            .Key("curvature").Value(m_BusDataPtr.value()->GetCurvature())
            .Key("request_id").Value(Dict.at("id").AsInt())
            .Key("route_length").Value(static_cast<int>(m_BusDataPtr.value()->GetRouteLength()))
            .Key("stop_count").Value(m_BusDataPtr.value()->GetStopsCount())
            .Key("unique_stop_count").Value(static_cast<int>(m_BusDataPtr.value()->GetUniqueStopsCount()))
            .EndDict().Build();
    }

    const json::Node JsonReader::GetStopInfo(const RequestHandler & RequestHandler, const json::Dict & Dict)
    {
        const auto StopName = Dict.at("name").AsString();
        m_StopDataPtr = RequestHandler.GetStopData(StopName);
        json::Dict StopInfo;
        json::Array Buses;

        if (m_StopDataPtr == nullptr)
        {
            return json::Builder{}.StartDict()
                .Key("request_id").Value(Dict.at("id").AsInt())
                .Key("error_message").Value("not found")
                .EndDict().Build().AsDict();
        }
        else
        {
            for (auto & Bus : m_StopDataPtr.value()->GetBusesForStop())
            {
                Buses.push_back(static_cast<std::string>(Bus));
            }
        }

        return json::Builder{}.StartDict()
            .Key("buses").Value(Buses)
            .Key("request_id").Value(Dict.at("id").AsInt())
            .EndDict().Build();
    }

    const json::Node JsonReader::GetMapRender(const RequestHandler & RequestHandler, const json::Dict & Dict)
    {
        svg::Document Render = RequestHandler.RenderMap();
        std::ostringstream strm;
        Render.Render(strm);
        return json::Builder{}.StartDict()
            .Key("map").Value(strm.str())
            .Key("request_id").Value(Dict.at("id").AsInt())
            .EndDict().Build();
    }

}//json_processing
