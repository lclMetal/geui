void enforceTextInputSettings(InputField *input);
void updateTextInputValue(InputField *input);
InputSettings createTextInputSettings();
void enforceIntInputSettings(InputField *input);
void updateIntInputValue(InputField *input);
InputSettings createIntInputSettings(int minVal, int maxVal, int defaultValue);
void enforceDecimalInputSettings(InputField *input);
void updateDecimalInputValue(InputField *input);
InputSettings createDecimalInputSettings(float minVal, float maxVal, float defaultValue, short precisionDigits);
WindowItem *addInputField(Panel *panel, char tag[256], const char *string, InputSettings settings, short maxWidth);
void buildInputFieldBackground(WindowItem *ptr, TileIndices *tiles);
void buildInputField(WindowItem *ptr);

void enforceTextInputSettings(InputField *input)
{
    return;
}

void updateTextInputValue(InputField *input)
{
    return;
}

InputSettings createTextInputSettings()
{
    InputSettings settings;

    settings.type = GEUI_TextInput;
    (settings.applyConstraintsFunc = enforceTextInputSettings);
    (settings.readValueFunc = updateTextInputValue);

    settings.data.textInput.empty = 0;

    return settings;
}

void enforceIntInputSettings(InputField *input)
{
    input->value.intValue = iLimit(input->value.intValue, input->settings.data.intInput.minVal, input->settings.data.intInput.maxVal);
}

void updateIntInputValue(InputField *input)
{
    if (greedyAtoi(input->text.pString) != 0)
    {
        input->value.intValue = greedyAtoi(input->text.pString);
    }
}

InputSettings createIntInputSettings(int minVal, int maxVal, int defaultValue)
{
    InputSettings settings;

    settings.type = GEUI_IntInput;
    (settings.applyConstraintsFunc = enforceIntInputSettings);
    (settings.readValueFunc = updateIntInputValue);

    settings.data.intInput.minVal = minVal;
    settings.data.intInput.maxVal = maxVal;
    settings.data.intInput.defaultValue = iLimit(defaultValue, minVal, maxVal);

    return settings;
}

void enforceDecimalInputSettings(InputField *input)
{
    input->value.decimalValue = fLimit(input->value.decimalValue, input->settings.data.decimalInput.minVal, input->settings.data.decimalInput.maxVal);
}

void updateDecimalInputValue(InputField *input)
{
    if (greedyAtof(input->text.pString) != 0)
    {
        input->value.decimalValue = greedyAtof(input->text.pString);
    }
}

InputSettings createDecimalInputSettings(float minVal, float maxVal, float defaultValue, short precisionDigits)
{
    InputSettings settings;

    settings.type = GEUI_DecimalInput;
    (settings.applyConstraintsFunc = enforceDecimalInputSettings);
    (settings.readValueFunc = updateDecimalInputValue);

    settings.data.decimalInput.minVal = minVal;
    settings.data.decimalInput.maxVal = maxVal;
    settings.data.decimalInput.defaultValue = dfLimit(defaultValue, minVal, maxVal);
    settings.data.decimalInput.precisionDigits = siLimit(precisionDigits, 0, 10);

    return settings;
}

WindowItem *addInputField(Panel *panel, char tag[256], const char *string, InputSettings settings, short maxWidth)
{
    WindowItem *ptr = initNewItem(GEUI_Input, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addInputField"); return NULL; }

    ptr->focusable = True;
    initializeCaret(&ptr->data.input.caret);
    ptr->data.input.text = createText(string, panel->parent->style->textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.input.text, panel->parent->style->textColor);
    setTextZDepth(&ptr->data.input.text, DEFAULT_ITEM_ZDEPTH);
    ptr->data.input.settings = settings;

    switch (settings.type)
    {
        case GEUI_TextInput: ptr->data.input.value.textValue = ptr->data.input.text.pString; break;
        case GEUI_IntInput: ptr->data.input.value.intValue = settings.data.intInput.defaultValue; break;
        case GEUI_DecimalInput: ptr->data.input.value.decimalValue = settings.data.decimalInput.defaultValue; break;
    }

    refreshValueText(&ptr->data.input);

    ptr->data.input.tiles = noIndices;

    ptr->layout.width = maxWidth + ptr->parent->style->tileWidth * 2;
    ptr->layout.height = ptr->parent->style->tileHeight;

    return addItemToWindow(ptr);
}

Actor *buildCaret(WindowItem *ptr, Text *pText, BlinkingCaret *caret)
{
    Actor *a = CreateActor("a_gui", ptr->parent->style->guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
    a->animpos = 19;
    a->myWindow = ptr->parent->index,
    a->myPanel = ptr->myPanel->index;
    a->myIndex = ptr->index;
    a->myProperties = GEUI_CARET;
    SendActivationEvent(a->clonename);
    ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
    strcpy(caret->actorCName, a->clonename);
    caret->pText = pText;
    colorActorByName(caret->actorCName, ptr->parent->style->textColor);
    updateCaretPosition(caret);

    return a;
}

void buildInputFieldBackground(WindowItem *ptr, TileIndices *tiles)
{
    short i;
    Actor *a;
    short fieldWidth;
    short tilesHorizontal;
    short tileWidth = ptr->parent->style->tileWidth;
    short tileHeight = ptr->parent->style->tileHeight;

    fieldWidth = ptr->layout.width;
    tilesHorizontal = ceil(fieldWidth / (float)tileWidth);

    for (i = 0; i < tilesHorizontal; i++)
    {
        a = CreateActor("a_gui", ptr->parent->style->guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
        a->x = ptr->layout.startx + tileWidth + i * tileWidth + (i >= 2 && i >= tilesHorizontal - 2) * (fieldWidth - tilesHorizontal * tileWidth)-tileWidth/2;
        a->x += ptr->parent->style->padding;
        a->y = ptr->layout.starty + tileHeight-tileWidth/2;
        a->y += ptr->parent->style->padding;
        a->myWindow = ptr->parent->index;
        a->myPanel  = ptr->myPanel->index;
        a->myIndex  = ptr->index;
        a->animpos = 12 + (i > 0) + (i == tilesHorizontal - 1) + (i > 0 && i == tilesHorizontal - 2 && fieldWidth < tileWidth * 2.5);
        SendActivationEvent(a->clonename);
        ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);

        updateGuiTileIndices(tiles, a->cloneindex);
    }
}

void buildInputField(WindowItem *ptr)
{
    if (ptr->type != GEUI_Input) { DEBUG_MSG_FROM("item is not a valid InputText item", "buildInputText"); return; }

    buildInputFieldBackground(ptr, &ptr->data.input.tiles);
    colorGuiTiles(ptr->data.input.tiles, ptr->parent->style->inputBgColor);

    setTextZDepth(&ptr->data.input.text, DEFAULT_ITEM_ZDEPTH);
    setTextPosition(&ptr->data.input.text,
        getTile(ptr->data.input.tiles.first)->x - ptr->parent->style->tileWidth / 4,
        getTile(ptr->data.input.tiles.last)->y - ptr->data.input.text.pFont->baselineOffset / 2);
    refreshText(&ptr->data.input.text);

    buildCaret(ptr, &ptr->data.input.text, &ptr->data.input.caret);
    hideCaret(&ptr->data.input.caret);
}
