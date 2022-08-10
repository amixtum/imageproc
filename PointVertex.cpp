#include "PointVertex.h"

PointVertex::PointVertex() : _point(cv::Point2i(0, 0)), _color(cv::Vec3b(0, 0, 0)) {
    std::cout << "In default constructor for PointVertex" << std::endl;
}

PointVertex::PointVertex(cv::Point2i point, cv::Vec3b color) : _point(point), _color(color) { }

PointVertex::PointVertex(PointVertex &copyFrom) : _point(copyFrom.point()), _color(copyFrom.color()) { }

PointVertex::PointVertex(const PointVertex &copyFrom) : _point(copyFrom._point), _color(copyFrom._color) { }

cv::Point2i PointVertex::point() const {
    return _point;
}

int PointVertex::x() const {
    return this->_point.x;
}

int PointVertex::y() const {
    return this->_point.y;
}

cv::Vec3b PointVertex::color() const {
    return this->_color;
}

unsigned char PointVertex::blue() const {
    return this->_color[0];
}

unsigned char PointVertex::green() const {
    return this->_color[1];
}

unsigned char PointVertex::red() const {
    return this->_color[2];
}

void PointVertex::setPoint(cv::Point point) {
    this->_point = point;
}

void PointVertex::setColor(cv::Vec3b color) {
    this->_color = color;
}

void PointVertex::setBlue(unsigned char blue) {
    this->_color = cv::Vec3b(blue, this->_color[1], this->_color[2]);
}

void PointVertex::setGreen(unsigned char green) {
    this->_color = cv::Vec3b(this->_color[0], green, this->_color[2]);
}

void PointVertex::setRed(unsigned char red) {
    this->_color = cv::Vec3b(this->_color[0], this->_color[1], red);
}

PointVertex &PointVertex::operator=(const PointVertex &rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }

    this->setPoint(rhs.point());
    this->setColor(rhs.color());

    return *this;
}

bool PointVertex::operator==(const PointVertex &rhs) const {
    return _point.x == rhs.x() && _point.y == rhs.y();
}

bool PointVertex::operator!=(const PointVertex &rhs) const {
    return _point.x != rhs.x() || _point.y != rhs.y();
}
