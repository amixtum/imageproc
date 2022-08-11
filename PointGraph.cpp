#include <queue>
#include <cmath>
#include <random>
#include <iostream>

#include <opencv2/imgproc.hpp>

#include "PointGraph.h"
#include "Neighborhood.h"

PointGraph::PointGraph()  { }

void PointGraph::paintImage(cv::Mat &canvas) {
    if (this->pointGraph.getAdjacencyLists().size() == 0) {
        return;
    }

    std::cout << "In PointGraph::paintImage()" << std::endl;
    std::queue<std::shared_ptr<PointVertex>> toSearch;
    std::map<std::shared_ptr<PointVertex>, bool> visited;
    
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

        cv::circle(canvas, searchVertex->point(), 1, searchVertex->color());

        for (auto &destVertex : this->pointGraph.connectedTo(searchVertex)) {
            if (!visited[destVertex.first]) {
                visited[destVertex.first] = true;
                toSearch.push(destVertex.first);
            }
        }
    }
}

void PointGraph::addVerticesFromImage(cv::Mat &fromImage) {
    this->_bottomRight = cv::Point2i(fromImage.size().width, fromImage.size().height);

    for (int x = 0; x < fromImage.size().width; x += 1) {
        this->pointMap.insert({x, std::map<int, std::shared_ptr<PointVertex>>()});
        for (int y = 0; y < fromImage.size().height; y += 1) {
            auto pV = std::make_shared<PointVertex>(cv::Point2i(x, y), fromImage.at<cv::Vec3b>(y, x));
            this->pointGraph.addVertex(pV);
            this->pointMap[x].insert({y, pV});
        }
    }

    std::cout << "Finished PointGraph::addVerticesFromImage()\n";
}

void PointGraph::connectAllNeighbors(int width, int height, Neighborhood nbr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int weight = 0;
    for (int x = 0; x < width; x += 1) {
        for (int y = 0; y < height; y += 1) {
            auto nbrs = neighbors(nbr, cv::Point(x, y), 0, 0, width - 1, height - 1);
            for (auto &n : nbrs) {
                weight = gen() % 10;
                this->pointGraph.addNewEdge(this->pointMap[x][y], this->pointMap[n.x][n.y], weight);
            }
        }
    }

    std::cout << "Finished PointGraph::connectAllNeighbors()\n";
}

void PointGraph::connectRandom(int maxEdges) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int weight = 0;
    int edges = 0;
    for (auto &startVertex : this->pointGraph.getAdjacencyLists()) {
        edges = 0;
        auto it = this->pointGraph.getAdjacencyLists().begin();
        std::advance(it, (gen() % (this->pointGraph.getAdjacencyLists().size())));

        while (it != this->pointGraph.getAdjacencyLists().end() && edges < maxEdges) {
            std::advance(it, (gen() % ((this->pointGraph.getAdjacencyLists().size()) / 2)));

            if (it->first != startVertex.first and it != this->pointGraph.getAdjacencyLists().end()) {
                weight = gen() % 7;
                this->pointGraph.addNewEdge(startVertex.first, it->first, weight);
                edges += 1;
            }
        }
    }

    std::cout << "Finished PointGraph::connectRandom\n";
}
