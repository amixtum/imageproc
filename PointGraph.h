#ifndef POINT_GRAPH_H_
#define POINT_GRAPH_H_

#include <map>
#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "Graph.h"
#include "PointVertex.h"
#include "Neighborhood.h"

class PointGraph {
    public:
        /*
         * Adds a PointVertex for each entry in fromImage and
         * adds an edge for each neighboring PointVertex
         */
        PointGraph();

        /*
         */
        void paintImage(cv::Mat &canvas);

        void addVerticesFromImage(cv::Mat &fromImage);

        void connectAllNeighbors(int width, int height, Neighborhood nbr);

        void connectRandom(int maxEdges);

    private:
        Graph<std::shared_ptr<PointVertex>> pointGraph;

        std::map<int, std::map<int, std::shared_ptr<PointVertex>>> pointMap;

        cv::Point2i _bottomRight;
};

#endif // PointGraph.h included
