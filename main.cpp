#include <iostream>
#include <vector>
#include <ctime>
#include <unistd.h>
#include <algorithm>

#include "gl.h"

struct vec3d
{
    double x, y, z;
};

struct tri
{
    vec3d p[3];
    short col;
};

struct mesh
{
    std::vector<tri> tris;
};

struct mat4x4
{
    double m[4][4] = {0};
};

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
}

short GetColour(float lum)
{
    short bg_col, fg_col;
    wchar_t sym;
    int pixel_bw = (int)(20.0f * lum);
    switch (pixel_bw)
    {
    case 0:
        return 232;

    case 1:
        return 233;
        // break;
    case 2:
        return 234;
        break;
    case 3:
        return 235;
        // break;
    case 4:
        return 236;
    case 5:
        return 237;
    case 6:
        return 238;
    case 7:
        return 239;
    case 8:
        return 240;
    case 9:
        return 241;
    case 10:
        return 242;
    case 11:
        return 243;
    case 12:
        return 244;
    case 13:
        return 245;
    case 14:
        return 246;
    case 15:
        return 247;
    case 16:
        return 248;
    case 17:
        return 249;
    case 18:
        return 250;
    case 19:
        return 251;
    }
    return 232;
}

bool cmp(std::vector<double> &v1, std::vector<double> &v2)
{
    return v1[1] < v2[1];
}

void drawTri(double x1, double y1, double x2, double y2, double x3, double y3, Canvas *c)
{
    c->draw_line(x1, y1, x2, y2);
    c->draw_line(x2, y2, x3, y3);
    c->draw_line(x3, y3, x1, y1);
}

int main()
{
    Canvas *c = new_canvas();
    mesh mCube;
    mat4x4 matProj, matRotX, matRotZ;
    // camera for part 3
    vec3d camera;
    time_t startTime = time(0);

    // random height and width scaling
    double height = c->get_height() * 3.0f, width = 1.5f * c->get_width();
    mCube.tris = {
        {0, 0, 0, 0, 1, 0, 1, 1, 0},
        {0, 0, 0, 1, 1, 0, 1, 0, 0},
        {1, 0, 0, 1, 1, 0, 1, 1, 1},
        {1, 0, 0, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1, 1, 0, 0, 1},
        {0, 0, 1, 0, 1, 1, 0, 1, 0},
        {0, 0, 1, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 1, 1, 1, 1},
        {0, 1, 0, 1, 1, 1, 1, 1, 0},
        {1, 0, 1, 0, 0, 1, 0, 0, 0},
        {1, 0, 1, 0, 0, 0, 1, 0, 0}};

    double inc = 0;
    double fNear = 0.1f, fFar = 1000.0f, fFov = 90.0f, fAspectRatio = (double)height / width;
    double fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    while (1)
    {
        time_t now = time(0);
        // double fTheta = 1.0f * (now - startTime);
        double fTheta = 1.0f + inc;

        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        for (auto tr : mCube.tris)
        {
            tri triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate in Z-Axis
            MultiplyMatrixVector(tr.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tr.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tr.p[2], triRotatedZ.p[2], matRotZ);

            // Rotate in X-Axis
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            triTranslated = triRotatedZX;
            triTranslated.p[0].z += 3.0f;
            triTranslated.p[1].z += 3.0f;
            triTranslated.p[2].z += 3.0f;

            vec3d normal, line1, line2;
            line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
            line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            // normalized normal into unit vector
            double l = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= l;
            normal.y /= l;
            normal.z /= l;

            double dot_product = normal.x * (triTranslated.p[0].x - camera.x) + normal.y * (triTranslated.p[0].y - camera.y) + normal.z * (triTranslated.p[0].z - camera.z);
            if (dot_product < 0)
            {
                // illumination
                vec3d light_dir = {0.0f, 0.0f, -1.0f};
                double l = sqrt(light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z);
                light_dir.x /= l;
                light_dir.y /= l;
                light_dir.z /= l;

                dot_product = normal.x * light_dir.x + normal.y * light_dir.y + normal.z * light_dir.z;

                short col = GetColour(dot_product);
                triTranslated.col = col;

                MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
                MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
                MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

                triProjected.col = triTranslated.col;

                triProjected.p[0].x += 1.0f;
                triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f;
                triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f;
                triProjected.p[2].y += 1.0f;

                triProjected.p[0].x *= 0.5f * width;
                triProjected.p[1].x *= 0.5f * width;
                triProjected.p[2].x *= 0.5f * width;
                triProjected.p[0].y *= 0.5f * height;
                triProjected.p[1].y *= 0.5f * height;
                triProjected.p[2].y *= 0.5f * height;

                std::vector<double> v1 = {triProjected.p[0].x, triProjected.p[0].y};
                std::vector<double> v2 = {triProjected.p[1].x, triProjected.p[1].y};
                std::vector<double> v3 = {triProjected.p[2].x, triProjected.p[2].y};
                std::vector<std::vector<double>> V = {v1, v2, v3};
                std::sort(V.begin(), V.end(), cmp);
                c->fill_triangle(V[0], V[1], V[2], triProjected.col);
                // c->fill_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y);
                // drawTri(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, c);
            }
        }
        // c->draw_line(0, 0, 100, 100);

        std::cout << c->string();
        usleep(100000);
        c->clear();
        inc += 0.1f;
    }
    // c->draw_line(0, 0, 100, 100);
    // std::cout << c->string();
}