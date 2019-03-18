/*enum resMouseButtonsEnum // TODO: add mouse buttons
{
    RES_MOUSE_LEFT = 0,
    RES_MOUSE_RIGHT,
    RES_MOUSE_MIDDLE,
    RES_MOUSE_WHEEL_UP,
    RES_MOUSE_WHEEL_DOWN,
    RES_MOUSE_BUTTONS     // Number of supported mouse buttons (5)
};*/
#define DEFAULT_ZDEPTH 0.51
#define USE_DEFAULT_STYLE GEUIController.sDefault

enum guiPropertyFlags
{
    GEUI_TITLE_BAR  = (1 << 0),
    GEUI_FAKE_ACTOR = (1 << 1),
    GEUI_CLICKED    = (1 << 2)
};

typedef enum ItemTypeEnum
{
    GEUI_Text,
    GEUI_Button
}ItemType;

struct WindowStruct;

typedef struct WindowItemStruct
{
    int index;          // item index
    char tag[256];      // item identifier tag

    ItemType type;      // item type

    union ItemDataUnion // item data union for different item types
    {
        struct TextItem     { Text text; }text;
        struct ButtonItem
        {
            Text text;
            short state;
            int bActorStartIndex;
            int bActorEndIndex;
            void (*actionFunction)(struct WindowStruct *, struct WindowItemStruct *);
        }button;
    }data;

    //GEUI_Item item;     // item container

    struct WindowStruct *parent;     // pointer to parent window
    struct WindowItemStruct *next;   // pointer to next item in list
}WindowItem;

typedef struct WindowStruct
{
    int index;          // window index
    int iIndex;         // next available item index
    char tag[256];      // window identifier tag
    bool isOpen;        // is window currently open or not
    Style style;        // window style
    double zDepth;      // window z depth
    char parentCName[256]; // clonename of the window parent actor
    int wTileStartIndex;   // cloneindex of the first window tile
    int wTileEndIndex;     // cloneindex of the last window tile
    struct WindowItemStruct *iList;  // list of items in window
    struct WindowStruct *next;       // pointer to next window in list
}Window;

void initGEUI(void);
int mouseIsOverClones(int startIndex, int endIndex);
WindowItem *initNewItem(ItemType type, Window *window, char tag[256]);
WindowItem *addItemToWindow(Window *window, WindowItem *ptr);
WindowItem *addText(Window *window, char tag[256], char *string);
WindowItem *addButton(Window *window, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *));
WindowItem *searchItemByTag(Window *window, char tag[256]);
WindowItem *searchItemByIndex(Window *window, int index);
void doMouseEnter(const char *actorName);
void doMouseLeave(const char *actorName);
void doMouseButtonDown(const char *actorName, short mButton);
void doMouseButtonUp(const char *actorName, short mButton);
void doKeyDown(Window *window, WindowItem *item, short key);
void doKeyUp(Window *window, WindowItem *item, short key);
void eraseWindowItem(WindowItem *ptr);
void destroyWindowItem(WindowItem *ptr);
int calculateAnimpos(unsigned short w, unsigned short h, unsigned short i, unsigned short j);
Window *createWindow(char tag[256], Style style);
Window *searchWindowByTag(char tag[256]);
Window *searchWindowByIndex(int index);
Window *openWindow(char tag[256]);
void setWindowBaseParent(Window *window, char *parentName);
void closeWindow(Window *window);
void destroyWindow(Window *window);
void destroyWindowList(void);
void destroyWindowItemList(Window *window);

struct GEUIControllerStruct
{
    int wIndex;
    Style sDefault;
    Window *wList;
}GEUIController;

