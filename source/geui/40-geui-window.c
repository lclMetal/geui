// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

#define GEUI_SCREEN_CENTER -1, -1
#define GEUI_MOUSE_POSITION xmouse, ymouse

Window *createWindow(char tag[256], Style style);
Window *getWindowByTag(char tag[256]);
Window *getWindowByIndex(int index);
Window *getFirstOpenWindow();
Window *openWindow(char tag[256], float startX, float startY);
void buildWindow(Window *window, float startX, float startY);
Actor *createWindowBaseParent(Window *window, float startX, float startY);
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
    ptr->tiles = noIndices;
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

Window *getFirstOpenWindow()
{
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        if (ptr->isOpen)
        {
            return ptr;
        }

        ptr = ptr->next;
    }

    return NULL;
}

Window *openWindow(char tag[256], float startX, float startY)
{
    Window *window = getWindowByTag(tag);

    if (!window) { DEBUG_MSG_FROM("window is NULL", "openWindow"); return NULL; }
    if (window->isOpen) { DEBUG_MSG_FROM("window is already open", "openWindow"); return window; }

    updatePanelLayout(NULL, &window->mainPanel);
    buildWindow(window, startX, startY);
    buildItems(&window->mainPanel);

    window->isOpen = True;
    bringWindowToFront(window);

    return window;
}

void buildWindow(Window *window, float startX, float startY)
{
    short i, j;
    Actor *tile;

    short tileWidth, tileHeight;
    short windowWidth, windowHeight;
    short tilesHorizontal, tilesVertical;

    setWindowBaseParent(window, createWindowBaseParent(window, startX, startY)->clonename);

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

            updateGuiTileIndices(&window->tiles, tile->cloneindex);
        }
    }
}

Actor *createWindowBaseParent(Window *window, float startX, float startY)
{
    Actor *baseParent;
    float posX = startX;
    float posY = startY;

    // Magic values to indicate that the window should be centered
    if (startX == -1 && startY == -1)
    {
        posX = view.width * 0.5f - window->mainPanel.width * 0.5f;
        posY = view.height * 0.5f - window->mainPanel.height * 0.5f;
    }

    baseParent = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)", view.x + posX, view.y + posY, true);
    baseParent->animpos = 0;
    baseParent->myWindow = window->index;
    baseParent->myPanel = -1;
    baseParent->myIndex = -1;
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

    changeParentOfClones("a_gui", window->tiles.first, window->tiles.last, parentName);
    setPanelBaseParent(&window->mainPanel, parentName);
}

void bringWindowToFront(Window *window)
{
    Window *ptr = NULL;
    Window *prev = NULL;

    if (!window) { DEBUG_MSG_FROM("window is NULL", "bringWindowToFront"); return; }

    ptr = GEUIController.wList;
    blurItem(GEUIController.focus);

    while (ptr)
    {
        if (ptr->index == window->index)
        {
            ptr->zDepth = ACTIVE_WINDOW_ZDEPTH;
            ChangeZDepth(ptr->parentCName, ptr->zDepth);
            GEUIController.topIndex = window->index;

            if (prev)
            {
                prev->next = ptr->next;
                ptr->next = GEUIController.wList;
                GEUIController.wList = ptr;
                ptr = prev;
            }
        }
        else
        {
            ptr->zDepth = DEFAULT_WINDOW_ZDEPTH;
            ChangeZDepth(ptr->parentCName, ptr->zDepth);
        }

        prev = ptr;
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

    eraseGuiTiles(&window->tiles);

    if (window->index == GEUIController.topIndex)
    {
        Window *nextTopWindow = getFirstOpenWindow();

        if (nextTopWindow)
        {
            GEUIController.topIndex = nextTopWindow->index;
        }
        else
        {
            GEUIController.topIndex = -1;
        }
    }

    closePanel(&window->mainPanel);
}

void destroyWindow(Window *window)
{
    closeWindow(window);
    destroyPanel(&window->mainPanel);
    free(window);
}
