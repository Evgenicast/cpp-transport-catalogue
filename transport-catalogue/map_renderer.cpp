#include "map_renderer.h"

bool renderer::IsZero(const double& value)
{
    return std::abs(value) < renderer::EPSILON;
}

svg::Point renderer::utilites::SphereProjector::operator()(const geo::Coordinates& coords) const
{
    return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
}

renderer::draw::StopPointRender::StopPointRender(const svg::Point & StopCoordinates_, const RenderSettings & RenderSettings_)
: StopCoordinates(StopCoordinates_), RenderSettingsData(RenderSettings_) {}

void renderer::draw::StopPointRender::Draw(svg::ObjectContainer & Objects) const
{
    svg::Circle Circle;
    Circle.SetCenter(StopCoordinates);
    Circle.SetRadius(RenderSettingsData.StopRadius);
    Circle.SetFillColor("white");
    Objects.Add(Circle);
}

renderer::draw::TextRender::TextRender(const svg::Point & Coord_, const std::string_view & Data_, const svg::Color& Fill_, const bool & Stop_, const RenderSettings & RenderSettingsData)
: Coord(Coord_), Data(Data_), Fill(Fill_), Stop(Stop_), RenderSettings_(RenderSettingsData){}

svg::Text renderer::draw::TextRender::CreateText() const
{
    svg::Text Text_;
    if (Stop == true)
    {
        Text_.SetOffset(RenderSettings_.StopLabelOffset);
        Text_.SetFontSize(RenderSettings_.StopLabelFontSize);
        Text_.SetFillColor("black");
    }
    else
    {
        Text_.SetOffset(RenderSettings_.BusLabelOffset);
        Text_.SetFontSize(RenderSettings_.BusLabelFontSize);
        Text_.SetFillColor(Fill);
        Text_.SetFontWeight("bold");
    }
    Text_.SetFontFamily("Verdana");
    Text_.SetPosition(Coord);
    Text_.SetData(Data);
    return Text_;
}

svg::Text renderer::draw::TextRender::CreateCanvas() const
{
    svg::Text Text_{ CreateText() };
    Text_.SetFillColor(RenderSettings_.UnderLayerColor).SetStrokeColor(RenderSettings_.UnderLayerColor);
    Text_.SetStrokeWidth(RenderSettings_.UnderlayerWidth);
    Text_.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    Text_.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    return Text_;
}

void renderer::draw::TextRender::Draw(svg::ObjectContainer & Object) const
{
    Object.Add(CreateCanvas());
    Object.Add(CreateText());
}

renderer::draw::RouteRender::RouteRender(const std::vector<svg::Point> & StopCoords_, const svg::Color & StrokeColor_, const RenderSettings & RenderSettingsData)
: StopCoords(StopCoords_), StrokeColor(StrokeColor_), RenderSettings_(RenderSettingsData) {}

void renderer::draw::RouteRender::Draw(svg::ObjectContainer & Object) const
{
    svg::Polyline Render;
    for (const auto & StopCoords_ : StopCoords)
    {
        Render.AddPoint(StopCoords_);
    }
    Render.SetFillColor(svg::NoneColor);
    Render.SetStrokeColor(StrokeColor);
    Render.SetStrokeWidth(RenderSettings_.LineWidth);
    Render.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    Render.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    Object.Add(Render);
}

void renderer::MapRenderer::SetRenderSettings(const RenderSettings & RenderSettingsData)
{
    RenderSettings_ = RenderSettingsData;
}

void renderer::MapRenderer::AddRoutRender(const std::vector<svg::Point> & StopsCoords, const svg::Color & StrokeColor)
{
    RoutesRenders.emplace_back(StopsCoords, StrokeColor, RenderSettings_);
}

void renderer::MapRenderer::AddStopPointRender(const svg::Point & StopCoords)
{
    StopsPointsRenders.emplace_back(StopCoords, RenderSettings_);
}

void renderer::MapRenderer::AddTextRender(const svg::Point & StopCoord, const std::string & Data, const svg::Color & TextColor, bool Stop)
{
    (Stop) ? StopsNamesRenders.emplace_back(StopCoord, Data, TextColor, Stop, RenderSettings_)
           : RouteNamesRenders.emplace_back(StopCoord, Data, TextColor, Stop, RenderSettings_);
}

svg::Document renderer::MapRenderer::GetRender() const
{
    svg::Document Render;
    for (const auto & RouteLine : RoutesRenders)
    {
        RouteLine.Draw(Render);
    }
    for (const auto & RouteNames : RouteNamesRenders)
    {
        RouteNames.Draw(Render);
    }
    for (const auto & StopPoint : StopsPointsRenders)
    {
        StopPoint.Draw(Render);
    }
    for (const auto& StopName : StopsNamesRenders)
    {
        StopName.Draw(Render);
    }
    return Render;
}

const std::vector<svg::Color> & renderer::MapRenderer::GetColorPallete() const
{
    return RenderSettings_.ColorPalette;
}


