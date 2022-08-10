#include <opencv2/core.hpp>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "ImageOps.h"

int main(int argc, char ** argv) {
    if (argc != 5) {
        printf("usage: DisplayImage <Image_Path1> <channel> <threshold> <replacement>\n");
        return -1;
    }

    cv::Mat image1;
    cv::Mat glued;

    image1 = cv::imread(argv[1], 1);

    if (!image1.data) {
        printf("No image data for image 1\n");
        return -1;
    }

    int channel = atoi(argv[2]);
    int threshold = atoi(argv[3]);
    int replacement = atoi(argv[4]);
    auto colorFn = [&] (std::vector<cv::Vec3b> colors) {
        return remove_replace(colors, channel, threshold, replacement);
    };

    applyColorFnRecursive(image1, colorFn, Neighborhood::Moore, true);

    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);

    cv::imshow("Display Image", image1);

    cv::waitKey(0);

    return 0;
}
