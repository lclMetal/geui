#define DEFAULT_ZDEPTH 0.5
#define USE_DEFAULT_STYLE GEUIController.sDefault

#define GEUI_MOUSE_UP 0
#define GEUI_MOUSE_DOWN 1

#define GEUI_CLONENAME_SIZE 42

enum mouseButtonsEnum // Used as array indices, don't touch!
{
    GEUI_MOUSE_LEFT = 0,
    GEUI_MOUSE_RIGHT,
    GEUI_MOUSE_MIDDLE,
    GEUI_MOUSE_WHEEL_UP,
    GEUI_MOUSE_WHEEL_DOWN,
    GEUI_MOUSE_BUTTONS     // Number of supported mouse buttons (5)
};

typedef struct LayoutStruct
{
    short row;
    short col;
    short width;
    short height;
    short startx;
    short starty;
}Layout;

const unsigned long GEUI_TITLE_BAR  = (1 << 0);
const unsigned long GEUI_FAKE_ACTOR = (1 << 1);
const unsigned long GEUI_CLICKED    = (1 << 2);

typedef enum ItemTypeEnum
{
    GEUI_Text,
    GEUI_Button,
    GEUI_Panel
}ItemType;

struct WindowStruct;
struct PanelStruct;

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
            char state;
            long bTileStartIndex;
            long bTileEndIndex;
            void (*actionFunction)(struct WindowStruct *, struct WindowItemStruct *);
        }button;
        struct PanelItem
        {
            struct PanelStruct *panel;
        }panel;
    }data;

    Layout layout;
    struct PanelStruct *myPanel;
    struct WindowStruct *parent;     // pointer to parent window
    struct WindowItemStruct *next;   // pointer to next item in list
}WindowItem;

typedef struct PanelStruct
{
    int iIndex;
    Layout layout;
    struct WindowItemStruct *iList;
}Panel;

void updatePanelLayout(Panel *panel);
void setPosition(WindowItem *this, short row, short col);
short getColWidth(Panel *panel, short col);
short getRowHeight(Panel *panel, short row);
short getPanelWidth(Panel *panel);
short getPanelHeight(Panel *panel);

typedef struct WindowStruct
{
    int index;          // window index
    // int iIndex;         // next available item index
    char tag[256];      // window identifier tag
    bool isOpen;        // is window currently open or not
    Style style;        // window style
    double zDepth;      // window z depth
    // Layout layout;
    char parentCName[256]; // clonename of the window parent actor
    long wTileStartIndex;   // cloneindex of the first window tile
    long wTileEndIndex;     // cloneindex of the last window tile
    Panel mainPanel;
    // struct WindowItemStruct *iList;  // list of items in window
    struct WindowStruct *next;       // pointer to next window in list
}Window;

void initGEUI(void);
void quitGEUI(void);
Actor *getTile(long index);
void updateIndexBounds(long *low, long *hi, long val);
int isTopmostItemAtMouse(WindowItem *item);
WindowItem *initNewItem(ItemType type, Window *window, Panel *panel, char tag[256]);
WindowItem *addItemToWindow(WindowItem *ptr);
WindowItem *addText(Window *window, Panel *panel, char tag[256], char *string);
WindowItem *addButton(Window *window, Panel *panel, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *));
WindowItem *getItemFromPanelByTag(Panel *panel, char tag[256]);
WindowItem *getItemByTag(Window *window, char tag[256]);
WindowItem *getItemFromPanelByIndex(Panel *panel, int index);
WindowItem *getItemByIndex(Window *window, int index);
void doMouseEnter(const char *actorName);
void doMouseLeave(const char *actorName);
void doMouseButtonDown(const char *actorName, enum mouseButtonsEnum mButtonNumber);
void doMouseButtonUp(const char *actorName, enum mouseButtonsEnum mButtonNumber);
int onMouseClick(enum mouseButtonsEnum mButtonNumber);
int onMouseRelease(enum mouseButtonsEnum mButtonNumber);
int onMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
int onMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
int isMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
int isMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
int updateMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
void updateMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
void doKeyDown(WindowItem *item, short key);
void doKeyUp(WindowItem *item, short key);
void eraseWindowItem(WindowItem *ptr);
void destroyWindowItem(WindowItem *ptr);
int calculateAnimpos(short w, short h, short i, short j);
Window *createWindow(char tag[256], Style style);
Window *getWindowByTag(char tag[256]);
Window *getWindowByIndex(int index);
void buildItems(Panel *panel);
void buildItem(WindowItem *ptr);
void buildText(WindowItem *ptr);
void buildPanel(WindowItem *ptr);
void buildButtonText(WindowItem *ptr);
void buildButton(WindowItem *ptr);
void buildWindow(Window *window);
Window *openWindow(char tag[256]);
Actor *createWindowBaseParent(Window *window);
void setPanelBaseParent(Panel *panel, char *parentName);
void setWindowBaseParent(Window *window, char *parentName);
void bringWindowToFront(Window *window);
void closePanel(Panel *panel);
void closeWindow(Window *window);
void destroyWindow(Window *window);
void destroyWindowList(void);
void destroyPanel(Panel *panel);
// void destroyWindowItemList(Window *window);

