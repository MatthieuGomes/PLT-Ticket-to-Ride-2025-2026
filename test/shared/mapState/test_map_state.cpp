
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

BOOST_AUTO_TEST_CASE(ConstructorInitializesGraph)
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


/* vim: set sw=2 sts=2 et : */
