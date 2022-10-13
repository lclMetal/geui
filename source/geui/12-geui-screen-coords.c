ScreenCoords createScreenCoords(float x, float y)
{
    ScreenCoords coords;
    coords.x = x;
    coords.y = y;
    return coords;
}

ScreenCoords getWPosAtMouse(struct WindowStruct *window, WPosSetting mode)
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
        case GEUI_WPosMouseCenter:  result = mouseAtCenterCoords;   break;
        case GEUI_WPosMouseTopLeft: result = mouseCoords;           break;
        case GEUI_WPosMouseTop:
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
