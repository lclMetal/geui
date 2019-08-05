typedef struct LayoutStruct
{
    short row;
    short col;
    short width;
    short height;
}Layout;

void setPosition(WindowItem *this, short row, short col)
{
    if (!this) return;

    this->layout.row = row;
    this->layout.col = col;
}

short getColWidth(Window *window, short col)
{
    short width = 0;
    WindowItem *item;

    if (!window || !window->layout.col) return -1;
    if (col < 0 || col >= window->layout.col) return -2;

    for (item = window->iList; item != NULL; item = item->next)
    {
        if (item->layout.col == col && item->layout.width > width)
            width = item->layout.width;
    }

    return width;
}

short getRowHeight(Window *window, short row)
{
    short height = 0;
    WindowItem *item;

    if (!window || !window->layout.row) return -1;
    if (row < 0 || row >= window->layout.row) return -2;

    for (item = window->iList; item != NULL; item = item->next)
    {
        if (item->layout.row == row && item->layout.height > height)
            height = item->layout.height;
    }

    return height;
}

int getWindowWidth(Window *window)
{
    short col;
    int width = 0;
    int tempWidth;

    if (!window || !window->layout.col) return -1;

    for (col = 0; col < window->layout.col; col ++)
    {
        if ((tempWidth = getColWidth(window, col)) >= 0)
            width += tempWidth;
    }

    return width;
}

int getWindowHeight(Window *window)
{
    short row;
    int height = 0;
    int tempHeight;

    if (!window || !window->layout.row) return -1;

    for (row = 0; row < window->layout.row; row ++)
    {
        if ((tempHeight = getRowHeight(window, row)) >= 0)
            height += tempHeight;
    }

    return height;
}
