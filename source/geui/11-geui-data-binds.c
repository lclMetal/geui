void initWindowDataBinds(Window *window)
{
    int i;

    if (!window) return;

    for (i = 0; i < GEUI_MAX_DATA_BINDS_PER_WINDOW; i++)
    {
        window->dataBinds[i].data = NULL;
        strcpy(window->dataBinds[i].tag, "");
    }
}

int addDataBind(Window *window, char tag[256], void *data)
{
    if (!window) return 1; // window is NULL
    if (window->dataBindIndex >= GEUI_MAX_DATA_BINDS_PER_WINDOW)
        return 2; // max data binds already in use

    window->dataBinds[window->dataBindIndex].data = data;
    strcpy(window->dataBinds[window->dataBindIndex].tag, tag);
    window->dataBindIndex++;

    return 0;
}

int updateDataBind(Window *window, char tag[256], void *data)
{
    int i;

    if (!window) return 1; // window is NULL

    for (i = 0; i < GEUI_MAX_DATA_BINDS_PER_WINDOW; i++)
    {
        if (!strcmp(window->dataBinds[i].tag, tag))
        {
            window->dataBinds[i].data = data;
            return 0;
        }
    }

    return 2; // data bind not found
}

void removeDataBind(Window *window, char tag[256])
{
    int i;

    if (!window) return; // window is NULL

    for (i = 0; i < GEUI_MAX_DATA_BINDS_PER_WINDOW; i++)
    {
        if (!strcmp(window->dataBinds[i].tag, tag))
        {
            window->dataBinds[i].data = NULL;
            strcpy(window->dataBinds[i].tag, "");
            return;
        }
    }
}

void *getBoundData(Window *window, char tag[256])
{
    int i;

    if (!window) return NULL; // window is NULL

    for (i = 0; i < GEUI_MAX_DATA_BINDS_PER_WINDOW; i++)
    {
        if (!strcmp(window->dataBinds[i].tag, tag))
        {
            return window->dataBinds[i].data;
        }
    }

    return NULL;
}