struct GEUIControllerStruct
{
    int wIndex;
    int topIndex;
    Style sDefault;
    Window *wList;

    Actor *mButtonActors[GEUI_MOUSE_BUTTONS];   // Clicked actors by mouse button
    char mButtonTopActorCName[GEUI_MOUSE_BUTTONS][GEUI_CLONENAME_SIZE]; // ...and of those the one
                                                                        // with the highest z depth
    int mButtonActorCount[GEUI_MOUSE_BUTTONS];  // Amount of clicked actors by mouse button
    int mButtonState[GEUI_MOUSE_BUTTONS];       // Mouse button states
    enum mouseButtonsEnum activeButton;         // The last active mouse button
}GEUIController;

void initGEUI(void)
{
    //DEBUG_INIT(); // debug file initialization on startup disabled for now
    // TODO: separate DEBUG_INIT() and DEBUG_INIT_FILE()

    strcpy(defStyle.guiAnim, "gui_sheet_default");  // GUI animation name
    getTileDimensions(&defStyle);
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
    GEUIController.topIndex = 0;
    GEUIController.sDefault = defStyle;
    GEUIController.wList = NULL;
}

void quitGEUI(void)
{
    int mb;

    for (mb = 0; mb < GEUI_MOUSE_BUTTONS; mb ++)
    {
        if (GEUIController.mButtonActors[mb])
            free(GEUIController.mButtonActors[mb]);
    }

    destroyWindowList();
}

Actor *getTile(long index)
{
    return gc2("a_gui", index);
}

void updateIndexBounds(long *low, long *hi, long val)
{
    if (*low == -1)
        *low = val;
    if (*hi < val)
        *hi = val;
}

int isTopmostItemAtMouse(WindowItem *item)
{
    int count;
    Actor *actors;

    actors = getAllActorsInCollision("a_geuiMouse.0", &count);

    if (actors)
    {
        int i;

        for (i = count - 1; i >= 0; i --)
        {
            if (!strcmp(actors[i].name, "a_gui"))
            {
                return (actors[i].myIndex > -1 &&
                        actors[i].myWindow == item->parent->index &&
                        actors[i].myIndex == item->index);
            }
        }
    }

    return 0;
}

WindowItem *initNewItem(ItemType type, Window *window, Panel *panel, char tag[256])
{
    WindowItem *ptr = NULL;

    if (!panel) return NULL;

    ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->type = type;
    ptr->index = panel->iIndex ++;
    strcpy(ptr->tag, tag);
    ptr->myPanel = panel;
    ptr->parent = window;

    return ptr;
}

WindowItem *addItemToWindow(WindowItem *ptr)
{
    if (!ptr) return NULL;

    ptr->next = ptr->myPanel->iList;
    ptr->myPanel->iList = ptr;

    return ptr;
}

WindowItem *addText(Window *window, Panel *panel, char tag[256], char *string)
{
    WindowItem *ptr = initNewItem(GEUI_Text, window, panel, tag);
    if (!ptr) return NULL;

    ptr->data.text.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.text.text, window->style.textColor);
    setTextZDepth(&ptr->data.text.text, 0.6);

    ptr->layout.row = 0;
    ptr->layout.col = 0;
    ptr->layout.width = ptr->data.text.text.width;
    ptr->layout.height = ptr->data.text.text.height;
    ptr->layout.startx = 0;
    ptr->layout.starty = 0;

    return addItemToWindow(ptr);
}

