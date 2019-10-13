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
            panel = panelItem->data.panel.panel;
        else
            panel = &window->mainPanel;

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
                y = getRowStart(panelItem, panel, row);
                x = getColStart(panelItem, panel, col);
                geuiDebugIconAnimpos = calculateAnimpos(panel->cols + 1, panel->rows + 1, col, row);
                SendActivationEvent(a_debugIcon.clonename);
                SendActivationEvent(a_debugIcon.clonename); // coloring doesn't work properly
                                                            // without the second activation event
                draw_from("a_debugIcon", xOrigin + x, yOrigin + y, 1);
            }
        }
    }
}
