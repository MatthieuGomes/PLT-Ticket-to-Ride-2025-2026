#include "MapState.h"
#include "Path.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp> 
#include <boost/graph/undirected_graph.hpp>

namespace mapState {

  MapState::MapState(){
      this->gameGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, mapState::Station*, mapState::Road*>();

      StationData* parisData  = new StationData{"Paris",  1};
      StationData* berlinData = new StationData{"Berlin", 2};
      StationData* madridData = new StationData{"Madrid", 3};
      StationData* romeData   = new StationData{"Rome",   4};

      Station* paris  = new Station{boost::add_vertex(gameGraph), parisData};
      Station* berlin = new Station{boost::add_vertex(gameGraph), berlinData};
      Station* madrid = new Station{boost::add_vertex(gameGraph), madridData};
      Station* rome   = new Station{boost::add_vertex(gameGraph), romeData};

      Stations = {paris, berlin, madrid, rome};

      // roads
      RoadData* p_b = new RoadData{"P-B", "red",    4, false, {parisData,  berlinData}};
      RoadData* b_r = new RoadData{"B-R", "blue",   3, false, {berlinData, romeData}};
      RoadData* p_m = new RoadData{"P-M", "yellow", 5, false, {parisData,  madridData}};
      RoadData* m_r = new RoadData{"M-R", "green",  6, false, {madridData, romeData}};

      Road* roadPB = new Road{boost::add_edge(paris->vertex,  berlin->vertex, gameGraph).first, p_b};
      Road* roadBR = new Road{boost::add_edge(berlin->vertex, rome->vertex, gameGraph).first, b_r};
      Road* roadPM = new Road{boost::add_edge(paris->vertex,  madrid->vertex, gameGraph).first, p_m};
      Road* roadMR = new Road{boost::add_edge(madrid->vertex, rome->vertex, gameGraph).first, m_r};

      Roads = {roadPB, roadBR, roadPM, roadMR};

      // fill graph bundles
      for (Station* s : Stations) gameGraph[s->vertex] = s;
      for (Road* r : Roads)    gameGraph[r->edge]   = r;

  };
/*
  MapState* MapState::generateMap() {
      // hardcoded stations 
      StationData* parisData  = new StationData{"Paris",  1};
      StationData* berlinData = new StationData{"Berlin", 2};
      StationData* madridData = new StationData{"Madrid", 3};
      StationData* romeData   = new StationData{"Rome",   4};

      Station* paris  = new Station{add_vertex(gameGraph), parisData};
      Station* berlin = new Station{add_vertex(gameGraph), berlinData};
      Station* madrid = new Station{add_vertex(gameGraph), madridData};
      Station* rome   = new Station{add_vertex(gameGraph), romeData};

      Stations = {paris, berlin, madrid, rome};

      // roads
      RoadData* p_b = new RoadData{"P-B", "red",    4, false, {parisData,  berlinData}};
      RoadData* b_r = new RoadData{"B-R", "blue",   3, false, {berlinData, romeData}};
      RoadData* p_m = new RoadData{"P-M", "yellow", 5, false, {parisData,  madridData}};
      RoadData* m_r = new RoadData{"M-R", "green",  6, false, {madridData, romeData}};

      Road* roadPB = new Road{add_edge(paris->vertex,  berlin->vertex, gameGraph).first, p_b};
      Road* roadBR = new Road{add_edge(berlin->vertex, rome->vertex,   gameGraph).first, b_r};
      Road* roadPM = new Road{add_edge(paris->vertex,  madrid->vertex, gameGraph).first, p_m};
      Road* roadMR = new Road{add_edge(madrid->vertex, rome->vertex,   gameGraph).first, m_r};

      Roads = {roadPB, roadBR, roadPM, roadMR};

      // fill graph bundles
      for (auto* s : Stations) gameGraph[s->vertex] = s;
      for (auto* r : Roads)    gameGraph[r->edge]   = r;

      return this;
  }*/

};