WindowItem *addButton(Panel *panel, char tag[256], char *string, GUIAction action);
void buildButtonText(WindowItem *ptr);
void buildButton(WindowItem *ptr);

WindowItem *addButton(Panel *panel, char tag[256], char *string, GUIAction action)
{
    short buttonMinWidth;
    WindowItem *ptr = initNewItem(GEUI_Button, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addButton"); return NULL; }

    ptr->focusable = True;
    ptr->data.button.text = createText(string, panel->parent->style->textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.button.text, panel->parent->style->textColor);
    setTextZDepth(&ptr->data.button.text, DEFAULT_ITEM_ZDEPTH);
    ptr->data.button.state = 0;
    ptr->data.button.tiles = noIndices;

    ptr->data.button.action = action;
    ptr->data.button.action.window = panel->parent;
    ptr->data.button.action.panel = panel;
    ptr->data.button.action.itemIndex = ptr->index;

    ptr->layout.width = ptr->data.button.text.width + ptr->parent->style->tileWidth * ptr->parent->style->buttonPadding * 2;
    buttonMinWidth = ptr->parent->style->tileWidth * 2;
    if (ptr->layout.width < buttonMinWidth)
        ptr->layout.width = buttonMinWidth;

    ptr->layout.height = ptr->parent->style->tileHeight;

    return addItemToWindow(ptr);
}

void buildButtonText(WindowItem *ptr)
{
    long start = ptr->data.button.tiles.first;
    long end = ptr->data.button.tiles.last;
    short tileWidth = ptr->parent->style->tileWidth;

    Text *buttonText = &ptr->data.button.text;

    colorClones("a_gui", start, end, ptr->parent->style->buttonColor);
    setTextZDepth(buttonText, DEFAULT_ITEM_ZDEPTH);

    if (ptr->parent->style->buttonProperties & GEUI_BUTTON_TEXT_ALIGN_LEFT)
    {
        setTextAlignment(buttonText, ALIGN_LEFT);
        setTextPosition(buttonText, getTile(start)->x - tileWidth / 2 + tileWidth * ptr->parent->style->buttonPadding, getTile(start)->y - ceil(ptr->data.button.text.pFont->baselineOffset * 0.5));
    }
    else
    {
        setTextAlignment(buttonText, ALIGN_CENTER);
        setTextPosition(buttonText,
            ceil((getTile(end)->x - getTile(start)->x) * 0.5) + getTile(start)->x, getTile(start)->y - ceil(ptr->data.button.text.pFont->baselineOffset * 0.5));
    }

    refreshText(buttonText);
}

void buildButton(WindowItem *ptr)
{
    short i;
    Actor *a;
    Text *buttonText;
    long start, end;
    short buttonWidth;
    short tilesHorizontal;
    short tileWidth = ptr->parent->style->tileWidth;
    short tileHeight = ptr->parent->style->tileHeight;

    if (ptr->type != GEUI_Button) { DEBUG_MSG_FROM("item is not a valid Button item", "buildButton"); return; }

    buttonWidth = ptr->layout.width;
    tilesHorizontal = ceil(buttonWidth / (float)tileWidth);

    for (i = 0; i < tilesHorizontal; i ++)
    {
        a = CreateActor("a_gui", ptr->parent->style->guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
        // TODO: layout / positioning
        a->x = ptr->layout.startx + tileWidth + i * tileWidth + (i >= 2 && i >= tilesHorizontal - 2) * (buttonWidth - tilesHorizontal * tileWidth)-tileWidth/2;// + (ptr->layout.col > 0); // TODO: make nicer
        a->x += ptr->parent->style->padding;
        a->y = ptr->layout.starty + tileHeight-tileWidth/2;// + (ptr->layout.row > 0);
        a->y += ptr->parent->style->padding;
        a->myWindow = ptr->parent->index;
        a->myPanel  = ptr->myPanel->index;
        a->myIndex  = ptr->index;
        ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
        a->animpos = 9 + (i > 0) + (i == tilesHorizontal - 1) + (i > 0 && i == tilesHorizontal - 2 && buttonWidth < tileWidth * 2.5);// TODO: make nicer

        updateGuiTileIndices(&ptr->data.button.tiles, a->cloneindex);
    }

    buildButtonText(ptr);
}
