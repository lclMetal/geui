typedef struct WindowStruct Window;
typedef struct WindowItemStruct WindowItem;

typedef enum ItemTypeEnum
{
    GEUI_Text,
    GEUI_Button
}ItemType;

typedef struct GEUI_ItemStruct
{
    ItemType type;
    
    union ItemVarsUnion
    {
        struct TextItem     { Text text; }text;
        struct ButtonItem
        {
            Text text;
            short state;
            void (*actionFunction)(Window *win, WindowItem *item);
        }button;
    }data;
}GEUI_Item;

struct WindowItemStruct
{
    int index;          // item index
    char tag[256];      // item identifier tag
    GEUI_Item item;     // item container
    Window *parent;     // pointer to parent window
    WindowItem *next;   // pointer to next item in list    
};

struct WindowStruct
{
    int index;          // window index
    int iIndex;         // next available item index
    char tag[256];      // window identifier tag
    Style style;        // window style
    WindowItem *iList;  // list of items in window
    Window *next;       // pointer to next window in list
};

void initGEUI(void);
GEUI_Item newText(char *string);
GEUI_Item newButton(Text text, void (*actionFunction)(Window *win, WindowItem *item));
void destroyGEUI_Item(GEUI_Item *ptr);
int calculateAnimpos(unsigned short w, unsigned short h, unsigned short i, unsigned short j);
Window *createWindow(char tag[256]);
Window *searchWindow(char tag[256]);
Window *openWindow(char tag[256]);
void destroyWindowList(void);
WindowItem *addToWindow(Window *window, char tag[256], GEUI_Item item);
void destroyWindowItem(WindowItem *ptr);
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
    defStyle.buttonHilitColor   = DEFAULT_COLOR;    // button hilit color
    defStyle.buttonPressedColor = DEFAULT_COLOR;    // button pressed color
    
    GEUIController.wIndex = 0;
    GEUIController.sDefault = defStyle;
    GEUIController.wList = NULL;
}

GEUI_Item newText(char *string)
{
    GEUI_Item item;
    
    item.type = GEUI_Text;
    item.data.text.text = createText(string, GEUIController.sDefault.textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&item.data.text.text, BLACK);
    
    return item;
}

GEUI_Item newButton(Text text, void (*actionFunction)(Window *win, WindowItem *item))
{
    GEUI_Item item;
    
    item.type = GEUI_Button;
    //strcpy(item.data.button.text, text);
    item.data.button.state = 0;
    item.data.button.actionFunction = actionFunction;
    
    return item;
}

void destroyGEUI_Item(GEUI_Item *ptr)
{
    if (!ptr) return;
    
    switch (ptr->type)
    {
        case GEUI_Text: destroyText(&ptr->data.text.text); break;
        case GEUI_Button: break;
        
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
    ptr->iList = NULL;
    ptr->next = GEUIController.wList;
    
    GEUIController.wList = ptr;
    
    return ptr;
}

Window *searchWindow(char tag[256])
{
    Window *ptr = GEUIController.wList;
    
    while (ptr)
    {
        if (!strcmp(ptr->tag, tag))
            return ptr;
        
        ptr = ptr->next;
    }
    
    DEBUG_MSG_FROM("window not found", "searchWindow");
    return NULL;
}

Window *openWindow(char tag[256])
{
    int i, j;
    Actor *guiActor = NULL;
    unsigned short width = 0, height = 0;
    unsigned short tileWidth = 0, tileHeight = 0;
    unsigned short tilesH = 0, tilesV = 0;
    Window *win = searchWindow(tag);
    WindowItem *ptr = NULL;
    
    if (!win) {DEBUG_MSG_FROM("window is NULL", "openWindow"); return NULL;}
    
    ptr = win->iList;
    
    while (ptr)
    {
        switch (ptr->item.type)
        {
            case GEUI_Text:
                width = ptr->item.data.text.text.width;
                height = ptr->item.data.text.text.height;
                refreshText(&ptr->item.data.text.text);
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
            guiActor = CreateActor("a_gui", win->style.guiAnim, "(none)", "(none)",
                i * tileWidth, j * tileHeight, true);
            ChangeAnimationDirection(guiActor->clonename, STOPPED);
            ChangeZDepth(guiActor->clonename, 0.4);
            guiActor->animpos = calculateAnimpos(tilesH, tilesV, i, j);
        }
    }
    
    return NULL;
}

void destroyWindowList(void)
{
    Window *temp = NULL;
    Window *ptr = GEUIController.wList;
    
    while (ptr)
    {
        temp = ptr->next;
        destroyWindowItemList(ptr);
        free(ptr);
        ptr = temp;
    }
    
    GEUIController.wList = NULL;
    GEUIController.wIndex = 0;
}

WindowItem *addToWindow(Window *window, char tag[256], GEUI_Item item)
{
    WindowItem *ptr = NULL;
    
    if (!window) return NULL;
    
    ptr = malloc(sizeof *ptr);
    
    if (!ptr) return NULL;
    
    ptr->index = window->iIndex ++;
    strcpy(ptr->tag, tag);
    ptr->next = window->iList;
    
    ptr->item = item;
    
    /*switch (ptr->item.type)
    {
        case GEUI_Text: refreshText(&ptr->item.data.text.text); break;
        default: break;
    }*/
    
    window->iList = ptr;
    return ptr;
}

void destroyWindowItem(WindowItem *ptr)
{
    if (!ptr) return;
    destroyGEUI_Item(&ptr->item);
}

void destroyWindowItemList(Window *window)
{
    WindowItem *temp = NULL;
    WindowItem *ptr = NULL;
    
    if (!window || !window->iList) return;
    
    ptr = window->iList;
    
    while (ptr)
    {
        /*switch (ptr->item.type)
        {
            case GEUI_Text: destroyText(&ptr->item.data.text.text); break;
            default: break;
        }*/
        destroyWindowItem(ptr);
        
        temp = ptr->next;
        free(ptr);
        ptr = temp;
    }
    
    window->iList = NULL;
    window->iIndex = 0;
}