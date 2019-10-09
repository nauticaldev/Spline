#include <iostream>
#include <cassert>
#include <vector>
#include "Spline.hpp"
#include <string>

using Real = double;

//Minimum requirement example vector class. 
struct Vec3
{
  Real x, y, z;
  Vec3 operator+(const Vec3& v){ return Vec3{ x+v.x,y+v.y,z+v.z}; }
  Vec3 operator-(const Vec3& v){ return Vec3{ x-v.x,y-v.y,z-v.z}; }
  template <class T> Vec3 operator*(const T& s){return Vec3{ s * x,s * y,s * z };}
};

int main(int argc, char* argv[])
{
  Real t0 = 0.0;
  Real t1 = 1.0;
  Real samples = argc > 1 ? Real(std::stoi(argv[1])) : 100.0;
  Real delta = (t1 - t0) / samples;
  t1 += delta;

  {
    std::vector<int> ints{ 0, 100, 0 };
    std::vector<int*> intPtrs{ ints.data(), ints.data() + 1, ints.data() + 2 };
    std::vector<float> floats{ 0.f, 100.f, 0.f };
    std::vector<float*> floatPtrs{ floats.data(), floats.data() + 1, floats.data() + 2 };
    std::vector<double> doubles{ 0.0 , 100.0 , 0.0 };

    Spline<int> IntSpline{ ints };
    Spline<int*> IntPtrSpline{ &ints };

    Spline<float> FloatSpline{ floats };

    Spline<float*> FloatPtrSpline{ &floats };
    Spline<float*> FloatPtrSpline2{ floatPtrs };


    std::cout << "\nInts:" << std::endl;
    for (Real t = t0; t < t1; t += delta)
    {
      std::cout << IntSpline.GetPoint(t) << std::endl;
    }
    assert(IntSpline.GetPoint(0.0) == IntSpline.GetPoint(1.0));

    std::cout << "\nInt Ptrs:" << std::endl;
    for (Real t = t0; t < t1; t += delta)
    {
      std::cout << IntPtrSpline.GetPoint(t) << std::endl;
    }
    assert(IntPtrSpline.GetPoint(0.0) == IntPtrSpline.GetPoint(1.0));


    std::cout << "\nFloats:" << std::endl;
    for (Real t = t0; t < t1; t += delta)
    {
      std::cout << FloatSpline.GetPoint(t) << std::endl;
    }
    assert(FloatSpline.GetPoint(0.0) == FloatSpline.GetPoint(1.0));

    std::cout << "\nFloat Ptrs:" << std::endl;
    for (Real t = t0; t < t1; t += delta)
    {
      std::cout << FloatPtrSpline.GetPoint(t) << std::endl;
    }
    assert(FloatPtrSpline.GetPoint(0.0) == FloatPtrSpline.GetPoint(1.0));

    std::cout << "\nFloat Ptrs2:" << std::endl;
    for (Real t = t0; t < t1; t += delta)
    {
      std::cout << FloatPtrSpline2.GetPoint(t) << std::endl;
    }
    assert(FloatPtrSpline2.GetPoint(0.0) == FloatPtrSpline2.GetPoint(1.0));
  }

  //Test Cases
  {
    //Usage 1 : ( Points are copied directly from a vector )

    std::vector<float> MyFloats{ 1.f, 2.f, 3.f };
    Spline<float> spline{ MyFloats }; // creates a spline through the points 1 2 and 3.
    float t = 0.5f;
    assert(spline.GetPoint(t) == 2.f); // should give 2.f
  }

  {
    //Usage 2: ( No copies are made, uses pointers to the values in the original vector, to allow hassle-free updating of control points )

    std::vector<Vec3> MyVecs{ Vec3{1.f,1.1,1.f} , Vec3{2.f,2.f,2.f}, Vec3{3.f,3.f,3.f } };
    Spline<Vec3*> spline{ &MyVecs }; // creates a spline through the 3 vectors
    Vec3 v = spline.GetPoint(0.5f);
    assert(v.x == 2.f && v.y == 2.f && v.z == 2.f); // should give a Vec3{2.f,2.f,2.f}

    MyVecs[1] = Vec3{ 5.f,5.f,5.f }; // Now we can move the original points in the vector
    v = spline.GetPoint(0.5f);
    assert(v.x == 5.f && v.y == 5.f && v.z == 5.f); // should give a Vec3{5.f,5.f,5.f}
  }

  {
    //Usage 3: ( Constructed from a vector of pointers to values )

    std::vector<int> ints{ 0, 100, 0 };
    std::vector<int*> intPtrs{ ints.data(), ints.data() + 1, ints.data() + 2 };
    Spline<int*> spline{ intPtrs }; //Creates a spline through 0, 100, 0
    float t = 0.5f;
    assert(spline.GetPoint(t) == 100); // should give 100

    int Test = 200;
    intPtrs[1] = &Test; //Now we can move around the addresses
    int out = spline.GetPoint(t);
    assert(out == 200); //should give 200  
  }
}
