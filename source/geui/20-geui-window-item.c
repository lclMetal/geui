// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

// from geui-panel.c
void closePanel(Panel *panel);
void destroyPanel(Panel *panel);
void updatePanelLayout(WindowItem *panelItem, Panel *panel);

bool bothPointToSameItem(WindowItem *item1, WindowItem *item2);
void eraseGuiTiles(TileIndices *indices);
WindowItem *initNewItem(ItemType type, Panel *panel, char tag[256]);
WindowItem *addItemToWindow(WindowItem *ptr);
WindowItem *setPosition(WindowItem *this, short row, short col);
WindowItem *getItemFromPanelByTag(Panel *panel, char tag[256]);
WindowItem *getItemByTag(Window *window, char tag[256]);
WindowItem *getItemFromPanelByIndex(Panel *panel, int index);
WindowItem *getItemByIndex(Window *window, int index);
void updateItemLayout(WindowItem *ptr);
void updateItemLayouts(Panel *panel);
void buildItems(Panel *panel);
void buildItem(WindowItem *ptr);
void buildText(WindowItem *ptr);
void buildPanel(WindowItem *ptr);
void buildButtonText(WindowItem *ptr);
void buildButton(WindowItem *ptr);
void buildCheckbox(WindowItem *ptr);
void buildInputField(WindowItem *ptr);
void buildEmbedder(WindowItem *ptr);
void eraseWindowItem(WindowItem *ptr);
void destroyWindowItem(WindowItem *ptr);

bool bothPointToSameItem(WindowItem *item1, WindowItem *item2)
{
    return (item1 && item2 && item1 == item2);
}

WindowItem *initNewItem(ItemType type, Panel *panel, char tag[256])
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
    ptr->parent = panel->parent;
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

WindowItem *setPosition(WindowItem *this, short row, short col)
{
    if (!this) { DEBUG_MSG_FROM("item is NULL", "setPosition"); return NULL; }

    this->layout.row = row;
    this->layout.col = col;

    return this;
}

Panel *getPanel(WindowItem *item)
{
    if (item && item->type == GEUI_Panel)
    {
        return item->data.panel;
    }

    return NULL;
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

    ptr = getItemFromPanelByTag(&window->root, tag);

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

    ptr = getItemFromPanelByIndex(&window->root, index);

    if (ptr)
        return ptr;

    return NULL;
}

void updateItemLayout(WindowItem *ptr)
{
    Actor *actor;
    char temp[256];

    switch (ptr->type)
    {
        case GEUI_Text:
            updateTextDimensions(&ptr->data.text);
            ptr->layout.width = ptr->data.text.width;
            ptr->layout.height = max(ptr->data.text.height + ptr->data.text.pFont->baselineOffset, ptr->parent->style->tileHeight);
        break;
        case GEUI_Button:
            DEBUG_MSG_FROM("Layout updating not implemented for item type: Button", "updateItemLayout");
        break;
        case GEUI_Checkbox:
            DEBUG_MSG_FROM("Layout updating not implemented for item type: Checkbox", "updateItemLayout");
        break;
        case GEUI_Input:
            DEBUG_MSG_FROM("Layout updating not implemented for item type: Input", "updateItemLayout");
        break;
        case GEUI_Panel:
            updateItemLayouts(ptr->data.panel);
        break;
        case GEUI_Embedder:
            actor = getclone(ptr->data.embedder.actorCName);

            if (actorExists2(actor))
            {
                ptr->layout.width = actor->width;
                ptr->layout.height = actor->height;
            }
            else
            {
                DEBUG_MSG_FROM("Actor doesn't exist, layout for Embedder item not updated", "updateItemLayout");
            }
        break;
    }

    sprintf(temp, "Updated layout for item: %s/%s", ptr->parent->tag, ptr->tag);
    DEBUG_MSG_FROM(temp, "updateItemLayout");
}

void updateItemLayouts(Panel *panel)
{
    WindowItem *ptr;

    ptr = panel->iList;

    while (ptr)
    {
        updateItemLayout(ptr);
        ptr = ptr->next;
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
        case GEUI_Checkbox: buildCheckbox(ptr); break;
        case GEUI_Input: buildInputField(ptr); break;
        case GEUI_Panel: buildPanel(ptr); break;
        case GEUI_Embedder: buildEmbedder(ptr); break;
    }
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
        case GEUI_Checkbox:
            DestroyActor(getTile(ptr->data.checkbox.tileIndex)->clonename);
            ptr->data.checkbox.tileIndex = -1;
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
        case GEUI_Checkbox:
            DestroyActor(getTile(ptr->data.checkbox.tileIndex)->clonename);
            ptr->data.checkbox.tileIndex = -1;
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
