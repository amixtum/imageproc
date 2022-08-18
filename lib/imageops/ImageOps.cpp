#include <vector>
#include <cmath>
#include <random>

#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "../../include/imageops/ImageOps.h"

void gp::padHeight(cv::Mat &mat, int newHeight) {
    if (mat.size().height < newHeight) {
        auto bottom = 
            cv::Mat::zeros(newHeight - mat.size().height, // rows
                    mat.size().width, // cols
                    CV_8UC3); // type

        vconcat(mat, bottom, mat);
    }
}

void gp::padWidth(cv::Mat &mat, int newWidth) {
    if (mat.size().width < newWidth) {
        auto bottom = 
            cv::Mat::zeros(mat.size().height, // rows
                    newWidth - mat.size().width, // cols
                    CV_8UC3); // type

        hconcat(mat, bottom, mat);
    }
}

void gp::glueHorizontal(cv::Mat &left, cv::Mat &right, cv::Mat &glued) {
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

void gp::glueVertical(cv::Mat &left, cv::Mat &right, cv::Mat &glued) {
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

std::array<cv::Mat, 4> gp::splitQuadrantsClone(cv::Mat &split) {
    std::array<cv::Mat, 4> quadrants;

    int midX = std::floor(static_cast<float>(split.size().width) / 2.f);
    int midY = std::floor(static_cast<float>(split.size().height) / 2.f);

    auto topLeft = cv::Rect(0, 0, midX, midY);
    auto topRight = cv::Rect(midX, 0, split.size().width - midX, midY);
    auto bottomLeft = cv::Rect(0, midY, midX, split.size().height - midY);
    auto bottomRight = cv::Rect(midX, midY, split.size().width - midX, split.size().height - midY);

    quadrants[0] = split(topLeft).clone();
    quadrants[1] = split(topRight).clone();
    quadrants[2] = split(bottomLeft).clone();
    quadrants[3] = split(bottomRight).clone();

    return quadrants;
}

std::array<cv::Mat, 4> gp::splitQuadrantsRef(cv::Mat &split) {
    std::array<cv::Mat, 4> quadrants;

    int midX = std::ceil(static_cast<float>(split.size().width) / 2.f);
    int midY = std::ceil(static_cast<float>(split.size().height) / 2.f);

    auto topLeft = cv::Rect(0, 0, midX, midY);
    auto topRight = cv::Rect(midX, 0, split.size().width - midX, midY);
    auto bottomLeft = cv::Rect(0, midY, midX, split.size().height - midY);
    auto bottomRight = cv::Rect(midX, midY, split.size().width - midX, split.size().height - midY);

    quadrants[0] = split(topLeft);
    quadrants[1] = split(topRight);
    quadrants[2] = split(bottomLeft);
    quadrants[3] = split(bottomRight);

    return quadrants;
}

void gp::applyColorFn(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>&)> colorFn, Neighborhood nbr) {
    std::vector<std::pair<cv::Point, std::vector<cv::Vec3b>>> toApply;
    for (int x = 0; x < image.size().width; x += 1) {
        for (int y = 0; y < image.size().height; y += 1) {
            auto center = cv::Point(x, y);
            auto neighborhood = neighbors(nbr, center, 0, 0, image.size().width - 1, image.size().height - 1);
            std::vector<cv::Vec3b> colors;
            for (auto &neighbor : neighborhood) {
                colors.push_back(image.at<cv::Vec3b>(neighbor.y, neighbor.x));
            }
            colors.push_back(image.at<cv::Vec3b>(center));
            toApply.push_back(std::make_pair(center, colors));
        }
    }

    for (auto &p : toApply) {
        cv::circle(image, p.first, 0, colorFn(p.second));
    }
}

void gp::applyColorFnRange(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>&)> colorFn, Neighborhood nbr, cv::Point topLeft, cv::Point bottomRight) {
    if (topLeft.x < 0 || topLeft.y < 0 || 
        topLeft.x >= image.size().width || 
        topLeft.y >= image.size().height || 
        bottomRight.x < 0 || bottomRight.y < 0 || 
        bottomRight.x > image.size().width 
        || bottomRight.y > image.size().height) {
        return;
    }
    std::vector<std::pair<cv::Point, std::vector<cv::Vec3b>>> toApply;
    for (int x = topLeft.x; x < bottomRight.x; x += 1) {
        for (int y = topLeft.y; y < bottomRight.y; y += 1) {
            auto center = cv::Point(x, y);
            auto neighborhood = neighbors(nbr, center, topLeft.x, topLeft.y, bottomRight.x - 1, bottomRight.y - 1);
            std::vector<cv::Vec3b> colors;
            for (auto &neighbor : neighborhood) {
                colors.push_back(image.at<cv::Vec3b>(neighbor));
            }
            colors.push_back(image.at<cv::Vec3b>(center));
            toApply.push_back(std::make_pair(center, colors));
        }
    }

    for (auto &p : toApply) {
        cv::circle(image, p.first, 0, colorFn(p.second));
    }
}

