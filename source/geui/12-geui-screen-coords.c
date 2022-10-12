#define GEUI_MOUSE_AT_CENTER        0
#define GEUI_MOUSE_AT_TOP_LEFT      1
#define GEUI_MOUSE_AT_TOP_CENTER    2

ScreenCoords createScreenCoords(float x, float y)
{
    ScreenCoords coords;
    coords.x = x;
    coords.y = y;
    return coords;
}

ScreenCoords getWPosAtMouse(struct WindowStruct *window, unsigned char mode)
{
    ScreenCoords result;
    ScreenCoords mouseAtCenterCoords;
    ScreenCoords mouseCoords = createScreenCoords(xmouse, ymouse);

    if (!window)
        return mouseCoords;

    mouseAtCenterCoords = createScreenCoords(xmouse - window->root.width * 0.5f,
                                             ymouse - window->root.height * 0.5f);

    switch (mode)
    {
        case GEUI_MOUSE_AT_CENTER:
            result = mouseAtCenterCoords;
            break;
        case GEUI_MOUSE_AT_TOP_LEFT:
            result = mouseCoords;
            break;
        case GEUI_MOUSE_AT_TOP_CENTER:
            result = createScreenCoords(mouseAtCenterCoords.x, mouseCoords.y);
            break;
        default:
            result = mouseCoords;
            DEBUG_MSG_FROM("invalid value given for parameter 'mode'", "getWPosAtMouse");
            break;
    }

    return result;
}

ScreenCoords getWPosAtScreenCenter(struct WindowStruct *window)
{
    if (!window)
        return createScreenCoords(view.width * 0.5f, view.width * 0.5f);

    return createScreenCoords(view.width * 0.5f - window->root.width * 0.5f,
                              view.height * 0.5f - window->root.height * 0.5f);
}
