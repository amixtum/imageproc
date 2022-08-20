#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <cmath>
#include <iostream>
#include <memory>
#include <random>

template<class K, class V>
class BinarySearchTree
{
public:
  using Node = std::shared_ptr<BinarySearchTree<K, V>>;

  BinarySearchTree(K key, V value);

  K key() const;

  V value() const;

  V minValue();

  V maxValue();

  // Returns nullptr if this tree does not have any connections
  Node node();

  Node parent();

  Node left();

  Node right();

  Node search(K key);

public:
  static Node Min(Node root);

  static Node Max(Node root);

  static Node Predecessor(Node node);

  static Node Successor(Node node);

  static void InsertNode(Node root, K key, V value);

  static void InsertNode(Node root, Node node);

  static void DeleteNode(Node toDelete);

  static Node Select(Node root, int index);

  static int Rank(Node root, Node node);

  static int Rank(Node root, K key);

  static Node MakeNode(K key, V value);

protected:
  void setPtrs(Node parent, Node left, Node right);

  void setParent(Node parent);

  void setLeft(Node left);

  void setRight(Node right);

  void setValues(K key, V value);

  int _size = 1;

private:
  static int _Rank(Node node, K key);

  static void _Transplant(Node node, Node transplantWith);

  static void _DecrementSize(Node toDelete);