WindowItem *addButton(Window *window, Panel *panel, char tag[256], char *string, void (*actionFunction)(Window *, WindowItem *))
{
    WindowItem *ptr = initNewItem(GEUI_Button, window, panel, tag);
    if (!ptr) return NULL;

    ptr->data.button.text = createText(string, window->style.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.button.text, window->style.textColor);
    setTextZDepth(&ptr->data.button.text, 0.5);
    ptr->data.button.state = 0;
    ptr->data.button.bTileStartIndex = -1;
    ptr->data.button.bTileEndIndex = -1;
    ptr->data.button.actionFunction = actionFunction;

    ptr->layout.row = 0;
    ptr->layout.col = 0;
    ptr->layout.width = ptr->data.button.text.width + ptr->parent->style.padding * 2;
    ptr->layout.height = ptr->parent->style.tileHeight;
    ptr->layout.startx = 0;
    ptr->layout.starty = 0;

    return addItemToWindow(ptr);
}

WindowItem *addPanel(Window *window, Panel *panel, char tag[256])
{
    WindowItem *ptr = initNewItem(GEUI_Panel, window, panel, tag);
    if (!ptr) return NULL;

    ptr->data.panel.panel = malloc(sizeof *ptr->data.panel.panel);
    if (!ptr->data.panel.panel)
    {
        free(ptr);
        return NULL;
    }

    ptr->data.panel.panel->layout.row = 0;
    ptr->data.panel.panel->layout.col = 0;
    ptr->data.panel.panel->layout.width = -1;
    ptr->data.panel.panel->layout.height = -1;
    ptr->data.panel.panel->layout.startx = 0;
    ptr->data.panel.panel->layout.starty = 0;

    ptr->layout.row = 0;
    ptr->layout.col = 0;
    ptr->layout.width = -1;
    ptr->layout.height = -1;
    ptr->layout.startx = 0;
    ptr->layout.starty = 0;

    return addItemToWindow(ptr);
}