void gp::applyColorFnRecursive(cv::Mat &image, std::function<cv::Vec3b(std::vector<cv::Vec3b>&)> colorFn, Neighborhood nbr, int quadrant) {
    if (image.size().width <= 2 || image.size().height <= 2) {
        applyColorFn(image, colorFn, nbr);
        return;
    }

    auto quadrants = splitQuadrantsRef(image);

    applyColorFn(quadrants[quadrant], colorFn, nbr);

    switch (quadrant) {
        case 0:
            applyColorFnRecursive(quadrants[0], colorFn, nbr, 0);
            applyColorFnRecursive(quadrants[1], colorFn, nbr, 1);
            applyColorFnRecursive(quadrants[2], colorFn, nbr, 2);
            applyColorFnRecursive(quadrants[3], colorFn, nbr, 3);

            applyColorFn(quadrants[1], colorFn, nbr);
            applyColorFn(quadrants[2], colorFn, nbr);
            applyColorFn(quadrants[3], colorFn, nbr);
            break;
        case 1: 
            applyColorFnRecursive(quadrants[0], colorFn, nbr, 0);
            applyColorFnRecursive(quadrants[2], colorFn, nbr, 2);
            applyColorFnRecursive(quadrants[3], colorFn, nbr, 3);

            applyColorFn(quadrants[0], colorFn, nbr);
            applyColorFn(quadrants[2], colorFn, nbr);
            applyColorFn(quadrants[3], colorFn, nbr);
            break;
        case 2:
            applyColorFnRecursive(quadrants[1], colorFn, nbr, 1);
            applyColorFnRecursive(quadrants[0], colorFn, nbr, 0);
            applyColorFnRecursive(quadrants[3], colorFn, nbr, 3);

            applyColorFn(quadrants[1], colorFn, nbr);
            applyColorFn(quadrants[0], colorFn, nbr);
            applyColorFn(quadrants[3], colorFn, nbr);
            break;
        case 3:
            applyColorFnRecursive(quadrants[1], colorFn, nbr, 1);
            applyColorFnRecursive(quadrants[2], colorFn, nbr, 2);
            applyColorFnRecursive(quadrants[0], colorFn, nbr, 0);

            applyColorFn(quadrants[1], colorFn, nbr);
            applyColorFn(quadrants[2], colorFn, nbr);
            applyColorFn(quadrants[0], colorFn, nbr);
            break;
    }
}

cv::Vec3b gp::remove_replace_min(
        std::vector<cv::Vec3b> &colors, 
        int channel, 
        int threshold, 
        int replacement
) {
    auto toAdd = static_cast<unsigned char>(
            static_cast<float>(replacement) * 
            (static_cast<float>(replacement) / threshold)
    );

    auto avg = avgColor(colors);
    auto selected = colors[0];
    for (auto &color : colors) {
        if (color[channel] < selected[channel]) {
            selected = ((color * 2) + selected) / 3.f;
        }
    }


    if (selected[channel] <= threshold) {
        switch (channel) {
        case 0:
            return cv::Vec3b(selected[0] + toAdd, selected[1], replacement);
        case 1:
            return cv::Vec3b(selected[0], selected[1] + toAdd, replacement);
        case 2:
            return cv::Vec3b(selected[0], replacement, selected[2] + toAdd);
        default:
            return selected;
        }
    }

    else {
        switch (channel) {
        case 0:
            return (cv::Vec3b(
                    replacement, 
                    selected[1] - toAdd, 
                    selected[2] + toAdd 
            ));
        case 1:
            return (cv::Vec3b(
                    selected[0] + toAdd, 
                    replacement, 
                    selected[2] - toAdd 
            ));
        case 2:
            return (cv::Vec3b(
                    selected[0] + toAdd, 
                    selected[1] - toAdd, 
                    replacement 
            ));
        default:
            return selected;
        }
    }
}

