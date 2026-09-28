/**
 * 練習 1: Point class
 *
 * 實作一個 2D Point class：
 *   - Constructor：接受 x, y (double)
 *   - getter: x(), y()（const 方法）
 *   - distance_to(const Point&)：計算到另一個點的歐氏距離
 *
 * 重點練習：class 宣告、const 方法、初始化列表
 */

#include <cmath>
#include <cassert>
#include <iostream>
using namespace std;

class Point {
public:
    // TODO: 建構子，儲存 x, y
    Point(double x, double y) : x_(x), y_(y) {}

    // TODO: getter，注意加 const
    double x() const { return x_; }
    double y() const { return y_; }

    // TODO: 計算到另一點的距離
    // 公式: sqrt((x1-x2)^2 + (y1-y2)^2)
    double distance_to(const Point& other) const {
        const double dx = x_ - other.x_;
        const double dy = y_ - other.y_;
        return sqrt(dx * dx + dy * dy);
    }

private:
    // TODO: 成員變數
    double x_;
    double y_;
};

int main() {
    Point p1(0, 0);
    Point p2(3, 4);

    assert(p1.x() == 0);
    assert(p1.y() == 0);
    assert(p2.x() == 3);
    assert(p2.y() == 4);

    assert(abs(p1.distance_to(p2) - 5.0) < 1e-9);
    assert(abs(p2.distance_to(p1) - 5.0) < 1e-9);
    assert(p1.distance_to(p1) == 0);

    // const 物件應該也能呼叫 getter 和 distance_to
    const Point p3(1, 1);
    assert(p3.x() == 1);
    assert(abs(p3.distance_to(p1) - sqrt(2)) < 1e-9);

    cout << "ex01 passed!" << endl;
    return 0;
}
