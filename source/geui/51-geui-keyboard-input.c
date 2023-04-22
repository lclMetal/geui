void doKeyDown(WindowItem *item, int key);
void doKeyUp(WindowItem *item, int key);

void doKeyDown(WindowItem *item, int key)
{
    char *keys = GetKeyState();

    if (item)
    {
        switch (item->type)
        {
            case GEUI_Button:
                if (key == KEY_RETURN || key == KEY_SPACE)
                {
                    colorGuiTiles(item->data.button.tiles, item->parent->style->buttonPressedColor);
                    item->data.button.state = 1;
                }
            break;
            case GEUI_Input:
                handleInputFieldInput(&item->data.input, key);
                refreshText(&item->data.input.text);
                updateCaretPosition(&item->data.input.caret);
            break;
        }
    }

    switch (key)
    {
        case KEY_TAB:
            focusNextItemInWindow(keys[KEY_LSHIFT] || keys[KEY_RSHIFT]);
        break;
    }
}

void doKeyUp(WindowItem *item, int key)
{
    if (item)
    {
        switch (item->type)
        {
            case GEUI_Button:
                if ((key == KEY_RETURN || key == KEY_SPACE) && item->data.button.state == 1)
                {
                    if (item->data.button.action.fpAction)
                        item->data.button.action.fpAction(&item->data.button.action);

                    colorGuiTiles(item->data.button.tiles, item->parent->style->buttonColor);
                    item->data.button.state = 0;
                }
            break;
            case GEUI_Checkbox:
                if (key == KEY_RETURN || key == KEY_SPACE)
                {
                    item->data.checkbox.state = !item->data.checkbox.state;
                    getTile(item->data.checkbox.tileIndex)->animpos = 24 + item->data.checkbox.state;
                }
            break;
        }
    }
}
