#include "input_reader.h"
#include <algorithm>

namespace transport_catalogue
{
    namespace input
    {
        std::string ReadLine(std::istream & in)
        {
            std::string Line;
            std::getline(in, Line);
            return Line;
        }

        int ReadLineWithNumber(std::istream & in)
        {
            int Result;
            in >> Result;
            ReadLine(in);
            return Result;
        }


        void LoadData(TransportCatalogue & TC, std::istream & in)
        {
            int n = ReadLineWithNumber(in);
            std::vector<std::string> DistanceData;
            std::vector<std::string> BusData;
            std::vector<std::string> StopNames;
            std::string Query;
            DistanceData.reserve(n);
            BusData.reserve(n);

            for (int i = 0; i < n; i++)
            {
                Query = ReadLine(in);
                if (Query[0] == 'S')
                {
                    DistanceData.push_back(Query);

                    auto [Name, Lat, Lng] = SplitNameAndExpressionForStop(Query);
                    TC.AddStop(Name, Lat, Lng);
                    StopNames.push_back(Name);
                }
                else
                {
                    BusData.push_back(move(Query));
                }
            }
            if (DistanceData.size() != 0)
            {
                for (const auto & Stop : DistanceData)
                {
                    for (const auto & [StopFrom, StopTo, Distance] : SplitNameAndExpressionForStopDistance(Stop))
                    {
                        TC.SetDistanceBetweenStops(StopFrom, StopTo, Distance);
                    }
                }
            }
            for (auto & BusName : BusData)
            {
                auto [Name, IsCircleRoute, Stops] = SplitNameAndExpressionForBus(BusName);
                TC.AddBus(Name, IsCircleRoute, Stops);
                TC.CalcAndAddLengthToBusData(Name);
            }
            for (auto & StopName : StopNames)
            {
                TC.FindAndAddBusesForStop(StopName);
            }
        }

        std::deque<StopToStopDistance> SplitNameAndExpressionForStopDistance(std::string_view Line)
        {
            std::deque<StopToStopDistance> Distances;

            //----------SplitName----------------//

            LeftSpaceTrim(Line);
            Line.remove_prefix(Line.find_first_of(' '));
            size_t Colon = Line.find_first_of(':');
            std::string StopFrom_(Line.substr(0, Colon));
            std::string StopFrom = RemoveLeadingAndTrailingSpaces(StopFrom_);

            //----------SplitExpression---------//

            std::string StopTo;
            int LastComma = Line.find(',', Colon + 2);
            size_t StopToEndChar = Line.find(',', LastComma + 1);

            if (StopToEndChar == Line.npos)
            {
                return{};
            }
            std::string_view Expression = Line.substr(StopToEndChar + 2, Line.length());

            int Distance;

            while (true)
            {
                size_t CutPos = Expression.find('m', 0);
                Distance = abs(std::stoi(std::string(Expression.substr(0, CutPos))));
                CutPos = Expression.find(' ');
                Expression = (Expression.substr(CutPos + 1, Expression.length()));
                CutPos = Expression.find(' ');
                Expression = (Expression.substr(CutPos + 1, Expression.length()));
                CutPos = Expression.find(',');
                StopTo = std::string(Expression.substr(0, CutPos));
                Distances.emplace_back(StopToStopDistance{StopFrom, StopTo, Distance});
                if (CutPos == Expression.npos)
                {
                    break;
                }
                Expression = (Expression.substr(CutPos + 2, Expression.length()));
            }
             return Distances;
        }

        std::string_view LeftSpaceTrim(std::string_view Line)
        {
            while (!Line.empty() && isspace(Line[0]))
            {
                Line.remove_prefix(1);
            }
            return Line;
        }


        std::string RemoveLeadingAndTrailingSpaces(std::string_view Line)
        {
            const auto StrBegin = Line.find_first_not_of(' ');
            const size_t PosEnd = Line.npos;
            if (StrBegin == PosEnd)
                return "";

            const auto StrEnd = Line.find_last_not_of(' ');
            const auto StrRange = StrEnd - StrBegin + 1;
            std::string Result (Line.substr(StrBegin, StrRange));
            return Result;

        }

