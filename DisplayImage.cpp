#include <opencv2/core.hpp>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "ImageOps.h"
#include "PointGraph.h"

void testGraph(cv::Mat &image1, cv::Mat &image2);

void testColorFn(cv::Mat &image, Neighborhood nbr);

void testPartition(cv::Mat &image);

int main(int argc, char ** argv) {
    if (argc != 3) {
        printf("Usage: DisplayImage <ImagePath> <ImagePath>");
        return -1;
    }
    cv::Mat image1;
    cv::Mat image2;
    cv::Mat glued;

    image1 = cv::imread(argv[1], 1);
    image2 = cv::imread(argv[2], 1);

    if (!image1.data) {
        printf("No image data for image 1\n");
        return -1;
    }

    if (!image2.data) {
        printf("No image data for image 2\n");
        return -1;
    }

    testColorFn(image1, Neighborhood::Neumann);

    //cv::imwrite("img/output.jpg", image2);

    return 0;
}

void testGraph(cv::Mat &image1, cv::Mat &image2) {
    PointGraph g;
    g.addVerticesFromImage(image1);
    g.connectAllNeighbors(image1.size().width, image1.size().height, Neighborhood::Neumann);
    g.paintImage(image2);
}

void testColorFn(cv::Mat &image, Neighborhood nbr) {


    int channel;
    int threshold;
    int replacement;

    int method;

    std::cout << "color channel: ";
    std::cin >> channel;

    std::cout << "channel threshold: ";
    std::cin >> threshold;

    std::cout << "channel replacement: ";
    std::cin >> replacement;

    auto colorFn = [&] (std::vector<cv::Vec3b> colors) {
        return remove_replace(colors, channel, threshold, replacement);
    };

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    auto k = cv::waitKey(0);

    while (k != 'q') {
        switch (k) {
        case 'q':
            break;
        case 'a':
            applyColorFnRange(image, colorFn, nbr, cv::Point(0, 0), cv::Point(image.size().width, image.size().height));
            break;
        case 'b':
            applyColorFnRecursive(image, colorFn, nbr, true);
            break;
        default:
            break;
        }

        cv::imshow("Display Image", image);
        k = cv::waitKey(0);
    }
    
}