WindowItem *getItemFromPanelByTag(Panel *panel, char tag[256])
{
    WindowItem *ptr;
    WindowItem *result = NULL;

    if (!panel) return NULL;

    ptr = panel->iList;

    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;

        if (ptr->type == GEUI_Panel)
        {
            result = getItemFromPanelByTag(ptr->data.panel.panel, tag);

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

    if (!window) return NULL;

    ptr = getItemFromPanelByTag(&window->mainPanel, tag);

    if (ptr)
        return ptr;

    DEBUG_MSG_FROM("item not found", "getItemByTag");
    return NULL;
}

WindowItem *getItemFromPanelByIndex(Panel *panel, int index)
{
    WindowItem *ptr;
    WindowItem *result = NULL;

    if (!panel) return NULL;

    ptr = panel->iList;

    while (ptr)
    {
        if (ptr->index == index)
            return ptr;

        if (ptr->type == GEUI_Panel)
        {
            result = getItemFromPanelByIndex(ptr->data.panel.panel, index);

            if (result)
                return result;
        }

        ptr = ptr->next;
    }

    return NULL;
}

WindowItem *getItemByIndex(Window *window, int index)
{
    WindowItem *ptr;

    if (!window) return NULL;

    ptr = getItemFromPanelByIndex(&window->mainPanel, index);

    if (ptr)
        return ptr;

    DEBUG_MSG_FROM("item not found", "getItemByIndex");
    return NULL;
}

void doMouseEnter(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0 || actor->myIndex < 0) return;
    if (!(window = getWindowByIndex(actor->myWindow))) return;
    if (!(item = getItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (isTopmostItemAtMouse(item))
            {
                long start = item->data.button.bTileStartIndex;
                long end   = item->data.button.bTileEndIndex;

                if (item->data.button.state)
                    colorClones("a_gui", start, end, item->parent->style.buttonPressedColor);
                else
                    colorClones("a_gui", start, end, item->parent->style.buttonHilitColor);
            }
            else doMouseLeave(actorName);
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
    if (!(window = getWindowByIndex(actor->myWindow))) return;
    if (!(item = getItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (!isTopmostItemAtMouse(item))
            {
                long start = item->data.button.bTileStartIndex;
                long end   = item->data.button.bTileEndIndex;

                if (item->data.button.state)
                    colorClones("a_gui", start, end, item->parent->style.buttonPressedColor);
                else
                    colorClones("a_gui", start, end, item->parent->style.buttonColor);
            }
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonDown(const char *actorName, enum mouseButtonsEnum mButtonNumber)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0) return;
    if (!(window = getWindowByIndex(actor->myWindow))) return;

    if (actor->myProperties & GEUI_TITLE_BAR)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        ChangeParent(actor->clonename, "(none)");
        ChangeZDepth(actor->clonename, 0.6);

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
        ChangeZDepth(fake->clonename, 0.2);
        SendActivationEvent(getTile(actor->myFakeIndex)->clonename);

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

    bringWindowToFront(window);

    if (actor->myIndex < 0 || !(item = getItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            colorClones("a_gui",
                item->data.button.bTileStartIndex,
                item->data.button.bTileEndIndex, window->style.buttonPressedColor);
            item->data.button.state = 1;
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonUp(const char *actorName, enum mouseButtonsEnum mButtonNumber)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (actor->myWindow < 0) return;
    if (!(window = getWindowByIndex(actor->myWindow))) return;

    if (actor->myProperties & GEUI_TITLE_BAR && actor->myProperties & GEUI_CLICKED)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        wParent = getclone(window->parentCName);

        // reset window base parent's parent to none
        ChangeParent(wParent->clonename, "(none)");
        ChangeZDepth(actor->clonename, 0.1);

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
        DestroyActor(getTile(actor->myFakeIndex)->clonename);
    }

    //ChangeZDepth(window->parentCName, 0.5);

    if (actor->myIndex < 0 || !(item = getItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
        {
            long start = item->data.button.bTileStartIndex;
            long end   = item->data.button.bTileEndIndex;

            if (isTopmostItemAtMouse(item))
            {
                colorClones("a_gui", start, end, window->style.buttonHilitColor);
                if (item->data.button.state)
                    item->data.button.actionFunction(window, item);
            }
            else
            {
                colorClones("a_gui", start, end, window->style.buttonColor);
            }
            item->data.button.state = 0;
        }
        break;
    }
}

int onMouseClick(enum mouseButtonsEnum mButtonNumber)
{
    return (!strcmp(GEUIController.mButtonTopActorCName[mButtonNumber], clonename) &&
            GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseRelease(enum mouseButtonsEnum mButtonNumber)
{
    return (!strcmp(GEUIController.mButtonTopActorCName[mButtonNumber], clonename) &&
            !GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    return (GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    return (!GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int isMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    return (GEUIController.mButtonState[mButtonNumber]);
}

int isMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    return (!GEUIController.mButtonState[mButtonNumber]);
}

int updateMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    int i;         // Array iterator variable
    int count;     // Count of actors in collision
    Actor *actors = NULL; // A pointer for the array of actors in collision

    // Set the mouse button's state to pressed
    GEUIController.mButtonState[mButtonNumber] = GEUI_MOUSE_DOWN;

    // Get the actors currently in collision with the mouse actor, and their count
    actors = getAllActorsInCollision("a_geuiMouse.0", &count);

    // If there's no actors in collision with the mouse actor
    if (!actors) return 1; // Finish

    // If there currently is an array of actors stored for the mouse button, it needs to be emptied
    if (GEUIController.mButtonActors[mButtonNumber])
    {

        free(GEUIController.mButtonActors[mButtonNumber]);   // Free the memory
        GEUIController.mButtonActors[mButtonNumber] = NULL;  // Set the pointer to NULL
        GEUIController.mButtonActorCount[mButtonNumber] = 0; // Set the count to 0
    }

    // Reset the variable used to store the top actor's name
    strcpy(GEUIController.mButtonTopActorCName[mButtonNumber], "");

    // Allocate memory for the array of actors in the current mouse actor position
    GEUIController.mButtonActors[mButtonNumber] = malloc(count * sizeof *actors);
    GEUIController.mButtonActorCount[mButtonNumber] = count; // Store the count of actors

    if (GEUIController.mButtonActors[mButtonNumber]) // If the memory allocation succeeded
    {
        // Copy the contents of the array returned by getAllActorsInCollision to
        // the allocated chunk of memory
        memcpy(GEUIController.mButtonActors[mButtonNumber], actors, count * sizeof *actors);
    }
    else // If the memory allocation failed
        return 2; // Finish

    GEUIController.activeButton = mButtonNumber; // Set the mouse button as the active one

    // Of the clicked actors, find the one with the highest z depth
    if (GEUIController.mButtonActorCount[mButtonNumber] > 0)
        strcpy(GEUIController.mButtonTopActorCName[mButtonNumber],
            GEUIController.mButtonActors[mButtonNumber][GEUIController.mButtonActorCount[mButtonNumber] - 1].clonename);

    // Iterate through the array of actors and send an activation event to each of them
    for (i = 0; i < GEUIController.mButtonActorCount[mButtonNumber]; i ++)
    {
        SendActivationEvent(GEUIController.mButtonActors[mButtonNumber][i].clonename);
    }

    return 0; // Finish
}

void updateMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    int i; // Array iterator variable

    // Set the mouse button's state to not pressed
    GEUIController.mButtonState[mButtonNumber] = GEUI_MOUSE_UP;
    GEUIController.activeButton = mButtonNumber; // Set the mouse button as the active one

    // If a top actor exists
    if (strlen(GEUIController.mButtonTopActorCName[mButtonNumber]) > 0)
        // Send activation event to the top actor
        SendActivationEvent(GEUIController.mButtonTopActorCName[mButtonNumber]);

    // Reset the variable used to store the top actor's name
    strcpy(GEUIController.mButtonTopActorCName[mButtonNumber], "");

    // If there is an array actors stored for the mouse button
    if (GEUIController.mButtonActors[mButtonNumber])
    {
        // Iterate through the array of actors and send an activation event to each of them
        for (i = 0; i < GEUIController.mButtonActorCount[mButtonNumber]; i ++)
        {
            SendActivationEvent(GEUIController.mButtonActors[mButtonNumber][i].clonename);
        }

        // Empty the array of actors
        free(GEUIController.mButtonActors[mButtonNumber]);   // Free the memory
        GEUIController.mButtonActors[mButtonNumber] = NULL;  // Set the pointer to NULL
        GEUIController.mButtonActorCount[mButtonNumber] = 0; // Set the count to 0
    }
}

void doKeyDown(WindowItem *item, short key)
{
    // TODO: implement doKeyDown
}

void doKeyUp(WindowItem *item, short key)
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
            if (ptr->data.button.bTileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex);
                ptr->data.button.bTileStartIndex = -1;
                ptr->data.button.bTileEndIndex = -1;
            }
        break;
        case GEUI_Panel:
            closePanel(ptr->data.panel.panel);
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
            if (ptr->data.button.bTileStartIndex > -1)
            {
                destroyClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex);
                ptr->data.button.bTileStartIndex = -1;
                ptr->data.button.bTileEndIndex = -1;
            }
        break;
        case GEUI_Panel:
            destroyPanel(ptr->data.panel.panel);
            free(ptr->data.panel.panel);
        break;

        default: break;
    }
}

int calculateAnimpos(short w, short h, short i, short j)
{
    short pw = (i) ? (i / (w - 1)) + 1 : 0; // column  0, 1 or 2
    short ph = (j) ? (j / (h - 1)) + 1 : 0; //    row  0, 1 or 2

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
    // ptr->iIndex = 0;
    strcpy(ptr->tag, tag);
    ptr->isOpen = False;
    ptr->style = style;
    ptr->zDepth = DEFAULT_ZDEPTH;
    strcpy(ptr->parentCName, "");
    ptr->wTileStartIndex = -1;
    ptr->wTileEndIndex = -1;
    // ptr->iList = NULL;
    ptr->mainPanel.iIndex = 0;
    ptr->mainPanel.layout.row = 0;
    ptr->mainPanel.layout.col = 0;
    ptr->mainPanel.layout.width = -1;
    ptr->mainPanel.layout.height = -1;
    ptr->mainPanel.layout.startx = 0;
    ptr->mainPanel.layout.starty = 0;
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

    DEBUG_MSG_FROM("window not found", "getWindowByTag");
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

    DEBUG_MSG_FROM("window not found", "getWindowByIndex");
    return NULL;
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
        case GEUI_Panel: buildPanel(ptr); break;
    }
}

void buildText(WindowItem *ptr)
{
    if (ptr->type != GEUI_Text) { DEBUG_MSG_FROM("Item was not a valid Text item", "buildText"); return; }

    setTextZDepth(&ptr->data.text.text, 0.3);
    // TODO: layout / positioning
    refreshText(&ptr->data.text.text);
}

void buildPanel(WindowItem *ptr)
{
    if (ptr->type != GEUI_Panel) { DEBUG_MSG_FROM("Item was not a valid Panel item", "buildPanel"); return; }

    buildItems(ptr->data.panel.panel);
}

void buildButtonText(WindowItem *ptr)
{
    long start = ptr->data.button.bTileStartIndex;
    long end = ptr->data.button.bTileEndIndex;

    Text *buttonText = &ptr->data.button.text;

    colorClones("a_gui", start, end, ptr->parent->style.buttonColor);
    setTextZDepth(buttonText, 0.4);
    setTextAlignment(buttonText, ALIGN_CENTER);
    setTextPosition(buttonText,
        (getTile(end)->x - getTile(start)->x) * 0.5 + getTile(start)->x, getTile(start)->y);
    refreshText(buttonText);
}

void buildButton(WindowItem *ptr)
{
    short i;
    Text *buttonText;
    long start, end;
    short buttonWidth;
    short tilesHorizontal;
    short tileWidth = ptr->parent->style.tileWidth;
    short tileHeight = ptr->parent->style.tileHeight;

    if (ptr->type != GEUI_Button) { DEBUG_MSG_FROM("Item was not a valid Button item", "buildButton"); return; }

    buttonWidth = ptr->layout.width; //ptr->data.button.text.width + ptr->parent->style.padding * 2;
    tilesHorizontal = ceil(buttonWidth / (float)tileWidth);

    for (i = 0; i < tilesHorizontal; i ++)
    {
        Actor *a;
        a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
        // TODO: layout / positioning
        a->x = tileWidth + i * tileWidth + (i >= 2 && i >= tilesHorizontal - 2) * (buttonWidth - tilesHorizontal * tileWidth);
        a->y = tileHeight + 25 * ptr->index;
        a->myWindow = ptr->parent->index;
        a->myIndex  = ptr->index;
        ChangeZDepth(a->clonename, 0.35); // TODO: change back to 0.3 after testing
        a->animpos = 9 + ((i) ? (i / (tilesHorizontal - 1)) + 1 : 0);

        updateIndexBounds(&ptr->data.button.bTileStartIndex, &ptr->data.button.bTileEndIndex, a->cloneindex);
    }

    buildButtonText(ptr);
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

    windowWidth = 300; // TODO: window size calculations
    windowHeight = 200;

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
            tile->myIndex = -1;
            tile->animpos = calculateAnimpos(tilesHorizontal, tilesVertical, i, j);
            colorActor(tile, window->style.windowBgColor);
            ChangeZDepth(tile->clonename, 0.1);

            if (j == 0) tile->myProperties = GEUI_TITLE_BAR; // part of the window title bar

            updateIndexBounds(&window->wTileStartIndex, &window->wTileEndIndex, tile->cloneindex);
        }
    }
}

