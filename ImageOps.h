#ifndef IMAGE_OPS_H_
#define IMAGE_OPS_H_

#include <array>
#include <cmath>
#include <functional>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "Graph.h"
#include "PointGraph.h"
#include "Neighborhood.h"

namespace gp {
    void padHeight(cv::Mat &mat, int newHeight);

    void padWidth(cv::Mat &mat, int newWidth);

    void glueHorizontal(cv::Mat &left, cv::Mat &right, cv::Mat &glued);

    void glueVertical(cv::Mat &left, cv::Mat &right, cv::Mat &glued);

    std::array<cv::Mat, 4> splitQuadrantsClone(cv::Mat &split);

    std::array<cv::Mat, 4> splitQuadrantsRef(cv::Mat &split);

    void applyColorFn(
            cv::Mat &image, 
            std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, 
            Neighborhood nbr
    );

    void applyColorFnRange(
            cv::Mat &image, 
            std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, 
            Neighborhood nbr, 
            cv::Point topLeft, 
            cv::Point bottomRight
    );

    void applyColorFnRecursive(
            cv::Mat &image, 
            std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, 
            Neighborhood nbr, 
            bool shouldApply
    );

    cv::Vec3b remove_replace(
            std::vector<cv::Vec3b> colors, 
            int channel, 
            int threshold, 
            int replacement
    );

    cv::Vec3b avgColor(std::vector<cv::Vec3b> colors);

    int compareIntensity(cv::Vec3b l, cv::Vec3b r);

    void swapPixels(cv::Mat &image, cv::Point l, cv::Point r);

    int partitionRow(
            cv::Mat &image, 
            std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, 
            int xMin, 
            int xMax, 
            int y
    );

    int partitionCol(
            cv::Mat &image, 
            std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, 
            int yMin, 
            int yMax, 
            int x
    );

    void quicksortRow(
            cv::Mat &image, 
            std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, 
            int xMin, 
            int xMax, 
            int y
    );

    void quicksortCol(
            cv::Mat &image, 
            std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, 
            int yMin, 
            int yMax, 
            int x
    );
}

#endif // ImageOps.h included