void initGEUI(void)
{
    //DEBUG_INIT(); // debug file initialization on startup disabled for now
    // TODO: separate DEBUG_INIT() and DEBUG_INIT_FILE()

    strcpy(defStyle.guiAnim, "gui_sheet_default");  // GUI animation name
    defStyle.titleFont          = &defTitleFont;    // title font
    defStyle.labelFont          = &defLabelFont;    // label font
    defStyle.textFont           = &defTextFont;     // text font
    defStyle.padding            = 10;               // padding in pixels
    defStyle.titleBgColor       = DEFAULT_COLOR;    // title background color
    defStyle.windowBgColor      = DEFAULT_COLOR;    // window background color
    defStyle.titleColor         = BLACK;            // title color
    defStyle.labelColor         = BLACK;            // label color
    defStyle.textColor          = BLACK;            // text color
    defStyle.buttonColor        = DEFAULT_COLOR;    // button color
    defStyle.buttonHilitColor   = CYAN;             // button hilit color
    defStyle.buttonPressedColor = BLUE;             // button pressed color

    GEUIController.wIndex = 0;
    GEUIController.sDefault = defStyle;
    GEUIController.wList = NULL;
}

int mouseIsOverClones(int startIndex, int endIndex)
{
    int count;
    Actor *actors;

    actors = getAllActorsInCollision("a_geuiMouse.0", &count);

    if (actors)
    {
        int i;

        for (i = 0; i < count; i ++)
        {
            if (!strcmp(actors[i].name, "a_gui") &&
                actors[i].cloneindex >= startIndex &&
                actors[i].cloneindex <= endIndex)
                return 1;
        }
    }

    return 0;
}

WindowItem *initNewItem(ItemType type, Window *window, char tag[256])
{
    WindowItem *ptr = NULL;

    if (!window) return NULL;

    ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->type = type;
    ptr->index = window->iIndex ++;
    strcpy(ptr->tag, tag);

    return ptr;
}

WindowItem *addItemToWindow(Window *window, WindowItem *ptr)
{
    if (!window || !ptr) return NULL;

    ptr->next = window->iList;
    window->iList = ptr;

    return ptr;
}

WindowItem *addText(Window *window, char tag[256], char *string)
{
    WindowItem *ptr = initNewItem(GEUI_Text, window, tag);
    if (!ptr) return NULL;

    ptr->data.text.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.text.text, window->style.textColor);

    return addItemToWindow(window, ptr);
}

WindowItem *addButton(Window *window, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *))
{
    WindowItem *ptr = initNewItem(GEUI_Button, window, tag);
    if (!ptr) return NULL;

    ptr->data.button.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.button.text, window->style.textColor);
    ptr->data.button.state = 0;
    ptr->data.button.bActorStartIndex = -1;
    ptr->data.button.bActorEndIndex = -1;
    ptr->data.button.actionFunction = actionFunction;

    return addItemToWindow(window, ptr);
}

WindowItem *searchItemByTag(Window *window, char tag[256])
{
    WindowItem *ptr = NULL;

    if (!window) return NULL;

    ptr = window->iList;

    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;

        ptr = ptr->next;
    }

    DEBUG_MSG_FROM("item not found", "searchItemByTag");
    return NULL;
}

WindowItem *searchItemByIndex(Window *window, int index)
{
    WindowItem *ptr = NULL;

    if (!window) return NULL;

    ptr = window->iList;

    while (ptr)
    {
        if (ptr->index == index)
            return ptr;

        ptr = ptr->next;
    }

    DEBUG_MSG_FROM("item not found", "searchItemByIndex");
    return NULL;
}

void doMouseEnter(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0 || actor->myIndex < 0) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (item->data.button.state)
                colorClones("a_gui",
                    item->data.button.bActorStartIndex,
                    item->data.button.bActorEndIndex, window->style.buttonPressedColor);
            else
                colorClones("a_gui",
                    item->data.button.bActorStartIndex,
                    item->data.button.bActorEndIndex, window->style.buttonHilitColor);
        break;
    }
}