Window *openWindow(char tag[256])
{
    Window *window = getWindowByTag(tag);

    if (!window) { DEBUG_MSG_FROM("Window is NULL", "openWindow"); return NULL; }
    if (window->isOpen) { DEBUG_MSG_FROM("Window is already open", "openWindow"); return window; }

    buildWindow(window);
    buildItems(&window->mainPanel);

    window->isOpen = True;
    bringWindowToFront(window);

    return window;
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

void setPanelBaseParent(Panel *panel, char *parentName)
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("Panel is NULL", "setPanelBaseParent"); return; }
    if (!actorExists(parentName)) { DEBUG_MSG_FROM("Actor does not exist!", "setPanelBaseParent"); return; }

    ptr = panel->iList;

    while (ptr)
    {
        switch (ptr->type)
        {
            case GEUI_Text: setTextParent(&ptr->data.text.text, parentName, True); break;
            case GEUI_Button:
                setTextParent(&ptr->data.button.text, parentName, True);
                if (ptr->data.button.bTileStartIndex > -1)
                    changeParentOfClones("a_gui", ptr->data.button.bTileStartIndex, ptr->data.button.bTileEndIndex, parentName);
                break;
             case GEUI_Panel: setPanelBaseParent(ptr->data.panel.panel,  parentName); break;

            default: break;
        }

        ptr = ptr->next;
    }
}

