#include "serialization.h"

namespace serialize
{
    // Serialize

    void Serializer::Serialize(const std::string & filename)
    {
        std::ofstream out(filename, std::ios::binary);
        SerializeStop(m_TransportCatalogueSer);
        SerializeDistance(m_TransportCatalogueSer);
        SerializeBus(m_TransportCatalogueSer);
        SerealizeRenderSettings(m_TransportCatalogueSer);
        SerealizeRoutingSettings(m_TransportCatalogueSer);
        m_TransportCatalogueSer.SerializeToOstream(&out);
    }

    void Serializer::SerializeStop(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer)
    {
        for (const auto & Stop : m_TransportCatalogue.GetAllStops())
        {
            transport_catalogue_serialize::Stop StopSer;
            transport_catalogue_serialize::Coordinates CoordsSer;
            CoordsSer.set_lat(Stop->m_Coords.lat);
            CoordsSer.set_lng(Stop->m_Coords.lng);
            StopSer.set_name(Stop->m_Name);
            *StopSer.mutable_coordinates() = CoordsSer;
            *TransportCatalogueSer.add_stops() = StopSer;
        }
    }

    void Serializer::SerializeBus(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer)
    {
        for (const auto & Bus : m_TransportCatalogue.GetBuses())
        {
            transport_catalogue_serialize::Bus BusSer;
            BusSer.set_bus_num(Bus->m_BusNumber);
            BusSer.set_is_circular(Bus->m_IsCircleRoute);
            for (auto Stop : Bus->m_BusStopsDeque)
            {
                transport_catalogue_serialize::Stop StopSer;
                transport_catalogue_serialize::Coordinates CoordSer;
                CoordSer.set_lat(Stop->m_Coords.lat);
                CoordSer.set_lng(Stop->m_Coords.lng);
                StopSer.set_name(Stop->m_Name);
                *StopSer.mutable_coordinates() = CoordSer;
                *BusSer.add_stops() = StopSer;
            }
            *TransportCatalogueSer.add_buses() = BusSer;
        }
    }

    void Serializer::SerializeDistance(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer)
    {
        for (const auto& distance : m_TransportCatalogue.GetAllDistances())
        {
            transport_catalogue_serialize::Distance DistnaceSer;
            DistnaceSer.set_from(distance.first.first->m_Name);
            DistnaceSer.set_to(distance.first.second->m_Name);
            DistnaceSer.set_distance(distance.second);
            *TransportCatalogueSer.add_distances() = DistnaceSer;
        }
    }
    void Serializer::SerealizeRenderSettings(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer)
    {
        auto RenderSettings = m_MapRenderer.GetRenderSettings();
        transport_catalogue_serialize::RenderSettings RenderSettingsSer;
        transport_catalogue_serialize::Point PointSer;
        RenderSettingsSer.set_width(RenderSettings.Width);
        RenderSettingsSer.set_height(RenderSettings.Height);
        RenderSettingsSer.set_padding(RenderSettings.Padding);
        RenderSettingsSer.set_line_width(RenderSettings.LineWidth);
        RenderSettingsSer.set_stop_radius(RenderSettings.StopRadius);
        RenderSettingsSer.set_bus_label_font_size(RenderSettings.BusLabelFontSize);
        RenderSettingsSer.set_stop_label_font_size(RenderSettings.StopLabelFontSize);
        RenderSettingsSer.set_underlayer_width(RenderSettings.UnderlayerWidth);

        PointSer.set_x(RenderSettings.BusLabelOffset.x);
        PointSer.set_y(RenderSettings.BusLabelOffset.y);
        *RenderSettingsSer.mutable_bus_label_offset() = PointSer;

        PointSer.set_x(RenderSettings.StopLabelOffset.x);
        PointSer.set_y(RenderSettings.StopLabelOffset.y);
        *RenderSettingsSer.mutable_stop_label_offset() = PointSer;

        *RenderSettingsSer.mutable_underlayer_color() = SerealizeColor(RenderSettings.UnderLayerColor);

        for (const auto & color : RenderSettings.ColorPalette)
        {
            *RenderSettingsSer.add_color_palette() = SerealizeColor(color);
        }

        *TransportCatalogueSer.mutable_render_settings() = RenderSettingsSer;
    }

