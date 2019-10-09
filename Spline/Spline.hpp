// Author: Jon Koelzer
// Link to this gist if you plan on using it. https://gist.github.com/nauticaldev/53b96ebfe95736d00afbdf29884b40c9
//
// Description: Light-weight, templated, header only, any-dimensional catmull-rom spline implementation.
//
// Supports: - Incremental or vector construction
//           - POD, User defined types, Pointers (via either vector of pointers, or pointers to elements in a vector)
//
// Requirements: - All types must define operator+() and operator-() with themselves, and operator* with any scalar type.
//                 e.g.
// struct Vec2
// {
//   Real x, y;
//   Vec3 operator+(const Vec2& v){ return Vec2{ x+v.x,y+v.y}; }
//   Vec3 operator-(const Vec2& v){ return Vec2{ x-v.x,y-v.y}; }
//   template<class T>Vec3 operator*(const T& s){ return Vec2{ s*x,s*y}; }
// };
//
///////////////////////////////////////////////////////////////////////////
//
// Usage 1 : ( Points are copied directly from a vector )
//  
//  std::vector<float> MyFloats{ 1.f, 2.f, 3.f };
//  Spline<float> spline{ MyFloats }; // creates a spline through the points 1 2 and 3.
//  float t = 0.5f;
//  assert(spline.GetPoint(t) == 2.f); // should give 2.f
//  
// Usage 2: ( No copies are made, uses pointers to the values in the original vector, to allow hassle-free updating of control points )
//  
//  std::vector<Vec3> MyVecs{ Vec3{1.f,1.1,1.f} , Vec3{2.f,2.f,2.f}, Vec3{3.f,3.f,3.f } };
//  Spline<Vec3*> spline{ &MyVecs }; // creates a spline through the 3 vectors
//  assert(spline.GetPoint(0.5f) == Vec3(2.f, 2.f, 2.f)); // should give a Vec3{2.f,2.f,2.f}
//  
//  MyVecs[1] = Vec3{ 5.f,5.f,5.f }; // Now we can move the original points in the vector
//  assert(spline.GetPoint(0.5f) == Vec3(5.f, 5.f, 5.f)); // should give a Vec3{5.f,5.f,5.f}
//  
// Usage 3: ( Constructed from a vector of pointers to values )
//  
//  std::vector<int> ints{ 0, 100, 0 };
//  std::vector<int*> intPtrs{ ints.data(), ints.data() + 1, ints.data() + 2 };
//  Spline<int*> spline{ intPtrs }; //Creates a spline through 0, 100, 0
//  float t = 0.5f;
//  assert(spline.GetPoint(t) == 100); // should give 100
//  
//  int Test = 200;
//  intPtrs[1] = &Test; //Now we can move around the addresses
//  int out = spline.GetPoint(t);
//  assert(out == 200); //should give 200  
//

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

