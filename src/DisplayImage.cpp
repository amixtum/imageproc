#include <string.h>

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <random>
#include <string>
#include <vector>

#include "../include/generic/BinarySearchTree.h"
#include "../include/imageops/ImageOps.h"
#include "../include/imageops/PointGraph.h"

using namespace gp;

void
testBinarySearchTree();

void
testGraph(cv::Mat& image1, cv::Mat& image2, int connectRandom, float vP);

void
testPartition(cv::Mat& image);

void
testQuicksort(cv::Mat& image);

void
testOptions(cv::Mat& image, char** argv);

int
main(int argc, char** argv)
{
  if (argc != 4) {
    printf("Usage: DisplayImage <ImagePath> <GraphConnectedness> "
           "<UnionPercentage>");
    return -1;
  }

  testBinarySearchTree();

  cv::Mat image1;
  // cv::Mat image2;

  image1 = cv::imread(argv[1], 1);
  // image2 = cv::imread(argv[2], 1);

  if (!image1.data) {
    printf("No image data for image 1\n");
    return -1;
  }

  /*
  if (!image2.data) {
      printf("No image data for image 2\n");
      return -1;
  }
  */

  testOptions(image1, argv);

  // cv::imwrite("img/output.jpg", image1);

  return 0;
}

std::shared_ptr<BinarySearchTree<int, PointVertex>>
makeNode(int k, int x, int y, int r, int g, int b)
{
  return BinarySearchTree<int, PointVertex>::MakeNode(
    k, PointVertex(cv::Point(x, y), cv::Vec3b(b, g, r)));
}

void
testBinarySearchTree()
{
  using NodeType = BinarySearchTree<int, PointVertex>::Node;
  auto t1 = makeNode(4, 4, 0, 0, 0, 0);
  auto t2 = makeNode(1, 1, 0, 0, 0, 0);
  auto t3 = makeNode(5, 5, 0, 0, 0, 0);
  auto t4 = makeNode(2, 2, 0, 0, 0, 0);
  auto t5 = makeNode(3, 3, 0, 0, 0, 0);
  auto t6 = makeNode(6, 6, 0, 0, 0, 0);

  BinarySearchTree<int, PointVertex>::InsertNode(t1, t2);
  BinarySearchTree<int, PointVertex>::InsertNode(t1, t3);
  BinarySearchTree<int, PointVertex>::InsertNode(t1, t4);
  BinarySearchTree<int, PointVertex>::InsertNode(t1, t5);
  BinarySearchTree<int, PointVertex>::InsertNode(t1, t6);

  BinarySearchTree<int, PointVertex>::DeleteNode(t5);

  std::vector<NodeType> nodes{ t1, t2, t3, t4, t6 };

  for (auto& node : nodes) {
    auto succ = BinarySearchTree<int, PointVertex>::Successor(node);

    auto pred = BinarySearchTree<int, PointVertex>::Predecessor(node);

    auto rnk = BinarySearchTree<int, PointVertex>::Rank(t1, node);

    if (succ != nullptr) {
      std::cout << "Successor of " << node->key() << ": " << succ->key()
                << std::endl;
    }

    if (pred != nullptr) {
      std::cout << "Predecessor of " << node->key() << ": " << pred->key()
                << std::endl;
    }

    std::cout << "Rank of " << node->key() << ": " << rnk << std::endl;
  }

  std::cout << "\n";
}

void
testGraph(cv::Mat& image1, cv::Mat& image2, int connectRandom, float vP)
{
  PointGraph g;
  std::cout << "Adding vertices\n";
  g.addVerticesFromImage(image1);

  std::cout << "Connecting neighbors\n";
  g.connectAllNeighbors(
    image1.size().width, image1.size().height, Neighborhood::Neumann);
  // g.connectRandom(connectRandom);
  //
  std::cout << "Performing unions\n";
  g.unionChunks(Neighborhood::Moore, vP);

  std::cout << "Painting image\n";
  g.paintImage(image2);
}

void
testPartition(cv::Mat& image)
{
  for (int y = 0; y < image.size().height; y += 1) {
    partitionRow(image, compareIntensity, 0, image.size().width - 1, y);
  }
}

void
testQuicksort(cv::Mat& image)
{
  for (int y = 0; y < image.size().height; y += 1) {
    quicksortRow(image, compareIntensity, 0, image.size().width - 1, y);
  }
}

void
testOptions(cv::Mat& image, char** argv)
{
  cv::Mat blank = cv::Mat::zeros(1, 1, CV_8UC3);

  int channel;
  int threshold;
  int replacement;

  std::cout << "color channel: ";
  std::cin >> channel;

  std::cout << "channel threshold: ";
  std::cin >> threshold;

  std::cout << "channel replacement: ";
  std::cin >> replacement;

  std::function<cv::Vec3b(std::vector<cv::Vec3b>)> minColorFn =
    [&](std::vector<cv::Vec3b> colors) {
      return remove_replace_min(colors, channel, threshold, replacement);
    };

  std::function<cv::Vec3b(std::vector<cv::Vec3b>)> maxColorFn =
    [&](std::vector<cv::Vec3b> colors) {
      return remove_replace_max(colors, channel, threshold, replacement);
    };

  cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display Image", image);

  auto k = cv::waitKey(0);

  while (k != 'q') {
    switch (k) {
      case 'x':
        cv::resize(image,
                   image,
                   cv::Size(image.size().width * 2, image.size().height * 2));
        break;
      case 'c':
        cv::resize(image,
                   image,
                   cv::Size(std::floor(image.size().width / 2),
                            std::floor(image.size().height / 2)));
        break;
      case 'p':
        testPartition(image);
        break;
      case 's':
        cv::imwrite("img/output.jpg", image);
        break;
      case 'g':
        testGraph(image, blank, atoi(argv[2]), atof(argv[3]));
        image = blank;
        blank = cv::Mat::zeros(1, 1, CV_8UC3);
        break;
      case 'f':
        applyColorFn(image, minColorFn, Neighborhood::Moore);
        break;
      case 'F':
        applyColorFn(image, maxColorFn, Neighborhood::Moore);
        break;
      case 'r':
        applyColorFnRecursive(image, minColorFn, Neighborhood::Moore, 0);
        break;
      case 'R':
        applyColorFnRecursive(image, maxColorFn, Neighborhood::Moore, 0);
        break;
      case 'm':
        std::cout << "color channel: ";
        std::cin >> channel;

        std::cout << "channel threshold: ";
        std::cin >> threshold;

        std::cout << "channel replacement: ";
        std::cin >> replacement;

        minColorFn = [&](std::vector<cv::Vec3b> colors) {
          return remove_replace_min(colors, channel, threshold, replacement);
        };
        maxColorFn = [&](std::vector<cv::Vec3b> colors) {
          return remove_replace_max(colors, channel, threshold, replacement);
        };
        break;
      default:
        break;
    }

    cv::imshow("Display Image", image);
    k = cv::waitKey(0);
  }
}
