void updatePanelLayout(Panel *panel);
void setPosition(WindowItem *this, short row, short col);
short getColWidth(Panel *panel, short col);
short getRowHeight(Panel *panel, short row);
short getPanelWidth(Panel *panel);
short getPanelHeight(Panel *panel);

void updatePanelLayout(Panel *panel)
{
    WindowItem *item;

    if (!panel) return;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->layout.row > panel->layout.row)
            panel->layout.row = item->layout.row + 1;
        if (item->layout.col > panel->layout.col)
            panel->layout.col = item->layout.col + 1;
    }

    panel->layout.width = getPanelWidth(panel);
    panel->layout.height = getPanelHeight(panel);
}

void setPosition(WindowItem *this, short row, short col)
{
    if (!this) return;

    this->layout.row = row;
    this->layout.col = col;

    updatePanelLayout(this->myPanel);
}

short getColWidth(Panel *panel, short col)
{
    short width = 0;
    WindowItem *item;

    if (!panel || !panel->layout.col) return -1;
    if (col < 0 || col >= panel->layout.col) return -2;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item->data.panel.panel);
        if (item->layout.col == col && item->layout.width > width)
            width = item->layout.width;
    }

    return width;
}

short getRowHeight(Panel *panel, short row)
{
    short height = 0;
    WindowItem *item;

    if (!panel || !panel->layout.row) return -1;
    if (row < 0 || row >= panel->layout.row) return -2;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item->data.panel.panel);
        if (item->layout.row == row && item->layout.height > height)
            height = item->layout.height;
    }

    return height;
}

short getPanelWidth(Panel *panel)
{
    short col;
    short width = 0;
    short tempWidth;

    if (!panel) return -1;
    updatePanelLayout(panel);

    if (!panel->layout.col) return -2;

    for (col = 0; col < panel->layout.col; col ++)
    {
        if ((tempWidth = getColWidth(panel, col)) >= 0)
            width += tempWidth;
    }

    return width;
}

short getPanelHeight(Panel *panel)
{
    short row;
    short height = 0;
    short tempHeight;

    if (!panel) return -1;
    updatePanelLayout(panel);

    if (!panel->layout.row) return -2;

    for (row = 0; row < panel->layout.row; row ++)
    {
        if ((tempHeight = getRowHeight(panel, row)) >= 0)
            height += tempHeight;
    }

    return height;
}
