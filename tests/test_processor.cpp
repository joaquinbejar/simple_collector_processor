//
// Created by Joaquin Bejar Garcia on 23/12/23.
//


#include "collector/config.h"
#include <catch2/catch_test_macros.hpp>
#include <collector/processor.h>

using namespace collector::config;
using Params = simple_polygon_io::tickers::TickersParams ;
using JsonResponse = simple_polygon_io::tickers::JsonResponse;

TEST_CASE("CollectorConfig Tests", "[CollectorConfig]") {
    collector::config::CollectorConfig config;
    simple_polygon_io::client::PolygonIOClient polygon_client(config);

    auto params = simple_polygon_io::tickers::TickersParams(); // Define params closer to usage
    params.set_market(simple_polygon_io::tickers::Market::STOCKS); // Set params


    std::function<JsonResponse(Params)> get_tickers = [&polygon_client](const Params& param) {
        return polygon_client.get_tickers(param);
    };
    SECTION("Default Configuration") {
        processor::CollectorProcessor<Params, JsonResponse> processor(config);
        processor.start(params, get_tickers);
    }
}