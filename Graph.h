#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstdint>
#include <map>
#include <vector>
#include <queue>
#include <climits>

#include "Heap.h"

template <class VertexType>
class Graph {
    public:
        template <class KeyCompare>
        Graph(KeyCompare compare);

        Graph(Graph<VertexType> &copy_from);

        void addVertex(VertexType &v);

        void removeVertex(VertexType &v);

        void addEdge(VertexType &from, VertexType &to, int weight);

        void removeEdge(VertexType &from, VertexType &to);

        bool hasEdge(VertexType &from, VertexType &to);

        int weight(VertexType &from, VertexType &to);

        std::map<VertexType, std::map<VertexType, int>> & getAdjacencyLists();

        std::map<VertexType, int> & connectedTo(VertexType &from);

        Graph<VertexType> bfs(VertexType &sourceVertex);

        std::map<VertexType, std::vector<std::pair<VertexType, VertexType>>> djikstra(VertexType &sourceVertex);
    private:
        std::map<VertexType, std::map<VertexType, int>> adjacencyLists;
};

template <class VertexType>
template <class KeyCompare>
Graph<VertexType>::Graph(KeyCompare compare) : adjacencyLists(compare) { }

template <class VertexType>
Graph<VertexType>::Graph(Graph<VertexType> &copy_from) : adjacencyLists(copy_from.key_comp()) {
    for (auto &elem : copy_from.getAdjacencyLists()) {
        this->addVertex(elem.first);
        for (auto &destVertex : elem.second) {
            this->addEdge(elem.first, destVertex);
        }
    }
}

template <class VertexType>
void Graph<VertexType>::addVertex(VertexType &v) {
    this->adjacencyLists.insert({v, std::map<VertexType, int>(this->adjacencyLists.key_comp())});
}

template <class VertexType>
void Graph<VertexType>::removeVertex(VertexType &v) {
    for (auto &sourceVertex : this->adjacencyLists) {
        auto toRemove = sourceVertex.second.find(v);
        if (toRemove != sourceVertex.second.end()) {
            sourceVertex.second.erase(toRemove);
        }
    }

    this->adjacencyLists.erase(v);
}

template <class VertexType>
void Graph<VertexType>::addEdge(VertexType &from, VertexType &to, int weight) {
    auto findTo = this->adjacencyLists[from].find(to);
    if (findTo != this->adjacencyLists[from].end()) {
        this->adjacencyLists[from][to] += weight;
    }
    else {
        this->adjacencyLists[from].insert({to, weight});
    }
}

template <class VertexType>
void Graph<VertexType>::removeEdge(VertexType &from, VertexType &to) {
    this->adjacencyLists[from].erase(to);
}

template <class VertexType>
bool Graph<VertexType>::hasEdge(VertexType &from, VertexType &to) {
    return this->adjacencyLists[from].find(to) != this->adjacencyLists[from].end();
}

template <class VertexType>
std::map<VertexType, std::map<VertexType, int>> & Graph<VertexType>::getAdjacencyLists() {
    return &this->adjacencyLists;
}

template <class VertexType>
std::map<VertexType, int> & Graph<VertexType>::connectedTo(VertexType &from) {
    return &this->adjacencyLists[from];
}

template <class VertexType>
Graph<VertexType> Graph<VertexType>::bfs(VertexType &sourceVertex) {
    std::queue<VertexType> toSearch;
    std::map<VertexType, bool> visited;
    Graph<VertexType> connected;

    for (auto &vertex : this->adjacencyLists) {
        visited.insert({vertex.first, false});
    }

    visited[sourceVertex] = true;

    toSearch.push(sourceVertex);

    while (toSearch.size() > 0) {
        auto vertex = toSearch.pop();

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

template <class VertexType>
std::map<VertexType, std::vector<std::pair<VertexType, VertexType>>> Graph<VertexType>::djikstra(VertexType &sourceVertex) {
    std::map<VertexType, std::vector<std::pair<VertexType, VertexType>>> paths;

    std::map<VertexType, bool> processed(this->adjacencyLists.key_comp());
    std::map<VertexType, int> key(this->adjacencyLists.key_comp());
    std::map<std::pair<VertexType, VertexType>, int> distances(this->adjacencyLists.key_comp());

    Heap<int, VertexType> unprocessed;
    
    for (auto &vertex : this->adjacencyLists) {
        processed.insert({vertex.first, false});
        distances.insert({vertex.first, INT_MAX});
        key.insert({vertex.first, INT_MAX});
        paths.insert({vertex, std::vector<std::pair<VertexType, VertexType>>()});
    }

    key[sourceVertex] = 0;
    distances[sourceVertex] = 0;
    processed[sourceVertex] = true;

    for (auto &destVertex : this->adjacencyLists[sourceVertex]) {
        unprocessed.insert(distances[sourceVertex] + this->weight(sourceVertex, destVertex.first), {sourceVertex, destVertex.first});
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
            paths[minEdge.second].push_back({minEdge.first, minEdge.second});
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
