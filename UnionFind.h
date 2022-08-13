#ifndef UNION_FIND_H_
#define UNION_FIND_H_

#include <map>
#include <vector>

template <class VertexType>
class UnionFind {
    public:
        void insert(const VertexType &vertex);

        VertexType find(VertexType &vertex);

        void unionVertices(VertexType v1, VertexType v2);

        void clear();
    private:
        VertexType & findHelper(VertexType &vertex);

        std::map<VertexType, VertexType> _leaderMap;

        std::map<VertexType, int> _rank;
};

template <class VertexType>
void UnionFind<VertexType>::insert(const VertexType &vertex) {
    _leaderMap.insert(std::make_pair(vertex, vertex));
    _rank.insert(std::make_pair(vertex, 0));
}

template <class VertexType>
VertexType UnionFind<VertexType>::find(VertexType &vertex) {
    auto leader = findHelper(vertex);

    auto toCompress = std::vector<VertexType>();
    auto temp = vertex;
    while (_leaderMap[temp] != leader) {
        toCompress.push_back(temp);
        temp = _leaderMap[temp];
    }
    for (auto &compress : toCompress) {
        _leaderMap[compress] = leader;
    }

    return leader;
}

template <class VertexType>
void UnionFind<VertexType>::unionVertices(VertexType v1, VertexType v2) {
    auto leader1 = find(v1);
    auto leader2 = find(v2);
    if (_rank[leader1] < _rank[leader2]) {
        _leaderMap[leader1] = _leaderMap[leader2];
    }
    else if (_rank[leader1] == _rank[leader2]) {
        _leaderMap[leader1] = _leaderMap[leader2];
        _rank[leader2] += 1;
    }
    else {
        _leaderMap[leader2] = _leaderMap[leader1];
    }
}

template <class VertexType>
VertexType & UnionFind<VertexType>::findHelper(VertexType &vertex) {
    if (vertex == _leaderMap[vertex]) {
        return vertex;
    }

    return findHelper(_leaderMap[vertex]);
}

template <class VertexType>
void UnionFind<VertexType>::clear() {
    _leaderMap.clear();
    _rank.clear();
}

#endif // UnionFind.h included
