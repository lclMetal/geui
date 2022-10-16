WindowItem *getNextFocusableItem(WindowItem *ptr, WindowItem *start, bool reverse);
WindowItem *focusItem(WindowItem *ptr);
void blurItem(WindowItem *ptr);
void buildFocus(WindowItem *ptr);
void eraseFocus();

// from geui-window-item.c
WindowItem *getItemFromPanelByIndex(Panel *panel, int index);
bool bothPointToSameItem(WindowItem *item1, WindowItem *item2);

// from geui-panel.c
Panel *getPanelByIndex(Panel *panel, int index);

WindowItem *getNextFocusableItem(WindowItem *ptr, WindowItem *start, bool reverse)
{
    Panel *panel = ptr->myPanel;
    Window *window = ptr->parent;
    WindowItem *next = getItemFromPanelByIndex(panel, ptr->index + (reverse ? -1 : 1));

    // The search looped around, there's no focusable items in this window
    if (bothPointToSameItem(next, start))
        return NULL;

    // If there was a next item in the same panel
    if (next)
    {
        if (next->focusable)
            return next;
        return getNextFocusableItem(next, start, reverse);
    }

    // Otherwise get the next panel in this window
    panel = getPanelByIndex(&window->root, panel->index + (reverse ? -1 : 1));

    // If there was a next panel in the same window
    if (panel)
    {
        next = getItemFromPanelByIndex(panel, reverse ? panel->iIndex - 1 : 0);

        // The search looped around, there's no focusable items in this window
        if (bothPointToSameItem(next, start))
            return NULL;

        // Panel had an item inside with index 0
        if (next)
        {
            if (next->focusable)
                return next;
            return getNextFocusableItem(next, start, reverse);
        }
    }

    // Otherwise use the main panel (always has index 0) of the window
    panel = getPanelByIndex(&window->root, reverse ? window->pIndex - 1 : 0);

    if (panel)
    {
        next = getItemFromPanelByIndex(panel, reverse ? panel->iIndex - 1 : 0);

        // The search looped around, there's no focusable items in this window
        if (bothPointToSameItem(next, start))
            return NULL;

        if (next)
        {
            if (next->focusable)
                return next;
            return getNextFocusableItem(next, start, reverse);
        }
    }

    return NULL;
}

WindowItem *focusItem(WindowItem *ptr)
{
    if (ptr->focusable)
    {
        if (GEUIController.focus != ptr)
        {
            blurItem(GEUIController.focus);
            GEUIController.focus = ptr;
            buildFocus(ptr);

            switch (ptr->type)
            {
                case GEUI_Input:
                    showCaret(&ptr->data.input.caret);
                    updateCaretPosition(&ptr->data.input.caret);
                break;
            }
        }

        return ptr;
    }

    return NULL;
}

void blurItem(WindowItem *ptr)
{
    if (GEUIController.focus != NULL && GEUIController.focus == ptr)
    {
        eraseFocus(ptr);
        GEUIController.focus = NULL;

        if (ptr->type == GEUI_Button)
        {
            ptr->data.button.state = 0;
            colorGuiTiles(ptr->data.button.tiles, ptr->parent->style.buttonColor);
        }
        else if (ptr->type == GEUI_Input)
        {
            ptr->data.input.settings.readValueFunc(&ptr->data.input);
            ptr->data.input.settings.applyConstraintsFunc(&ptr->data.input);
            refreshInputValue(&ptr->data.input);
            updateCaretPosition(&ptr->data.input.caret);
            hideCaret(&ptr->data.input.caret);
        }
    }
}

void buildFocus(WindowItem *ptr)
{
    short i, j;
    Actor *tile;
    short tempAnimpos;
    short focusWidth;
    short focusHeight;
    short focusLineWidth = ptr->parent->style.focusWidth;
    short tilesHorizontal;
    short tilesVertical;
    short tileWidth = ptr->parent->style.tileWidth;
    short tileHeight = ptr->parent->style.tileHeight;

    focusWidth = ptr->layout.width + focusLineWidth * 2;
    tilesHorizontal = ceil(focusWidth / (float)tileWidth);
    focusHeight = max(ptr->layout.height, tileHeight) + focusLineWidth * 2;
    tilesVertical = ceil(focusHeight / (float)tileHeight);

    for (j = 0; j < tilesVertical; j++)
    {
        for (i = 0; i < tilesHorizontal; i++)
        {
            tempAnimpos = calculateAnimpos(tilesHorizontal, tilesVertical, i, j) + 15;

            if (tempAnimpos == 19)
                continue;

            tile = CreateActor("a_gui", ptr->parent->style.guiAnim,
                               ptr->parent->parentCName, "(none)", 0, 0, true);
            tile->x = ptr->layout.startx + tileWidth + i * tileWidth + (i == tilesHorizontal-1) * (focusWidth  - tilesHorizontal * tileWidth)-tileWidth/2;
            tile->x += ptr->parent->style.padding - focusLineWidth;
            tile->y = ptr->layout.starty + tileHeight + j * tileHeight + (j == tilesVertical - 1) * (focusHeight - tilesVertical * tileHeight)-tileHeight/2;
            tile->y += ptr->parent->style.padding - focusLineWidth;
            tile->animpos = tempAnimpos;

            tile->myWindow = -1;
            tile->myPanel = -1;
            tile->myIndex = -1;
            colorActor(tile, ptr->parent->style.focusColor);
            ChangeZDepth(tile->clonename, DEFAULT_ITEM_ZDEPTH);
            EventDisable(tile->clonename, EVENTCOLLISION);
            EventDisable(tile->clonename, EVENTCOLLISIONFINISH);
            updateGuiTileIndices(&GEUIController.focusTiles, tile->cloneindex);
        }
    }
}

void eraseFocus()
{
    eraseGuiTiles(&GEUIController.focusTiles);
}
