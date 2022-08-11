#include <vector>
#include <random>

#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

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

std::array<cv::Mat, 4> splitQuadrantsClone(cv::Mat &split) {
    std::array<cv::Mat, 4> quadrants;

    int midX = floor(split.size().width / 2);
    int midY = floor(split.size().height / 2);

    auto topLeft = cv::Rect(0, 0, midX, midY);
    auto topRight = cv::Rect(midX, 0, midX, midY);
    auto bottomLeft = cv::Rect(0, midY, midX, midY);
    auto bottomRight = cv::Rect(midX, midY, midX, midY);

    quadrants[0] = split(topLeft).clone();
    quadrants[1] = split(topRight).clone();
    quadrants[2] = split(bottomLeft).clone();
    quadrants[3] = split(bottomRight).clone();

    return quadrants;
}

std::array<cv::Mat, 4> splitQuadrantsRef(cv::Mat &split) {
    std::array<cv::Mat, 4> quadrants;

    int midX = floor(split.size().width / 2);
    int midY = floor(split.size().height / 2);

    auto topLeft = cv::Rect(0, 0, midX, midY);
    auto topRight = cv::Rect(midX, 0, midX, midY);
    auto bottomLeft = cv::Rect(0, midY, midX, midY);
    auto bottomRight = cv::Rect(midX, midY, midX, midY);

    quadrants[0] = split(topLeft);
    quadrants[1] = split(topRight);
    quadrants[2] = split(bottomLeft);
    quadrants[3] = split(bottomRight);

    return quadrants;
}

void applyColorFn(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, Neighborhood nbr) {
    for (int x = 0; x < image.size().width; x += 1) {
        for (int y = 0; y < image.size().height; y += 1) {
            auto center = cv::Point(x, y);
            auto neighborhood = neighbors(nbr, center, 0, 0, image.size().width - 1, image.size().height - 1);
            std::vector<cv::Vec3b> colors;
            for (auto &neighbor : neighborhood) {
                colors.push_back(image.at<cv::Vec3b>(neighbor.y, neighbor.x));
            }

            cv::circle(image, center, 1, colorFn(colors));
        }
    }
}

void applyColorFnRange(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, Neighborhood nbr, cv::Point topLeft, cv::Point bottomRight) {
    if (topLeft.x < 0 || topLeft.y < 0 || topLeft.x >= image.size().width || topLeft.y >= image.size().height || 
        bottomRight.x < 0 || bottomRight.y < 0 || bottomRight.x > image.size().width || bottomRight.y > image.size().height) {
        return;
    }

    for (int x = topLeft.x; x < bottomRight.x; x += 1) {
        for (int y = topLeft.y; y < bottomRight.y; y += 1) {
            auto center = cv::Point(x, y);
            auto neighborhood = neighbors(nbr, center, 0, 0, image.size().width - 1, image.size().height - 1);
            std::vector<cv::Vec3b> colors;
            for (auto &neighbor : neighborhood) {
                colors.push_back(image.at<cv::Vec3b>(neighbor));
            }

            cv::circle(image, center, 1, colorFn(colors));
        }
    }
}

void applyColorFnRecursive(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>)> colorFn, Neighborhood nbr, bool shouldApply) {
    if (image.size().width < 2 || image.size().height < 2) {
        return;
    }


    auto quadrants = splitQuadrantsRef(image);
    if (shouldApply) {
        applyColorFnRange(quadrants[0], colorFn, nbr, cv::Point(0, 0), cv::Point(quadrants[0].size().width - 1, quadrants[0].size().height - 1));
        applyColorFnRange(quadrants[3], colorFn, nbr, cv::Point(0, 0), cv::Point(quadrants[3].size().width - 1, quadrants[3].size().height - 1));

        applyColorFnRecursive(quadrants[1], colorFn, nbr, true);
        applyColorFnRecursive(quadrants[2], colorFn, nbr, false);
    }
    else {
        applyColorFnRange(quadrants[1], colorFn, nbr, cv::Point(0, 0), cv::Point(quadrants[1].size().width - 1, quadrants[1].size().height - 1));
        applyColorFnRange(quadrants[2], colorFn, nbr, cv::Point(0, 0), cv::Point(quadrants[2].size().width - 1, quadrants[2].size().height - 1));

        applyColorFnRecursive(quadrants[0], colorFn, nbr, true);
        applyColorFnRecursive(quadrants[3], colorFn, nbr, false);
    }
    

}

