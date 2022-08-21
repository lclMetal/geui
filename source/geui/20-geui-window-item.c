// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

// from geui-panel.c
void closePanel(Panel *panel);
void destroyPanel(Panel *panel);
Panel *getPanelByIndex(Panel *panel, int index);

void updateGuiTileIndices(TileIndices *indices, long newIndex);
void eraseGuiTiles(TileIndices *indices);
void colorGuiTiles(TileIndices indices, Color color);
WindowItem *initNewItem(ItemType type, Window *window, Panel *panel, char tag[256]);
WindowItem *addItemToWindow(WindowItem *ptr);
WindowItem *addText(Window *window, Panel *panel, char tag[256], char *string, short maxWidth);
WindowItem *addButton(Window *window, Panel *panel, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *));
WindowItem *addInputField(Window *window, Panel *panel, char tag[256], const char *string, InputSettings settings, short maxWidth);
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
void buildInputFieldBackground(WindowItem *ptr, TileIndices *tiles);
void buildInputField(WindowItem *ptr);
void buildEmbedder(WindowItem *ptr);
void eraseWindowItem(WindowItem *ptr);
void destroyWindowItem(WindowItem *ptr);

void updateGuiTileIndices(TileIndices *indices, long newIndex)
{
    updateIndexBounds(&indices->first, &indices->last, newIndex);
}

void eraseGuiTiles(TileIndices *indices)
{
    destroyClones("a_gui", indices->first, indices->last);
    *indices = noIndices;
}

void colorGuiTiles(TileIndices indices, Color color)
{
    colorClones("a_gui", indices.first, indices.last, color);
}

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
    ptr->data.button.tiles = noIndices;
    ptr->data.button.actionFunction = actionFunction;

    ptr->layout.width = ptr->data.button.text.width + ptr->parent->style.tileWidth * 2;
    ptr->layout.height = ptr->parent->style.tileHeight;

    return addItemToWindow(ptr);
}

void enforceTextInputSettings(InputField *input)
{
    return;
}

void updateTextInputValue(InputField *input)
{
    return;
}

InputSettings createTextInputSettings()
{
    InputSettings settings;

    settings.type = GEUI_TextInput;
    (settings.settingsFunction = enforceTextInputSettings);
    (settings.valueFunction = updateTextInputValue);

    settings.data.textInput.empty = 0;

    return settings;
}

void enforceIntInputSettings(InputField *input)
{
    input->value.intValue = iLimit(input->value.intValue, input->settings.data.intInput.minVal, input->settings.data.intInput.maxVal);
    refreshInputValue(input);
}

void updateIntInputValue(InputField *input)
{
    if (greedyAtoi(input->text.pString) != 0)
    {
        input->value.intValue = greedyAtoi(input->text.pString);
        refreshInputValue(input);
    }
}

InputSettings createIntInputSettings(int minVal, int maxVal, int defaultValue)
{
    InputSettings settings;

    settings.type = GEUI_IntInput;
    (settings.settingsFunction = enforceIntInputSettings);
    (settings.valueFunction = updateIntInputValue);

    settings.data.intInput.minVal = minVal;
    settings.data.intInput.maxVal = maxVal;
    settings.data.intInput.defaultValue = iLimit(defaultValue, minVal, maxVal);

    return settings;
}

void enforceRealInputSettings(InputField *input)
{
    input->value.realValue = fLimit(input->value.realValue, input->settings.data.realInput.minVal, input->settings.data.realInput.maxVal);
    refreshInputValue(input);
}

void updateRealInputValue(InputField *input)
{
    int readCount = 0;

    if (greedyAtof(input->text.pString, &readCount) != 0)
    {
        input->value.realValue = greedyAtof(input->text.pString, NULL);

        if (readCount > 1)
            refreshInputValue(input);
    }
}

InputSettings createRealInputSettings(float minVal, float maxVal, float defaultValue, short precisionDigits)
{
    InputSettings settings;

    settings.type = GEUI_RealInput;
    (settings.settingsFunction = enforceRealInputSettings);
    (settings.valueFunction = updateRealInputValue);

    settings.data.realInput.minVal = minVal;
    settings.data.realInput.maxVal = maxVal;
    settings.data.realInput.defaultValue = dfLimit(defaultValue, minVal, maxVal);
    settings.data.realInput.precisionDigits = siLimit(precisionDigits, 0, 10);

    return settings;
}

