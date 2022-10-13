// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

Window *createWindow(char tag[256], Style style);
Window *getWindowByTag(char tag[256]);
Window *getWindowByIndex(int index);
Window *getFirstOpenWindow();
Window *openWindow(char tag[256], WindowPosition pos);
void buildWindow(Window *window, WindowPosition pos);
Actor *createWindowBaseParent(Window *window, WindowPosition pos);
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
    ptr->fakeIndex = -1;
    ptr->style = style;
    ptr->zDepth = DEFAULT_WINDOW_ZDEPTH;
    strcpy(ptr->parentCName, "");
    ptr->tiles = noIndices;
    ptr->root.index = ptr->pIndex++;
    ptr->root.iIndex = 0;
    ptr->root.rows = 0;
    ptr->root.cols = 0;
    ptr->root.width = -1;
    ptr->root.height = -1;
    ptr->root.parent = ptr;
    ptr->root.iList = NULL;
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

Panel *getWindowRootPanel(Window *window)
{
    if (window)
    {
        return &window->root;
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

WindowPosition createWPos(float x, float y)
{
    WindowPosition pos;
    pos.type = GEUI_WPosCoords;
    pos.pos = createScreenCoords(x, y);
    return pos;
}

#define GEUI_WPOS_MOUSE_CENTER      createWPosAtMouse(GEUI_WPosMouseCenter)
#define GEUI_WPOS_MOUSE_TOP_LEFT    createWPosAtMouse(GEUI_WPosMouseTopLeft)
#define GEUI_WPOS_MOUSE_TOP         createWPosAtMouse(GEUI_WPosMouseTop)

WindowPosition createWPosAtMouse(WPosSetting mode)
{
    WindowPosition pos;

    switch (mode)
    {
        case GEUI_WPosMouseCenter:
        case GEUI_WPosMouseTopLeft:
        case GEUI_WPosMouseTop:     pos.type = mode;                    break;
        default:                    pos.type = GEUI_WPosMouseTopLeft;   break;
    }

    pos.pos = createScreenCoords(0, 0);
    return pos;
}

#define GEUI_WPOS_SCREEN_CENTER createWPosAtScreenCenter()

WindowPosition createWPosAtScreenCenter()
{
    WindowPosition pos;
    pos.type = GEUI_WPosScreenCenter;
    pos.pos = createScreenCoords(0, 0);
    return pos;
}

Window *openWindow(char tag[256], WindowPosition pos)
{
    Window *window = getWindowByTag(tag);

    if (!window) { DEBUG_MSG_FROM("window is NULL", "openWindow"); return NULL; }
    if (window->isOpen) { DEBUG_MSG_FROM("window is already open", "openWindow"); return window; }

    updatePanelLayout(NULL, &window->root);
    buildWindow(window, pos);
    buildItems(&window->root);

    window->isOpen = True;
    bringWindowToFront(window);

    return window;
}

void buildWindow(Window *window, WindowPosition pos)
{
    short i, j;
    Actor *tile;

    short tileWidth, tileHeight;
    short windowWidth, windowHeight;
    short tilesHorizontal, tilesVertical;

    setWindowBaseParent(window, createWindowBaseParent(window, pos)->clonename);

    tileWidth = window->style.tileWidth;
    tileHeight = window->style.tileHeight;

    windowWidth = window->root.width + window->style.tileWidth + window->style.padding * 2;
    windowHeight = window->root.height + window->style.tileHeight + window->style.padding * 2;

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
            tile->myPanel = window->root.index;
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

Actor *createWindowBaseParent(Window *window, WindowPosition pos)
{
    Actor *baseParent;
    ScreenCoords realPos;

    switch (pos.type)
    {
        case GEUI_WPosCoords:       realPos = pos.pos;                          break;
        case GEUI_WPosMouseCenter:  // all mouse related positionings are handled based on the
        case GEUI_WPosMouseTopLeft: // WindowPosition type, so the control is allowed to fall-through
        case GEUI_WPosMouseTop:     realPos = getWPosAtMouse(window, pos.type); break;
        case GEUI_WPosScreenCenter: realPos = getWPosAtScreenCenter(window);    break;
        default: realPos = getWPosAtScreenCenter(window);                       break;
    }

    baseParent = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)", view.x + realPos.x, view.y + realPos.y, true);
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
    setPanelBaseParent(&window->root, parentName);
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

    if (window->fakeIndex > -1)
    {
        DestroyActor(getTile(window->fakeIndex)->clonename);
        window->fakeIndex = -1;
    }

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

    closePanel(&window->root);
}

void destroyWindow(Window *window)
{
    closeWindow(window);
    destroyPanel(&window->root);
    free(window);
}