void doMouseLeave(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0 || actor->myIndex < 0) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (!mouseIsOverClones(item->data.button.bActorStartIndex,
                item->data.button.bActorEndIndex))
            {
                if (item->data.button.state)
                    colorClones("a_gui",
                        item->data.button.bActorStartIndex,
                        item->data.button.bActorEndIndex, window->style.buttonPressedColor);
                else
                    colorClones("a_gui",
                        item->data.button.bActorStartIndex,
                        item->data.button.bActorEndIndex, window->style.buttonColor);
            }
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonDown(const char *actorName, short mButton)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;

    if (actor->myProperties & GEUI_TITLE_BAR)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        ChangeParent(actor->clonename, "(none)");

        // store the current color of the event actor
        actor->myColorR = actor->r;
        actor->myColorG = actor->g;
        actor->myColorB = actor->b;
        //VisibilityState(actor->clonename, DONT_DRAW_ONLY);

        // make the event actor white to keep children's colors unchanged after parenting
        actor->r = actor->g = actor->b = 255;

        // create fake actor to cover the now white event actor
        fake = CreateActor("a_gui", window->style.guiAnim, window->parentCName, "(none)", 0, 0, false);
        fake->myWindow = window->index;
        fake->myProperties = GEUI_FAKE_ACTOR;
        actor->myFakeIndex = fake->cloneindex;
        SendActivationEvent(gc2("a_gui", actor->myFakeIndex)->clonename);

        wParent = getclone(window->parentCName);

        // calculate the window base parent's coordinates relative to the event actor
        xDist = ceil(wParent->x - x);
        yDist = ceil(wParent->y - y);

        // make event actor the parent of the window base parent
        ChangeParent(wParent->clonename, actor->clonename);
        wParent->x = xDist;
        wParent->y = yDist;

        actor->myProperties |= GEUI_CLICKED;
        FollowMouse(actor->clonename, BOTH_AXIS);
    }

    if (actor->myIndex < 0 || !(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            colorClones("a_gui",
                item->data.button.bActorStartIndex,
                item->data.button.bActorEndIndex, window->style.buttonPressedColor);
            item->data.button.state = mButton + 1;
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonUp(const char *actorName, short mButton)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;

    if (actor->myProperties & GEUI_TITLE_BAR && actor->myProperties & GEUI_CLICKED)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        wParent = getclone(window->parentCName);

        // reset window base parent's parent to none
        ChangeParent(wParent->clonename, "(none)");

        // calculate the event actor's coordinates relative to window the base parent
        xDist = ceil(actor->x - wParent->x);
        yDist = ceil(actor->y - wParent->y);

        // make the window base parent the parent of the event actor
        ChangeParent(actor->clonename, wParent->clonename);
        actor->x = xDist;
        actor->y = yDist;

        actor->myProperties &= ~GEUI_CLICKED;
        FollowMouse(actor->clonename, NONE_AXIS);

        // restore the event actor's original color
        actor->r = actor->myColorR;
        actor->g = actor->myColorG;
        actor->b = actor->myColorB;
        //VisibilityState(actor->clonename, ENABLE);

        // destroy the fake actor
        DestroyActor(gc2("a_gui", actor->myFakeIndex)->clonename);
    }

    if (actor->myIndex < 0 || !(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (mouseIsOverClones(item->data.button.bActorStartIndex,
                item->data.button.bActorEndIndex))
            {
                colorClones("a_gui",
                    item->data.button.bActorStartIndex,
                    item->data.button.bActorEndIndex, window->style.buttonHilitColor);
                if (item->data.button.state)
                    item->data.button.actionFunction(window, item);
            }
            else
            {
                colorClones("a_gui",
                    item->data.button.bActorStartIndex,
                    item->data.button.bActorEndIndex, window->style.buttonColor);
            }
            item->data.button.state = 0;
        break;
    }
}

void doKeyDown(Window *window, WindowItem *item, short key)
{
    // TODO: implement doKeyDown
}

void doKeyUp(Window *window, WindowItem *item, short key)
{
    // TODO: implement doKeyUp
}

void eraseWindowItem(WindowItem *ptr)
{
    if (!ptr) return;

    switch (ptr->type)
    {
        case GEUI_Text:
            eraseText(&ptr->data.text.text);
            setTextParent(&ptr->data.text.text, "(none)", False);
        break;
        case GEUI_Button:
            eraseText(&ptr->data.button.text);
            if (ptr->data.button.bActorStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bActorStartIndex, ptr->data.button.bActorEndIndex);
                ptr->data.button.bActorStartIndex = -1;
                ptr->data.button.bActorEndIndex = -1;
            }
        break;

        default: break;
    }
}

