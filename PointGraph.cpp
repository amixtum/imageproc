#include <queue>
#include <cmath>
#include <random>
#include <iostream>

#include <opencv2/imgproc.hpp>

#include "PointGraph.h"
#include "ImageOps.h"
#include "Heap.h"

PointGraph::PointGraph()  { }

void PointGraph::paintImage(cv::Mat &canvas) {
    if (_pointGraph.getAdjacencyLists().size() == 0) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());


    std::queue<Vertex> toSearch;
    std::map<Vertex, bool> visited;
        
    auto it = _pointGraph.getAdjacencyLists().begin();
    std::advance(it, gen() % _pointGraph.getAdjacencyLists().size());
    auto srcVertex = it->first;
    auto key = djikstraKey(srcVertex->x(), srcVertex->y());
    int maxDistance = 0;
    
    for (auto &vertex : _pointGraph.getAdjacencyLists()) {
        visited.insert({vertex.first, false});

        if (key[vertex.first] > maxDistance) {
            maxDistance = key[vertex.first];
        }
    }

    visited[srcVertex] = true;
    toSearch.push(srcVertex);

    cv::resize(
            canvas, 
            canvas, 
            cv::Size(_bottomRight.x, _bottomRight.y), 
            cv::INTER_LINEAR
    );

    while (!toSearch.empty()) {
        auto searchVertex = toSearch.front();
        auto colorVertex = _unionFind.find(searchVertex);
        auto distance = key[colorVertex];
        auto color = 
            colorVertex->color() + colorVertex->color() * 
            (std::sin((M_PI * 2 * distance) / maxDistance + 
                     ((float)distance / maxDistance) 
                     * std::cos((M_PI * 2 * distance) / maxDistance)) + 1);
        toSearch.pop();

        cv::circle(
                canvas, 
                searchVertex->point(), 
                0, 
                color 
        );

        for (auto &destVertex : _pointGraph.connectedTo(searchVertex)) {
            if (!visited[destVertex.first]) {
                visited[destVertex.first] = true;
                toSearch.push(destVertex.first);
            }
        }
    }
}

void PointGraph::addVerticesFromImage(cv::Mat &fromImage) {
    _pointGraph.clear();
    _pointMap.clear();
    _unionFind.clear();

    _bottomRight = cv::Point2i(fromImage.size().width,
                               fromImage.size().height);

    for (int x = 0; x < fromImage.size().width; x += 1) {
        _pointMap.insert({x, std::map<int, Vertex>()});
        for (int y = 0; y < fromImage.size().height; y += 1) {
            auto pV = 
                std::make_shared<PointVertex>(cv::Point2i(x, y), 
                                  fromImage.at<cv::Vec3b>(y, x));

            _pointGraph.addVertex(pV);

            _pointMap[x].insert({y, pV});
            
            _unionFind.insert(pV);
        }
    }
}

void PointGraph::connectAllNeighbors(int width, 
                                     int height, 
                                     Neighborhood nbr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int weight = 0;
    for (int x = 0; x < width; x += 1) {
        for (int y = 0; y < height; y += 1) {
            auto nbrs = neighbors(nbr, 
                                  cv::Point(x, y), 
                                  0, 0, 
                                  width - 1, height - 1);

            for (auto &n : nbrs) {
                weight = gen() % 10;

                _pointGraph.addNewEdge(_pointMap[x][y], 
                                      _pointMap[n.x][n.y], 
                                      weight);
            }
        }
    }
}

void PointGraph::connectRandom(int maxEdges) {
    std::random_device rd;
    std::mt19937 gen(rd());

    int edges = 0;
    int weight = 0;
    
    for (auto &startVertex : _pointGraph.getAdjacencyLists()) {
        edges = (gen() % maxEdges) + 1;

        auto nbrs = neighbors(Neighborhood::Moore,
                              startVertex.first->point(), 
                              0, 0, 
                              _bottomRight.x - 1, _bottomRight.y - 1);

        int nIndex = 0;
        while (edges > 0) { 
            if (gen() % 1000 < 100) {
                _pointGraph.addNewEdge(
                        startVertex.first, 
                        _pointMap[nbrs[nIndex].x][nbrs[nIndex].y], 
                        weight
                );
                edges -= 1;
            }
            nIndex = 
            (static_cast<unsigned long>(nIndex) == nbrs.size() - 1) ? 
                0 : nIndex + 1;
        }
    }
    
    for (auto &startVertex : _pointGraph.getAdjacencyLists()) {
        for (auto &endVertex : 
             _pointGraph.connectedTo(startVertex.first)) {
            if (gp::compareIntensity(startVertex.first->color(), 
                                     endVertex.first->color()) < 0) {
                _unionFind.unionVertices(startVertex.first, 
                                         endVertex.first);
            }
        }
    }
}

