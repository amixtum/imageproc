#include <queue>
#include <cmath>
#include <random>
#include <iostream>

#include <opencv2/imgproc.hpp>

#include "PointGraph.h"
#include "Neighborhood.h"

PointGraph::PointGraph(cv::Mat &fromImage) {
    this->_bottomRight = cv::Point2i(fromImage.size().width, fromImage.size().height);

    for (int x = 0; x < fromImage.size().width; x += 1) {
        for (int y = 0; y < fromImage.size().height; y += 1) {
            auto vertex = PointVertex(cv::Point2i(x, y), fromImage.at<cv::Vec3b>(y, x));

            this->pointGraph.addVertex(vertex);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    int weight = 0;
    for (auto &startVertex : this->pointGraph.getAdjacencyLists()) {
        auto it = this->pointGraph.getAdjacencyLists().begin();
        while (it != this->pointGraph.getAdjacencyLists().end()) {
            std::advance(it, (gen() % ((this->pointGraph.getAdjacencyLists().size()) / 2)));
            if (it->first != startVertex.first) {
                weight = gen() % 7;
                this->pointGraph.addEdge(startVertex.first, it->first, weight);
            }
        }
    }
}

void PointGraph::paintImage(cv::Mat &canvas) {
    std::queue<PointVertex> toSearch;
    std::map<PointVertex, bool, VComp> visited;
    
    for (auto &vertex : this->pointGraph.getAdjacencyLists()) {
        visited.insert({vertex.first, false});
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    
    auto it = visited.begin();
    std::advance(it, gen() % visited.size());
    auto srcVertex = it->first;
    visited[srcVertex] = true;
    toSearch.push(srcVertex);

    cv::resize(canvas, canvas, cv::Size(_bottomRight.x, _bottomRight.y), cv::INTER_LINEAR);

    while (!toSearch.empty()) {
        auto searchVertex = toSearch.front();
        toSearch.pop();

        cv::circle(canvas, searchVertex.point(), 1, searchVertex.color());

        for (auto &destVertex : this->pointGraph.connectedTo(searchVertex)) {
            if (!visited[destVertex.first]) {
                visited[destVertex.first] = true;
                toSearch.push(destVertex.first);
            }
        }
    }
}