    transport_catalogue_serialize::Color Serializer::SerealizeColor(const svg::Color & Color)
    {
        transport_catalogue_serialize::Color ColorSer;
        transport_catalogue_serialize::Rgba RGBAser;

        if (std::holds_alternative<svg::Rgb>(Color))
        {
        svg::Rgb rgb = std::get<svg::Rgb>(Color);
        RGBAser.set_blue(rgb.blue);
        RGBAser.set_green(rgb.green);
        RGBAser.set_red(rgb.red);
        *ColorSer.mutable_rgba() = RGBAser;
        }
        else if (std::holds_alternative<svg::Rgba>(Color))
        {
            svg::Rgba rgba = std::get<svg::Rgba>(Color);
            RGBAser.set_blue(rgba.blue);
            RGBAser.set_green(rgba.green);
            RGBAser.set_red(rgba.red);
            RGBAser.set_opacity(rgba.opacity);
            ColorSer.set_is_rgba(true);
            *ColorSer.mutable_rgba() = RGBAser;
        }
        else if(std::holds_alternative<std::string>(Color))
        {
            ColorSer.set_name(std::get<std::string>(Color));
        }
        return ColorSer;
    }

    void Serializer::SerealizeRoutingSettings(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer)
    {
        transport_catalogue_serialize::RoutingSettings RoutingSettingsSer;
        auto RoutingSettings = m_Router.GetRoutingSettings();
        RoutingSettingsSer.set_bus_velocity(RoutingSettings.second);
        RoutingSettingsSer.set_bus_wait_time(RoutingSettings.first);
        *TransportCatalogueSer.mutable_routing_settings() = RoutingSettingsSer;
    }
    //Deserializer

    void Deserializer::DeserializeCatalogAndRenderer(transport_catalogue::TransportCatalogue & TransportCatalogue, renderer::MapRenderer & MapRenderer, const std::string & filename)
    {
        std::ifstream in(filename, std::ios::binary);
        transport_catalogue_serialize::TransportCatalogue TransportCatalogueSer;
        TransportCatalogueSer.ParseFromIstream(&in);

        for (int i = 0; i < TransportCatalogueSer.stops().size(); ++i)
        {
            auto StopSer = TransportCatalogueSer.stops(i);
            TransportCatalogue.AddStop(StopSer.name(), StopSer.coordinates().lat(), StopSer.coordinates().lng());
        }
        for (int i = 0; i < TransportCatalogueSer.buses().size(); ++i)
        {
            auto BusSer = TransportCatalogueSer.buses(i);
            std::vector<std::string_view> stops;
            for (const auto & StopSer : BusSer.stops())
            {
                stops.push_back(StopSer.name());
            }
            TransportCatalogue.AddBus(BusSer.bus_num(), stops, BusSer.is_circular());
        }
        for (int i = 0; i < TransportCatalogueSer.distances_size(); ++i)
        {
            const auto DistanceDeser = DeserializeDistance(TransportCatalogueSer.distances(i), TransportCatalogue);
            TransportCatalogue.SetDistanceBetweenStops(DistanceDeser.first.first->m_Name, DistanceDeser.first.second->m_Name, DistanceDeser.second);
        }
        // map_renderer
        auto Settings = DeserealizeRenderSettings(TransportCatalogueSer.render_settings());
        MapRenderer.SetRenderSettings(Settings);
        const auto StopsCoords = TransportCatalogue.GetStopsCoordinates();
        renderer::utilites::SphereProjector Projector(StopsCoords.begin(), StopsCoords.end(), Settings.Width, Settings.Height, Settings.Padding);
        const std::vector<svg::Color> ColorPallete = MapRenderer.GetColorPallete();
        size_t ColorVal = 0;
        std::map<std::string, svg::Point> stops;
        for (const auto & it : TransportCatalogue.GetBuses())
        {
            std::vector<svg::Point> stops_points;
            for (const auto & Stop : TransportCatalogue.GetStops(it->m_BusNumber))
            {
                stops_points.emplace_back(Projector(Stop->m_Coords));
                stops[Stop->m_Name] = stops_points.back();
            }
            if (it->m_IsCircleRoute == true)
            {
                MapRenderer.AddTextRender(*stops_points.begin(), it->m_BusNumber, ColorPallete[ColorVal], false);
                MapRenderer.AddRoutRender(stops_points, ColorPallete[ColorVal]);
            }
            else if (it->m_IsCircleRoute == false)
            {
                MapRenderer.AddTextRender(*stops_points.begin(), it->m_BusNumber, ColorPallete[ColorVal], false);
                if (*it->m_BusStopsDeque.begin() != it->m_BusStopsDeque.back())MapRenderer.AddTextRender(stops_points.back(), it->m_BusNumber, ColorPallete[ColorVal], false);
                stops_points.insert(stops_points.end(), stops_points.rbegin() + 1, stops_points.rend());
                MapRenderer.AddRoutRender(stops_points, ColorPallete[ColorVal]);
            }
            (ColorVal == ColorPallete.size() - 1) ? ColorVal = 0 : ++ColorVal;
        }
        for (const auto & [Name, Coordinate] : stops)
        {
            MapRenderer.AddStopPointRender(Coordinate);
            MapRenderer.AddTextRender(Coordinate, Name, ColorPallete[ColorVal], true);
        }
    }

