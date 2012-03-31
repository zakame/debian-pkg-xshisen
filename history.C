#include "components.h"

History::History(int c)
{
    piece1 = new Point[c];
    piece2 = new Point[c];
    via1   = new Point[c];
    via2   = new Point[c];
    count  = 0;
}

History::~History(void)
{
    delete[] piece1;
    delete[] piece2;
    delete[] via1;
    delete[] via2;
}

void
History::ChangeHistorySize(int c)
{
    delete[] piece1;
    delete[] piece2;
    delete[] via1;
    delete[] via2;
    piece1 = new Point[c];
    piece2 = new Point[c];
    via1   = new Point[c];
    via2   = new Point[c];
    count  = 0;
}

void
History::AddHistory(const Point &a, const Point &b, const Point &c, const Point &d)
{
    piece1[count] = a;   // Pai 1
    piece2[count] = b;   // Pai 2
    via1[count] = c;     // Via point 1
    via2[count] = d;     // Via point 2
    count++;
}

int
History::BackHistory(void)
{
    if (count > 0) {
        count--;
        return 1;
    }
    else
        return 0;  // Cannot back any more
}

void
History::GetXY(int c, Point &a, Point &b)
{
    // c<0 means count back from current
    if (c < 0)
        c = count + c;
    a = piece1[c];
    b = piece2[c];
}

void
History::GetV(int c, Point &a, Point &b)
{
    // c<0 means count back from current
    if (c < 0)
        c = count + c;
    a = via1[c];
    b = via2[c];
}

void
History::ResetHistory(void)
{
    count = 0;
}
