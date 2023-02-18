typedef struct ScreenCoordsStruct
{
    float x;
    float y;
}ScreenCoords;

typedef struct LayoutStruct
{
    short row;      // row in a panel
    short col;      // column in a panel
    short width;
    short height;
    short startx;
    short starty;
}Layout;

typedef enum ItemTypeEnum
{
    GEUI_Text,
    GEUI_Button,
    GEUI_Checkbox,
    GEUI_Input,
    GEUI_Panel,
    GEUI_Embedder
}ItemType;

struct WindowStruct;
struct PanelStruct;
struct InputFieldStruct;

#define GEUI_DEFAULT_CARET_BLINK_RATE 2

typedef struct BlinkingCaretStruct
{
    float blinkRate;
    float timer;
    short state;
    char actorCName[256];
    Text *pText;
}BlinkingCaret;

typedef struct TileIndicesStruct
{
    long first;
    long last;
}TileIndices;

TileIndices noIndices = { -1, -1 };

typedef enum InputTypeEnum
{
    GEUI_TextInput,
    GEUI_IntInput,
    GEUI_DecimalInput
}InputType;

typedef struct TextInputSettingsStruct
{
    int empty;
}TextInputSettings;

typedef struct IntInputSettingsStruct
{
    int minVal;
    int maxVal;
    int defaultValue;
}IntInputSettings;

typedef struct DecimalInputSettingsStruct
{
    float minVal;
    float maxVal;
    float defaultValue;
    short precisionDigits;
}DecimalInputSettings;

typedef union InputSettingsDataUnion
{
    TextInputSettings textInput;
    IntInputSettings intInput;
    DecimalInputSettings decimalInput;
}InputSettingsData;

typedef union InputValueUnion
{
    char *textValue;
    int intValue;
    float decimalValue;
}InputValue;

typedef struct InputSettingsStruct
{
    InputType type;
    InputSettingsData data;
    void (*applyConstraintsFunc)(struct InputFieldStruct *);
    void (*readValueFunc)(struct InputFieldStruct *);
}InputSettings;

typedef enum KeyboardLayoutEnum
{
    GEUI_KeyboardUS = 0,
    GEUI_KeyboardFI,
    GEUI_KeyboardLayoutCount
}KeyboardLayout;

typedef struct InputFieldStruct
{
    InputSettings settings;
    InputValue value;

    Text text;
    BlinkingCaret caret;

    TileIndices tiles;
}InputField;

typedef enum ActionTypeEnum
{
    GEUI_ACTION,
    GEUI_ACTION_OPEN_WINDOW,
    GEUI_ACTION_CLOSE_WINDOW
}ActionType;

typedef enum WPosSettingEnum
{
    GEUI_WPosCoords,
    GEUI_WPosMouseCenter,
    GEUI_WPosMouseTopLeft,
    GEUI_WPosMouseTop,
    GEUI_WPosScreenCenter
}WPosSetting;

typedef struct WindowPositionStruct
{
    WPosSetting type;
    ScreenCoords pos;
}WindowPosition;

typedef struct GUIActionStruct
{
    ActionType type;
    union ActionDataEnum
    {
        struct actionOpenWindowStruct
        {
            char tag[256];
            WindowPosition pos;
        }openWindow;
        struct actionCloseWindowStruct
        {
            char tag[256];
        }closeWindow;
    }data;

    struct WindowStruct *window;
    struct PanelStruct *panel;
    int itemIndex;

    void (*fpAction)(struct GUIActionStruct *);
}GUIAction;

typedef struct WindowItemStruct
{
    int index;          // item index
    char tag[256];      // item identifier tag

    ItemType type;      // item type
    bool focusable;

    union ItemDataUnion // item data union for different item types
    {
        Text text;
        struct ButtonItem
        {
            Text text;
            char state;
            TileIndices tiles;
            GUIAction action;
        }button;
        struct CheckboxItem
        {
            bool state;
            long tileIndex;
        }checkbox;
        InputField input;
        struct PanelStruct *panel;
        struct EmbedderItem
        {
            char actorCName[256];
        }embedder;
    }data;

    Layout layout;
    struct PanelStruct *myPanel;    // pointer to parent panel
    struct WindowStruct *parent;    // pointer to parent window
    struct WindowItemStruct *next;  // pointer to next item in list
}WindowItem;

typedef struct PanelStruct
{
    int index;      // panel index
    int iIndex;     // next available item index
    short rows;     // number of rows in panel
    short cols;     // number of columns in panel
    short width;
    short height;
    struct WindowStruct *parent;
    struct WindowItemStruct *iList;
}Panel;

typedef struct DataBindStruct
{
    void *data;
    char tag[256];
}DataBind;

#define GEUI_MAX_DATA_BINDS_PER_WINDOW 5

typedef struct WindowStruct
{
    int index;          // window index
    int pIndex;         // next available panel index
    char tag[256];      // window identifier tag
    bool isOpen;        // is window currently open or not
    long fakeIndex;     // fake tile index
    Style style;        // window style
    double zDepth;      // window z depth
    char parentCName[256]; // clonename of the window parent actor
    int dataBindIndex;  // next available data bind index
    DataBind dataBinds[GEUI_MAX_DATA_BINDS_PER_WINDOW]; // data binds array
    bool hasTitle;              // does the window have a title
    Text title;                 // window title text
    TileIndices tiles;          // cloneindices of the window tiles
    Panel root;                 // window main panel
    struct WindowStruct *next;  // pointer to next window in list
}Window;

#define GEUI_MOUSE_UP 0
#define GEUI_MOUSE_DOWN 1

const unsigned long GEUI_TITLE_BAR  = (1 << 0);
const unsigned long GEUI_FAKE_ACTOR = (1 << 1);
const unsigned long GEUI_CLICKED    = (1 << 2);
const unsigned long GEUI_CARET      = (1 << 3);

enum mouseButtonsEnum // Used as array indices, don't touch!
{
    GEUI_MOUSE_LEFT = 0,
    GEUI_MOUSE_RIGHT,
    GEUI_MOUSE_MIDDLE,
    GEUI_MOUSE_WHEEL_UP,
    GEUI_MOUSE_WHEEL_DOWN,
    GEUI_MOUSE_BUTTONS     // Number of supported mouse buttons (5)
};

#define GEUI_CLONENAME_SIZE 42

#define DEFAULT_WINDOW_ZDEPTH 0.5
#define ACTIVE_WINDOW_ZDEPTH 0.6
#define DEFAULT_ITEM_ZDEPTH 0.3
#define WINDOW_TILE_ZDEPTH 0.1
#define FAKE_ACTOR_ZDEPTH 0.2

#define USE_DEFAULT_STYLE GEUIController.sDefault

struct GEUIControllerStruct
{
    int wIndex;
    int topIndex;
    Style sDefault;
    KeyboardLayout kbLayout;
    Window *wList;

    WindowItem *focus;
    TileIndices focusTiles;

    Actor *mButtonActors[GEUI_MOUSE_BUTTONS];
    char mButtonTopActorCName[GEUI_MOUSE_BUTTONS][GEUI_CLONENAME_SIZE];
    int mButtonActorCount[GEUI_MOUSE_BUTTONS];
    int mButtonState[GEUI_MOUSE_BUTTONS];
    enum mouseButtonsEnum activeButton;
}GEUIController;

#define CURRENT_KEYBOARD GEUIController.kbLayout
