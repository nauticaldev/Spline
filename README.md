# Super Light-Weight, Header Only, Templated Splines in C++ 

### Tested and compiled with C++17/20 in MSVC 142, clang 6.0.0, gcc 7.4.0

## Supports: 
* Any compiler
* Incremental or vector construction
* POD, Any n-dimensonal user-defined types, Pointers to valid types (via either vector of pointers, or pointers to elements in a vector)
	

## Requirements: 
* Works with all POD types, user-defined types must define operator+() and operator-() with themselves, and operator* with any scalar type. e.g.
```	  
struct Vec2
{
  Real x, y;
  Vec2 operator+(const Vec2& v){ return Vec2{ x+v.x,y+v.y}; }
  Vec2 operator-(const Vec2& v){ return Vec2{ x-v.x,y-v.y}; }
  template<class T>Vec3 operator*(const T& s){ return Vec2{ s*x,s*y}; }
};
```

### Usage 1 : 
###### Points are copied directly from a vector 
 ```
 std::vector<float> MyFloats{ 1.f, 2.f, 3.f };
 Spline<float> spline{ MyFloats }; // creates a spline through the points 1 2 and 3.
 float t = 0.5f;
 assert(spline.GetPoint(t) == 2.f); // should give 2.f
 ```
### Usage 2:
###### No copies are made, uses pointers to the values in the original vector, to allow hassle-free updating of control points 
 ```
 std::vector<Vec3> MyVecs{ Vec3{1.f,1.1,1.f} , Vec3{2.f,2.f,2.f}, Vec3{3.f,3.f,3.f } };
 Spline<Vec3*> spline{ &MyVecs }; // creates a spline through the 3 vectors
 assert(spline.GetPoint(0.5f) == Vec3(2.f, 2.f, 2.f)); // should give a Vec3{2.f,2.f,2.f}
 
 MyVecs[1] = Vec3{ 5.f,5.f,5.f }; // Now we can move the original points in the vector
 assert(spline.GetPoint(0.5f) == Vec3(5.f, 5.f, 5.f)); // should give a Vec3{5.f,5.f,5.f}
``` 
### Usage 3: 
###### Constructed from a vector of pointers to values 
 ```
 std::vector<int> ints{ 0, 100, 0 };
 std::vector<int*> intPtrs{ ints.data(), ints.data() + 1, ints.data() + 2 };
 Spline<int*> spline{ intPtrs }; //Creates a spline through 0, 100, 0
 float t = 0.5f;
 assert(spline.GetPoint(t) == 100); // should give 100
 
 int Test = 200;
 intPtrs[1] = &Test; //Now we can move around the addresses
 int out = spline.GetPoint(t);
 assert(out == 200); //should give 200  
 ```
 
 
*** Note: Makefile is for testing purposes, compiles in both gcc and clang.