WindowItem *addInputField(Window *window, Panel *panel, char tag[256], const char *string, InputSettings settings, short maxWidth)
{
    WindowItem *ptr = initNewItem(GEUI_Input, window, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addInputField"); return NULL; }

    ptr->focusable = True;
    initializeCaret(&ptr->data.input.caret);
    ptr->data.input.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.input.text, window->style.textColor);
    setTextZDepth(&ptr->data.input.text, DEFAULT_ITEM_ZDEPTH);
    ptr->data.input.settings = settings;

    switch (settings.type)
    {
        case GEUI_TextInput: ptr->data.input.value.textValue = ptr->data.input.text.pString; break;
        case GEUI_IntInput: ptr->data.input.value.intValue = settings.data.intInput.defaultValue; break;
        case GEUI_RealInput: ptr->data.input.value.realValue = settings.data.realInput.defaultValue; break;
    }

    refreshValue(&ptr->data.input);

    ptr->data.input.tiles = noIndices;

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
            ptr->data.input.settings.settingsFunction(&ptr->data.input);
            ptr->data.input.settings.valueFunction(&ptr->data.input);
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
            updateGuiTileIndices(&GEUIController.focusTiles, tile->cloneindex);

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
            updateGuiTileIndices(&GEUIController.focusTiles, tile->cloneindex);
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
                updateGuiTileIndices(&GEUIController.focusTiles, tile->cloneindex);
            }
        }
    }
}

void eraseFocus()
{
    eraseGuiTiles(&GEUIController.focusTiles);
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
        case GEUI_Input: buildInputField(ptr); break;
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
    long start = ptr->data.button.tiles.first;
    long end = ptr->data.button.tiles.last;

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

        updateGuiTileIndices(&ptr->data.button.tiles, a->cloneindex);
    }

    buildButtonText(ptr);
}

Actor *buildCaret(WindowItem *ptr, Text *pText, BlinkingCaret *caret)
{
    Actor *a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
    a->animpos = 19;
    a->myWindow = ptr->parent->index,
    a->myPanel = ptr->myPanel->index;
    a->myIndex = ptr->index;
    a->myProperties = GEUI_CARET;
    SendActivationEvent(a->clonename);
    ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
    strcpy(caret->actorCName, a->clonename);
    caret->pText = pText;
    colorActorByName(caret->actorCName, ptr->parent->style.textColor);
    updateCaretPosition(caret);

    return a;
}

void buildInputFieldBackground(WindowItem *ptr, TileIndices *tiles)
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

        updateGuiTileIndices(tiles, a->cloneindex);
    }
}

void buildInputField(WindowItem *ptr)
{
    if (ptr->type != GEUI_Input) { DEBUG_MSG_FROM("item is not a valid InputText item", "buildInputText"); return; }

    buildInputFieldBackground(ptr, &ptr->data.input.tiles);
    colorGuiTiles(ptr->data.input.tiles, ptr->parent->style.inputBgColor);

    setTextZDepth(&ptr->data.input.text, DEFAULT_ITEM_ZDEPTH);
    setTextPosition(&ptr->data.input.text,
        getTile(ptr->data.input.tiles.first)->x,
        getTile(ptr->data.input.tiles.last)->y);
    refreshText(&ptr->data.input.text);

    buildCaret(ptr, &ptr->data.input.text, &ptr->data.input.caret);
    hideCaret(&ptr->data.input.caret);
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
            eraseGuiTiles(&ptr->data.button.tiles);
        break;
        case GEUI_Input:
            eraseText(&ptr->data.input.text);
            eraseCaret(&ptr->data.input.caret);
            setTextParent(&ptr->data.input.text, "(none)", False);
            eraseGuiTiles(&ptr->data.input.tiles);
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
            eraseGuiTiles(&ptr->data.button.tiles);
        break;
        case GEUI_Input:
            destroyText(&ptr->data.input.text);
            eraseCaret(&ptr->data.input.caret);
            eraseGuiTiles(&ptr->data.input.tiles);
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
