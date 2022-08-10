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
        PointGraph(cv::Mat &fromImage);

        /*
         */
        void paintImage(cv::Mat &canvas);

    private:
        struct VComp {
            bool operator()(const PointVertex &l, const PointVertex &r) const {
                return ((l.point().x + 1) * l.point().y + l.point().x) < ((r.point().x + 1) * r.point().y + r.point().x);
            }
        };

        struct PComp {
            bool operator()(const std::pair<int, int> l, const std::pair<int, int> r) const {
                return ((l.first + 1) * l.second + l.first) < ((r.first + 1) * r.second + r.first);
            }
        };

    private:
        Graph<PointVertex, VComp> pointGraph;

        cv::Point2i _bottomRight;
};

#endif // PointGraph.h included
