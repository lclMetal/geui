void initGEUI(KeyboardLayout kbLayout);
void quitGEUI(void);
void destroyWindowList(void);

void initGEUI(KeyboardLayout kbLayout)
{
    //DEBUG_INIT(); // debug file initialization on startup disabled for now
    // TODO: separate DEBUG_INIT() and DEBUG_INIT_FILE()

    strcpy(defStyle.guiAnim, "gui_sheet_default");  // GUI animation name
    getTileDimensions(&defStyle);
    defStyle.titleFont          = &defTitleFont;
    defStyle.labelFont          = &defLabelFont;
    defStyle.textFont           = &defTextFont;
    defStyle.padding            = 5;
    defStyle.focusWidth         = 2;
    defStyle.titleProperties    = 0;
    defStyle.buttonProperties   = 0;
    defStyle.buttonPadding      = 1.0f;
    defStyle.titleBgColor       = DEFAULT_COLOR;
    defStyle.windowBgColor      = DEFAULT_COLOR;
    defStyle.inputBgColor       = DEFAULT_COLOR;
    defStyle.titleColor         = BLACK;
    defStyle.labelColor         = BLACK;
    defStyle.textColor          = BLACK;
    defStyle.buttonColor        = DEFAULT_COLOR;
    defStyle.buttonHilitColor   = CYAN;
    defStyle.buttonPressedColor = BLUE;
    defStyle.focusColor         = createRGB(255, 0, 128, 1.0);

    GEUIController.wIndex = 0;
    GEUIController.topIndex = -1;
    GEUIController.sDefault = &defStyle;
    GEUIController.kbLayout = kbLayout;
    GEUIController.wList = NULL;
    GEUIController.focus = NULL;
    GEUIController.focusTiles = noIndices;
}

void setKeyboardLayout(KeyboardLayout kbLayout)
{
    GEUIController.kbLayout = kbLayout;
}

void quitGEUI(void)
{
    int mb;

    for (mb = 0; mb < GEUI_MOUSE_BUTTONS; mb ++)
    {
        if (GEUIController.mButtonActors[mb])
        {
            free(GEUIController.mButtonActors[mb]);
            GEUIController.mButtonActors[mb] = NULL;
        }
    }

    destroyWindowList();
}

void destroyWindowList(void)
{
    Window *temp = NULL;
    Window *ptr = GEUIController.wList;

    while (ptr)
    {
        temp = ptr->next;
        destroyWindow(ptr);
        ptr = temp;
    }

    GEUIController.wList = NULL;
    GEUIController.wIndex = 0;
}