cv::Vec3b gp::remove_replace_max(
        std::vector<cv::Vec3b> &colors, 
        int channel, 
        int threshold, 
        int replacement
) {
    auto toAdd = static_cast<unsigned char>(
            static_cast<float>(replacement) * 
            (static_cast<float>(replacement) / threshold)
    );

    auto avg = avgColor(colors);
    auto selected = colors[0];
    for (auto &color : colors) {
        if (color[channel] > selected[channel]) {
            selected = (color + selected) / 2;
        }
    }


    if (selected[channel] >= threshold) {
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
            return cv::Vec3b(
                    selected[0] + toAdd, 
                    selected[1], 
                    selected[2] - toAdd
            );
        case 1:
            return cv::Vec3b(
                    selected[0] - toAdd, 
                    selected[1] + toAdd, 
                    selected[2]
            );
        case 2:
            return cv::Vec3b(
                    selected[0], 
                    selected[1] - toAdd, 
                    selected[2] + toAdd 
            );
        default:
            return selected;
        }
    }
}

cv::Vec3b gp::avgColor(std::vector<cv::Vec3b> &colors) {
    auto color = cv::Vec3b(0, 0, 0);

    for (auto &c : colors){
        color += c;
    }

    color /= static_cast<float>(colors.size());

    return color;
}

cv::Vec3b gp::spherical(std::vector<cv::Vec3b> &colors) {
    auto avg = gp::avgColor(colors);
    auto color = avg;

    color[0] += color[0] * static_cast<unsigned char>(
        static_cast<float>(color[0]) * 
        (std::sin((M_PI * 2.f * color[0]) / 255.f))
    );
    
    color[1] += color[1] * static_cast<unsigned char>(
        static_cast<float>(color[1]) * 
        (std::cos((M_PI * 2.f * color[1]) / 255.f))
    );

    color[2] -= color[2] * static_cast<unsigned char>(
        static_cast<float>(color[2]) * 
        (std::sin((M_PI * 2.f * color[2]) / 255.f))
    );

    return color;
}

int gp::compareIntensity(cv::Vec3b l, cv::Vec3b r) {
    auto lIntensity = l[0] + l[1] + l[2];
    auto rIntensity = r[0] + r[1] + r[2];

    if (lIntensity < rIntensity) {
        return -1;
    }
    else if (lIntensity == rIntensity) {
        return 0;
    }

    else {
        return 1;
    }
}

void gp::swapPixels(cv::Mat &image, cv::Point l, cv::Point r) {
    auto temp = image.at<cv::Vec3b>(l);
    cv::circle(image, l, 0, cv::Vec3b(0, 0, 0));
    cv::circle(image, l, 0, image.at<cv::Vec3b>(r));

    cv::circle(image, r, 0, cv::Vec3b(0, 0, 0));
    cv::circle(image, r, 0, temp);
}

int gp::partitionRow(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int xMin, int xMax, int y) {
    std::random_device rd; 
    std::mt19937 gen(rd());
    auto pX = gen() % image.size().width;

    auto pivot = image.at<cv::Vec3b>(y, pX);
    swapPixels(image, cv::Point(xMin, y), cv::Point(pX, y));

    auto left = xMin + 1;
    auto right = xMax;

    while (left < right) {
        while (left < xMax && colorCmp(image.at<cv::Vec3b>(y, left), pivot) < 0) {
            left += 1;
        }

        while (right > xMin && colorCmp(image.at<cv::Vec3b>(y, right), pivot) >= 0) {
            right -= 1;
        }

        swapPixels(image, cv::Point(left, y), cv::Point(right, y));
    }

    swapPixels(image, cv::Point(left, y), cv::Point(right, y));

    swapPixels(image, cv::Point(xMin, y), cv::Point(right, y));

    return right;
}

int gp::partitionCol(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int yMin, int yMax, int x) {
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

void gp::quicksortRow(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int xMin, int xMax, int y) {
    if (xMin < xMax) {
        auto p = partitionRow(image, colorCmp, xMin, xMax, y);
        quicksortRow(image, colorCmp, xMin, p - 1, y);
        quicksortRow(image, colorCmp, p + 1, xMax, y);
    }
}

void gp::quicksortCol(cv::Mat &image, std::function<int(cv::Vec3b, cv::Vec3b)> colorCmp, int yMin, int yMax, int x) {
    if (yMin < yMax) {
        auto p = partitionRow(image, colorCmp, yMin, yMax, x);
        quicksortRow(image, colorCmp, yMin, p - 1, x);
        quicksortRow(image, colorCmp, p + 1, yMax, x);
    }
}
