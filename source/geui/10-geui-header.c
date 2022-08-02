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
    GEUI_InputInt,
    GEUI_InputText,
    GEUI_Panel,
    GEUI_Embedder
}ItemType;

struct WindowStruct;
struct PanelStruct;

typedef struct IntInputFieldStruct
{
    int    value;
    int    minValue;
    int    maxValue;

    short  intSign;
    short  typedZero;

    Text   text;

    long tileStartIndex;
    long tileEndIndex;
}IntInputField;

typedef struct TextInputFieldStruct
{
    long modifier;

    Text text;

    long tileStartIndex;
    long tileEndIndex;
}TextInputField;

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
            long bTileStartIndex;
            long bTileEndIndex;
            void (*actionFunction)(struct WindowStruct *, struct WindowItemStruct *);
        }button;
        IntInputField inputInt;
        TextInputField inputText;
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

typedef struct WindowStruct
{
    int index;          // window index
    int pIndex;         // next available panel index
    char tag[256];      // window identifier tag
    bool isOpen;        // is window currently open or not
    Style style;        // window style
    double zDepth;      // window z depth
    char parentCName[256]; // clonename of the window parent actor
    long wTileStartIndex;   // cloneindex of the first window tile
    long wTileEndIndex;     // cloneindex of the last window tile
    Panel mainPanel;            // window main panel
    struct WindowStruct *next;  // pointer to next window in list
}Window;

#define GEUI_MOUSE_UP 0
#define GEUI_MOUSE_DOWN 1

const unsigned long GEUI_TITLE_BAR  = (1 << 0);
const unsigned long GEUI_FAKE_ACTOR = (1 << 1);
const unsigned long GEUI_CLICKED    = (1 << 2);
const unsigned long GEUI_INPUT_BG   = (1 << 3);

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
    Window *wList;

    WindowItem *focus;
    long focusTileStartIndex;
    long focusTileEndIndex;

    Actor *mButtonActors[GEUI_MOUSE_BUTTONS];
    char mButtonTopActorCName[GEUI_MOUSE_BUTTONS][GEUI_CLONENAME_SIZE];
    int mButtonActorCount[GEUI_MOUSE_BUTTONS];
    int mButtonState[GEUI_MOUSE_BUTTONS];
    enum mouseButtonsEnum activeButton;
}GEUIController;