        Stop SplitNameAndExpressionForStop(std::string_view Line)
        {

        //    ----------SplitName----------------//

            NameAndExpression Buffer;
            LeftSpaceTrim(Line);
            Line.remove_prefix(Line.find_first_of(' '));
            const size_t Colon = Line.find_first_of(':');
            const std::string_view First_(Line.substr(0, Colon));
            std::string First = RemoveLeadingAndTrailingSpaces(First_);

            //----------SplitExpression---------//

            Line.remove_prefix(Colon + 1); // delete the ":";
            const std::string Second = RemoveLeadingAndTrailingSpaces(Line);
            Buffer = {First, Second};
            auto Name = First;
            auto Coords = GetCoordFromExpression(Buffer);
            return Stop {Name, Coords.first, Coords.second};
        }

        Bus SplitNameAndExpressionForBus(std::string_view Line)
        {
            //----------SplitName----------------//

            LeftSpaceTrim(Line);
            Line.remove_prefix(Line.find_first_of(' '));
            const size_t Colon = Line.find_first_of(':');
            const std::string First_ = std::string(Line.substr(0, Colon));
            std::string First = RemoveLeadingAndTrailingSpaces(First_);

            //----------SplitExpression---------//

            Line.remove_prefix(Colon + 1); // delete the ":";
            bool IsCircleRoute;
            std::string Second = RemoveLeadingAndTrailingSpaces(Line);

           auto BusRoute = (GetBusRoutesFromExpression(Second,IsCircleRoute));
           return Bus {First, IsCircleRoute, BusRoute};
        }

        std::string RemoveSpace(std::string_view Line)
        {
            Line.remove_prefix(std::min(Line.size(), Line.find_first_not_of(' ')));
            const size_t PosEnd = Line.npos;
            std::string Result;

            while(!Line.empty())
            {
                size_t Space = Line.find(' ');
                if(Space == PosEnd)
                {
                    Result += Line.substr(0, Line.size());
                }
                else
                {
                    Result += Line.substr(0, Space);
                }
                Line.remove_prefix(std::min(Line.size(),  Line.find_first_not_of(' ', Space)));
            }
            return Result;
        }

        StopCoordinates GetCoordFromExpression(NameAndExpression & Section)
        {
            auto [Name, Coord] = Section;
            std::string FirstCoord = {};
            std::string SecondCoord = {};
            std::string NoSpaceCoord;
            NoSpaceCoord = RemoveSpace(Coord);
            const size_t PosEnd = NoSpaceCoord.npos;
            FirstCoord = NoSpaceCoord.substr(0, Coord.find_first_of(','));
            NoSpaceCoord.erase(0, (NoSpaceCoord.find_first_of(',') + 1));
            SecondCoord = NoSpaceCoord.substr(NoSpaceCoord.find_first_not_of(','), PosEnd);

            return StopCoordinates{ static_cast<double>(std::stod(FirstCoord)), static_cast<double>(std::stod(SecondCoord)) };
        }

        std::deque<std::string> GetBusRoutesFromExpression(std::string_view Section, bool & IsCircleRoute)
        {
            std::deque<std::string> Words;
            std::string Buffer;
            auto WordBeginIter = Section.begin();
            auto WordEndIter = Section.end();

            for (auto Letter = Section.begin(); Letter < Section.end(); ++Letter)
            {
                if (*Letter == '>' || *Letter == '-')
                {
                    if (*Letter == '>')
                    {
                        IsCircleRoute = true;
                    }
                    else
                    {
                        IsCircleRoute = false;
                    }
                    WordEndIter = Letter; // указывает на ' '; word_begin_iter все еще на начале.
                    if (WordBeginIter != WordEndIter)
                    {
                        Buffer = Section.substr(std::distance(Section.begin(), WordBeginIter), std::distance(WordBeginIter, WordEndIter));
                        Buffer = RemoveLeadingAndTrailingSpaces(Buffer);
                        Words.emplace_back(Buffer);
                    }
                    WordBeginIter = std::next(WordEndIter);
                }
            }
            if (WordBeginIter != WordEndIter)
            {
                Buffer = Section.substr(WordBeginIter - Section.begin(), std::distance(WordBeginIter, WordEndIter));
                Buffer = RemoveLeadingAndTrailingSpaces(Buffer);
                Words.emplace_back(Buffer);
            }

            return Words;
        }
    }//input
}//transport_catalogue

