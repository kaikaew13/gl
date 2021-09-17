#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "gl.h"

int main(void)
{
    Canvas *c = new_canvas();
    // bottom flat tri passed /
    // std::vector<double> v1 = {75, 10}, v2 = {50, 50}, v3 = {100, 50};

    // c->draw_line(v1[0], v1[1], v2[0], v2[1]);
    // c->draw_line(v1[0], v1[1], v3[0], v3[1]);
    // c->draw_line(v2[0], v2[1], v3[0], v3[1]);
    // c->fillBottomFlatTri(v1, v2, v3);
    // std::cout << c->string();

    // top flat tri passed /
    // std::vector<double> v1 = {75, 10}, v2 = {100, 10}, v3 = {100, 50};

    // c->draw_line(v1[0], v1[1], v2[0], v2[1]);
    // c->draw_line(v1[0], v1[1], v3[0], v3[1]);
    // c->draw_line(v2[0], v2[1], v3[0], v3[1]);

    // c->fillTopFlatTri(v1, v2, v3);
    // std::cout << c->string();

    std::vector<double> v1 = {75, 10}, v2 = {100, 30}, v3 = {100, 50};

    c->draw_line(v1[0], v1[1], v2[0], v2[1]);
    c->draw_line(v1[0], v1[1], v3[0], v3[1]);
    c->draw_line(v2[0], v2[1], v3[0], v3[1]);

    std::cout << c->string();

    c->fill_triangle(v1, v2, v3);
    std::cout << c->string();
}