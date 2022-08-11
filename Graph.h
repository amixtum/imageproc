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

        void addVertex(VertexType v);

        void removeVertex(VertexType &v);

        void addEdge(const VertexType &from, const VertexType &to, int weight);

        void addNewEdge(const VertexType &from, const VertexType &to, int weight);

        void removeEdge(VertexType &from, VertexType &to);

        bool hasEdge(VertexType &from, VertexType &to);

        int weight(VertexType &from, VertexType &to);

        std::map<VertexType, std::map<VertexType, int>> & getAdjacencyLists();

        std::map<VertexType, int> & connectedTo(VertexType &from);

        Graph<VertexType> bfs(VertexType &sourceVertex) const;

        std::map<VertexType, std::vector<VertexType>> djikstraPaths(VertexType &sourceVertex) const;

        void contractRandomEdgeNoParallel();

    private:
        std::pair<VertexType, VertexType> chooseRandomEdge();
    private:
        std::map<VertexType, std::map<VertexType, int>> adjacencyLists;
};

template <class VertexType>
Graph<VertexType>::Graph() { }

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
void Graph<VertexType>::addVertex(VertexType v) {
    this->adjacencyLists.insert({v, std::map<VertexType, int>()});
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
void Graph<VertexType>::addEdge(const VertexType &from, const VertexType &to, int weight) {
    auto findTo = this->adjacencyLists[from].find(to);
    if (findTo != this->adjacencyLists[from].end()) {
        this->adjacencyLists[from][to] += weight;
    }
    else {
        this->adjacencyLists[from].insert({to, weight});
    }
}

template <class VertexType>
void Graph<VertexType>::addNewEdge(const VertexType &from, const VertexType &to, int weight) {
    this->adjacencyLists[from].insert({to, weight});
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
    return this->adjacencyLists;
}

template <class VertexType>
std::map<VertexType, int> & Graph<VertexType>::connectedTo(VertexType &from) {
    return this->adjacencyLists[from];
}

template <class VertexType>
Graph<VertexType> Graph<VertexType>::bfs(VertexType &sourceVertex) const {
    std::queue<VertexType> toSearch;
    std::map<VertexType, bool> visited;
    Graph<VertexType> connected;

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

template <class VertexType>
std::map<VertexType, std::vector<VertexType>> Graph<VertexType>::djikstraPaths(VertexType &sourceVertex) const {
    std::map<VertexType, std::vector<VertexType>> paths;

    std::map<VertexType, bool> processed;
    std::map<VertexType, int> key;
    std::map<VertexType, int> distances;

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

template <class VertexType>
void Graph<VertexType>::contractRandomEdgeNoParallel() {
    auto edge = this->chooseRandomEdge();

    for (auto &destVertex : this->adjacencyLists[edge.second]) {
        if (this->adjacencyLists[edge.first].find(destVertex.first) == this->adjacencyLists[edge.first].end()) {
            this->addNewEdge(edge.first, destVertex.first, this->weight(edge.first, edge.second) + this->weight(edge.second, destVertex.first));
        }

        this->removeEdge(edge.second, destVertex.first);
    }

    for (auto &sourceVertex : this->adjacencyLists) {
        auto it = this->adjacencyLists[sourceVertex.first].find(edge.second);
        if (it != this->adjacencyLists[sourceVertex.first].end()) {
            if (this->adjacencyLists[sourceVertex.first].find(edge.first) != this->adjacencyLists[sourceVertex.first].end()) {
                this->addNewEdge(sourceVertex.first, edge.first, this->weight(sourceVertex.first, edge.second));
            }

            this->removeEdge(sourceVertex.first, edge.second);
        }
    }

    this->removeVertex(edge.second);
}

template <class VertexType>
std::pair<VertexType, VertexType> Graph<VertexType>::chooseRandomEdge() {
    std::random_device rd;
    std::mt19937 gen(rd());

    auto vIt = this->getAdjacencyLists().begin(); 
    std::forward(vIt, gen() % this->getAdjacencyLists().size());
    auto startVertex = vIt->first;

    auto eIt = vIt->second.begin();
    while (eIt != vIt->second.end()) {
        vIt = this->getAdjacencyLists().begin();
        std::forward(vIt, gen() % this->getAdjacencyLists().size());
        startVertex = vIt->first;
        eIt = vIt->second.begin();
    }

    std::forward(eIt, gen() % vIt->second.size());
    auto endVertex = eIt->first;

    return std::pair<VertexType, VertexType>({startVertex, endVertex});
}

#endif // Graph.h included
