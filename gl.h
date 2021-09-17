#ifndef GL_H
#define GL_H

#include <iostream>
#include <codecvt>
#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>

int pixel_map[4][2] = {
    {0x1, 0x8},
    {0x2, 0x10},
    {0x4, 0x20},
    {0x40, 0x80}};

int braille_char_offset = 0x2800;

double radians(double d)
{
    return d * (M_PI / 180);
}

int get_pixel(int y, int x)
{
    int cy, cx;
    if (y >= 0)
    {
        cy = y % 4;
    }
    else
    {
        cy = 3 + ((y + 1) % 4);
    }

    if (x >= 0)
    {
        cx = x % 2;
    }
    else
    {
        cx = 1 + ((x + 1) % 2);
    }

    return pixel_map[cy][cx];
}

typedef struct Canvas
{
private:
    std::string line_ending;
    int width, height;

public:
    std::map<int, std::map<int, std::pair<int, short>>> chars;
    Canvas() : line_ending("\n")
    {
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        this->width = ws.ws_col;
        this->height = ws.ws_row;
    }

    int get_width()
    {
        return this->width;
    }

    int get_height()
    {
        return this->height;
    }

    void clear()
    {
        this->chars.clear();
        system("clear");
    }

    int max_y()
    {
        int max = 0;
        for (auto i : this->chars)
        {
            if (i.first > max)
            {
                max = i.first;
            }
        }

        return max * 4;
    }

    int min_y()
    {
        int min = 0;
        for (auto i : this->chars)
        {
            if (i.first < min)
            {
                min = i.first;
            }
        }

        return min * 4;
    }

    int max_x()
    {
        int max = 0;
        for (auto i : this->chars)
        {
            for (auto j : i.second)
            {
                if (j.first > max)
                {
                    max = j.first;
                }
            }
        }

        return max * 2;
    }

    int min_x()
    {
        int min = 0;
        for (auto i : this->chars)
        {
            for (auto j : i.second)
            {
                if (j.first < min)
                {
                    min = j.first;
                }
            }
        }

        return min * 2;
    }

    int get_pos_x(int x)
    {
        return x / 2;
    }

    int get_pos_y(int y)
    {
        return y / 4;
    }

    void set(int x, int y, short col)
    {
        int px = this->get_pos_x(x), py = this->get_pos_y(y);
        // if (this->chars[py].size() == 0)
        // {
        //     this->chars[py].clear();
        // }
        int val = this->chars[py][px].first;
        int mapv = get_pixel(y, x);
        this->chars[py][px].first = (val | mapv);
        this->chars[py][px].second = col;
    }

    void unset(int x, int y)
    {
        int px = this->get_pos_x(x), py = this->get_pos_y(y);
        x = abs(x);
        y = abs(y);
        // if (this->chars[py].size() == 0)
        // {
        //     this->chars[py].clear();
        // }

        this->chars[py][px].first = this->chars[py][px].first & ~get_pixel(y, x);
    }

    void toggle(int x, int y, short col)
    {
        int px = this->get_pos_x(x), py = this->get_pos_y(y);
        if ((this->chars[py][px].first & get_pixel(y, x)))
        {
            // this->unset(x, y);
        }
        else
        {
            this->set(x, y, col);
        }
    }

    void setText(int x, int y, std::string text)
    {
        x /= 2;
        y /= 4;
        for (int i = 0; i < text.size(); i++)
        {
            this->chars[y][x + i].first = (int)text[i] - braille_char_offset;
        }
    }

    bool get(int x, int y)
    {
        int dot_index = pixel_map[y % 4][x % 2];
        x /= 2;
        y /= 4;
        int ch = this->chars[y][x].first;
        return (ch & dot_index) != 0;
    }

    std::string get_screen_wchar(int x, int y)
    {
        std::wstring ws;
        ws += (wchar_t)(braille_char_offset + this->chars[y][x].first);
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
        std::string s = cv.to_bytes(ws);
        return s;
    }

    std::string get_wchar(int x, int y)
    {
        return this->get_screen_wchar(x / 4, y / 4);
    }

    std::string getAnsi(int col)
    {
        char c[15];
        sprintf(c, "\033[38;5;%dm", col);
        return std::string(c);
    }

    std::vector<std::string> rows(int min_x, int min_y, int max_x, int max_y)
    {
        int minrow = min_y / 4, maxrow = max_y / 4;
        int mincol = min_x / 2, maxcol = max_x / 2;

        std::vector<std::string> ret;
        for (int rownum = minrow; rownum < (maxrow + 1); rownum++)
        {
            std::string row = "";
            for (int colnum = mincol; colnum < (maxcol + 1); colnum++)
            {
                row += getAnsi(this->chars[rownum][colnum].second);
                int ch = this->chars[rownum][colnum].first;
                std::wstring ws;
                ws += (wchar_t)(braille_char_offset + ch);
                std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
                row += cv.to_bytes(ws);
                row += "\033[m";
            }

            ret.push_back(row);
        }

        return ret;
    }

    std::string frame(int min_x, int min_y, int max_x, int max_y)
    {
        std::string ret = "";
        for (auto i : this->rows(min_x, min_y, max_x, max_y))
        {
            ret += i;
            ret += this->line_ending;
        }

        return ret;
    }

    std::string string()
    {
        return this->frame(this->min_x(), this->min_y(), this->max_x(), this->max_y());
    }

    void draw_line(double x1, double y1, double x2, double y2, short col = 0x000F)
    {
        double xdiff = abs(x1 - x2), ydiff = abs(y1 - y2);

        double xdir, ydir;
        if (x1 <= x2)
        {
            xdir = 1;
        }
        else
        {
            xdir = -1;
        }

        if (y1 <= y2)
        {
            ydir = 1;
        }
        else
        {
            ydir = -1;
        }

        double r = std::max(xdiff, ydiff);

        for (int i = 0; i < round(r) + 1; i++)
        {
            double x = x1, y = y1;
            if (ydiff)
            {
                y += ((double)i * ydiff) / (r * ydir);
            }

            if (xdiff)
            {
                x += ((double)i * xdiff) / (r * xdir);
            }

            this->toggle(round(x), round(y), col);
        }
    }

    void fillBottomFlatTri(std::vector<double> &v1, std::vector<double> &v2, std::vector<double> &v3, short col)
    {
        double invslope1 = (v2[0] - v1[0]) / (v2[1] - v1[1]);
        double invslope2 = (v3[0] - v1[0]) / (v3[1] - v1[1]);

        double curx1 = v1[0], curx2 = v1[0];

        for (int scanlineY = round(v1[1]); scanlineY <= round(v2[1]); scanlineY++)
        {
            draw_line(curx1, scanlineY, curx2, scanlineY, col);
            curx1 += invslope1;
            curx2 += invslope2;
        }
    }

    void fillTopFlatTri(std::vector<double> &v1, std::vector<double> &v2, std::vector<double> &v3, short col)
    {
        double invslope1 = (v3[0] - v1[0]) / (v3[1] - v1[1]);
        double invslope2 = (v3[0] - v2[0]) / (v3[1] - v2[1]);

        double curx1 = v3[0], curx2 = v3[0];

        for (int scanlineY = round(v3[1]); scanlineY > round(v1[1]); scanlineY--)
        {
            draw_line(curx1, scanlineY, curx2, scanlineY, col);
            curx1 -= invslope1;
            curx2 -= invslope2;
        }
    }

    void fill_triangle(std::vector<double> &v1, std::vector<double> &v2, std::vector<double> &v3, short col = 255)
    {

        // we know that y1 <= y2 <= y3
        // if y2 == y3
        if (v2[1] == v3[1])
        {
            fillBottomFlatTri(v1, v2, v3, col);
        }
        // if y1 == y2
        else if (v1[1] == v2[1])
        {
            fillTopFlatTri(v1, v2, v3, col);
        }
        // else split tri into topflat and bottom flat tris
        else
        {
            // double x4 = v[0][1] + ((double)(v[1][0] - v[0][0]) / (double)(v[2][0] - v[0][0])) * (v[2][1] - v[0][1]);
            // double y4 = v[1][0];
            double x4 = v1[0] + ((double)(v2[1] - v1[1]) / (double)(v3[1] - v1[1])) * (v3[0] - v1[0]);
            double y4 = v2[1];

            std::vector<double> v4 = {x4, y4};

            fillBottomFlatTri(v1, v2, v4, col);
            fillTopFlatTri(v2, v4, v3, col);
        }
    }

    void draw_polygon(double center_x, double center_y, double sides, double radius)
    {
        double degree = 360 / sides;
        for (int n = 0; n < (int)sides; n++)
        {
            double a = (double)n * degree, b = (double)(n + 1) * degree;

            double x1 = (center_x + (cos(radians(a)) * (radius / 2 + 1)));
            double y1 = (center_y + (sin(radians(a)) * (radius / 2 + 1)));
            double x2 = (center_y + (sin(radians(b)) * (radius / 2 + 1)));
            double y2 = (center_y + (sin(radians(b)) * (radius / 2 + 1)));

            this->draw_line(x1, y1, x2, y2);
        }
    }

} Canvas;

Canvas *new_canvas()
{
    Canvas *c = new Canvas();
    c->clear();
    return c;
}

#endif
