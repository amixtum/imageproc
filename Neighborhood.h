#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

enum Neighborhood {
    Moore,
    Neumann
};

std::vector<cv::Point> neighbors(Neighborhood n, cv::Point center, int leftBound, int lowBound, int rightBound, int upBound);

#endif // Neighborhood.h included
