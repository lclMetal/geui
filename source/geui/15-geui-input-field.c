#define GEUI_KEYBOARD_MAPPING_SIZE GEUI_KeyboardLayoutCount + 1
#define GEUI_MAX_VALID_KEYS_LIMIT 26

#define GEUI_NUM_STRING_LENGTH 30

const short validKeysForInputType[][GEUI_MAX_VALID_KEYS_LIMIT] =
{
    { -1 },
    {
        KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
        KEY_6, KEY_7, KEY_8, KEY_9, KEY_MINUS,
        KEY_PAD_0, KEY_PAD_1, KEY_PAD_2, KEY_PAD_3, KEY_PAD_4, KEY_PAD_5,
        KEY_PAD_6, KEY_PAD_7, KEY_PAD_8, KEY_PAD_9, KEY_PAD_MINUS, KEY_BACKSPACE,
    },
    {
        KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5,
        KEY_6, KEY_7, KEY_8, KEY_9, KEY_MINUS,
        KEY_PAD_0, KEY_PAD_1, KEY_PAD_2, KEY_PAD_3, KEY_PAD_4, KEY_PAD_5,
        KEY_PAD_6, KEY_PAD_7, KEY_PAD_8, KEY_PAD_9, KEY_PAD_MINUS, KEY_BACKSPACE,
        KEY_PERIOD, KEY_PAD_PERIOD, KEY_e
    }
};

int checkKeyValidity(short key, InputType type)
{
    short i;

    if (type == GEUI_TextInput)
        return 1;

    for (i = 0; i < GEUI_MAX_VALID_KEYS_LIMIT; i++)
    {
        if (validKeysForInputType[type][i] == key)
            return 1;
    }

    return 0;
}

int greedyAtoi(const char *str)
{
    int val = 0;
    size_t offset = 0;
    size_t len = strlen(str);

    do
    {
        val = atoi(&str[offset]);
        offset++;
    } while (!val && offset < len);

    return val;
}

float greedyAtof(const char *str)
{
    int count = 0;
    float val = 0;
    size_t offset = 0;
    size_t len = strlen(str);

    do
    {
        count = sscanf(&str[offset], "%f%*s", &val, NULL);
        offset++;
    } while (!val && offset < len);

    return val;
}

const short keyboardLocalizations[][GEUI_KEYBOARD_MAPPING_SIZE] =
{
    { KEY_0,            ')',    '='     },
    { KEY_1,            '!',    '!'     },
    { KEY_2,            '@',    '"'     },
    { KEY_3,            '#',    '#'     },
    { KEY_4,            '$',    '_'     },
    { KEY_5,            '%',    '%'     },
    { KEY_6,            '^',    '&'     },
    { KEY_7,            '&',    '/'     },
    { KEY_8,            '*',    '('     },
    { KEY_9,            '(',    ')'     },
    { KEY_EQUALS,       '=',    '+'     },
    { KEY_SLASH,        '/',    '\''    },
    { KEY_PAD_PERIOD,   '.',    '.'     }
};

char getLocalizedKeyboardChar(int key, KeyboardLayout kbLayout)
{
    int i;

    for (i = 0; i < sizeof keyboardLocalizations / sizeof (short[GEUI_KEYBOARD_MAPPING_SIZE]); i++)
    {
        if (keyboardLocalizations[i][0] == key)
        {
            return keyboardLocalizations[i][(size_t)kbLayout + 1];
        }
    }

    return '\0';
}

void refreshValueText(InputField *field)
{
    char tempNumText[GEUI_NUM_STRING_LENGTH];

    switch (field->settings.type)
    {
        case GEUI_TextInput: break;
        case GEUI_IntInput:
            sprintf(tempNumText, "%d", field->value.intValue);
            setTextText(&field->text, tempNumText);
        break;
        case GEUI_DecimalInput:
            sprintf(tempNumText, "%.*f", field->settings.data.decimalInput.precisionDigits, field->value.decimalValue);
            field->value.decimalValue = greedyAtof(tempNumText);
            setTextText(&field->text, tempNumText);
        break;
    }
}

void refreshInputValue(InputField *field)
{
    refreshValueText(field);
    refreshText(&field->text);
    updateCaretPosition(&field->caret);
}

