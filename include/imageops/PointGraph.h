#ifndef POINT_GRAPH_H_
#define POINT_GRAPH_H_

#include <map>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "../generic/Graph.h"
#include "../generic/UnionFind.h"
#include "../util/Neighborhood.h"
#include "PointVertex.h"

using Vertex = std::shared_ptr<PointVertex>;

class PointGraph
{
public:
  PointGraph();

  void paintImage(cv::Mat& canvas);

  void addVerticesFromImage(cv::Mat& fromImage);

  void connectAllNeighbors(int width, int height, Neighborhood nbr);

  void connectRandom(int maxEdges);

  void unionChunk(int x, int y, int depth, Neighborhood n);

  void unionChunks(Neighborhood nbr, float vP);

  std::map<Vertex, Vertex> djikstraPaths(int x, int y);

  std::map<Vertex, int> djikstraKey(int x, int y);

  Graph<Vertex> bfs(int x, int y);

private:
  Graph<Vertex> _pointGraph;

  std::map<int, std::map<int, Vertex>> _pointMap;

  UnionFind<Vertex> _unionFind;

  cv::Point2i _bottomRight;
};

#endif // PointGraph.h included
