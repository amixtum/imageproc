#include "Neighborhood.h"

std::vector<cv::Point> neighbors(Neighborhood n, cv::Point center, int leftBound, int rightBound, int upBound, int lowBound) {
    std::vector<cv::Point> points;
    
    if (center.x > leftBound) {
        points.push_back(cv::Point(center.x - 1, center.y));

        if (n == Neighborhood::Moore) {
            if (center.y > lowBound) {
                points.push_back(cv::Point(center.x - 1, center.y - 1));
            }

            if (center.y < upBound) {
                points.push_back(cv::Point(center.x - 1, center.y + 1));
            }
        }
    }

    if (center.x < rightBound) {
        points.push_back(cv::Point(center.x + 1, center.y));

        if (n == Neighborhood::Moore) {

            if (center.y > lowBound) {
                points.push_back(cv::Point(center.x + 1, center.y - 1));
            }

            if (center.y < upBound) {
                points.push_back(cv::Point(center.x + 1, center.y + 1));
            }
        }
    }

    if (center.y > lowBound) {
        points.push_back(cv::Point(center.x, center.y - 1));
    }

    if (center.y < upBound) {
        points.push_back(cv::Point(center.x, center.y + 1));
    }

    return points;
}