void PointGraph::unionChunk(int x, int y, int depth, Neighborhood n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto center = cv::Point(x, y);

    for (auto i = 0; i < depth; i += 1) {
        auto nbrs = neighbors(n, 
                              center, 
                              0, 0, 
                              _bottomRight.x - 1, _bottomRight.y - 1);

        for (auto &nbr : nbrs) {
            _unionFind.unionVertices(_pointMap[center.x][center.y], 
                                     _pointMap[nbr.x][nbr.y]);
        }

        center = nbrs[gen() % nbrs.size()];
    }
}

void PointGraph::unionChunks(Neighborhood nbr, float vP) {
    std::random_device rd;
    std::mt19937 gen(rd());

    int x = 0;        
    int y = 0;

    while (x < _bottomRight.x) {
        while (y < _bottomRight.y) {
            if (gen() % 1000 < (1000.f * vP)) {
                unionChunk(x, y, 1, nbr);
            }

            y += 1;
        }

        x += 1;
        y = 0;
    }
}

std::map<Vertex, Vertex> PointGraph::djikstraPaths(int x, int y) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::map<Vertex, Vertex> paths;

    std::map<Vertex, bool> processed;
    std::map<Vertex, int> key;
    std::map<Vertex, int> distances;

    Heap<int, std::pair<Vertex, Vertex>> unprocessed;

    for (auto &vertex : _pointGraph.getAdjacencyLists()) {
        processed.insert({vertex.first, false});
        key.insert({vertex.first, INT_MAX});
        distances.insert({vertex.first, INT_MAX});
        paths.insert({vertex.first, nullptr});
    }

    auto sourceVertex = _pointMap[x][y];

    processed[sourceVertex] = true;
    key[sourceVertex] = 0;
    distances[sourceVertex] = 0;

    for (auto &aroundSource : _pointGraph.connectedTo(sourceVertex)) {
        unprocessed.insert(
                _pointGraph.weight(sourceVertex, aroundSource.first), 
                std::make_pair(sourceVertex, aroundSource.first)
        );
        processed[aroundSource.first] = true;
        distances[aroundSource.first] = _pointGraph.weight(
                sourceVertex, 
                aroundSource.first
        );
        key[aroundSource.first] = _pointGraph.weight(
                sourceVertex, 
                aroundSource.first
        );
        paths[aroundSource.first] = sourceVertex;
    }

    while (!unprocessed.empty()) {
        auto toProcess = unprocessed.extractMin();
        
        for (auto &destVertex : _pointGraph.connectedTo(toProcess.second)) {
            if (!processed[destVertex.first]) {
                processed[destVertex.first] = true;

                distances[destVertex.first] = 
                    _pointGraph.weight(toProcess.second, destVertex.first) +
                    key[toProcess.second];

                if (distances[destVertex.first] < key[destVertex.first]) {
                    key[destVertex.first] = distances[destVertex.first];
                    paths[destVertex.first] = toProcess.second;
                }

                unprocessed.insert(
                        distances[destVertex.first], 
                        std::make_pair(toProcess.second, destVertex.first)
                );
            }
        }
    }

    return paths;
}

std::map<Vertex, int> PointGraph::djikstraKey(int x, int y) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::map<Vertex, bool> processed;
    std::map<Vertex, int> key;
    std::map<Vertex, int> distances;

    Heap<int, std::pair<Vertex, Vertex>> unprocessed;

    for (auto &vertex : _pointGraph.getAdjacencyLists()) {
        processed.insert({vertex.first, false});
        key.insert({vertex.first, INT_MAX});
        distances.insert({vertex.first, INT_MAX});
    }

    auto sourceVertex = _pointMap[x][y];

    processed[sourceVertex] = true;
    key[sourceVertex] = 0;
    distances[sourceVertex] = 0;

    for (auto &aroundSource : _pointGraph.connectedTo(sourceVertex)) {
        unprocessed.insert(
                _pointGraph.weight(sourceVertex, aroundSource.first), 
                std::make_pair(sourceVertex, aroundSource.first));
        processed[aroundSource.first] = true;
        distances[aroundSource.first] = _pointGraph.weight(
                sourceVertex, 
                aroundSource.first
        );
        key[aroundSource.first] = _pointGraph.weight(
                sourceVertex, 
                aroundSource.first
        );
    }

    while (!unprocessed.empty()) {
        auto toProcess = unprocessed.extractMin();

        for (auto &destVertex : _pointGraph.connectedTo(toProcess.second)) {
            if (!processed[destVertex.first]) {
                processed[destVertex.first] = true;

                distances[destVertex.first] = 
                    _pointGraph.weight(toProcess.second, destVertex.first) +                    key[toProcess.second];

                if (distances[destVertex.first] < key[destVertex.first]) {
                    key[destVertex.first] = distances[destVertex.first];
                }

                unprocessed.insert(
                        distances[destVertex.first], 
                        std::make_pair(toProcess.second, destVertex.first)
                );
            }
        }
    }
    
    return key;
}

Graph<Vertex> PointGraph::bfs(int x, int y) {
    return _pointGraph.bfs(_pointMap[x][y]);
}


