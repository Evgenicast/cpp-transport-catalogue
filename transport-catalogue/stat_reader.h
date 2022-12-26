#ifndef STAT_READER_H
#define STAT_READER_H

#include "transport_catalogue.h"

#include <string_view>
#include <iomanip>
#include <unordered_map>

namespace transport_catalogue
{
    namespace output
    {
        struct StatReader
        {
            std::string_view ExtractName( const std::string_view& Data );
            void PrintRouteInfo(TransportCatalogue &TC, std::string Query, std::ostream& out );
            void PrintStopInfo(TransportCatalogue & TC, std::string Query, std::ostream & out);
            void RequestAndOutput(TransportCatalogue & TC, std::ostream& out );
        };
    }//output
}//transport_catalogue
#endif // STAT_READER_H
