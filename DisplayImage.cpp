#include <opencv2/core.hpp>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "ImageOps.h"

int main(int argc, char ** argv) {
    if (argc != 3) {
        printf("usage: DisplayImage <Image_Path1> <Image_Path2>\n");
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

    auto quads1 = splitQuadrants(image1);
    auto quads2 = splitQuadrants(image2);
    
    cv::Mat newTop;
    cv::Mat newBottom;
    cv::Mat newImage;
    std::array<cv::Mat, 4> newQuadrants; 

    glueHorizontal(quads1[0], quads2[3], newQuadrants[0]);
    glueHorizontal(quads1[1], quads2[2], newQuadrants[1]);
    glueHorizontal(quads1[2], quads2[1], newQuadrants[2]);
    glueHorizontal(quads1[3], quads2[0], newQuadrants[3]);

    glueVertical(newQuadrants[0], newQuadrants[2], newTop);
    glueVertical(newQuadrants[1], newQuadrants[3], newBottom);

    glueHorizontal(newTop, newBottom, newImage);

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);

    cv::imshow("Display Image", newImage);

    cv::waitKey(0);

    return 0;
}
