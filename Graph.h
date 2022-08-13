#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstdint>
#include <map>
#include <random>
#include <vector>
#include <queue>
#include <climits>

#include "Heap.h"

template <class VertexType>
class Graph {
    public:
        Graph();

        Graph(Graph<VertexType> &copy_from);

        void clear();

        void addVertex(const VertexType &v);

        void removeVertex(const VertexType &v);

        void addEdge(const VertexType &from, const VertexType &to, int weight);

        void addNewEdge(const VertexType &from, const VertexType &to, int weight);

        void removeEdge(const VertexType &from, const VertexType &to);

        bool hasEdge(const VertexType &from, const VertexType &to);

        bool hasEdges(const VertexType &from);

        int weight(const VertexType &from, const VertexType &to);

        std::map<VertexType, std::map<VertexType, int>> & getAdjacencyLists();

        std::map<VertexType, int> & connectedTo(const VertexType &from);

        Graph<VertexType> bfs(VertexType &sourceVertex);

        void contractRandomEdgeNoParallel();

        void contractEdgeNoParallel(const std::pair<VertexType, VertexType> edge);

        std::pair<VertexType, VertexType> chooseRandomEdge();

        std::pair<VertexType, VertexType> chooseRandomEdge(VertexType &startVertex);
    private:
        std::map<VertexType, std::map<VertexType, int>> _adjacencyLists;
};

template <class VertexType>
Graph<VertexType>::Graph() { }

template <class VertexType>
Graph<VertexType>::Graph(Graph<VertexType> &copy_from) {
    _adjacencyLists.clear();

    for (auto &elem : copy_from.getAdjacencyLists()) {
        addVertex(elem.first);
        for (auto &destVertex : elem.second) {
            addNewEdge(elem.first, destVertex.first, destVertex.second);
        }
    }
}

template <class VertexType>
void Graph<VertexType>::clear() {
    _adjacencyLists.clear(); 
}

template <class VertexType>
void Graph<VertexType>::addVertex(const VertexType &v) {
    _adjacencyLists.insert({v, std::map<VertexType, int>()});
}

template <class VertexType>
void Graph<VertexType>::removeVertex(const VertexType &v) {
    for (auto &sourceVertex : _adjacencyLists) {
        auto toRemove = sourceVertex.second.find(v);
        if (toRemove != sourceVertex.second.end()) {
            sourceVertex.second.erase(toRemove);
        }
    }

    _adjacencyLists.erase(v);
}

template <class VertexType>
void Graph<VertexType>::addEdge(const VertexType &from, const VertexType &to, int weight) {
    auto findTo = _adjacencyLists[from].find(to);
    if (findTo != _adjacencyLists[from].end()) {
        _adjacencyLists[from][to] += weight;
    }
    else {
        _adjacencyLists[from].insert({to, weight});
    }
}

template <class VertexType>
void Graph<VertexType>::addNewEdge(const VertexType &from, const VertexType &to, int weight) {
    _adjacencyLists[from].insert({to, weight});
}

template <class VertexType>
void Graph<VertexType>::removeEdge(const VertexType &from, const VertexType &to) {
    _adjacencyLists[from].erase(to);
}

template <class VertexType>
bool Graph<VertexType>::hasEdge(const VertexType &from, const VertexType &to) {
    return _adjacencyLists[from].find(to) != _adjacencyLists[from].end();
}

template <class VertexType>
bool Graph<VertexType>::hasEdges(const VertexType &from) {
    return _adjacencyLists[from].size() > 0;
}

template <class VertexType>
int Graph<VertexType>::weight(const VertexType &from, const VertexType &to) {
    return _adjacencyLists[from][to];
}

template <class VertexType>
std::map<VertexType, std::map<VertexType, int>> & Graph<VertexType>::getAdjacencyLists() { 
    return _adjacencyLists;
}

template <class VertexType>
std::map<VertexType, int> & Graph<VertexType>::connectedTo(const VertexType &from) {
    return _adjacencyLists[from];
}

