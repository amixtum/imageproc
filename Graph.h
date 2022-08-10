#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstdint>
#include <map>
#include <vector>
#include <queue>
#include <climits>

#include "Heap.h"


template <class VertexType, class KeyCompare>
class Graph {
    public:
        Graph();

        Graph(Graph<VertexType, KeyCompare> &copy_from);

        void addVertex(VertexType v);

        void removeVertex(VertexType &v);

        void addEdge(const VertexType &from, const VertexType &to, int weight);

        void removeEdge(VertexType &from, VertexType &to);

        bool hasEdge(VertexType &from, VertexType &to);

        int weight(VertexType &from, VertexType &to);

        std::map<VertexType, std::map<VertexType, int, KeyCompare>, KeyCompare> & getAdjacencyLists();

        std::map<VertexType, int, KeyCompare> & connectedTo(VertexType &from);

        Graph<VertexType, KeyCompare> bfs(VertexType &sourceVertex) const;

        std::map<VertexType, std::vector<VertexType>, KeyCompare> djikstraPaths(VertexType &sourceVertex) const;
    private:
        std::map<VertexType, std::map<VertexType, int, KeyCompare>, KeyCompare> adjacencyLists;
};

template <class VertexType, class KeyCompare>
Graph<VertexType, KeyCompare>::Graph() { }

template <class VertexType, class KeyCompare>
Graph<VertexType, KeyCompare>::Graph(Graph<VertexType, KeyCompare> &copy_from) : adjacencyLists(copy_from.key_comp()) {
    for (auto &elem : copy_from.getAdjacencyLists()) {
        this->addVertex(elem.first);
        for (auto &destVertex : elem.second) {
            this->addEdge(elem.first, destVertex);
        }
    }
}

template <class VertexType, class KeyCompare>
void Graph<VertexType, KeyCompare>::addVertex(VertexType v) {
    this->adjacencyLists.insert({v, std::map<VertexType, int, KeyCompare>()});
}

template <class VertexType, class KeyCompare>
void Graph<VertexType, KeyCompare>::removeVertex(VertexType &v) {
    for (auto &sourceVertex : this->adjacencyLists) {
        auto toRemove = sourceVertex.second.find(v);
        if (toRemove != sourceVertex.second.end()) {
            sourceVertex.second.erase(toRemove);
        }
    }

    this->adjacencyLists.erase(v);
}

template <class VertexType, class KeyCompare>
void Graph<VertexType, KeyCompare>::addEdge(const VertexType &from, const VertexType &to, int weight) {
    auto findTo = this->adjacencyLists[from].find(to);
    if (findTo != this->adjacencyLists[from].end()) {
        this->adjacencyLists[from][to] += weight;
    }
    else {
        this->adjacencyLists[from].insert({to, weight});
    }
}

template <class VertexType, class KeyCompare>
void Graph<VertexType, KeyCompare>::removeEdge(VertexType &from, VertexType &to) {
    this->adjacencyLists[from].erase(to);
}

template <class VertexType, class KeyCompare>
bool Graph<VertexType, KeyCompare>::hasEdge(VertexType &from, VertexType &to) {
    return this->adjacencyLists[from].find(to) != this->adjacencyLists[from].end();
}

template <class VertexType, class KeyCompare>
std::map<VertexType, std::map<VertexType, int, KeyCompare>, KeyCompare> & Graph<VertexType, KeyCompare>::getAdjacencyLists() { 
    return this->adjacencyLists;
}

template <class VertexType, class KeyCompare>
std::map<VertexType, int, KeyCompare> & Graph<VertexType, KeyCompare>::connectedTo(VertexType &from) {
    return this->adjacencyLists[from];
}

template <class VertexType, class KeyCompare>
Graph<VertexType, KeyCompare> Graph<VertexType, KeyCompare>::bfs(VertexType &sourceVertex) const {
    std::queue<VertexType> toSearch;
    std::map<VertexType, bool, KeyCompare> visited;
    Graph<VertexType, KeyCompare> connected;

    for (auto &vertex : this->adjacencyLists) {
        visited.insert({vertex.first, false});
    }

    visited[sourceVertex] = true;

    toSearch.push(sourceVertex);

    while (toSearch.size() > 0) {
        auto vertex = toSearch.front();
        toSearch.pop();

        connected.addVertex(vertex);

        for (auto &destVertex : this->connectedTo(vertex)) {
            connected.addEdge(vertex, destVertex.first, this->weight(vertex, destVertex.first));

            if (!visited[destVertex]) {
                visited[destVertex] = true;
                
                toSearch.push(destVertex);
            }
        }
    }

    return connected;
}

template <class VertexType, class KeyCompare>
std::map<VertexType, std::vector<VertexType>, KeyCompare> Graph<VertexType, KeyCompare>::djikstraPaths(VertexType &sourceVertex) const {
    std::map<VertexType, std::vector<VertexType>, KeyCompare> paths;

    std::map<VertexType, bool, KeyCompare> processed;
    std::map<VertexType, int, KeyCompare> key;
    std::map<VertexType, int, KeyCompare> distances;

    Heap<int, VertexType> unprocessed;
    
    for (auto &vertex : this->adjacencyLists) {
        processed.insert({vertex.first, false});
        distances.insert({vertex.first, INT_MAX});
        key.insert({vertex.first, INT_MAX});
        paths.insert({vertex, std::vector<VertexType>()});
    }

    key[sourceVertex] = 0;
    distances[sourceVertex] = 0;
    processed[sourceVertex] = true;
    paths[sourceVertex].push_back(sourceVertex);

    for (auto &destVertex : this->adjacencyLists[sourceVertex]) {
        unprocessed.insert(this->weight(sourceVertex, destVertex.first), {sourceVertex, destVertex.first});
    }
    
    while (!unprocessed.empty()) {
        auto minEdge = unprocessed.extractMin();
        processed[minEdge.second] = true;
        distances[minEdge.second] = key[minEdge.first] + this->weight(minEdge.first, minEdge.second);
        if (distances[minEdge.second] < key[minEdge.second]) {
            key[minEdge.second] = distances[minEdge.second];
            for (auto &e : paths[minEdge.first]) {
                paths[minEdge.second].push_back(e);
            }
            paths[minEdge.second].push_back(minEdge.second);
        }

        for (auto &destVertex : this->connectedTo(minEdge.second)) {
            if (!processed[destVertex.first]) {
                distances[destVertex.first] = key[minEdge.second] + this->weight(minEdge.second, destVertex.first);
                if (distances[destVertex.first] < key[destVertex.first]) {
                    key[destVertex.first] = distances[destVertex.first];
                }
                unprocessed.insert(distances[destVertex.first], {minEdge.second, destVertex.first});
            }
        }
    }

    return paths;
}

#endif // Graph.h included
