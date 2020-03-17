#include <ncurses.h>
#include <math.h>
#include <vector>

#define CELL_CHAR "@"

typedef struct point
{
    float x;
    float y;

    point(float x_cord = 0, float y_cord = 0)
    {
        x = x_cord;
        y = y_cord;
    }

    void translate(float x_diff, float y_diff)
    {
        x += x_diff;
        y += y_diff;
    }

    void draw()
    {
        move(floor(y), floor(2 * x) );
        printw(CELL_CHAR);
    }
} point;

inline float findSlope(point p1, point p2)
{
    if (p1.x == p2.x)
    {
        return -1;
    }
    return (p2.y - p1.y)/ (p2.x - p1.x);
}

inline float findDistance(point p1, point p2)
{
    return sqrt(pow((p1.x - p2.x), 2) + pow(p1.y - p2.y, 2) );
}

void drawColumn(float x, float y1, float y2)
{
    int bottom_y;
    int top_y;

    if (y1 < y2)
    {
        bottom_y = floor(y1);
        top_y = floor(y2);
    } else
    {
        bottom_y = floor(y2);
        top_y = floor(y1);
    }
    
    for (int i = bottom_y; i <= top_y; i++)
    {
        move(i, 2 * x);
        printw(CELL_CHAR);
    }
}

float columnCeil(float x)
{
    if (x - floor(x) == 0)
    {
        return x + 0.5;
    }

    float ceiling = ceil(x);

    if (x > ceiling - 0.5)
    {
        return ceiling;
    } else
    {
        return ceiling - 0.5;
    }
}

float columnFloor(float x)
{
    return columnCeil(x) - 0.5;
}

void drawLine(point p1, point p2)
{
    p1.draw();
    p2.draw();

    // for every column between p1 and p2, find where the line interects the left and right boundaries of that column and fill the gap.

    point left_p;
    point right_p;
    float slope = findSlope(p1, p2);

    if (p1.x < p2.x)
    {
        left_p = p1;
        right_p = p2;
    } else if (columnFloor(p1.x) == columnFloor(p2.x) ) // something wrong here?
    {
        drawColumn(p1.x, p1.y, p2.y);
        return;
    } else
    {
        left_p = p2;
        right_p = p1;
    }

    // draw left and rightmost columns first

    float lBoundary = left_p.y + (columnCeil(left_p.x) - left_p.x) * slope;
    drawColumn(left_p.x, left_p.y, lBoundary);

    float rBoundary = right_p.y + (columnFloor(right_p.x) - right_p.x) * slope;
    drawColumn(right_p.x, right_p.y, rBoundary);

    int columnGap = floor(2 * right_p.x) - floor(2 * left_p.x) - 1;
    float columnIncrement = (rBoundary - lBoundary) / columnGap;

    float previous_y = lBoundary;
    float next_y;
    for(int col = 1; col <= columnGap; col += 1)
    {
        float x = left_p.x + col * 0.5;
        next_y = previous_y + columnIncrement;
        drawColumn(x, previous_y, next_y);
        previous_y = next_y;
    }
}

typedef struct snowflake
{
    std::vector<point> points;

    snowflake(point p1, point p2) // support reversal?
    {
        points.push_back(p1);
        points.push_back(p2);
    }

    void reset(point p1, point p2)
    {
        points.clear();
        points.push_back(p1);
        points.push_back(p2);
    }

    void iterate()
    {
        std::vector<point> new_points;

        for (int i = 0; i < points.size() - 1; i ++)
        {
            point p1 = points[i];
            point p2 = points[i + 1];
            float slope = findSlope(p1, p2);
            float length = findDistance(p1, p2) / 3;
            float h = sqrt(pow(length, 2) - pow(length / 2, 2) );
            
            new_points.push_back(p1);
            new_points.push_back(point((2 * p1.x + p2.x) / 3, (2 * p1.y + p2.y) / 3) );

            if (slope == -1)
            {
                // ambiguous, what to do?
            } else if (slope == 0)
            {
                new_points.push_back(point((p1.x + p2.x) / 2, p1.y - h) );
            }else
            {
                float perp = (-1/slope);
                float yComp = (perp * h) / sqrt(pow(perp, 2) + 1);
                float xComp = yComp * (1 / perp);
                bool inverted = perp > 0;
                
                if ((inverted && p2.x > p1.x) || (!inverted && p1.x > p2.x) )
                {
                    new_points.push_back(point((p1.x + p2.x) / 2 - xComp, (p1.y + p2.y) / 2 - yComp) );
                } else
                {
                    new_points.push_back(point((p1.x + p2.x) / 2 + xComp, (p1.y + p2.y) / 2 + yComp) );
                }
            }

            new_points.push_back(point((2 * p2.x + p1.x) / 3, (2 * p2.y + p1.y) / 3) );
            new_points.push_back(p2);
        }

        points = new_points;
    }

    void draw()
    {
        for (int i = 0; i < points.size() - 1; i ++)
        {
            point p1 = points[i];
            point p2 = points[i + 1];
            drawLine(p1, p2);
        }
    }
} snowflake;

int main()
{
    initscr();
    noecho();
    keypad(stdscr, true);

    int iterations = 0;
    point start(5, 30);
    point p(95, 55);

    snowflake koch(start, p);
    koch.draw();
    refresh();

    while (true)
    {
        int c = getch();
        if (c == 119) // "W"
        {
            p.translate(0, -1);
            koch.reset(start, p);
            for (int i = 0; i < iterations; i ++)
            {
                koch.iterate();
            }
        }else if (c == 97) // "A"
        {
            p.translate(-1, 0);
            koch.reset(start, p);
            for (int i = 0; i < iterations; i ++)
            {
                koch.iterate();
            }
        }else if (c == 115) // "S"
        {
            p.translate(0, 1);
            koch.reset(start, p);
            for (int i = 0; i < iterations; i ++)
            {
                koch.iterate();
            }
        }else if (c == 100) // "D"
        {
            p.translate(1, 0);
            koch.reset(start, p);
            for (int i = 0; i < iterations; i ++)
            {
                koch.iterate();
            }
        } else if (c == KEY_LEFT)
        {
            if (iterations > 0)
            {
                iterations -= 1;
                koch.reset(start, p);
                for (int i = 0; i < iterations; i ++)
                {
                    koch.iterate();
                }
            }
        } else if (c == KEY_RIGHT)
        {
            iterations += 1;
            koch.iterate();
        } else if (c == 113) // "Q"
        {
            break;
        }

        clear();
        
        koch.draw();
        refresh();
    }

    endwin();
    return 0;
}
