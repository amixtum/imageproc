#ifndef POINT_VERTEX_H_
#define POINT_VERTEX_H_

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

class PointVertex
{
public:
  PointVertex();

  PointVertex(cv::Point2i point, cv::Vec3b color);

  PointVertex(PointVertex& copyFrom);

  PointVertex(const PointVertex& copyFrom);

  cv::Point2i point() const;

  int x() const;

  int y() const;

  cv::Vec3b color() const;

  unsigned char blue() const;

  unsigned char green() const;

  unsigned char red() const;

  void setPoint(cv::Point2i point);

  void setColor(cv::Vec3b color);

  void setBlue(unsigned char blue);

  void setGreen(unsigned char green);

  void setRed(unsigned char red);

  PointVertex& operator=(const PointVertex& rhs) noexcept;

  bool operator==(const PointVertex& rhs) const;

  bool operator!=(const PointVertex& rhs) const;

private:
  cv::Point2i _point;
  cv::Vec3b _color;
};

#endif // PointVertex.h included