void setWindowBaseParent(Window *window, char *parentName)
{
    WindowItem *ptr = NULL;

    if (!window) {DEBUG_MSG_FROM("window is NULL", "setWindowBaseParent"); return;}
    if (!actorExists(parentName)) {DEBUG_MSG_FROM("actor does not exist!", "setWindowBaseParent"); return;}

    strcpy(window->parentCName, parentName);

    if (window->wTileStartIndex > -1)
        changeParentOfClones("a_gui", window->wTileStartIndex, window->wTileEndIndex, parentName);

    setPanelBaseParent(&window->mainPanel, parentName);
}

void bringWindowToFront(Window *window)
{
    Window *ptr = NULL;

    if (!window) return;

    ptr = GEUIController.wList;

    while (ptr)
    {
        if (ptr->index == window->index)
        {
            ptr->zDepth = 0.6;
            ChangeZDepth(ptr->parentCName, 0.6);
            GEUIController.topIndex = window->index;
        }
        else
        {
            ptr->zDepth = 0.5;
            ChangeZDepth(ptr->parentCName, 0.5);
        }

        ptr = ptr->next;
    }
}

void closePanel(Panel *panel)
{
    WindowItem *ptr;

    if (!panel) { DEBUG_MSG_FROM("Panel is NULL", "closePanel"); return; }

    ptr = panel->iList;

    while (ptr)
    {
        eraseWindowItem(ptr);
        ptr = ptr->next;
    }
}

