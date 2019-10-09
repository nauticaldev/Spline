// Author: Jon Koelzer
// https://github.com/nauticaldev/Spline

#pragma once
#include <memory>
#include <vector>

template<typename T>
struct Spline;

template<typename T, typename T2, bool isPtr = std::is_pointer<T>::value>
struct GetCatmullRom;

template<typename T, typename Real>
struct GetCatmullRom<T, Real, false>
{
  friend struct Spline<T>;
  using ReturnType = typename std::remove_pointer<T>::type;
private:
  T v0, v1;
  Real t, t2, t3;
  const T* points;

  GetCatmullRom(const T p[4], Real& t_) : v0(static_cast<T>((p[2] - p[0])* static_cast<Real>(0.5))), v1(static_cast<T>((p[3] - p[1])* static_cast<Real>(0.5))), t(t_), t2(t* t), t3(t2* t), points(p) {}

  operator T()
  {
    return static_cast<ReturnType>((points[1] * Real(2.) - points[2] * Real(2.) + v0 + v1) * t3 + (points[1] * Real(-3.) + points[2] * Real(3.) - v0 * Real(2.) - v1) * t2 + v0 * t + points[1]);
  }
};

template<typename T, typename Real>
struct GetCatmullRom<T, Real, true>
{
  friend struct Spline<T>;
  using ReturnType = typename std::remove_pointer<T>::type;
private:
  ReturnType v0, v1;
  Real t, t2, t3;
  const T* points;

  GetCatmullRom(const T p[4], Real& t_) : v0(static_cast<ReturnType>((*p[2] - *p[0])* static_cast<Real>(0.5f))), v1(static_cast<ReturnType>((*p[3] - *p[1])* static_cast<Real>(0.5f))), t(t_), t2(t* t), t3(t2* t), points(p) {}

  operator typename std::pointer_traits<T>::element_type()
  {
    return static_cast<ReturnType>((*points[1] * Real(2.) - *points[2] * Real(2.) + v0 + v1) * t3 + (*points[1] * Real(-3.) + *points[2] * Real(3.) - v0 * Real(2.) - v1) * t2 + v0 * t + *points[1]);
  }
};

template <typename T>
struct Spline
{
  using ReturnType = typename std::remove_pointer<T>::type;
private:
  std::vector<T>points_internal;

  template <typename Real>
  ReturnType CatmullRom(Real t, const T p[4]) { return ReturnType(GetCatmullRom<T, Real>(p, t)); }

public:
  std::vector<T>& points;

  Spline() : points(points_internal)
  {
    points.reserve(4);
  }
  Spline(size_t size) : points_internal(size), points(points_internal) {}
  Spline(std::vector<T>& pts) : points_internal(!std::is_same<T, ReturnType>::value ? std::vector<T>() : pts), points(!std::is_same<T, ReturnType>::value ? pts : points_internal) {}
  Spline(std::vector<ReturnType>* Pts) : points_internal{ Pts->size() }, points{ points_internal }
  {
    for (std::size_t i = 0; i < Pts->size(); ++i)
      points[i] = &(*Pts)[i];
  }

  void AddPoint(const T& point)
  {
    points.emplace_back(point);
  }

  void AddPoints(const std::vector<T>& newPoints)
  {
    points.reserve(points.size() + newPoints.size());
    points.insert(points.end(), newPoints.begin(), newPoints.end());
  }

  template <typename Real>
  ReturnType GetPoint(Real percent)
  {
    static_assert(std::is_floating_point<Real>::value, "Must be float or double, between 0.0 and 1.0");
    const int size = static_cast<int>(points.size());
    const Real p = percent * static_cast<Real>((size - 1));
    const int intPoint = static_cast<int>(p);
    const Real weight = p - intPoint;
    const T pointArray[4] = {
      points[intPoint == 0 ? intPoint : intPoint - 1],
      points[intPoint],
      points[intPoint > size - 2 ? size - 1 : intPoint + 1],
      points[intPoint > size - 3 ? size - 1 : intPoint + 2]
    };

    return CatmullRom(weight, pointArray);
  }

  constexpr T & operator[](unsigned index)
  {
    return points[index];
  }

};