void destroyWindowItem(WindowItem *ptr)
{
    if (!ptr) return;

    switch (ptr->type)
    {
        case GEUI_Text: destroyText(&ptr->data.text.text); break;
        case GEUI_Button:
            destroyText(&ptr->data.button.text);
            if (ptr->data.button.bActorStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bActorStartIndex, ptr->data.button.bActorEndIndex);
                ptr->data.button.bActorStartIndex = -1;
                ptr->data.button.bActorEndIndex = -1;
            }
        break;

        default: break;
    }
}

int calculateAnimpos(unsigned short w, unsigned short h, unsigned short i, unsigned short j)
{
    unsigned short pw = (i) ? (i / (w - 1)) + 1 : 0; // column  0, 1 or 2
    unsigned short ph = (j) ? (j / (h - 1)) + 1 : 0; //    row  0, 1 or 2

    // Array of possible outcomes:
    // 0, 1, 2,
    // 3, 4, 5,
    // 6, 7, 8

    return ph * 3 + pw;

    // The values given to different parts of a window:
    // 0, 1, 1, 1, 2,
    // 3, 4, 4, 4, 5,
    // 3, 4, 4, 4, 5,
    // 6, 7, 7, 7, 8
}

Window *createWindow(char tag[256], Style style)
{
    Window *ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->index = GEUIController.wIndex ++;
    ptr->iIndex = 0;
    strcpy(ptr->tag, tag);
    ptr->isOpen = False;
    ptr->style = style;
    ptr->zDepth = DEFAULT_ZDEPTH;
    ptr->wTileStartIndex = -1;
    ptr->wTileEndIndex = -1;
    ptr->iList = NULL;
    ptr->next = GEUIController.wList;

    GEUIController.wList = ptr;

    return ptr;
}

Window *searchWindowByTag(char tag[256])
{
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;

        ptr = ptr->next;
    }

    DEBUG_MSG_FROM("window not found", "searchWindowByTag");
    return NULL;
}

Window *searchWindowByIndex(int index)
{
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        if (ptr->index == index)
            return ptr;

        ptr = ptr->next;
    }

    DEBUG_MSG_FROM("window not found", "searchWindowByIndex");
    return NULL;
}

Window *openWindow(char tag[256])
{
    int i, j;
    Actor *guiActor = NULL;
    unsigned short width = 0, height = 0;
    unsigned short tileWidth = 0, tileHeight = 0;
    unsigned short tilesH = 0, tilesV = 0;
    Window *window = searchWindowByTag(tag);
    WindowItem *ptr = NULL;

    if (!window) {DEBUG_MSG_FROM("window is NULL", "openWindow"); return NULL;}
    if (window->isOpen) { DEBUG_MSG_FROM("window is already open", "openWindow"); return window; }

    ptr = window->iList;

    while (ptr)
    {
        switch (ptr->type)
        {
            case GEUI_Text:
                width = ptr->data.text.text.width;
                height = ptr->data.text.text.height;
                refreshText(&ptr->data.text.text);
            break;

            case GEUI_Button:
                for (i = 0; i * 20 < ptr->data.button.text.width + window->style.padding * 2; i ++)
                {
                    Actor *a = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)", -20 + i * 20, -20, true);
                    a->myWindow = window->index;
                    a->myIndex = ptr->index;

                    if (i == 0)a->animpos = 9; // TODO: calculate actual values
                    else a->animpos = 10;

                    if (ptr->data.button.bActorStartIndex == -1)
                        ptr->data.button.bActorStartIndex = a->cloneindex;
                    if (ptr->data.button.bActorEndIndex < a->cloneindex)
                        ptr->data.button.bActorEndIndex = a->cloneindex;
                }

                colorClones("a_gui",
                    ptr->data.button.bActorStartIndex,
                    ptr->data.button.bActorEndIndex, window->style.buttonColor);
                setTextParent(&ptr->data.button.text,
                    gc2("a_gui", ptr->data.button.bActorEndIndex)->clonename, False);
                setTextAlignment(&ptr->data.button.text, ALIGN_CENTER);
                setTextPosition(&ptr->data.button.text,
                     ceil((gc2("a_gui", ptr->data.button.bActorStartIndex)->x - gc2("a_gui", ptr->data.button.bActorEndIndex)->x)*0.5), 0); // TODO: actual calculations
                refreshText(&ptr->data.button.text);
            break;

            default: break;
        }

        ptr = ptr->next;
    }

    guiActor = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)", 0, 0, true);
    tileWidth = guiActor->width;
    tileHeight = guiActor->height;
    tilesH = ceil(width / (float)tileWidth);
    tilesV = ceil(height / (float)tileHeight);
    ChangeZDepth(guiActor->clonename, window->zDepth);
    VisibilityState(window->parentCName, DISABLE);

    setWindowBaseParent(window, guiActor->clonename);

    for (j = 0; j < tilesV; j ++)
    {
        for (i = 0; i < tilesH; i ++)
        {
            guiActor = CreateActor("a_gui", window->style.guiAnim, window->parentCName, "(none)",
                i * tileWidth, j * tileHeight, true);
            guiActor->myWindow = window->index;
            guiActor->myIndex = -1;
            guiActor->animpos = calculateAnimpos(tilesH, tilesV, i, j);
            colorActor(guiActor, window->style.windowBgColor);
            ChangeZDepth(guiActor->clonename, window->zDepth);
            if (j == 0) guiActor->myProperties = GEUI_TITLE_BAR; // window title bar

            if (window->wTileStartIndex == -1)
                window->wTileStartIndex = guiActor->cloneindex;
            if (window->wTileEndIndex < guiActor->cloneindex)
                window->wTileEndIndex = guiActor->cloneindex;
        }
    }

    window->isOpen = True;

    return window;
}