void closeWindow(Window *window)
{
    WindowItem *ptr = NULL;

    if (!window) {DEBUG_MSG_FROM("window is NULL", "closeWindow"); return;}

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
        destroyPanel(&ptr->mainPanel);
        free(ptr);
        ptr = temp;
    }

    GEUIController.wList = NULL;
    GEUIController.wIndex = 0;
}

void destroyPanel(Panel *panel)
{
    WindowItem *temp;
    WindowItem *ptr;

    if (!panel) return;

    ptr = panel->iList;

    while (ptr)
    {
        destroyWindowItem(ptr);
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }

    panel->iList = NULL;
    panel->iIndex = 0;
}

void updatePanelLayout(Panel *panel)
{
    WindowItem *item;

    if (!panel) return;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->layout.row > panel->layout.row)
            panel->layout.row = item->layout.row + 1;
        if (item->layout.col > panel->layout.col)
            panel->layout.col = item->layout.col + 1;
    }

    panel->layout.width = getPanelWidth(panel);
    panel->layout.height = getPanelHeight(panel);
}

void setPosition(WindowItem *this, short row, short col)
{
    if (!this) return;

    this->layout.row = row;
    this->layout.col = col;

    updatePanelLayout(this->myPanel);
}

short getColWidth(Panel *panel, short col)
{
    short width = 0;
    WindowItem *item;

    if (!panel || !panel->layout.col) return -1;
    if (col < 0 || col >= panel->layout.col) return -2;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item->data.panel.panel);
        if (item->layout.col == col && item->layout.width > width)
            width = item->layout.width;
    }

    return width;
}

short getRowHeight(Panel *panel, short row)
{
    short height = 0;
    WindowItem *item;

    if (!panel || !panel->layout.row) return -1;
    if (row < 0 || row >= panel->layout.row) return -2;

    for (item = panel->iList; item != NULL; item = item->next)
    {
        if (item->type == GEUI_Panel) updatePanelLayout(item->data.panel.panel);
        if (item->layout.row == row && item->layout.height > height)
            height = item->layout.height;
    }

    return height;
}

short getPanelWidth(Panel *panel)
{
    short col;
    short width = 0;
    short tempWidth;

    if (!panel) return -1;
    updatePanelLayout(panel);

    if (!panel->layout.col) return -2;

    for (col = 0; col < panel->layout.col; col ++)
    {
        if ((tempWidth = getColWidth(panel, col)) >= 0)
            width += tempWidth;
    }

    return width;
}

short getPanelHeight(Panel *panel)
{
    short row;
    short height = 0;
    short tempHeight;

    if (!panel) return -1;
    updatePanelLayout(panel);

    if (!panel->layout.row) return -2;

    for (row = 0; row < panel->layout.row; row ++)
    {
        if ((tempHeight = getRowHeight(panel, row)) >= 0)
            height += tempHeight;
    }

    return height;
}
