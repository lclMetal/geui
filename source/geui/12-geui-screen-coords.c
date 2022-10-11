ScreenCoords createScreenCoords(float x, float y)
{
    ScreenCoords coords;
    coords.x = x;
    coords.y = y;
    return coords;
}

ScreenCoords getMouseCoords()
{
    return createScreenCoords(xmouse, ymouse);
}

ScreenCoords getCenteredWindowCoords(struct WindowStruct *window)
{
    if (!window)
        return createScreenCoords(view.width * 0.5f, view.width * 0.5f);

    return createScreenCoords(view.width * 0.5f - window->root.width * 0.5f,
                              view.height * 0.5f - window->root.height * 0.5f);
}
