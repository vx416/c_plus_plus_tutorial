/**
 * 練習 3: Shape Hierarchy
 *
 * 實作一個 Shape 階層，練習繼承與多型：
 *   Shape (abstract)
 *     ├── Circle
 *     ├── Rectangle
 *     └── Triangle
 *
 * Shape 有 pure virtual area() 和 name()。
 * 每個子類別實作自己的 area() 計算。
 *
 * 最後寫一個 total_area() 函式，接受 vector<unique_ptr<Shape>>，
 * 回傳所有形狀的總面積。
 *
 * 重點練習：pure virtual、override、多型容器
 */

#include <vector>
#include <memory>
#include <cmath>
#include <cassert>
#include <iostream>
using namespace std;

class Shape {
public:
    // TODO: virtual destructor
    // TODO: pure virtual area() 和 name()
};

class Circle /* : ...*/ {
    // TODO: constructor 接受半徑
    // TODO: override area() (公式: π * r²)
    // TODO: override name() 回傳 "Circle"
};

class Rectangle /* : ... */ {
    // TODO: constructor 接受寬高
    // TODO: override area() 和 name()
};

class Triangle /* : ... */ {
    // TODO: constructor 接受底和高
    // TODO: override area() (公式: 0.5 * base * height) 和 name()
};

// TODO: total_area 接受 shapes 並回傳總面積
double total_area(const vector<unique_ptr<Shape>>& shapes) {
    (void)shapes;
    return 0;
}

int main() {
    // vector<unique_ptr<Shape>> shapes;
    // shapes.push_back(make_unique<Circle>(1));       // area = π
    // shapes.push_back(make_unique<Rectangle>(2, 3)); // area = 6
    // shapes.push_back(make_unique<Triangle>(4, 5));  // area = 10
    //
    // double expected = M_PI + 6 + 10;
    // assert(abs(total_area(shapes) - expected) < 1e-9);
    //
    // assert(shapes[0]->name() == "Circle");
    // assert(shapes[1]->name() == "Rectangle");
    // assert(shapes[2]->name() == "Triangle");

    cout << "ex03 passed!" << endl;
    return 0;
}
