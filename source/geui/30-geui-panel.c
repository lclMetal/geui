// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

Panel *getPanelByTag(Panel *panel, char tag[256]);
Panel *getPanelByIndex(Panel *panel, int index);
void calculateRowsAndCols(Panel *panel);
short getColWidth(Panel *panel, short col);
short getRowHeight(Panel *panel, short row);
short getRowStart(WindowItem *panelItem, Panel *panel, short row);
short getColStart(WindowItem *panelItem, Panel *panel, short col);
short getPanelWidth(Panel *panel);
short getPanelHeight(Panel *panel);
void setPanelBaseParent(Panel *panel, char *parentName);
void updatePanelLayout(WindowItem *panelItem, Panel *panel);
void closePanel(Panel *panel);
void destroyPanel(Panel *panel);

Panel *getPanelByTag(Panel *panel, char tag[256])
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getPanelByTag"); return NULL; }

    for (ptr = panel->iList; ptr != NULL; ptr = ptr->next)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr->data.panel.panel;
        if (ptr->type == GEUI_Panel)
        {
            Panel *p = getPanelByTag(ptr->data.panel.panel, tag);
            if (p) return p;
        }
    }

    return NULL;
}

Panel *getPanelByIndex(Panel *panel, int index)
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getPanelByIndex"); return NULL; }

    if (panel->index == index) return panel;

    for (ptr = panel->iList; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->type == GEUI_Panel)
        {
            Panel *p;

            if (ptr->data.panel.panel->index == index)
                return ptr->data.panel.panel;

            p = getPanelByIndex(ptr->data.panel.panel, index);
            if (p) return p;
        }
    }

    return NULL;
}

void calculateRowsAndCols(Panel *panel)
{
    WindowItem *item;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "calculateRowsAndCols"); return; }

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->layout.row >= panel->rows)
            panel->rows = item->layout.row + 1;
        if (item->layout.col >= panel->cols)
            panel->cols = item->layout.col + 1;
    }
}

short getColWidth(Panel *panel, short col)
{
    short width = 0;
    WindowItem *item;

    if (!panel || !panel->cols)
        { DEBUG_MSG_FROM("panel is NULL or has no columns", "getColWidth"); return -1; }
    if (col < 0 || col >= panel->cols)
        { DEBUG_MSG_FROM("column number is invalid", "getColWidth"); return -2; }

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item, item->data.panel.panel);
        if (item->layout.col == col && item->layout.width > width)
            width = item->layout.width;
    }

    return width + panel->parent->style.padding * (col < panel->cols - 1);
}

short getRowHeight(Panel *panel, short row)
{
    short height = 0;
    WindowItem *item;

    if (!panel || !panel->rows)
        { DEBUG_MSG_FROM("panel is NULL or has no rows", "getRowHeight"); return -1; }
    if (row < 0 || row >= panel->rows)
        { DEBUG_MSG_FROM("row number is invalid", "getRowHeight"); return -2; }

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item, item->data.panel.panel);
        if (item->layout.row == row && item->layout.height > height)
            height = item->layout.height;
    }

    return height + panel->parent->style.padding * (row < panel->rows - 1);
}

short getRowStart(WindowItem *panelItem, Panel *panel, short row)
{
    WindowItem *ptr;

    if (!panel || !panel->iList) { DEBUG_MSG_FROM("panel is NULL or has no items", "getRowStart"); return 0; }

    if (row >= panel->rows)
        return panel->height + getRowStart(panelItem, panel, 0) + panel->parent->style.padding;

    for (ptr = panel->iList; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->layout.row == row)
        {
            return ptr->layout.starty + ptr->parent->style.padding / 2;
        }
    }

    return 0;
}

short getColStart(WindowItem *panelItem, Panel *panel, short col)
{
    WindowItem *ptr;

    if (!panel || !panel->iList) { DEBUG_MSG_FROM("panel is NULL or has no items", "getColStart"); return 0; }

    if (col >= panel->cols)
        return panel->width + getColStart(panelItem, panel, 0) + panel->parent->style.padding;

    for (ptr = panel->iList; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->layout.col == col)
        {
            return ptr->layout.startx + ptr->parent->style.padding / 2;
        }
    }

    return 0;
}