void setWindowBaseParent(Window *window, char *parentName)
{
    WindowItem *ptr = NULL;

    if (!window) {DEBUG_MSG_FROM("window is NULL", "setWindowBaseParent"); return;}
    if (!actorExists(parentName)) {DEBUG_MSG_FROM("actor does not exist!", "setWindowBaseParent"); return;}

    strcpy(window->parentCName, parentName);

    if (window->wTileStartIndex > -1)
        changeParentOfClones("a_gui", window->wTileStartIndex, window->wTileEndIndex, parentName);

    ptr = window->iList;

    while (ptr)
    {
        switch (ptr->type)
        {
            case GEUI_Text: setTextParent(&ptr->data.text.text, parentName, True); break;
            case GEUI_Button:
                //setTextParent(&ptr->data.button.text, parentName, False); // TODO: is this needed?
                if (ptr->data.button.bActorStartIndex > -1)
                    changeParentOfClones("a_gui", ptr->data.button.bActorStartIndex, ptr->data.button.bActorEndIndex, parentName);
            break;

            default: break;
        }

        ptr = ptr->next;
    }
}

void closeWindow(Window *window)
{
    WindowItem *ptr = NULL;

    if (!window) {DEBUG_MSG_FROM("window is NULL", "openWindow"); return;}

    window->isOpen = False;

    DestroyActor(window->parentCName);
    strcpy(window->parentCName, "(none)");

    if (window->wTileStartIndex > -1)
    {
        destroyClones("a_gui", window->wTileStartIndex, window->wTileEndIndex);
        window->wTileStartIndex = -1;
        window->wTileEndIndex = -1;
    }

    if (!window->iList) return;

    ptr = window->iList;

    while (ptr)
    {
        eraseWindowItem(ptr);
        ptr = ptr->next;
    }
}

void destroyWindow(Window *window)
{
    // TODO: implement destroyWindow
}

void destroyWindowList(void)
{
    Window *temp = NULL;
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        temp = ptr->next;
        closeWindow(ptr);
        destroyWindowItemList(ptr);
        free(ptr);
        ptr = temp;
    }

    GEUIController.wList = NULL;
    GEUIController.wIndex = 0;
}

void destroyWindowItemList(Window *window)
{
    WindowItem *temp = NULL;
    WindowItem *ptr = NULL;

    if (!window || !window->iList) return;

    ptr = window->iList;

    while (ptr)
    {
        destroyWindowItem(ptr);
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }

    window->iList = NULL;
    window->iIndex = 0;
}
