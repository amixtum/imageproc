#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <algorithm>
#include <ios>
#include <memory>

template<class K, class V>
class BinarySearchTree
{
public:
  using Node = std::shared_ptr<BinarySearchTree<K, V>>;

  BinarySearchTree(K key, V value);

  BinarySearchTree(const BinarySearchTree<K, V> &other);

  void setPtrs(Node &parent,
               Node &left,
               Node &right);

  void setParent(Node &parent);

  void setLeft(Node &left);

  void setRight(Node &right);

  K key() const;

  V value() const;

  Node parent() const;

  Node left() const;

  Node right() const;

  Node search(K& key) const;

  V minValue() const;

  V maxValue() const;

  Node min() const;

  Node max() const;

  Node predecessor() const;

  Node successor() const;

  static void insert(Node root, K &key, V &value);

  static void insert(Node root, Node node);

  static Node makeNode(K &key, V &value);

private:
  const K _key;
  const V _value;

  Node _parent = nullptr;
  Node _left = nullptr;
  Node _right = nullptr;
};



template<class K, class V>
BinarySearchTree<K, V>::BinarySearchTree(K key, V value)
  : _key(key)
  , _value(value)
{
}

template <class K, class V>
BinarySearchTree<K, V>::BinarySearchTree(const BinarySearchTree<K, V> &other)
  : _key(other.key())
  , _value(other.value())
{
  _parent = other.parent();
  _left = other.left();
  _right = other.right();
}

template<class K, class V>
void
BinarySearchTree<K, V>::setPtrs(Node &parent,
                                Node &left,
                                Node &right)
{
  _parent = parent;
  _left = left;
  _right = right;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setParent(Node &parent)
{
  _parent = parent;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setLeft(Node &left)
{
  _left = left;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setRight(Node &right)
{
  _right = right;
}

template<class K, class V>
K
BinarySearchTree<K, V>::key()
  const 
{
  return _key;
}

template<class K, class V>
V
BinarySearchTree<K, V>::value()
  const
{
  return _value;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::parent()
  const
{
  return _parent;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::left()
  const
{
  return _left;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::right()
  const
{
  return _right;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::search(K& key)
  const
{
  if (key == _key) {
    return std::make_shared<BinarySearchTree<K, V>>(*this);
  }
  if (key < _key) {
    if (_left == nullptr) {
      return nullptr;
    }

    return left()->search(key);
  }

  else {
    if (_right == nullptr) {
      return nullptr;
    }

    return right()->search(key);
  }
}

template<class K, class V>
V
BinarySearchTree<K, V>::minValue()
  const
{
  if (_left == nullptr) {
    return _value;
  }

  return left()->minValue();
}

template<class K, class V>
V
BinarySearchTree<K, V>::maxValue()
  const
{
  if (_right == nullptr) {
    return _value;
  }

  return right()->maxValue();
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::min()
  const
{
  if (_left == nullptr) {
    return std::make_shared<BinarySearchTree<K, V>>(*this);
  }

  return left()->min();
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::max()
  const
{
  if (_right == nullptr) {
    return std::make_shared<BinarySearchTree<K, V>>(*this);
  }

  return right()->max();
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::predecessor()
  const
{
  if (_left != nullptr) {
    return left()->max();
  }

  auto iter1 = parent();
  auto iter2 = iter1;
  if (iter1->left()->key() == key()) {
    iter2 = iter1->left();
  }

  while (iter1 != nullptr && iter2 == iter1->left()) {
    iter2 = iter1;
    iter1 = iter1->parent();
  }

  return iter1;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::successor()
  const
{
  if (_right != nullptr) {
    return right()->min();
  }

  auto iter1 = parent();
  auto iter2 = iter1;
  if (iter1->right()->key() == key()) {
    iter2 = iter1->right();
  }

  while (iter1 != nullptr && iter2 == iter1->right()) {
    iter2 = iter1;
    iter1 = iter1->parent();
  }

  return iter1;
}

template<class K, class V>
void
BinarySearchTree<K, V>::insert(Node root, K &key, V &value)
{
  auto newNode = std::make_shared<BinarySearchTree<K, V>>(key, value);

  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter; 

    if (key < iter->key()) {
      iter = iter->left();
    }
    else {
      iter = iter->right();
    }

  }

  newNode->setParent(newParent);
  
  if (newNode->key() < newParent->key()) {
    newParent->setLeft(newNode);
  }
  else {
    newParent->setRight(newNode);
  }
}

template <class K, class V>
void
BinarySearchTree<K, V>::insert(BinarySearchTree<K, V>::Node root, BinarySearchTree<K, V>::Node node) 
{
  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter; 

    if (node->key() < iter->key()) {
      iter = iter->left();
    }
    else {
      iter = iter->right();
    }

  }

  node->setParent(newParent);
  
  if (node->key() < newParent->key()) {
    newParent->setLeft(node);
  }
  else {
    newParent->setRight(node);
  }
}

template <class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::makeNode(K &key, V &value) 
{
  return std::make_shared<BinarySearchTree<K, V>>(key, value);
}

#endif // BinarySearchTree.h included