short getPanelWidth(Panel *panel)
{
    short col;
    short width = 0;
    short tempWidth;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getPanelWidth"); return -1; }

    if (!panel->cols) { DEBUG_MSG_FROM("panel has no columns", "getPanelWidth"); return -2; }

    for (col = 0; col < panel->cols; col ++)
    {
        if ((tempWidth = getColWidth(panel, col)) >= 0)
            width += tempWidth;
    }

    return width + panel->cols - (panel->cols > 0);
}

short getPanelHeight(Panel *panel)
{
    short row;
    short height = 0;
    short tempHeight;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getPanelHeight"); return -1; }

    if (!panel->rows) { DEBUG_MSG_FROM("panel has no rows", "getPanelHeight"); return -2; }

    for (row = 0; row < panel->rows; row ++)
    {
        if ((tempHeight = getRowHeight(panel, row)) >= 0)
            height += tempHeight;
    }

    return height + panel->rows - (panel->rows > 0);
}

void setPanelBaseParent(Panel *panel, char *parentName)
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "setPanelBaseParent"); return; }
    if (!actorExists(parentName)) { DEBUG_MSG_FROM("actor doesn't exist", "setPanelBaseParent"); return; }

    ptr = panel->iList;

    while (ptr)
    {
        switch (ptr->type)
        {
            case GEUI_Text: setTextParent(&ptr->data.text, parentName, True); break;
            case GEUI_Button:
                setTextParent(&ptr->data.button.text, parentName, True);
                if (ptr->data.button.bTileStartIndex > -1)
                    changeParentOfClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex, parentName);
                break;
             case GEUI_Panel: setPanelBaseParent(ptr->data.panel.panel,  parentName); break;
             case GEUI_Embedder: ChangeParent(ptr->data.embedder.actorCName, parentName); break;

            default: break;
        }

        ptr = ptr->next;
    }
}

void updatePanelLayout(WindowItem *panelItem, Panel *panel)
{
    short i;
    short startx = 0;
    short starty = 0;
    short origx=0;
    short origy=0;
    short *rowValues;
    short *colValues;
    WindowItem *item;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "updatePanelLayout"); return; }

    if (panelItem && panelItem->type == GEUI_Panel)
    {
        origx=startx = panelItem->layout.startx;
        origy=starty = panelItem->layout.starty;
    }
    else panelItem = NULL;

    calculateRowsAndCols(panel);

    if (!panel->rows || !panel->cols)
        { DEBUG_MSG_FROM("panel has no rows or columns", "updatePanelLayout"); return; }

    rowValues = malloc((panel->rows + 1) * sizeof *rowValues);
    if (!rowValues) { DEBUG_MSG_FROM("memory allocation failed", "updatePanelLayout"); return; }
    colValues = malloc((panel->cols + 1) * sizeof *colValues);
    if (!colValues) { free(rowValues); DEBUG_MSG_FROM("memory allocation failed", "updatePanelLayout"); return; }

    rowValues[0] = origy;
    colValues[0] = origx;
    for (i = 0; i < panel->rows; i++)
        starty = rowValues[i+1] = starty + getRowHeight(panel, i) + 1;// + (i > 0);
    for (i = 0; i < panel->cols; i++)
        startx = colValues[i+1] = startx + getColWidth(panel, i) + 1;// + (i > 0);

    for (item = panel->iList; item != NULL; item = item->next)
    {
        item->layout.startx = colValues[item->layout.col];
        item->layout.starty = rowValues[item->layout.row];
    }

    panel->width = getPanelWidth(panel);
    panel->height = getPanelHeight(panel);

    if (panelItem)
    {
        panelItem->layout.width = panel->width;
        panelItem->layout.height = panel->height;
    }

    free(rowValues);
    free(colValues);
}

void closePanel(Panel *panel)
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "closePanel"); return; }

    ptr = panel->iList;

    while (ptr)
    {
        eraseWindowItem(ptr);
        ptr = ptr->next;
    }
}

void destroyPanel(Panel *panel)
{
    WindowItem *temp;
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "destroyPanel"); return; }

    ptr = panel->iList;

    while (ptr)
    {
        destroyWindowItem(ptr);
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }

    panel->iList = NULL;
    panel->iIndex = 0;
}
