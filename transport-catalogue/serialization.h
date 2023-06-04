#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_catalogue.pb.h"
#include "transport_router.h"

#include <fstream>
#include <vector>

namespace serialize
{

using hashed_distances = std::pair<std::pair<const tc_base::Stop*, const tc_base::Stop*>, int>;

    class Serializer
    {
    private:

        const transport_catalogue::TransportCatalogue & m_TransportCatalogue;
        const renderer::MapRenderer & m_MapRenderer;
        const TransportRouter & m_Router;

        transport_catalogue_serialize::TransportCatalogue m_TransportCatalogueSer;

        void SerializeStop(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer);
        void SerializeBus(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer);
        void SerializeDistance(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer);
        void SerealizeRenderSettings(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer);
        transport_catalogue_serialize::Color SerealizeColor(const svg::Color & Color);
        void SerealizeRoutingSettings(transport_catalogue_serialize::TransportCatalogue & TransportCatalogueSer);

        public:

        Serializer(const transport_catalogue::TransportCatalogue & TransportCatalogue,
                   const renderer::MapRenderer & MapRenderer, const TransportRouter & Router)
                   : m_TransportCatalogue(TransportCatalogue), m_MapRenderer(MapRenderer), m_Router(Router) {}

        void Serialize(const std::string & filename);
    };

    class Deserializer
    {
    private:

        hashed_distances DeserializeDistance(const transport_catalogue_serialize::Distance & DistanceSer, const transport_catalogue::TransportCatalogue & TransportCatalogue);
        renderer::RenderSettings DeserealizeRenderSettings(const transport_catalogue_serialize::RenderSettings & RendererSettingsSer);
        svg::Color DserealizeColor(const transport_catalogue_serialize::Color & ColorSer);

    public:

        void DeserializeCatalogAndRenderer(transport_catalogue::TransportCatalogue & TransportCatalogue, renderer::MapRenderer & MapRenderer, const std::string & filename);
        void DeserealizeRouter(TransportRouter& Router, const std::string & filename);
    };

} // namespace serialize
