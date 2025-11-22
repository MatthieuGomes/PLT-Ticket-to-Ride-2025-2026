
#include <boost/test/unit_test.hpp>

#include "../../src/shared/mapState/MapState.h"

using namespace ::mapState;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestExemple)
{
  {
    // ExampleA exA;
    // exA.setX(53);
    // Example ex;
    // ex.setA(exA);
    // BOOST_CHECK_EQUAL(ex.getA().x, 53);
    // exA.setX(21);
    // BOOST_CHECK_EQUAL(ex.getA().x, 53);
    // ex.setA(exA);
    // BOOST_CHECK_EQUAL(ex.getA().x, 21);
  }

  {
    // ExampleA exA;
    // exA.setX(21);
    // Example ex;
    // ex.setA(exA);
    // BOOST_CHECK_LE(ex.getA().x, 32); // Less than equal
    // BOOST_CHECK_GT(ex.getA().x, 11); // Greater than equal
  }
}
// MapState(), listStations() and listRoads() unit tests
BOOST_AUTO_TEST_CASE(TestConstructor)
{
    MapState map;

    auto stations = map.listStations();
    BOOST_REQUIRE_EQUAL(stations.size(), 4);

    BOOST_CHECK_EQUAL(stations[0]->data->name, "Paris");
    BOOST_CHECK_EQUAL(stations[1]->data->name, "Berlin");
    BOOST_CHECK_EQUAL(stations[2]->data->name, "Madrid");
    BOOST_CHECK_EQUAL(stations[3]->data->name, "Rome");

    auto roads = map.listRoads();
    BOOST_REQUIRE_EQUAL(roads.size(), 4);
}

//getStationByName()
BOOST_AUTO_TEST_CASE(TestGetStationByName)
{
    MapState map;

    Station* s = map.getStationByName("Berlin");
    BOOST_REQUIRE(s != nullptr);
    BOOST_CHECK_EQUAL(s->data->ID, 2);

    // check for nonexistant station
    BOOST_CHECK(map.getStationByName("Tokyo") == nullptr);
}

//getRoad()
BOOST_AUTO_TEST_CASE(TestGetRoad)
{
    MapState map;

    Station* berlin = map.getStationByName("Berlin");
    Station* rome   = map.getStationByName("Rome");

    BOOST_REQUIRE(berlin != nullptr);
    BOOST_REQUIRE(rome   != nullptr);

    Road* r = map.getRoad(berlin, rome);
    BOOST_REQUIRE(r != nullptr);
    BOOST_CHECK_EQUAL(r->data->ID, "B-R");

    // nonexistant
    Station* madrid = map.getStationByName("Madrid");
    Road* none = map.getRoad(berlin, madrid);

    BOOST_CHECK(none == nullptr);   
}

//findShortestPath()
BOOST_AUTO_TEST_CASE(TestShortestPath)
{
    MapState map;

    Station* paris = map.getStationByName("Paris");
    Station* rome  = map.getStationByName("Rome");

    Path p = map.findShortestPath(paris, rome);

    BOOST_REQUIRE(p.STATIONS.size() > 0);
    BOOST_CHECK_EQUAL(p.STATIONS.front()->data->name, "Paris");
    BOOST_CHECK_EQUAL(p.STATIONS.back()->data->name, "Rome");
}
// test with impossible stations, should always fail
/*
BOOST_AUTO_TEST_CASE(TestShortestPath_ImpossiblePath)
{
    MapState map;
    
    Station dummyA{{}, new StationData{"DummyA", 99}};
    Station dummyB{{}, new StationData{"DummyB", 100}};

    Path p = map.findShortestPath(&dummyA, &dummyB);

    BOOST_CHECK_EQUAL(p.STATIONS.size(), 0);
} */


//printToString()
BOOST_AUTO_TEST_CASE(TestPrintToString)
{
    MapState map;

    std::string repr = map.printToString();

    BOOST_CHECK(!repr.empty());
    BOOST_CHECK(repr.find("Paris") != std::string::npos);
    BOOST_CHECK(repr.find("Roads") != std::string::npos);
}




/* vim: set sw=2 sts=2 et : */