    void Deserializer::DeserealizeRouter(TransportRouter & Router, const std::string & filename)
    {
        std::ifstream in(filename, std::ios::binary);
        transport_catalogue_serialize::TransportCatalogue TrnasportCatalogueSer;
        TrnasportCatalogueSer.ParseFromIstream(&in);
        auto RoutingSettingsSer = TrnasportCatalogueSer.routing_settings();
        Router.SetRouteData(RoutingSettingsSer.bus_wait_time(), RoutingSettingsSer.bus_velocity());
    }

    hashed_distances Deserializer::DeserializeDistance(const transport_catalogue_serialize::Distance & DistanceSer, const transport_catalogue::TransportCatalogue & TransportCatalogue)
    {
        const tc_base::Stop* stop_from = TransportCatalogue.FindStop(DistanceSer.from());
        const tc_base::Stop* stop_to = TransportCatalogue.FindStop(DistanceSer.to());
        int64_t distance = DistanceSer.distance();
        return std::make_pair(std::make_pair(stop_from, stop_to), distance);
    }

    renderer::RenderSettings Deserializer::DeserealizeRenderSettings(const transport_catalogue_serialize::RenderSettings & RendererSettingsSer)
    {
        renderer::RenderSettings RenderSettings;

        RenderSettings.Width = RendererSettingsSer.width();
        RenderSettings.Height = RendererSettingsSer.height();
        RenderSettings.Padding = RendererSettingsSer.padding();
        RenderSettings.LineWidth = RendererSettingsSer.line_width();
        RenderSettings.StopRadius = RendererSettingsSer.stop_radius();
        RenderSettings.BusLabelFontSize = RendererSettingsSer.bus_label_font_size();
        RenderSettings.StopLabelFontSize = RendererSettingsSer.stop_label_font_size();
        RenderSettings.UnderlayerWidth = RendererSettingsSer.underlayer_width();

        RenderSettings.BusLabelOffset = { RendererSettingsSer.bus_label_offset().x(), RendererSettingsSer.bus_label_offset().y() };
        RenderSettings.StopLabelOffset = { RendererSettingsSer.stop_label_offset().x(), RendererSettingsSer.stop_label_offset().y() };

        RenderSettings.UnderLayerColor = DserealizeColor(RendererSettingsSer.underlayer_color());

        for (const auto & ColorSer : RendererSettingsSer.color_palette())
        {
            RenderSettings.ColorPalette.push_back(DserealizeColor(ColorSer));
        }

        return RenderSettings;
    }

    svg::Color Deserializer::DserealizeColor(const transport_catalogue_serialize::Color & ColorSer)
    {
        if (!ColorSer.name().empty())
        {
            return ColorSer.name();
        }
        else if (ColorSer.is_rgba())
        {
            return svg::Rgba(ColorSer.rgba().red(), ColorSer.rgba().green(), ColorSer.rgba().blue(), ColorSer.rgba().opacity());
        }
        return svg::Rgb(ColorSer.rgba().red(), ColorSer.rgba().green(), ColorSer.rgba().blue());
    }
} // namespace serialize
