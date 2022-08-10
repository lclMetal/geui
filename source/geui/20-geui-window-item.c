// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

// from geui-panel.c
void closePanel(Panel *panel);
void destroyPanel(Panel *panel);
Panel *getPanelByIndex(Panel *panel, int index);

WindowItem *initNewItem(ItemType type, Window *window, Panel *panel, char tag[256]);
WindowItem *addItemToWindow(WindowItem *ptr);
WindowItem *addText(Window *window, Panel *panel, char tag[256], char *string, short maxWidth);
WindowItem *addButton(Window *window, Panel *panel, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *));
WindowItem *addInputInt(Window *window, Panel *panel, char tag[256], int defaultVal, int minVal, int maxVal);
WindowItem *addInputText(Window *window, Panel *panel, char tag[256], const char *string, short maxWidth);
WindowItem *addPanel(Window *window, Panel *panel, char tag[256]);
WindowItem *addEmbedder(Window *window, Panel *panel, char tag[256], const char *actorName);
void setPosition(WindowItem *this, short row, short col);
WindowItem *getItemFromPanelByTag(Panel *panel, char tag[256]);
WindowItem *getItemByTag(Window *window, char tag[256]);
WindowItem *getItemFromPanelByIndex(Panel *panel, int index);
WindowItem *getItemByIndex(Window *window, int index);
WindowItem *getNextFocusableItem(WindowItem *ptr);
WindowItem *focusItem(WindowItem *ptr);
void blurItem(WindowItem *ptr);
void buildFocus(WindowItem *ptr);
void eraseFocus();
void buildItems(Panel *panel);
void buildItem(WindowItem *ptr);
void buildText(WindowItem *ptr);
void buildPanel(WindowItem *ptr);
void buildButtonText(WindowItem *ptr);
void buildButton(WindowItem *ptr);
Actor *buildCaret(WindowItem *ptr, Text *pText, BlinkingCaret *caret);
void buildInputField(WindowItem *ptr, long *tileStartIndex, long *tileEndIndex);
void buildInputInt(WindowItem *ptr);
void buildInputText(WindowItem *ptr);
void buildEmbedder(WindowItem *ptr);
void eraseWindowItem(WindowItem *ptr);
void destroyWindowItem(WindowItem *ptr);

WindowItem *initNewItem(ItemType type, Window *window, Panel *panel, char tag[256])
{
    WindowItem *ptr = NULL;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "initNewItem"); return NULL; }

    ptr = malloc(sizeof *ptr);

    if (!ptr) { DEBUG_MSG_FROM("memory allocation failed", "initNewItem"); return NULL; }

    ptr->type = type;
    ptr->focusable = False;
    ptr->index = panel->iIndex ++;
    strcpy(ptr->tag, tag);
    ptr->myPanel = panel;
    ptr->parent = window;
    ptr->layout.row = 0;
    ptr->layout.col = 0;
    ptr->layout.width = 0;
    ptr->layout.height = 0;
    ptr->layout.startx = 0;
    ptr->layout.starty = 0;

    return ptr;
}

WindowItem *addItemToWindow(WindowItem *ptr)
{
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addItemToWindow"); return NULL; }

    ptr->next = ptr->myPanel->iList;
    ptr->myPanel->iList = ptr;

    return ptr;
}

WindowItem *addText(Window *window, Panel *panel, char tag[256], char *string, short maxWidth)
{
    WindowItem *ptr = initNewItem(GEUI_Text, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addText"); return NULL; }

    ptr->data.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.text, window->style.textColor);
    setTextZDepth(&ptr->data.text, DEFAULT_ITEM_ZDEPTH);

    if (maxWidth > 0)
        fitTextInWidth(&ptr->data.text, maxWidth);

    ptr->layout.width = ptr->data.text.width;
    ptr->layout.height = ptr->data.text.height;

    return addItemToWindow(ptr);
}

