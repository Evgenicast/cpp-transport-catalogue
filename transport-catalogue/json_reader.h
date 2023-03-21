#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"

namespace json_proccessing
{
    class JsonReader
    {
    private:
        json::Dict Object;
        transport_catalogue::TransportCatalogue TC;
        renderer::MapRenderer MapRenderer;

        //-----------------храним для освобождения памяти----------//
        std::optional<const domain::BusOutputData*> BusDataPtr;
        json::Array OutputData;
    public:
        JsonReader() = delete;
        ~JsonReader()
        {
            OutputData.erase(OutputData.begin(), OutputData.end());
        }
        JsonReader(std::istream & Input);

        JsonReader(JsonReader & ob) = delete;
        JsonReader & operator=(JsonReader & ob) = delete;
        JsonReader(JsonReader && ob) noexcept = delete;
        JsonReader & operator=(JsonReader && ob) noexcept = delete;

       /*INPUT/OUTPUT*/

        void ReadBaseRequest(transport_catalogue::TransportCatalogue & TC);
        void ReadRenderSettings(transport_catalogue::TransportCatalogue & TC, renderer::MapRenderer & MapRenderer);
        json::Array ReadStatRequest(const RequestHandler & RequestHandler);
        void LoadOutput(RequestHandler & RequestHandler, const json::Array& Vector);

        /*TRANSPORT_CATALOGUE*/

        void ReadStopData(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict);
        void ReadStopDistance(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict);
        void ReadBusData(transport_catalogue::TransportCatalogue & TC, const json::Dict & Dict);
        const json::Node GetBusInfo(const RequestHandler & RequestHandler, const json::Dict & Dict);
        const json::Dict GetStopInfo(const RequestHandler & RequestHandler, const json::Dict & Dict);

        /*RENDER*/

        const svg::Color GetColor(const json::Node& Color);
        void SetMapRenderer(const transport_catalogue::TransportCatalogue & TC, renderer::MapRenderer & MapRenderer, const json::Dict & Dict);
        const json::Dict GetMapRender(const RequestHandler & RequestHandler, const json::Dict & Dict);
    };
}//json_processing
