#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <vector>
#include <queue>

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

#endif // Graph.h included
