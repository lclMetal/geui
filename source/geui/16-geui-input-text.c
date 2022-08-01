void handleTextInput(TextInputField *field, int key)
{
    char *keys = GetKeyState();

    field->modifier = 0;
    field->modifier |= GEUI_TEXT_INPUT_SHIFT * (keys[KEY_LSHIFT] || keys[KEY_RSHIFT]);
    field->modifier |= GEUI_TEXT_INPUT_CTRL  * (keys[KEY_LCTRL] || keys[KEY_RCTRL]);
    field->modifier |= GEUI_TEXT_INPUT_ALT   * (keys[KEY_LALT] || keys[KEY_RALT]);

    {
        char temp[256];
        sprintf(temp, "shift: %d\tctrl: %d\talt: %d", field->modifier & GEUI_TEXT_INPUT_SHIFT,
                                                      field->modifier & GEUI_TEXT_INPUT_CTRL,
                                                      field->modifier & GEUI_TEXT_INPUT_ALT);
        DEBUG_MSG_FROM(temp, "handleTextInput");
    }
}
