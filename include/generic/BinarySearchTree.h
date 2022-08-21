#ifndef BINARY_SEARCH_TREE_H_
#define BINARY_SEARCH_TREE_H_

#include <cmath>
#include <memory>
#include <random>

template<class K, class V>
class BinarySearchTree
{
public:
  using Node = std::shared_ptr<BinarySearchTree<K, V>>;

  BinarySearchTree(K key, V val);

  K key() const;

  V value() const;

  Node parent();

  Node left();

  Node right();

protected:
  void setPtrs(Node parent, Node left, Node right);

  void setParent(Node parent);

  void setLeft(Node left);

  void setRight(Node right);

  void setValues(K key, V value);

public:
  Node _root = nullptr;

protected:
  int _size = 1;

private:
  K _key;
  V _value;

  Node _parent = nullptr;
  Node _left = nullptr;
  Node _right = nullptr;

public:
  static Node MakeNode(K key, V value);

  static Node Search(Node root, K key);

  static Node Min(Node root);

  static Node Max(Node root);

  static Node Predecessor(Node node);

  static Node Successor(Node node);

  static Node Select(Node root, int index);

  static int Rank(Node root, Node node);

  static int Rank(Node root, K key);

  // protected:
  static Node InsertNode(Node root, K key, V value);

  static Node InsertNode(Node root, Node node);

  static Node DeleteNode(Node toDelete);

private:
  static Node RotateLeft(Node toRotate);

  static Node RotateRight(Node toRotate);

  static int _Rank(Node node, K key);

  static bool _Transplant(Node node, Node transplantWith);

  static void _DecrementSize(Node toDelete);

  static void _IncrementSize(Node toAdd);
};

template<class K, class V>
BinarySearchTree<K, V>::BinarySearchTree(K key, V val)
  : _key(key)
  , _value(val)
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
BinarySearchTree<K, V>::RotateLeft(Node toRotate)
{
  auto rightSubtree = toRotate->right();

  if (rightSubtree != nullptr) {
    toRotate->setRight(rightSubtree->left());
    rightSubtree->left()->setParent(toRotate);
    rightSubtree->setParent(toRotate->parent());
  }

  if (toRotate->parent() == nullptr) {
    // ?
  } else if (toRotate == toRotate->parent()->left()) {
    toRotate->parent()->setLeft(rightSubtree);
  } else {
    toRotate->parent()->setRight(rightSubtree);
  }

  if (rightSubtree != nullptr) {
    rightSubtree->setLeft(toRotate);
  }

  toRotate->setParent(rightSubtree);

  return rightSubtree;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::RotateRight(Node toRotate)
{
  auto leftSubtree = toRotate->left();

  toRotate->setLeft(leftSubtree->right());

  if (leftSubtree != nullptr) {
    leftSubtree->right()->setParent(toRotate);
    leftSubtree->setParent(toRotate->parent());
    leftSubtree->setRight(toRotate);
  }

  if (toRotate->parent() == nullptr) {
    // ?
  } else if (toRotate == toRotate->parent()->left()) {
    toRotate->parent()->setLeft(leftSubtree);
  } else {
    toRotate->parent()->setRight(leftSubtree);
  }

  toRotate->setParent(leftSubtree);

  return leftSubtree;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::Search(Node root, K key)
{
  if (key == root->key()) {
    return root;
  } else if (key < root->key()) {
    if (root->left() == nullptr) {
      return nullptr;
    } else {
      return BinarySearchTree<K, V>::Search(root->left(), key);
    }
  } else {
    if (root->right() == nullptr) {
      return nullptr;
    } else {
      return BinarySearchTree<K, V>::Search(root->right(), key);
    }
  }
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
  if (node == nullptr) {
    return nullptr;
  }

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
  if (node == nullptr) {
    return nullptr;
  }

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
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::InsertNode(Node root, K key, V value)
{
  std::random_device rd;
  std::mt19937 gen(rd());

  auto newNode = std::make_shared<BinarySearchTree<K, V>>(root);
  newNode->setValues(key, value);

  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter;

    if (key < iter->key()) {
      iter = iter->left();
    } else if (key > iter->key()) {
      iter = iter->right();
    } else {
      if (gen() % 255 < 127) { // assign randomly with
                               // equal(?) probability
        iter = iter->left();
      } else {
        iter = iter->right();
      }
    }
  }

  newNode->setParent(newParent);

  if (newNode->key() < newParent->key()) {
    newParent->setLeft(newNode);
  } else {
    newParent->setRight(newNode);
  }

  BinarySearchTree<K, V>::_IncrementSize(newNode);

  return newNode;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::InsertNode(Node root, Node node)
{
  std::random_device rd;
  std::mt19937 gen(rd());

  BinarySearchTree<K, V>::Node newParent = nullptr;
  BinarySearchTree<K, V>::Node iter = root;

  while (iter != nullptr) {
    newParent = iter;

    if (node->key() < iter->key()) {
      iter = iter->left();
    } else if (node->key() > iter->key()) {
      iter = iter->right();
    } else {
      if (gen() % 255 < 127) { // assign randomly with
                               // equal(?) probability
        iter = iter->left();
      } else {
        iter = iter->right();
      }
    }
  }

  node->setParent(newParent);

  if (node->key() < newParent->key()) {
    newParent->setLeft(node);
  } else {
    newParent->setRight(node);
  }

  BinarySearchTree<K, V>::_IncrementSize(node);

  return node;
}

template<class K, class V>
typename BinarySearchTree<K, V>::Node
BinarySearchTree<K, V>::DeleteNode(Node toDelete)
{
  auto deletedRoot = false;
  if (toDelete->left() == nullptr) {

    deletedRoot =
      BinarySearchTree<K, V>::_Transplant(toDelete, toDelete->right());

    BinarySearchTree<K, V>::_DecrementSize(toDelete);

    if (deletedRoot) {
      return toDelete->right();
    }
  } else if (toDelete->right() == nullptr) {

    deletedRoot =
      BinarySearchTree<K, V>::_Transplant(toDelete, toDelete->left());

    BinarySearchTree<K, V>::_DecrementSize(toDelete);

    if (deletedRoot) {
      return toDelete->left();
    }
  } else {
    auto pred = BinarySearchTree<K, V>::Predecessor(toDelete);

    toDelete->setValues(pred->key(), pred->value());

    deletedRoot = BinarySearchTree<K, V>::_Transplant(pred, pred->left());

    BinarySearchTree<K, V>::_DecrementSize(pred);

    return toDelete;
  }

  return toDelete;
}

template<class K, class V>
bool
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
        return true;
      }
    }
  }

  return false;
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
      BinarySearchTree<K, V>::Search(root, node->key()) != nullptr) {
    return BinarySearchTree<K, V>::_Rank(root, node->key());
  }
  return -1;
}

template<class K, class V>
int
BinarySearchTree<K, V>::Rank(Node root, K key)
{
  if (root != nullptr && BinarySearchTree<K, V>::Search(root, key) != nullptr) {
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