WindowItem *addButton(Window *window, Panel *panel, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *))
{
    WindowItem *ptr = initNewItem(GEUI_Button, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addButton"); return NULL; }

    ptr->focusable = True;
    ptr->data.button.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.button.text, window->style.textColor);
    setTextZDepth(&ptr->data.button.text, DEFAULT_ITEM_ZDEPTH);
    ptr->data.button.state = 0;
    ptr->data.button.bTileStartIndex = -1;
    ptr->data.button.bTileEndIndex = -1;
    ptr->data.button.actionFunction = actionFunction;

    ptr->layout.width = ptr->data.button.text.width + ptr->parent->style.tileWidth * 2;
    ptr->layout.height = ptr->parent->style.tileHeight;

    return addItemToWindow(ptr);
}

WindowItem *addInputInt(Window *window, Panel *panel, char tag[256], int defaultVal, int minVal, int maxVal)
{
    char temp[GEUI_INT_STRING_LENGTH];
    WindowItem *ptr = initNewItem(GEUI_InputInt, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addInputInt"); return NULL; }

    ptr->focusable = True;
    ptr->data.inputInt.value = defaultVal;
    initializeCaret(&ptr->data.inputInt.caret);
    sprintf(temp, "%d", defaultVal);
    ptr->data.inputInt.text = createText(temp, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.inputInt.text, window->style.textColor);
    setTextZDepth(&ptr->data.inputInt.text, DEFAULT_ITEM_ZDEPTH);

    setIntLimits(&ptr->data.inputInt, minVal, maxVal);
    ptr->data.inputInt.tileStartIndex = -1;
    ptr->data.inputInt.tileEndIndex = -1;
    ptr->layout.width = calculateRequiredWidthInPixels(&ptr->data.inputInt) + ptr->parent->style.tileWidth * 2;
    ptr->layout.height = ptr->parent->style.tileHeight;

    return addItemToWindow(ptr);
}

WindowItem *addInputText(Window *window, Panel *panel, char tag[256], const char *string, short maxWidth)
{
    WindowItem *ptr = initNewItem(GEUI_InputText, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addInputText"); return NULL; }

    ptr->focusable = True;
    initializeCaret(&ptr->data.inputText.caret);
    ptr->data.inputText.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.inputText.text, window->style.textColor);
    setTextZDepth(&ptr->data.inputText.text, DEFAULT_ITEM_ZDEPTH);

    ptr->data.inputText.tileStartIndex = -1;
    ptr->data.inputText.tileEndIndex = -1;

    ptr->layout.width = maxWidth + ptr->parent->style.tileWidth * 2;
    ptr->layout.height = ptr->parent->style.tileHeight;

    return addItemToWindow(ptr);
}

WindowItem *addPanel(Window *window, Panel *panel, char tag[256])
{
    WindowItem *ptr = initNewItem(GEUI_Panel, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addPanel"); return NULL; }

    ptr->data.panel = malloc(sizeof *ptr->data.panel);
    if (!ptr->data.panel)
    {
        free(ptr);
        DEBUG_MSG_FROM("memory allocation failed", "addPanel");
        return NULL;
    }

    ptr->data.panel->index = window->pIndex++;
    ptr->data.panel->iIndex = 0;
    ptr->data.panel->rows = 0;
    ptr->data.panel->cols = 0;
    ptr->data.panel->width = 0;
    ptr->data.panel->height = 0;
    ptr->data.panel->parent = window;

    return addItemToWindow(ptr);
}

WindowItem *addEmbedder(Window *window, Panel *panel, char tag[256], const char *actorName)
{
    Actor *actor;
    WindowItem *ptr = initNewItem(GEUI_Embedder, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addEmbedder"); return NULL; }

    if (!actorExists2(actor = getclone(actorName)))
    {
        DEBUG_MSG_FROM("actor doesn't exist", "addEmbedder");
        free(ptr);
        return NULL;
    }

    strcpy(ptr->data.embedder.actorCName, actor->clonename);
    VisibilityState(ptr->data.embedder.actorCName, DISABLE);

    ptr->layout.width = actor->width;
    ptr->layout.height = actor->height;

    return addItemToWindow(ptr);
}

void setPosition(WindowItem *this, short row, short col)
{
    if (!this) { DEBUG_MSG_FROM("item is NULL", "setPosition"); return; }

    this->layout.row = row;
    this->layout.col = col;
}

WindowItem *getItemFromPanelByTag(Panel *panel, char tag[256])
{
    WindowItem *ptr;
    WindowItem *result = NULL;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getItemFromPanelByTag"); return NULL; }

    ptr = panel->iList;

    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;

        if (ptr->type == GEUI_Panel)
        {
            result = getItemFromPanelByTag(ptr->data.panel, tag);

            if (result)
                return result;
        }

        ptr = ptr->next;
    }

    return NULL;
}

WindowItem *getItemByTag(Window *window, char tag[256])
{
    WindowItem *ptr;

    if (!window) { DEBUG_MSG_FROM("panel is NULL", "getItemByTag"); return NULL; }

    ptr = getItemFromPanelByTag(&window->mainPanel, tag);

    if (ptr)
        return ptr;

    return NULL;
}

WindowItem *getItemFromPanelByIndex(Panel *panel, int index)
{
    WindowItem *ptr;
    WindowItem *result = NULL;

    if (!panel) { DEBUG_MSG_FROM("panel is NULL", "getItemFromPanelByIndex"); return NULL; }

    ptr = panel->iList;

    while (ptr)
    {
        if (ptr->index == index)
            return ptr;

        ptr = ptr->next;
    }

    return NULL;
}

WindowItem *getItemByIndex(Window *window, int index)
{
    WindowItem *ptr;

    if (!window) { DEBUG_MSG_FROM("window is NULL", "getItemByIndex"); return NULL; }

    ptr = getItemFromPanelByIndex(&window->mainPanel, index);

    if (ptr)
        return ptr;

    return NULL;
}

WindowItem *getNextFocusableItem(WindowItem *ptr)
{
    Panel *panel = ptr->myPanel;
    Window *window = ptr->parent;
    WindowItem *next = getItemFromPanelByIndex(panel, ptr->index + 1);

    // If there was a next item in the same panel
    if (next)
    {
        if (next->focusable)
            return next;
        return getNextFocusableItem(next);
    }

    // Otherwise get the next panel in this window
    panel = getPanelByIndex(&window->mainPanel, panel->index + 1);

    // If there was a next panel in the same window
    if (panel)
    {
        next = getItemFromPanelByIndex(panel, 0);

        // Panel had an item inside with index 0
        if (next)
        {
            if (next->focusable)
                return next;
            return getNextFocusableItem(next);
        }
    }

    // Otherwise use the main panel (always has index 0) of the window
    panel = getPanelByIndex(&window->mainPanel, 0);

    if (panel)
    {
        next = getItemFromPanelByIndex(panel, 0);

        if (next)
        {
            if (next->focusable)
                return next;
            return getNextFocusableItem(next);
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
                case GEUI_InputInt: showCaret(&ptr->data.inputInt.caret); break;
                case GEUI_InputText: showCaret(&ptr->data.inputText.caret); break;
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
            colorClones("a_gui",
                        ptr->data.button.bTileStartIndex,
                        ptr->data.button.bTileEndIndex, ptr->parent->style.buttonColor);
        }
        else if (ptr->type == GEUI_InputInt)
        {
            enforceIntLimits(&ptr->data.inputInt);
            refreshText(&ptr->data.inputInt.text);
            updateCaretPosition(&ptr->data.inputInt.caret);
            hideCaret(&ptr->data.inputInt.caret);
        }
        else if (ptr->type == GEUI_InputText)
        {
            hideCaret(&ptr->data.inputText.caret);
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
    focusHeight = ptr->layout.height + focusLineWidth * 2;
    tilesVertical = ceil(focusHeight / (float)tileHeight);

    if (tilesVertical < 3)
    {
        for (i = 0; i < tilesHorizontal; i++)
        {
            tile = CreateActor("a_gui", ptr->parent->style.guiAnim,
                               ptr->parent->parentCName, "(none)", 0, 0, true);
            tile->x = ptr->layout.startx + tileWidth + i * tileWidth  + (i >= 2 && i >= tilesHorizontal - 2) * (focusWidth  - tilesHorizontal * tileWidth)-tileWidth/2;
            tile->x += ptr->parent->style.padding - focusLineWidth;
            tile->y = ptr->layout.starty + tileHeight -tileHeight/2;
            tile->y += ptr->parent->style.padding - focusLineWidth;
            tile->animpos = 15 + (i > 0) + (i == tilesHorizontal - 1);
            tile->myWindow = -1;
            tile->myPanel = -1;
            tile->myIndex = -1;
            colorActor(tile, ptr->parent->style.focusColor);
            ChangeZDepth(tile->clonename, DEFAULT_ITEM_ZDEPTH);
            EventDisable(tile->clonename, EVENTCOLLISION);
            EventDisable(tile->clonename, EVENTCOLLISIONFINISH);
            updateIndexBounds(&GEUIController.focusTileStartIndex, &GEUIController.focusTileEndIndex, tile->cloneindex);

            tile = CreateActor("a_gui", ptr->parent->style.guiAnim,
                           ptr->parent->parentCName, "(none)", 0, 0, true);
            tile->x = ptr->layout.startx + tileWidth + i * tileWidth  + (i >= 2 && i >= tilesHorizontal - 2) * (focusWidth  - tilesHorizontal * tileWidth)-tileWidth/2;
            tile->x += ptr->parent->style.padding - focusLineWidth;
            tile->y = ptr->layout.starty + tileHeight -tileHeight/2 + ptr->parent->style.padding - focusLineWidth + focusHeight - tileHeight;
            tile->animpos = 21 + (i > 0) + (i == tilesHorizontal - 1);
            tile->myWindow = -1;
            tile->myPanel = -1;
            tile->myIndex = -1;
            colorActor(tile, ptr->parent->style.focusColor);
            ChangeZDepth(tile->clonename, DEFAULT_ITEM_ZDEPTH);
            EventDisable(tile->clonename, EVENTCOLLISION);
            EventDisable(tile->clonename, EVENTCOLLISIONFINISH);
            updateIndexBounds(&GEUIController.focusTileStartIndex, &GEUIController.focusTileEndIndex, tile->cloneindex);
        }
    }
    else
    {
        for (j = 0; j < tilesVertical; j++)
        {
            for (i = 0; i < tilesHorizontal; i++)
            {
                tempAnimpos = calculateAnimpos(tilesHorizontal, tilesVertical, i, j) + 15;

                if (tempAnimpos == 19)
                    continue;

                tile = CreateActor("a_gui", ptr->parent->style.guiAnim,
                                   ptr->parent->parentCName, "(none)", 0, 0, true);
                tile->x = ptr->layout.startx + tileWidth + i * tileWidth  + (i >= 2 && i >= tilesHorizontal - 2) * (focusWidth  - tilesHorizontal * tileWidth)-tileWidth/2;
                tile->x += ptr->parent->style.padding - focusLineWidth;
                tile->y = ptr->layout.starty + tileHeight + j * tileHeight + (j >= 2 && j >= tilesVertical - 2) * (focusHeight - tilesVertical * tileHeight)-tileHeight/2;
                tile->y += ptr->parent->style.padding - focusLineWidth;
                tile->animpos = tempAnimpos;

                tile->myWindow = -1;
                tile->myPanel = -1;
                tile->myIndex = -1;
                colorActor(tile, ptr->parent->style.focusColor);
                ChangeZDepth(tile->clonename, DEFAULT_ITEM_ZDEPTH);
                EventDisable(tile->clonename, EVENTCOLLISION);
                EventDisable(tile->clonename, EVENTCOLLISIONFINISH);
                updateIndexBounds(&GEUIController.focusTileStartIndex, &GEUIController.focusTileEndIndex, tile->cloneindex);
            }
        }
    }
}

void eraseFocus()
{
    if (GEUIController.focusTileStartIndex > -1)
    {
        destroyClones("a_gui", GEUIController.focusTileStartIndex, GEUIController.focusTileEndIndex);
        GEUIController.focusTileStartIndex = -1;
        GEUIController.focusTileEndIndex = -1;
    }
}

void buildItems(Panel *panel)
{
    WindowItem *ptr;

    ptr = panel->iList;

    while (ptr)
    {
        buildItem(ptr);
        ptr = ptr->next;
    }
}

void buildItem(WindowItem *ptr)
{
    switch (ptr->type)
    {
        case GEUI_Text: buildText(ptr); break;
        case GEUI_Button: buildButton(ptr); break;
        case GEUI_InputInt: buildInputInt(ptr); break;
        case GEUI_InputText: buildInputText(ptr); break;
        case GEUI_Panel: buildPanel(ptr); break;
        case GEUI_Embedder: buildEmbedder(ptr); break;
    }
}

void buildText(WindowItem *ptr)
{
    if (ptr->type != GEUI_Text) { DEBUG_MSG_FROM("item is not a valid Text item", "buildText"); return; }

    setTextZDepth(&ptr->data.text, DEFAULT_ITEM_ZDEPTH);
    // TODO: layout / positioning
    setTextPosition(&ptr->data.text,
        ptr->layout.startx + ptr->parent->style.padding,
        ptr->layout.starty + ptr->data.text.pFont->lineSpacing * 0.5 + ptr->parent->style.padding);
    refreshText(&ptr->data.text);
}

void buildPanel(WindowItem *ptr)
{
    if (ptr->type != GEUI_Panel) { DEBUG_MSG_FROM("item is not a valid Panel item", "buildPanel"); return; }

    buildItems(ptr->data.panel);
}

void buildButtonText(WindowItem *ptr)
{
    long start = ptr->data.button.bTileStartIndex;
    long end = ptr->data.button.bTileEndIndex;

    Text *buttonText = &ptr->data.button.text;

    colorClones("a_gui", start, end, ptr->parent->style.buttonColor);
    setTextZDepth(buttonText, DEFAULT_ITEM_ZDEPTH);
    setTextAlignment(buttonText, ALIGN_CENTER);
    setTextPosition(buttonText,
        ceil((getTile(end)->x - getTile(start)->x) * 0.5) + getTile(start)->x, getTile(start)->y);
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
    short tileWidth = ptr->parent->style.tileWidth;
    short tileHeight = ptr->parent->style.tileHeight;

    if (ptr->type != GEUI_Button) { DEBUG_MSG_FROM("item is not a valid Button item", "buildButton"); return; }

    buttonWidth = ptr->layout.width; //ptr->data.button.text.width + ptr->parent->style.padding * 2;
    tilesHorizontal = ceil(buttonWidth / (float)tileWidth);

    for (i = 0; i < tilesHorizontal; i ++)
    {
        a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
        // TODO: layout / positioning
        a->x = ptr->layout.startx + tileWidth + i * tileWidth + (i >= 2 && i >= tilesHorizontal - 2) * (buttonWidth - tilesHorizontal * tileWidth)-tileWidth/2;// + (ptr->layout.col > 0); // TODO: make nicer
        a->x += ptr->parent->style.padding;
        a->y = ptr->layout.starty + tileHeight-tileWidth/2;// + (ptr->layout.row > 0);
        a->y += ptr->parent->style.padding;
        a->myWindow = ptr->parent->index;
        a->myPanel  = ptr->myPanel->index;
        a->myIndex  = ptr->index;
        ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
        a->animpos = 9 + (i > 0) + (i == tilesHorizontal - 1) + (i > 0 && i == tilesHorizontal - 2 && buttonWidth < tileWidth * 2.5);// TODO: make nicer

        updateIndexBounds(&ptr->data.button.bTileStartIndex, &ptr->data.button.bTileEndIndex, a->cloneindex);
    }

    buildButtonText(ptr);
}

Actor *buildCaret(WindowItem *ptr, Text *pText, BlinkingCaret *caret)
{
    Actor *a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
    a->animpos = 19;
    a->myWindow = ptr->parent->index,
    a->myPanel = ptr->myPanel->index;
    a->myIndex = ptr->index;//
    a->myProperties = GEUI_CARET;
    SendActivationEvent(a->clonename);
    ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
    strcpy(caret->actorCName, a->clonename);
    caret->pText = pText;
    colorActorByName(caret->actorCName, ptr->parent->style.textColor);
    updateCaretPosition(caret);

    return a;
}

void buildInputField(WindowItem *ptr, long *tileStartIndex, long *tileEndIndex)
{
    short i;
    Actor *a;
    short fieldWidth;
    short tilesHorizontal;
    short tileWidth = ptr->parent->style.tileWidth;
    short tileHeight = ptr->parent->style.tileHeight;

    fieldWidth = ptr->layout.width;
    tilesHorizontal = ceil(fieldWidth / (float)tileWidth);

    for (i = 0; i < tilesHorizontal; i++)
    {
        a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
        a->x = ptr->layout.startx + tileWidth + i * tileWidth + (i >= 2 && i >= tilesHorizontal - 2) * (fieldWidth - tilesHorizontal * tileWidth)-tileWidth/2;
        a->x += ptr->parent->style.padding;
        a->y = ptr->layout.starty + tileHeight-tileWidth/2;
        a->y += ptr->parent->style.padding;
        a->myWindow = ptr->parent->index;
        a->myPanel  = ptr->myPanel->index;
        a->myIndex  = ptr->index;
        a->myProperties = GEUI_INPUT_BG;
        a->animpos = 12 + (i > 0) + (i == tilesHorizontal - 1) + (i > 0 && i == tilesHorizontal - 2 && fieldWidth < tileWidth * 2.5);
        SendActivationEvent(a->clonename);
        ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);

        updateIndexBounds(tileStartIndex, tileEndIndex, a->cloneindex);
    }
}

void buildInputInt(WindowItem *ptr)
{
    if (ptr->type != GEUI_InputInt) { DEBUG_MSG_FROM("item is not a valid InputInt item", "buildInputInt"); return; }

    buildInputField(ptr, &ptr->data.inputInt.tileStartIndex, &ptr->data.inputInt.tileEndIndex);
    colorClones("a_gui", ptr->data.inputInt.tileStartIndex, ptr->data.inputInt.tileEndIndex, ptr->parent->style.inputBgColor);

    setTextZDepth(&ptr->data.inputInt.text, DEFAULT_ITEM_ZDEPTH);
    setTextPosition(&ptr->data.inputInt.text,
        getTile(ptr->data.inputInt.tileStartIndex)->x,
        getTile(ptr->data.inputInt.tileStartIndex)->y);
    refreshText(&ptr->data.inputInt.text);

    buildCaret(ptr, &ptr->data.inputInt.text, &ptr->data.inputInt.caret);
    hideCaret(&ptr->data.inputInt.caret);
}

void buildInputText(WindowItem *ptr)
{
    if (ptr->type != GEUI_InputText) { DEBUG_MSG_FROM("item is not a valid InputText item", "buildInputText"); return; }

    buildInputField(ptr, &ptr->data.inputText.tileStartIndex, &ptr->data.inputText.tileEndIndex);
    colorClones("a_gui", ptr->data.inputText.tileStartIndex, ptr->data.inputText.tileEndIndex, ptr->parent->style.inputBgColor);

    setTextZDepth(&ptr->data.inputText.text, DEFAULT_ITEM_ZDEPTH);
    setTextPosition(&ptr->data.inputText.text,
        getTile(ptr->data.inputText.tileStartIndex)->x,
        getTile(ptr->data.inputText.tileStartIndex)->y);
    refreshText(&ptr->data.inputText.text);

    buildCaret(ptr, &ptr->data.inputText.text, &ptr->data.inputText.caret);
    hideCaret(&ptr->data.inputText.caret);
}

void buildEmbedder(WindowItem *ptr)
{
    Actor *actor;
    if (ptr->type != GEUI_Embedder) { DEBUG_MSG_FROM("item is not a valid Embedder item", "buildEmbedder"); return; }

    if (!actorExists2(actor = getclone(ptr->data.embedder.actorCName))) { DEBUG_MSG_FROM("actor doesn't exist", "buildEmbedder"); return; }

    ChangeZDepth(ptr->data.embedder.actorCName, DEFAULT_ITEM_ZDEPTH);
    ChangeParent(ptr->data.embedder.actorCName, "(none)");
    actor->x = 0;
    actor->y = 0;
    ChangeParent(ptr->data.embedder.actorCName, ptr->parent->parentCName);
    actor->x = ptr->layout.startx + ptr->parent->style.padding + actor->width / 2;
    actor->y = ptr->layout.starty + ptr->parent->style.padding + actor->height / 2;
    VisibilityState(ptr->data.embedder.actorCName, ENABLE);

    // {
        // char temp[256];
        // sprintf(temp, "%d, %d - %d, %d", ptr->layout.startx + ptr->parent->style.padding + actor->width / 2,
                                // ptr->layout.starty + ptr->parent->style.padding + actor->height / 2,
                                // (int)actor->x,
                                // (int)actor->y);
        // DEBUG_MSG_FROM(temp, "buildEmbedder");
    // }
}

void eraseWindowItem(WindowItem *ptr)
{
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "eraseWindowItem"); return; }

    if (GEUIController.focus == ptr)
    {
        eraseFocus();
        GEUIController.focus = NULL;
    }

    switch (ptr->type)
    {
        case GEUI_Text:
            eraseText(&ptr->data.text);
            setTextParent(&ptr->data.text, "(none)", False);
        break;
        case GEUI_Button:
            eraseText(&ptr->data.button.text);
            if (ptr->data.button.bTileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex);
                ptr->data.button.bTileStartIndex = -1;
                ptr->data.button.bTileEndIndex = -1;
            }
        break;
        case GEUI_InputInt:
            eraseText(&ptr->data.inputInt.text);
            eraseCaret(&ptr->data.inputInt.caret);
            setTextParent(&ptr->data.inputInt.text, "(none)", False);
            if (ptr->data.inputInt.tileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.inputInt.tileStartIndex, ptr->data.inputInt.tileEndIndex);
                ptr->data.inputInt.tileStartIndex = -1;
                ptr->data.inputInt.tileEndIndex = -1;
            }
        break;
        case GEUI_InputText:
            eraseText(&ptr->data.inputText.text);
            eraseCaret(&ptr->data.inputText.caret);
            setTextParent(&ptr->data.inputText.text, "(none)", False);
            if (ptr->data.inputText.tileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.inputText.tileStartIndex, ptr->data.inputText.tileEndIndex);
                ptr->data.inputText.tileStartIndex = -1;
                ptr->data.inputText.tileEndIndex = -1;
            }
        break;
        case GEUI_Panel:
            closePanel(ptr->data.panel);
        break;
        case GEUI_Embedder:
            VisibilityState(ptr->data.embedder.actorCName, DISABLE);
        break;

        default: break;
    }
}

