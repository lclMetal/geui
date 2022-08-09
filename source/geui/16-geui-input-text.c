#define GEUI_KEYBOARD_MAPPING_SIZE GEUI_KeyboardLayoutCount + 1

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
    { KEY_PAD_PERIOD,   '.',    ','     }
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

void handleTextInput(TextInputField *field, int key)
{
    char newChar = '\0';
    char *keys = GetKeyState();
    short shift = (keys[KEY_LSHIFT] || keys[KEY_RSHIFT]);
    short ctrl  = (keys[KEY_LCTRL] || keys[KEY_RCTRL]);
    short alt   = (keys[KEY_LALT] || keys[KEY_RALT]);


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

            case KEY_MINUS:         newChar = '-'; break;
            case KEY_PAD_DIVIDE:    newChar = '/'; break;
            case KEY_PAD_MULTIPLY:  newChar = '*'; break;
            case KEY_PAD_MINUS:     newChar = '-'; break;
            case KEY_PAD_PLUS:      newChar = '+'; break;
            case KEY_TAB:           newChar = '\t'; break;
            case KEY_RETURN:        newChar = '\n'; break;
            case KEY_EQUALS:
            case KEY_SLASH:
            case KEY_PAD_PERIOD:
                newChar = getLocalizedKeyboardChar(key, CURRENT_KEYBOARD);
                break;
            case KEY_BACKSPACE:
                if (ctrl > 0)
                    setTextText(&field->text, "");
                else
                    newChar = '\b';
                break;
        }
    }

    concatenateCharToText(&field->text, newChar);
}
