#ifndef IMAGE_OPS_H_
#define IMAGE_OPS_H_

#include <array>
#include <cmath>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include "Graph.h"


void padHeight(cv::Mat &mat, int newHeight);

void padWidth(cv::Mat &mat, int newWidth);

void glueHorizontal(cv::Mat &left, cv::Mat &right, cv::Mat &glued);

void glueVertical(cv::Mat &left, cv::Mat &right, cv::Mat &glued);

std::array<cv::Mat, 4> splitQuadrants(cv::Mat &split);

#endif // ImageOps.h included
