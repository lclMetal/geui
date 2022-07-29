Actor *getTile(long index);
void updateIndexBounds(long *low, long *hi, long val);
int calculateAnimpos(short w, short h, short i, short j);

Actor *getTile(long index)
{
    return gc2("a_gui", index);
}

void updateIndexBounds(long *low, long *hi, long val)
{
    if (*low == -1)
        *low = val;
    if (*hi < val)
        *hi = val;
}

int calculateAnimpos(short w, short h, short i, short j)
{
    short pw = (i > 0) + (i == w - 1); // column 0, 1 or 2
    short ph = (j > 0) + (j == h - 1); // row 0, 1 or 2

    // Array of possible outcomes:
    // 0, 1, 2,
    // 3, 4, 5,
    // 6, 7, 8

    return ph * 3 + pw;

    // The values given to different parts of a window:
    // 0, 1, 1, 1, 2,
    // 3, 4, 4, 4, 5,
    // 3, 4, 4, 4, 5,
    // 6, 7, 7, 7, 8
}
