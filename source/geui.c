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
            Actor *actor;
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
    Style style;        // window style
    int wTileStartIndex; // cloneindex of the first window tile
    int wTileEndIndex;   // cloneindex of the last window tile
    struct WindowItemStruct *iList;  // list of items in window
    struct WindowStruct *next;       // pointer to next window in list
}Window;

void initGEUI(void);
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
Window *createWindow(char tag[256]);
Window *searchWindowByTag(char tag[256]);
Window *searchWindowByIndex(int index);
Window *openWindow(char tag[256]);
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
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button: colorActor(item->data.button.actor, window->style.buttonHilitColor); break;
    }
}

void doMouseLeave(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button: colorActor(item->data.button.actor, window->style.buttonColor); break;
    }
}

void doMouseButtonDown(const char *actorName, short mButton)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button: colorActor(item->data.button.actor, window->style.buttonPressedColor); break;
    }
}

void doMouseButtonUp(const char *actorName, short mButton)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName))) return;
    if (!(window = searchWindowByIndex(actor->myWindow))) return;
    if (!(item = searchItemByIndex(window, actor->myIndex))) return;

    switch (item->type)
    {
        case GEUI_Button:
            if (mouseOverActor(actorName))
            {
                colorActor(item->data.button.actor, window->style.buttonHilitColor);
                item->data.button.actionFunction(window, item);
            }
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
        case GEUI_Text: eraseText(&ptr->data.text.text); break;
        case GEUI_Button: eraseText(&ptr->data.button.text); DestroyActor(ptr->data.button.actor->clonename); break;
    }
}

void destroyWindowItem(WindowItem *ptr)
{
    if (!ptr) return;

    switch (ptr->type)
    {
        case GEUI_Text: destroyText(&ptr->data.text.text); break;
        case GEUI_Button: destroyText(&ptr->data.button.text); DestroyActor(ptr->data.button.actor->clonename); break;

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

Window *createWindow(char tag[256])
{
    Window *ptr = malloc(sizeof *ptr);

    if (!ptr) return NULL;

    ptr->index = GEUIController.wIndex ++;
    ptr->iIndex = 0;
    strcpy(ptr->tag, tag);
    ptr->style = GEUIController.sDefault;
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
                ptr->data.button.actor = CreateActor("a_gui", "gui_sheet_default", "(none)", "(none)", -20, -20, true);
                ptr->data.button.actor->myWindow = window->index;
                ptr->data.button.actor->myIndex = ptr->index;
                colorActor(ptr->data.button.actor, window->style.buttonColor);
                ChangeZDepth(ptr->data.button.actor->clonename, 1.0);
            break;

            default: break;
        }

        ptr = ptr->next;
    }

    guiActor = CreateActor("a_gui", "gui_sheet_default", "(none)", "(none)", 0, 0, true);
    tileWidth = guiActor->width;
    tileHeight = guiActor->height;
    tilesH = ceil(width / (float)tileWidth);
    tilesV = ceil(height / (float)tileHeight);
    DestroyActor(guiActor->clonename);

    for (j = 0; j < tilesV; j ++)
    {
        for (i = 0; i < tilesH; i ++)
        {
            guiActor = CreateActor("a_gui", window->style.guiAnim, "(none)", "(none)",
                i * tileWidth, j * tileHeight, true);
            guiActor->myWindow = window->index;
            guiActor->myIndex = -1;
            ChangeAnimationDirection(guiActor->clonename, STOPPED);
            ChangeZDepth(guiActor->clonename, 0.4);
            guiActor->animpos = calculateAnimpos(tilesH, tilesV, i, j);
            
            if (window->wTileStartIndex == -1)
                window->wTileStartIndex = guiActor->cloneindex;
            if (window->wTileEndIndex < guiActor->cloneindex)
                window->wTileEndIndex = guiActor->cloneindex;
        }
    }

    return NULL;
}

void closeWindow(Window *window)
{
    int i;
    WindowItem *ptr = NULL;
    
    if (!window) {DEBUG_MSG_FROM("window is NULL", "openWindow"); return;}
    
    if (window->wTileStartIndex > -1)
    {
        for (i = window->wTileStartIndex; i <= window->wTileEndIndex; i ++)
        {
            DestroyActor(gc2("a_gui", i)->clonename);
        }
        
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
