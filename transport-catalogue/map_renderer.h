#pragma once
#include "svg.h"
#include "geo.h"
#include <algorithm>

namespace renderer
{
    inline const double EPSILON = 1e-6;
    bool IsZero(const double & value);

    namespace utilites
    {
        class SphereProjector // чужой класс
        {
        public:

            template <typename PointInputIt>
            SphereProjector(const PointInputIt& points_begin, const PointInputIt& points_end, const double& max_width, const double& max_height, const double& padding);

            svg::Point operator()(const geo::Coordinates& coords) const;

        private:

            double padding_;
            double min_lon_ = 0;
            double max_lat_ = 0;
            double zoom_coeff_ = 0;
        };

        template<typename PointInputIt> // чужой класс
        inline SphereProjector::SphereProjector(const PointInputIt& points_begin, const PointInputIt& points_end, const double& max_width, const double& max_height, const double& padding)
            : padding_(padding)
        {
            if (points_begin == points_end)
            {
                return;
            }

            const auto [left_it, right_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
                {
                    return lhs.lng < rhs.lng;
                });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs)
                {
                    return lhs.lat < rhs.lat;
                });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_))
            {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat))
            {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom)
            {
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }

            else if (width_zoom)
            {
                zoom_coeff_ = *width_zoom;
            }


            else if (height_zoom)
            {
                zoom_coeff_ = *height_zoom;
            }
        }
    }//namespace utilites

    struct RenderSettings;

    namespace draw
    {
        class StopPointRender : public svg::Drawable
        {
        private:
            svg::Point StopCoordinates;
            const RenderSettings & RenderSettingsData;
        public:
            StopPointRender(const svg::Point & StopCoordinates_, const RenderSettings & RenderSettings_);
            void Draw(svg::ObjectContainer & Objects) const override;
        };


        class TextRender : public svg::Drawable
        {
        private:
            svg::Point Coord;
            std::string Data;
            svg::Color Fill;
            bool Stop;

            const RenderSettings & RenderSettings_;
        public:
            TextRender(const svg::Point & Coord_, const std::string_view & Data_, const svg::Color & Fill_, const bool & Stop_, const RenderSettings & RenderSettingsData);
            void Draw(svg::ObjectContainer & Object) const override;
            svg::Text CreateText() const;
            svg::Text CreateCanvas() const;
        };

        class RouteRender : public svg::Drawable
        {
        private:
            std::vector<svg::Point> StopCoords;
            svg::Color StrokeColor;
            const RenderSettings & RenderSettings_;
        public:
            RouteRender(const std::vector<svg::Point> & StopCoords_, const svg::Color & StrokeColor_, const RenderSettings & RenderSettingsData);
            void Draw(svg::ObjectContainer & Object) const override;
        };
    }//namespace draw

    struct RenderSettings
    {
        double Width = 0.0;
        double Height = 0.0;
        double Padding = 0.0;
        double LineWidth = 0.0;
        double StopRadius = 0.0;
        int BusLabelFontSize = 0;
        svg::Point BusLabelOffset;
        int StopLabelFontSize = 0;
        svg::Point StopLabelOffset{ 0.0, 0.0 };
        svg::Color UnderLayerColor;
        double UnderlayerWidth = 0.0;
        std::vector<svg::Color> ColorPalette;
    };

    class MapRenderer
    {
    private:
        std::vector<draw::RouteRender> RoutesRenders;
        std::vector<draw::TextRender> RouteNamesRenders;
        std::vector<draw::StopPointRender> StopsPointsRenders;
        std::vector<draw::TextRender> StopsNamesRenders;
        RenderSettings RenderSettings_;
    public:
        MapRenderer() = default;
        MapRenderer(const MapRenderer &) = delete;
        MapRenderer operator=(MapRenderer &) = delete;
        MapRenderer(MapRenderer &&) noexcept = delete;
        MapRenderer operator=(MapRenderer &&) noexcept = delete;

        void SetRenderSettings(const RenderSettings & RenderSettingsData);
        void AddRoutRender(const std::vector<svg::Point> & StopsCoords, const svg::Color & StrokeColor);
        void AddStopPointRender(const svg::Point & StopCoords);
        void AddTextRender(const svg::Point & StopCoord, const std::string & Data , const svg::Color& TextColor, bool Stop);
        svg::Document GetRender() const;
        const std::vector<svg::Color>& GetColorPallete() const;
    };

} // namespace renderer


