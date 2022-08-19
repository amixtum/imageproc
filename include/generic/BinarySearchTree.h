#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <memory>

template <class K, class V>
class BinarySearchTree {
    public:
        BinarySearchTree(K key, V value);

        void setPtrs(
                BinarySearchTree<K, V> &parent, 
                BinarySearchTree<K, V> &left,
                BinarySearchTree<K, V> &right
        );

        void setParent(BinarySearchTree<K, V> &parent);

        void setLeft(BinarySearchTree<K, V> &left);

        void setRight(BinarySearchTree<K, V> &right);

        K key();

        V value();

        void insert(K key, V value);

    private:
        const K _key;
        const V _value;

        std::shared_ptr<BinarySearchTree<K, V>> _parent;
        std::shared_ptr<BinarySearchTree<K, V>> _left; 
        std::shared_ptr<BinarySearchTree<K, V>> _right;
};

#endif // BinarySearchTree.h included