  static void _IncrementSize(Node toAdd);

private:
  K _key;
  V _value;

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

/*
template<class K, class V>
BinarySearchTree<K, V>::BinarySearchTree(BinarySearchTree<K, V>& other)
  : _key(other.key())
  , _value(other.value())
{
  _parent = other.parent();
  _left = other.left();
  _right = other.right();
}
*/

template<class K, class V>
void
BinarySearchTree<K, V>::setPtrs(Node parent, Node left, Node right)
{
  _parent = parent;
  _left = left;
  _right = right;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setParent(Node parent)
{
  _parent = parent;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setLeft(Node left)
{
  _left = left;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setRight(Node right)
{
  _right = right;
}

template<class K, class V>
void
BinarySearchTree<K, V>::setValues(K key, V value)
{
  _key = key;
  _value = value;
}

template<class K, class V>
K
BinarySearchTree<K, V>::key() const
{
  return _key;
}

template<class K, class V>
V
BinarySearchTree<K, V>::value() const
{
  return _value;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::node()
{
  if (left() != nullptr) {
    return left()->parent();
  } else if (right() != nullptr) {
    return right()->parent();
  } else if (parent() != nullptr) {
    if (parent()->left()->key() == key()) {
      return parent()->left();
    } else {
      return parent()->right();
    }
  } else {
    return nullptr;
  }
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::parent()
{
  return _parent;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::left()
{
  return _left;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::right()
{
  return _right;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::search(K key)
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
{
  if (_left == nullptr) {
    return _value;
  }

  return left()->minValue();
}

template<class K, class V>
V
BinarySearchTree<K, V>::maxValue()
{
  if (_right == nullptr) {
    return _value;
  }

  return right()->maxValue();
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Min(Node root)
{
  if (root->left() == nullptr) {
    return root;
  }

  return BinarySearchTree<K, V>::Min(root->left());
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Max(Node root)
{
  if (root->right() == nullptr) {
    return root;
  }

  return BinarySearchTree<K, V>::Max(root->right());
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Predecessor(Node node)
{
  if (node->left() != nullptr) {
    return BinarySearchTree<K, V>::Max(node->left());
  }

  auto iter1 = node->parent();
  auto iter2 = node;

  while (iter1 != nullptr && iter2 == iter1->left()) {
    iter2 = iter1;
    iter1 = iter1->parent();
  }

  return iter1;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Successor(Node node)
{
  if (node->right() != nullptr) {
    return BinarySearchTree<K, V>::Min(node->right());
  }

  auto iter1 = node->parent();
  auto iter2 = node;

  while (iter1 != nullptr && iter2 == iter1->right()) {
    iter2 = iter1;
    iter1 = iter1->parent();
  }

  return iter1;
}

template<class K, class V>
void
BinarySearchTree<K, V>::InsertNode(Node root, K key, V value)
{
  auto newNode = std::make_shared<BinarySearchTree<K, V>>(key, value);

  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter;

    if (key < iter->key()) {
      iter = iter->left();
    } else {
      iter = iter->right();
    }
  }

  newNode->setParent(newParent);

  if (newNode->key() < newParent->key()) {
    newParent->setLeft(newNode);
  } else {
    newParent->setRight(newNode);
  }

  BinarySearchTree<K, V>::_IncrementSize(newNode);
}

template<class K, class V>
void
BinarySearchTree<K, V>::InsertNode(Node root, Node node)
{
  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter;

    if (node->key() < iter->key()) {
      iter = iter->left();
    } else {
      iter = iter->right();
    }
  }

  node->setParent(newParent);

  if (node->key() < newParent->key()) {
    newParent->setLeft(node);
  } else {
    newParent->setRight(node);
  }

  BinarySearchTree<K, V>::_IncrementSize(node);
}

template<class K, class V>
void
BinarySearchTree<K, V>::DeleteNode(Node toDelete)
{
  if (toDelete->left() == nullptr) {
    BinarySearchTree<K, V>::_Transplant(toDelete, toDelete->right());
    BinarySearchTree<K, V>::_DecrementSize(toDelete);
  } else if (toDelete->right() == nullptr) {
    BinarySearchTree<K, V>::_Transplant(toDelete, toDelete->left());
    BinarySearchTree<K, V>::_DecrementSize(toDelete);
  } else {
    auto pred = BinarySearchTree<K, V>::Predecessor(toDelete);
    toDelete->setValues(pred->key(), pred->value());
    BinarySearchTree<K, V>::_Transplant(pred, pred->left());
    BinarySearchTree<K, V>::_DecrementSize(pred);
  }
}

template<class K, class V>
void
BinarySearchTree<K, V>::_Transplant(Node node, Node transplantWith)
{
  if (node != nullptr) {
    if (node->parent() != nullptr) {
      if (node->parent()->left() == node) {
        node->parent()->setLeft(transplantWith);
      } else {
        node->parent()->setRight(transplantWith);
      }

      if (transplantWith != nullptr) {
        transplantWith->setParent(node->parent());
      }
    } else {
      if (transplantWith != nullptr) {
        transplantWith->setParent(nullptr);
      }
    }
  }
}

template<class K, class V>
void
BinarySearchTree<K, V>::_DecrementSize(Node toDelete)
{
  auto size_iter = toDelete->parent();
  while (size_iter != nullptr) {
    size_iter->_size -= 1;
    size_iter = size_iter->parent();
  }
}

template<class K, class V>
void
BinarySearchTree<K, V>::_IncrementSize(Node toAdd)
{
  auto size_iter = toAdd->parent();
  while (size_iter != nullptr) {
    size_iter->_size += 1;
    size_iter = size_iter->parent();
  }
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Select(Node root, int index)
{
  if (root == nullptr) {
    return nullptr;
  }

  int leftSize = 0;

  if (root->left() != nullptr) {
    leftSize = root->left()->_size;
  }

  if (leftSize == index) {
    return root;
  }

  else if (leftSize > index) {
    return BinarySearchTree<K, V>::Select(root->left(), index);
  }

  else {
    if (root->right() != nullptr) {
      return BinarySearchTree<K, V>::Select(root->right(),
                                            index - leftSize - 1);
    }

    return root->right();
  }
}

template<class K, class V>
int
BinarySearchTree<K, V>::Rank(Node root, Node node)
{
  if (root != nullptr && node != nullptr &&
      root->search(node->key()) != nullptr) {
    return BinarySearchTree<K, V>::_Rank(root, node->key());
  }
  return -1;
}

template<class K, class V>
int
BinarySearchTree<K, V>::Rank(Node root, K key)
{
  if (root != nullptr && root->search(key) != nullptr) {
    return BinarySearchTree<K, V>::_Rank(root, key);
  }

  return -1;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::MakeNode(K key, V value)
{
  return std::make_shared<BinarySearchTree<K, V>>(key, value);
}

template<class K, class V>
int
BinarySearchTree<K, V>::_Rank(Node node, K key)
{
  if (node == nullptr) {
    return -1;
  }

  auto iter = node;
  int rnk = 0;

  while (iter != nullptr) {
    if (iter->key() < key) {
      if (iter->left() != nullptr) {
        rnk += 1 + iter->left()->_size;
      } else {
        rnk += 1;
      }
      iter = iter->right();
    } else if (iter->key() > key) {
      iter = iter->left();
    } else {
      if (iter->left() != nullptr) {
        return (rnk + iter->left()->_size);
      } else {
        return rnk;
      }
    }
  }

  return -1;
}

#endif // BinarySearchTree.h included