void handleInputFieldInput(InputField *field, short key)
{
    int readCount = 0;
    char tempNumText[GEUI_NUM_STRING_LENGTH];
    char newChar = '\0';
    char rest[30] = "";
    char *keys = GetKeyState();
    short isTextInput = (field->settings.type == GEUI_TextInput);
    short shift = (keys[KEY_LSHIFT] || keys[KEY_RSHIFT]) && isTextInput;
    short ctrl  = (keys[KEY_LCTRL] || keys[KEY_RCTRL]);
    short alt   = (keys[KEY_LALT] || keys[KEY_RALT]);

    if (key == KEY_RETURN)
    {
        field->settings.readValueFunc(field);
        field->settings.applyConstraintsFunc(field);
        refreshInputValue(field);
    }

    if (!checkKeyValidity(key, field->settings.type))
    {
        DEBUG_MSG_FROM("key not valid", "handleInputFieldInput");
        return;
    }

    if (key >= KEY_a && key <= KEY_z)
    {
        newChar = key - ('a' - 'A') * shift;
    }
    else if (key >= KEY_0 && key <= KEY_9)
    {
        newChar = shift ? getLocalizedKeyboardChar(key, CURRENT_KEYBOARD)
                        : key;
    }
    else if (key >= KEY_PAD_0 && key <= KEY_PAD_9)
    {
        newChar = key - (KEY_PAD_0 - KEY_0);
    }
    else
    {
        switch (key)
        {
            case KEY_SPACE:         newChar = ' '; break;
            case KEY_COMMA:         newChar = shift ? ';' : ','; break;
            case KEY_PERIOD:        newChar = shift ? ':' : '.'; break;
            case KEY_MINUS:         newChar = shift ? '_' : '-'; break;
            case KEY_PAD_DIVIDE:    newChar = '/'; break;
            case KEY_PAD_MULTIPLY:  newChar = '*'; break;
            case KEY_PAD_MINUS:     newChar = '-'; break;
            case KEY_PAD_PLUS:      newChar = '+'; break;
            // case KEY_TAB:           newChar = '\t'; break;
            case KEY_RETURN:        newChar = '\n'; break;
            case KEY_EQUALS:
            case KEY_SLASH:
            case KEY_PAD_PERIOD:
                newChar = getLocalizedKeyboardChar(key, CURRENT_KEYBOARD);
            break;
            case KEY_BACKSPACE:
                if (ctrl)
                    setTextText(&field->text, "");
                else
                    newChar = '\b';
            break;
        }
    }

    concatenateCharToText(&field->text, newChar);

    switch (field->settings.type)
    {
        case GEUI_IntInput:
        case GEUI_DecimalInput:
            field->settings.readValueFunc(field);
        break;
    }
}

char *getTextInputValue(InputField *field)
{
    if (field->settings.type != GEUI_TextInput)
    {
        DEBUG_MSG_FROM("field is not a text input", "getTextInputValue");
        return NULL;
    }

    return field->value.textValue;
}

char *readTextInput(WindowItem *item)
{
    if (item->type != GEUI_Input)
    {
        DEBUG_MSG_FROM("item is not an input field", "readTextInput");
        return NULL;
    }

    return getTextInputValue(&item->data.input);
}

int getIntInputValue(InputField *field)
{
    if (field->settings.type != GEUI_IntInput)
    {
        DEBUG_MSG_FROM("field is not an int input", "getIntInputValue");
        return 0;
    }

    field->settings.readValueFunc(field);
    field->settings.applyConstraintsFunc(field);

    return field->value.intValue;
}

int readIntInput(WindowItem *item)
{
    if (item->type != GEUI_Input)
    {
        DEBUG_MSG_FROM("item is not an input field", "readIntInput");
        return 0;
    }

    return getIntInputValue(&item->data.input);
}

float getDecimalInputValue(InputField *field)
{
    if (field->settings.type != GEUI_DecimalInput)
    {
        DEBUG_MSG_FROM("field is not a decimal input", "getDecimalInputValue");
        return 0;
    }

    field->settings.readValueFunc(field);
    field->settings.applyConstraintsFunc(field);

    return field->value.decimalValue;
}

float readDecimalInput(WindowItem *item)
{
    if (item->type != GEUI_Input)
    {
        DEBUG_MSG_FROM("item is not an input field", "readDecimalInput");
        return 0;
    }

    return getDecimalInputValue(&item->data.input);
}
