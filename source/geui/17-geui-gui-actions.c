GUIAction createAction(void (*fpAction)(struct GUIActionStruct *));

void guiActionOpenWindow(GUIAction *action);
GUIAction createOpenWindowAction(char tag[256], WindowPosition pos);

void guiActionCloseWindow(GUIAction *action);
GUIAction createCloseWindowAction(char tag[256]);

// from geui-window.c
Window *openWindow(char tag[256], WindowPosition pos);
void closeWindow(Window *window);
Window *getWindowByTag(char tag[256]);

GUIAction createAction(void (*fpAction)(struct GUIActionStruct *))
{
    GUIAction action;
    action.type = GEUI_ACTION;
    action.fpAction = fpAction;
    return action;
}

void guiActionOpenWindow(GUIAction *action)
{
    if (action->type == GEUI_ACTION_OPEN_WINDOW)
    {
        openWindow(action->data.openWindow.tag, action->data.openWindow.pos);
    }
}

GUIAction createOpenWindowAction(char tag[256], WindowPosition pos)
{
    GUIAction action;
    action.type = GEUI_ACTION_OPEN_WINDOW;
    strcpy(action.data.openWindow.tag, tag);
    action.data.openWindow.pos = pos;
    (action.fpAction = guiActionOpenWindow); // parentheses required due to GE bug
    return action;
}

void guiActionCloseWindow(GUIAction *action)
{
    if (action->type == GEUI_ACTION_CLOSE_WINDOW)
    {
        closeWindow(getWindowByTag(action->data.closeWindow.tag));
    }
}

GUIAction createCloseWindowAction(char tag[256])
{
    GUIAction action;
    action.type = GEUI_ACTION_CLOSE_WINDOW;
    strcpy(action.data.closeWindow.tag, tag);
    (action.fpAction = guiActionCloseWindow); // parentheses required due to GE bug
    return action;
}
