#pragma once
#include "json.h"
#include "transport_router.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

namespace json_proccessing
{
    class JsonReader
    {
    private:

        /*INPUT DATA*/
        json::Dict m_JsonObject;
        renderer::MapRenderer m_MapRenderer;       
        transport_catalogue::TransportCatalogue m_TransportCatalogue;

        /*OUTPUT DATA*/
        std::optional<const tc_request::Bus*> m_BusDataPtr;
        std::optional<const tc_request::Stop*> m_StopDataPtr;
        json::Array m_OutputData;

    public:

        JsonReader() = delete;
        JsonReader(std::istream & Input);

        JsonReader(JsonReader &) = delete;
        JsonReader & operator=(JsonReader &) = delete;
        JsonReader(JsonReader &&) noexcept = delete;
        JsonReader & operator=(JsonReader &&) noexcept = delete;

        /*INPUT*/
        void ReadBaseRequest(transport_catalogue::TransportCatalogue & m_TransportCatalogue);
        void ReadRouteSettings(TransportRouter & TransportRouter);
        void ReadRenderSettings(transport_catalogue::TransportCatalogue & m_TransportCatalogue, renderer::MapRenderer & MapRenderer);

        /*OUTPUT*/
        json::Array ReadStatRequest(const RequestHandler & RequestHandler, TransportRouter & TransportRouter);
        void LoadOutput(const RequestHandler & RequestHandler, const json::Array & Vector);

        /*BUILD_TRANSPORT_CATALOGE*/
        json::Node GetRoute(TransportRouter & TransportRouter, const json::Dict & Dict);
        void ReadStopData(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict);
        void ReadStopDistance(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict);
        void ReadBusData(transport_catalogue::TransportCatalogue & m_TransportCatalogue, const json::Dict & Dict);
        const json::Node GetBusInfo(const RequestHandler & RequestHandler, const json::Dict & Dict);
        const json::Node GetStopInfo(const RequestHandler & RequestHandler, const json::Dict & Dict);

        /*RENDER*/
        const svg::Color GetColor(const json::Node & Color);
        const json::Node GetMapRender(const RequestHandler & RequestHandler, const json::Dict & Dict);

        ~JsonReader()
        {
            m_OutputData.erase(m_OutputData.begin(), m_OutputData.end());
            if (m_BusDataPtr.has_value())
            {
                delete m_BusDataPtr.value();
            }
            if (m_StopDataPtr.has_value())
            {
                delete m_StopDataPtr.value();
            }
        }
    };

}//json_processing