cv::Vec3b remove_replace(std::vector<cv::Vec3b> colors, int channel, int threshold, int replacement) {
    auto selected = avgColor(colors);
    /*
    unsigned char maxIntensity = static_cast<unsigned char>(static_cast<float>(selected[channel]) / (selected[0] + selected[1] + selected[2]));
    for (auto &color : colors) {
        unsigned char channelPercent = static_cast<unsigned char>(static_cast<float>(color[channel]) / (color[0] + color[1] + color[2]));
        if (channelPercent > maxIntensity) {
            maxIntensity = channelPercent;
            selected = color;
        }
    }
    */
    
    if (selected[channel] >= threshold) {
        auto toAdd = static_cast<unsigned char>(static_cast<float>(replacement) * (static_cast<float>(replacement) / threshold));
        switch (channel) {
        case 0:
            return cv::Vec3b(0, selected[1], selected[2] - toAdd);
        case 1:
            return cv::Vec3b(selected[0] - toAdd, 0, selected[2]);
        case 2:
            return cv::Vec3b(selected[0], selected[1] - toAdd, 0);
        default:
            return selected;
        }
    }

    else {
        switch (channel) {
        case 0:
            return cv::Vec3b(selected[0] + replacement, selected[1], selected[2]);
        case 1:
            return cv::Vec3b(selected[0], selected[1] + replacement, selected[2]);
        case 2:
            return cv::Vec3b(selected[0], selected[1], selected[2] + replacement);
        default:
            return selected;
        }
    }
}

cv::Vec3b avgColor(std::vector<cv::Vec3b> colors) {
    float blue = 0;
    float green = 0;
    float red = 0;

    for (auto &color : colors){
        blue += color[0];
        green += color[1];
        red += color[2];
    }

    blue /= colors.size();
    green /= colors.size();
    red /= colors.size();

    return cv::Vec3b(blue, green, red);
}

int compareIntensity(cv::Vec3b l, cv::Vec3b r) {
    auto lintensity = static_cast<float>(l[0] + l[1] + l[2]) / 255.f;
    auto rintensity = static_cast<float>(r[0] + r[1] + r[2]) / 255.f;
    if (lintensity < rintensity) {
        return -1;
    }
    else if (lintensity == rintensity) {
        return 0;
    }

    else {
        return 1;
    }
}

void swapPixels(cv::Mat &image, cv::Point l, cv::Point r) {
    auto temp = image.at<cv::Vec3b>(l);
    cv::circle(image, l, 1, image.at<cv::Vec3b>(r));
    cv::circle(image, r, 1, temp);
}

int partitionRow(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int xMin, int xMax, int y) {
    std::random_device rd; 
    std::mt19937 gen(rd());
    auto pX = gen() % image.size().width;

    auto pivot = image.at<cv::Vec3b>(y, pX);
    swapPixels(image, cv::Point(xMin, y), cv::Point(pX, y));

    auto left = xMin + 1;
    auto right = xMax;

    while (left < right) {
        while (left < xMax && colorCmp(image.at<cv::Vec3b>(y, left), pivot) <= 0) {
            left += 1;
        }

        while (right > xMin && colorCmp(image.at<cv::Vec3b>(y, right), pivot) > 0) {
            right -= 1;
        }

        swapPixels(image, cv::Point(left, y), cv::Point(right, y));
    }

    swapPixels(image, cv::Point(left, y), cv::Point(right, y));

    swapPixels(image, cv::Point(xMin, y), cv::Point(right, y));

    return right;
}

int partitionCol(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int yMin, int yMax, int x) {
    std::random_device rd; 
    std::mt19937 gen(rd());
    auto pY = gen() % image.size().width;

    auto pivot = image.at<cv::Vec3b>(pY, x);
    swapPixels(image, cv::Point(x, yMin), cv::Point(x, pY));

    auto left = yMin + 1;
    auto right = yMax;

    while (left < right) {
        while (left < yMax && colorCmp(image.at<cv::Vec3b>(left, x), pivot) < 0) {
            left += 1;
        }

        while (right > yMin && colorCmp(image.at<cv::Vec3b>(right, x), pivot) >= 0) {
            right -= 1;
        }

        swapPixels(image, cv::Point(x, left), cv::Point(x, right));
    }

    swapPixels(image, cv::Point(x, left), cv::Point(x, right));

    swapPixels(image, cv::Point(x, yMin), cv::Point(x, right));

    return right;
}

void quicksortRow(cv::Mat &image, std::function<bool(cv::Vec3b, cv::Vec3b)> colorCmp, int xMin, int xMax, int y) {
    if (xMin < xMax) {
        auto p = partitionRow(image, colorCmp, xMin, xMax, y);
        quicksortRow(image, colorCmp, xMin, p - 1, y);
        quicksortRow(image, colorCmp, p + 1, xMax, y);
    }
}

void quicksortCol(cv::Mat &image, std::function<bool(cv::Vec3b, cv::Vec3b)> colorCmp, int yMin, int yMax, int x) {
    if (yMin < yMax) {
        auto p = partitionRow(image, colorCmp, yMin, yMax, x);
        quicksortRow(image, colorCmp, yMin, p - 1, x);
        quicksortRow(image, colorCmp, p + 1, yMax, x);
    }
}