void destroyWindowItem(WindowItem *ptr)
{
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "destroyWindowItem"); return; }

    if (GEUIController.focus == ptr)
        eraseFocus();

    switch (ptr->type)
    {
        case GEUI_Text: destroyText(&ptr->data.text); break;
        case GEUI_Button:
            destroyText(&ptr->data.button.text);
            if (ptr->data.button.bTileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex);
                ptr->data.button.bTileStartIndex = -1;
                ptr->data.button.bTileEndIndex = -1;
            }
        break;
        case GEUI_InputInt:
            destroyText(&ptr->data.inputInt.text);
            eraseCaret(&ptr->data.inputInt.caret);
            if (ptr->data.inputInt.tileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.inputInt.tileStartIndex, ptr->data.inputInt.tileEndIndex);
                ptr->data.inputInt.tileStartIndex = -1;
                ptr->data.inputInt.tileEndIndex = -1;
            }
        break;
        case GEUI_InputText:
            destroyText(&ptr->data.inputText.text);
            eraseCaret(&ptr->data.inputText.caret);
            if (ptr->data.inputText.tileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.inputText.tileStartIndex, ptr->data.inputText.tileEndIndex);
                ptr->data.inputText.tileStartIndex = -1;
                ptr->data.inputText.tileEndIndex = -1;
            }
        break;
        case GEUI_Panel:
            destroyPanel(ptr->data.panel);
            free(ptr->data.panel);
        break;
        case GEUI_Embedder:
            DestroyActor(ptr->data.embedder.actorCName);
        break;

        default: break;
    }
}
