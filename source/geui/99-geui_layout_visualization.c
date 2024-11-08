char geuiDebugIconAnimpos;
Color geuiDebugIconColor;

void visualize(Window *window, WindowItem *panelItem, Color color)
{
    if (window && window->isOpen)
    {
        long x, y, xOrigin = 0, yOrigin = 0;
        short row, col;
        Actor *parent;
        Panel *panel;

        if (panelItem)
            panel = panelItem->data.panel;
        else
            panel = &window->root;

        parent = getclone(window->parentCName);

        if (actorExists2(parent))
        {
            xOrigin = parent->xscreen;
            yOrigin = parent->yscreen;
        }

        geuiDebugIconColor = color;

        for (row = 0; row <= panel->rows; row++)
        {
            for (col = 0; col <= panel->cols; col++)
            {
                y = getRowStart(panelItem, panel, row)-(row > 0);
                x = getColStart(panelItem, panel, col)-(col > 0);
                geuiDebugIconAnimpos = calculateAnimpos(panel->cols + 1, panel->rows + 1, col, row);
                SendActivationEvent(a_debugIcon.clonename);
                SendActivationEvent(a_debugIcon.clonename); // coloring doesn't work properly
                                                            // without the second activation event
                draw_from("a_debugIcon", xOrigin + x, yOrigin + y, 1);
            }
        }
    }
}

void printVisualizationData(Window *window, WindowItem *panelItem)
{
    if (window && window->isOpen)
    {
        long x, y;
        short row, col;
        char temp[256];
        char panelTag[50];
        Panel *panel;

        if (panelItem)
            panel = panelItem->data.panel;
        else
            panel = &window->root;

        for (row = 0; row <= panel->rows; row++)
        {
            for (col = 0; col <= panel->cols; col++)
            {
                y = getRowStart(panelItem, panel, row);
                x = getColStart(panelItem, panel, col);

                if (panelItem)
                    sprintf(panelTag, "%s.%d w: %d h: %d cw: %d rh: %d", panelItem->tag, window->index, getPanelWidth(panelItem->data.panel), getPanelHeight(panelItem->data.panel), getColWidth(panelItem->data.panel, col), getRowHeight(panelItem->data.panel, row));
                else
                    sprintf(panelTag, "root.%d w: %d h: %d cw: %d rh: %d", window->index, getPanelWidth(&window->root), getPanelHeight(&window->root), getColWidth(&window->root, col), getRowHeight(&window->root, row));

                sprintf(temp, "%s row: %d, col: %d, x: %d, y: %d, rows: %d", panelTag, row, col, x, y, window->root.rows);
                DEBUG_MSG(temp);
            }
        }
    }
}
