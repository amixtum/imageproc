#include "ImageOps.h"

void padHeight(cv::Mat &mat, int newHeight) {
    if (mat.size().height < newHeight) {
        auto bottom = 
            cv::Mat::zeros(newHeight - mat.size().height, // rows
                    mat.size().width, // cols
                    CV_8UC3); // type

        vconcat(mat, bottom, mat);
    }
}

void padWidth(cv::Mat &mat, int newWidth) {
    if (mat.size().width < newWidth) {
        auto bottom = 
            cv::Mat::zeros(mat.size().height, // rows
                    newWidth - mat.size().width, // cols
                    CV_8UC3); // type

        hconcat(mat, bottom, mat);
    }
}

void glueHorizontal(cv::Mat &left, cv::Mat &right, cv::Mat &glued) {
    auto newHeight = std::max(left.size().height, 
                               right.size().height);
    
    if (newHeight == left.size().height) {
        padHeight(right, newHeight);
    }
    else {
        padHeight(left, newHeight);
    }

    hconcat(left, right, glued);
}

void glueVertical(cv::Mat &left, cv::Mat &right, cv::Mat &glued) {
    auto newWidth = std::max(left.size().width, 
                               right.size().width);
    
    if (newWidth == left.size().width) {
        padWidth(right, newWidth);
    }
    else {
        padWidth(left, newWidth);
    }

    vconcat(left, right, glued);
}

std::array<cv::Mat, 4> splitQuadrants(cv::Mat &split) {
    std::array<cv::Mat, 4> quadrants;

    int midX = floor(split.size().width / 2);
    int midY = floor(split.size().height / 2);

    auto topLeft = cv::Rect(0, 0, midX, midY);
    auto topRight = cv::Rect(midX, 0, midX, midY);
    auto bottomLeft = cv::Rect(0, midY, midX, midY);
    auto bottomRight = cv::Rect(midX, midY, midX, midY);

    quadrants[0] = cv::Mat(split, topLeft);
    quadrants[1] = cv::Mat(split, topRight);
    quadrants[2] = cv::Mat(split, bottomLeft);
    quadrants[3] = cv::Mat(split, bottomRight);

    return quadrants;
}
