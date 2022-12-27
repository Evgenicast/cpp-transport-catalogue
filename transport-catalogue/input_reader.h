#ifndef INPUT_READER_H
#define INPUT_READER_H
#include "transport_catalogue.h"
#include <string>
#include <vector>
#include <iostream>
#include <string_view>
#include <deque>

namespace transport_catalogue
{
    using NameAndExpression = std::pair<std::string_view, std::string_view>;
    namespace input
    {

        using StopCoordinates = std::pair<double, double>;

        struct Stop
        {
            std::string Name;
            double LAT;
            double LNG;
        };

        struct Bus
        {
            std::string name;
            bool IsCircleRoute;
            std::deque<std::string> Stops;
        };

        struct StopToStopDistance
        {
            std::string StopFrom;
            std::string StopTo;
            int Distance;
        };

        void LoadData(TransportCatalogue & TC, std::istream & in);

        std::string ReadLine(std::istream & in);
        int ReadLineWithNumber();

        std::deque<StopToStopDistance> SplitNameAndExpressionForStopDistance(std::string_view Line);
        Stop SplitNameAndExpressionForStop(std::string_view Line);
        Bus SplitNameAndExpressionForBus(std::string_view Line);

        std::string_view LeftSpaceTrim(std::string_view Line);
        std::string RemoveLeadingAndTrailingSpaces(std::string_view Line);
        std::string RemoveSpace(std::string_view Line);

        StopCoordinates GetCoordFromExpression(NameAndExpression & Section);
        std::deque<std::string> GetBusRoutesFromExpression(std::string_view Section, bool & IsCircleRoute);

    }//input
}//transport_catalogue
#endif // INPUT_READER_H
