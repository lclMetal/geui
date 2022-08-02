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

    concatenateCharToText(&field->text, newChar);
}
