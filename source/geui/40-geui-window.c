// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

Window *createWindow(char tag[256], Style style);
Window *getWindowByTag(char tag[256]);
Window *getWindowByIndex(int index);
Window *openWindow(char tag[256]);
void buildWindow(Window *window);
Actor *createWindowBaseParent(Window *window);
void setWindowBaseParent(Window *window, char *parentName);
void bringWindowToFront(Window *window);
void closeWindow(Window *window);
void destroyWindow(Window *window);


Window *createWindow(char tag[256], Style style)
{
    Window *ptr = malloc(sizeof *ptr);

    if (!ptr) { DEBUG_MSG_FROM("memory allocation failed", "createWindow"); return NULL; }

    ptr->index = GEUIController.wIndex ++;
    ptr->pIndex = 0;
    strcpy(ptr->tag, tag);
    ptr->isOpen = False;
    ptr->style = style;
    ptr->zDepth = DEFAULT_WINDOW_ZDEPTH;
    strcpy(ptr->parentCName, "");
    ptr->wTileStartIndex = -1;
    ptr->wTileEndIndex = -1;
    ptr->mainPanel.index = ptr->pIndex++;
    ptr->mainPanel.iIndex = 0;
    ptr->mainPanel.rows = 0;
    ptr->mainPanel.cols = 0;
    ptr->mainPanel.width = -1;
    ptr->mainPanel.height = -1;
    ptr->mainPanel.parent = ptr;
    ptr->mainPanel.iList = NULL;
    ptr->next = GEUIController.wList;

    GEUIController.wList = ptr;

    getTileDimensions(&ptr->style);
    ptr->style.tileWidth = defStyle.tileWidth;
    ptr->style.tileHeight = defStyle.tileHeight;

    return ptr;
}

Window *getWindowByTag(char tag[256])
{
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;

        ptr = ptr->next;
    }

    return NULL;
}

Window *getWindowByIndex(int index)
{
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        if (ptr->index == index)
            return ptr;

        ptr = ptr->next;
    }

    return NULL;
}

Window *openWindow(char tag[256])
{
    Window *window = getWindowByTag(tag);

    if (!window) { DEBUG_MSG_FROM("window is NULL", "openWindow"); return NULL; }
    if (window->isOpen) { DEBUG_MSG_FROM("window is already open", "openWindow"); return window; }

    updatePanelLayout(NULL, &window->mainPanel);
    buildWindow(window);
    buildItems(&window->mainPanel);

    window->isOpen = True;
    bringWindowToFront(window);

    return window;
}

void buildWindow(Window *window)
{
    short i, j;
    Actor *tile;

    short tileWidth, tileHeight;
    short windowWidth, windowHeight;
    short tilesHorizontal, tilesVertical;

    setWindowBaseParent(window, createWindowBaseParent(window)->clonename);

    tileWidth = window->style.tileWidth;
    tileHeight = window->style.tileHeight;

    windowWidth = window->mainPanel.width + window->style.tileWidth + window->style.padding * 2;
    windowHeight = window->mainPanel.height + window->style.tileHeight + window->style.padding * 2;

    tilesHorizontal = ceil(windowWidth / (float)tileWidth);
    tilesVertical = ceil(windowHeight / (float)tileHeight);

    for (j = 0; j < tilesVertical; j ++)
    {
        for (i = 0; i < tilesHorizontal; i ++)
        {
            tile = CreateActor("a_gui", window->style.guiAnim,
                               window->parentCName, "(none)", 0, 0, true);
            // TODO: actual positioning
            tile->x = i * tileWidth  + (i >= 2 && i >= tilesHorizontal - 2) * (windowWidth  - tilesHorizontal * tileWidth);
            tile->y = j * tileHeight + (j >= 2 && j >= tilesVertical - 2) * (windowHeight - tilesVertical * tileHeight);
            tile->myWindow = window->index;
            tile->myPanel = window->mainPanel.index;
            tile->myIndex = -1;
            tile->animpos = calculateAnimpos(tilesHorizontal, tilesVertical, i, j);
            colorActor(tile, window->style.windowBgColor);
            ChangeZDepth(tile->clonename, WINDOW_TILE_ZDEPTH);
            EventDisable(tile->clonename, EVENTCOLLISION);
            EventDisable(tile->clonename, EVENTCOLLISIONFINISH);

            if (j == 0) tile->myProperties = GEUI_TITLE_BAR; // part of the window title bar

            updateIndexBounds(&window->wTileStartIndex, &window->wTileEndIndex, tile->cloneindex);
        }
    }
}

Actor *createWindowBaseParent(Window *window)
{
    Actor *baseParent;

    baseParent = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)", 0, 0, true);
    baseParent->animpos = 0;
    ChangeZDepth(baseParent->clonename, window->zDepth);
    CollisionState(baseParent->clonename, DISABLE);

    return baseParent;
}

void setWindowBaseParent(Window *window, char *parentName)
{
    WindowItem *ptr = NULL;

    if (!window) { DEBUG_MSG_FROM("window is NULL", "setWindowBaseParent"); return; }
    if (!actorExists(parentName)) { DEBUG_MSG_FROM("actor doesn't exist", "setWindowBaseParent"); return; }

    strcpy(window->parentCName, parentName);

    if (window->wTileStartIndex > -1)
        changeParentOfClones("a_gui", window->wTileStartIndex, window->wTileEndIndex, parentName);

    setPanelBaseParent(&window->mainPanel, parentName);
}

void bringWindowToFront(Window *window)
{
    Window *ptr = NULL;

    if (!window) { DEBUG_MSG_FROM("window is NULL", "bringWindowToFront"); return; }

    ptr = GEUIController.wList;

    while (ptr)
    {
        if (ptr->index == window->index)
        {
            ptr->zDepth = ACTIVE_WINDOW_ZDEPTH;
            ChangeZDepth(ptr->parentCName, ptr->zDepth);
            GEUIController.topIndex = window->index;
        }
        else
        {
            ptr->zDepth = DEFAULT_WINDOW_ZDEPTH;
            ChangeZDepth(ptr->parentCName, ptr->zDepth);
        }

        ptr = ptr->next;
    }
}

void closeWindow(Window *window)
{
    WindowItem *ptr = NULL;

    if (!window) { DEBUG_MSG_FROM("window is NULL", "closeWindow"); return; }

    window->isOpen = False;

    DestroyActor(window->parentCName);
    strcpy(window->parentCName, "(none)");

    if (window->wTileStartIndex > -1)
    {
        destroyClones("a_gui", window->wTileStartIndex, window->wTileEndIndex);
        window->wTileStartIndex = -1;
        window->wTileEndIndex = -1;
    }

    closePanel(&window->mainPanel);
}

void destroyWindow(Window *window)
{
    closeWindow(window);
    destroyPanel(&window->mainPanel);
    free(window);
}