template <class VertexType>
Graph<VertexType> Graph<VertexType>::bfs(VertexType &sourceVertex) {
    std::queue<VertexType> toSearch;
    std::map<VertexType, bool> visited;
    Graph<VertexType> connected;

    for (auto &vertex : _adjacencyLists) {
        visited.insert(std::make_pair(vertex.first, false));
    }

    visited[sourceVertex] = true;

    toSearch.push(sourceVertex);

    while (toSearch.size() > 0) {
        auto vertex = toSearch.front();
        toSearch.pop();

        connected.addVertex(vertex);

        for (auto &destVertex : connectedTo(vertex)) {
            connected.addNewEdge(vertex, destVertex.first, destVertex.second);

            if (!visited[destVertex.first]) {
                visited[destVertex.first] = true;
                
                toSearch.push(destVertex.first);
            }
        }
    }

    return connected;
}

template <class VertexType>
void Graph<VertexType>::contractRandomEdgeNoParallel() {
    auto edge = chooseRandomEdge();

    for (auto &destVertex : _adjacencyLists[edge.second]) {
        if (_adjacencyLists[edge.first].find(destVertex.first) == _adjacencyLists[edge.first].end()) {
            addNewEdge(edge.first, destVertex.first, weight(edge.first, edge.second) + weight(edge.second, destVertex.first));
        }

        removeEdge(edge.second, destVertex.first);
    }

    for (auto &sourceVertex : _adjacencyLists) {
        auto it = _adjacencyLists[sourceVertex.first].find(edge.second);
        if (it != _adjacencyLists[sourceVertex.first].end()) {
            if (_adjacencyLists[sourceVertex.first].find(edge.first) != _adjacencyLists[sourceVertex.first].end()) {
                addNewEdge(sourceVertex.first, edge.first, weight(sourceVertex.first, edge.second));
            }

            removeEdge(sourceVertex.first, edge.second);
        }
    }

    removeVertex(edge.second);
}

template <class VertexType>
void Graph<VertexType>::contractEdgeNoParallel(const std::pair<VertexType, VertexType> edge) {
    for (auto &destVertex : _adjacencyLists[edge.second]) {
        if (_adjacencyLists[edge.first].find(destVertex.first) == _adjacencyLists[edge.first].end()) {
            addNewEdge(edge.first, destVertex.first, weight(edge.first, edge.second) + weight(edge.second, destVertex.first));
        }

        removeEdge(edge.second, destVertex.first);
    }

    for (auto &sourceVertex : _adjacencyLists) {
        auto it = _adjacencyLists[sourceVertex.first].find(edge.second);
        if (it != _adjacencyLists[sourceVertex.first].end()) {
            if (_adjacencyLists[sourceVertex.first].find(edge.first) != _adjacencyLists[sourceVertex.first].end()) {
                addNewEdge(sourceVertex.first, edge.first, weight(sourceVertex.first, edge.second));
            }

            removeEdge(sourceVertex.first, edge.second);
        }
    }

    removeVertex(edge.second);
}

template <class VertexType>
std::pair<VertexType, VertexType> Graph<VertexType>::chooseRandomEdge() {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto vIt = getAdjacencyLists().begin(); 
    std::forward(vIt, gen() % getAdjacencyLists().size());
    auto startVertex = vIt->first;

    auto eIt = vIt->second.begin();
    while (eIt != vIt->second.end()) {
        vIt = getAdjacencyLists().begin();
        std::forward(vIt, gen() % getAdjacencyLists().size());
        startVertex = vIt->first;
        eIt = vIt->second.begin();
    }

    std::forward(eIt, gen() % vIt->second.size());
    auto endVertex = eIt->first;

    return std::make_pair(startVertex, endVertex);
}

template <class VertexType>
std::pair<VertexType, VertexType> Graph<VertexType>::chooseRandomEdge(VertexType &startVertex) {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto eIt = _adjacencyLists[startVertex].begin();

    std::forward(eIt, gen() % _adjacencyLists[startVertex].size());
    auto endVertex = eIt->first;

    return std::make_pair(startVertex, endVertex);
}

#endif // Graph.h included
