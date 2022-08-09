// TODO: make functions return error codes instead of just exiting
// without doing anything, which can be difficult to debug

int isTopmostItemAtMouse(WindowItem *item);
void focusNextItemInWindow();
void doMouseEnter(const char *actorName);
void doMouseLeave(const char *actorName);
void doMouseButtonDown(const char *actorName, enum mouseButtonsEnum mButtonNumber);
void doMouseButtonUp(const char *actorName, enum mouseButtonsEnum mButtonNumber);
int onMouseClick(enum mouseButtonsEnum mButtonNumber);
int onMouseRelease(enum mouseButtonsEnum mButtonNumber);
int onMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
int onMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
int isMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
int isMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
int updateMouseButtonDown(enum mouseButtonsEnum mButtonNumber);
void updateMouseButtonUp(enum mouseButtonsEnum mButtonNumber);
void doKeyDown(WindowItem *item, int key);
void doKeyUp(WindowItem *item, int key);

int isTopmostItemAtMouse(WindowItem *item)
{
    int count;
    Actor *actors;

    actors = getAllActorsInCollision("a_geuiMouse.0", &count);

    if (actors)
    {
        int i;

        for (i = count - 1; i >= 0; i --)
        {
            if (!strcmp(actors[i].name, "a_gui"))
            {
                return (actors[i].myIndex > -1 &&
                        actors[i].myWindow == item->parent->index &&
                        actors[i].myPanel == item->myPanel->index &&
                        actors[i].myIndex == item->index);
            }
        }
    }

    return 0;
}

void focusNextItemInWindow()
{
    Window *window = getWindowByIndex(GEUIController.topIndex);
    WindowItem *nextFocus = NULL;

    if (GEUIController.focus && GEUIController.focus->parent->index == GEUIController.topIndex)
    {
        nextFocus = getNextFocusableItem(GEUIController.focus);
    }
    else
    {
        window = getWindowByIndex(GEUIController.topIndex);
        if (window && window->isOpen)
        {
            nextFocus = getItemFromPanelByIndex(&window->mainPanel, 0);
            if (nextFocus && nextFocus->focusable == False)
            {
                nextFocus = getNextFocusableItem(nextFocus);
            }
        }
    }

    if (nextFocus)
    {
        focusItem(nextFocus);
    }
}

void doMouseEnter(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName)))
        { DEBUG_MSG_FROM("actor doesn't exist", "doMouseEnter"); return; }
    if (actor->myWindow < 0 || actor->myPanel < 0 || actor->myIndex < 0)
        { DEBUG_MSG_FROM("actor window, panel or index is invalid", "doMouseEnter"); return; }
    if (!(window = getWindowByIndex(actor->myWindow)))
        { DEBUG_MSG_FROM("window is NULL", "doMouseEnter"); return; }
    if (!(item = getItemFromPanelByIndex(getPanelByIndex(&window->mainPanel, actor->myPanel), actor->myIndex)))
        { DEBUG_MSG_FROM("item is NULL", "doMouseEnter"); return; }

    switch (item->type)
    {
        case GEUI_Button:
            if (isTopmostItemAtMouse(item))
            {
                long start = item->data.button.bTileStartIndex;
                long end   = item->data.button.bTileEndIndex;

                if (item->data.button.state)
                    colorClones("a_gui", start, end, item->parent->style.buttonPressedColor);
                else
                    colorClones("a_gui", start, end, item->parent->style.buttonHilitColor);
            }
            else doMouseLeave(actorName);
        break;
    }
}

void doMouseLeave(const char *actorName)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName)))
        { DEBUG_MSG_FROM(actorName, "doMouseLeave"); DEBUG_MSG_FROM("actor doesn't exist", "doMouseLeave"); return; }
    if (actor->myWindow < 0 || actor->myPanel < 0 || actor->myIndex < 0)
        { DEBUG_MSG_FROM("actor window, panel or index is invalid", "doMouseLeave"); return; }
    if (!(window = getWindowByIndex(actor->myWindow)))
        { DEBUG_MSG_FROM("window is NULL", "doMouseLeave"); return; }
    if (!(item = getItemFromPanelByIndex(getPanelByIndex(&window->mainPanel, actor->myPanel), actor->myIndex)))
        { DEBUG_MSG_FROM("item is NULL", "doMouseLeave"); return; }

    switch (item->type)
    {
        case GEUI_Button:
            if (!isTopmostItemAtMouse(item))
            {
                long start = item->data.button.bTileStartIndex;
                long end   = item->data.button.bTileEndIndex;

                if (item->data.button.state)
                    colorClones("a_gui", start, end, item->parent->style.buttonPressedColor);
                else
                    colorClones("a_gui", start, end, item->parent->style.buttonColor);
            }
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonDown(const char *actorName, enum mouseButtonsEnum mButtonNumber)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName)))
        { DEBUG_MSG_FROM("actor doesn't exist", "doMouseButtonDown"); return; }
    if (actor->myWindow < 0)
        { DEBUG_MSG_FROM("actor window is invalid", "doMouseButtonDown"); return; }
    if (!(window = getWindowByIndex(actor->myWindow)))
        { DEBUG_MSG_FROM("window is NULL", "doMouseButtonDown"); return; }

    if (actor->myProperties & GEUI_TITLE_BAR)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        ChangeParent(actor->clonename, "(none)");
        ChangeZDepth(actor->clonename, ACTIVE_WINDOW_ZDEPTH);

        // store the current color of the event actor
        actor->myColorR = actor->r;
        actor->myColorG = actor->g;
        actor->myColorB = actor->b;
        //VisibilityState(actor->clonename, DONT_DRAW_ONLY);

        // make the event actor white to keep children's colors unchanged after parenting
        actor->r = actor->g = actor->b = 255;

        // create fake actor to cover the now white event actor
        fake = CreateActor("a_gui", window->style.guiAnim, window->parentCName, "(none)", 0, 0, false);
        fake->myWindow = window->index;
        fake->myPanel = -1;
        fake->myIndex = -1;
        fake->myProperties = GEUI_FAKE_ACTOR;
        actor->myFakeIndex = fake->cloneindex;
        ChangeZDepth(fake->clonename, FAKE_ACTOR_ZDEPTH);
        SendActivationEvent(getTile(actor->myFakeIndex)->clonename);

        wParent = getclone(window->parentCName);

        // calculate the window base parent's coordinates relative to the event actor
        xDist = ceil(wParent->x - x);
        yDist = ceil(wParent->y - y);

        // make event actor the parent of the window base parent
        ChangeParent(wParent->clonename, actor->clonename);
        wParent->x = xDist;
        wParent->y = yDist;

        actor->myProperties |= GEUI_CLICKED;
        FollowMouse(actor->clonename, BOTH_AXIS);
    }

    bringWindowToFront(window);

    if (actor->myIndex < 0) return;
    if (!(item = getItemFromPanelByIndex(getPanelByIndex(&window->mainPanel, actor->myPanel), actor->myIndex)))
        { DEBUG_MSG_FROM("item is NULL", "doMouseButtonDown"); return; }

    switch (item->type)
    {
        case GEUI_Button:
            focusItem(item);
            colorClones("a_gui",
                item->data.button.bTileStartIndex,
                item->data.button.bTileEndIndex, window->style.buttonPressedColor);
            item->data.button.state = 1;
        break;
        case GEUI_InputInt:
            focusItem(item);
        break;
        case GEUI_InputText:
            focusItem(item);
        break;
    }
}

// TODO: test without actorName, with just clonename
void doMouseButtonUp(const char *actorName, enum mouseButtonsEnum mButtonNumber)
{
    Actor *actor;
    Window *window;
    WindowItem *item;

    if (!actorExists2(actor = getclone(actorName)))
        { DEBUG_MSG_FROM("actor doesn't exist", "doMouseButtonUp"); return; }
    if (actor->myWindow < 0)
        { DEBUG_MSG_FROM("actor window is invalid", "doMouseButtonUp"); return; }
    if (!(window = getWindowByIndex(actor->myWindow)))
        { DEBUG_MSG_FROM("window is NULL", "doMouseButtonUp"); return; }

    if (actor->myProperties & GEUI_TITLE_BAR && actor->myProperties & GEUI_CLICKED)
    {
        Actor *fake;
        Actor *wParent;
        int xDist, yDist;

        wParent = getclone(window->parentCName);

        // reset window base parent's parent to none
        ChangeParent(wParent->clonename, "(none)");
        ChangeZDepth(actor->clonename, WINDOW_TILE_ZDEPTH);

        // calculate the event actor's coordinates relative to window the base parent
        xDist = ceil(actor->x - wParent->x);
        yDist = ceil(actor->y - wParent->y);

        // make the window base parent the parent of the event actor
        ChangeParent(actor->clonename, wParent->clonename);
        actor->x = xDist;
        actor->y = yDist;

        actor->myProperties &= ~GEUI_CLICKED;
        FollowMouse(actor->clonename, NONE_AXIS);

        // restore the event actor's original color
        actor->r = actor->myColorR;
        actor->g = actor->myColorG;
        actor->b = actor->myColorB;
        //VisibilityState(actor->clonename, ENABLE);

        // destroy the fake actor
        DestroyActor(getTile(actor->myFakeIndex)->clonename);
    }

    //ChangeZDepth(window->parentCName, 0.5);

    if (actor->myIndex < 0) return;
    if (!(item = getItemFromPanelByIndex(getPanelByIndex(&window->mainPanel, actor->myPanel), actor->myIndex)))
        { DEBUG_MSG_FROM("item is NULL", "doMouseButtonUp"); return; }

    switch (item->type)
    {
        case GEUI_Button:
        {
            long start = item->data.button.bTileStartIndex;
            long end   = item->data.button.bTileEndIndex;

            if (isTopmostItemAtMouse(item))
            {
                colorClones("a_gui", start, end, window->style.buttonHilitColor);
                if (item->data.button.state && item->data.button.actionFunction)
                    item->data.button.actionFunction(window, item);
            }
            else
            {
                colorClones("a_gui", start, end, window->style.buttonColor);
            }
            item->data.button.state = 0;
        }
        break;
    }
}

int onMouseClick(enum mouseButtonsEnum mButtonNumber)
{
    return (!strcmp(GEUIController.mButtonTopActorCName[mButtonNumber], clonename) &&
            GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseRelease(enum mouseButtonsEnum mButtonNumber)
{
    return (!strcmp(GEUIController.mButtonTopActorCName[mButtonNumber], clonename) &&
            !GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    return (GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int onMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    return (!GEUIController.mButtonState[mButtonNumber] &&
            GEUIController.activeButton == mButtonNumber);
}

int isMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    return (GEUIController.mButtonState[mButtonNumber]);
}

int isMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    return (!GEUIController.mButtonState[mButtonNumber]);
}

int updateMouseButtonDown(enum mouseButtonsEnum mButtonNumber)
{
    int i;
    int count;
    Actor *actors = NULL;

    GEUIController.mButtonState[mButtonNumber] = GEUI_MOUSE_DOWN;

    // Get the actors currently in collision with the mouse actor, and their count
    actors = getAllActorsInCollision("a_geuiMouse.0", &count);

    if (!actors) { DEBUG_MSG_FROM("no actors at mouse position", "updateMouseButtonDown"); return 1; }

    // If there currently is an array of actors stored for the mouse button, it needs to be emptied
    if (GEUIController.mButtonActors[mButtonNumber])
    {
        free(GEUIController.mButtonActors[mButtonNumber]);
        GEUIController.mButtonActors[mButtonNumber] = NULL;
        GEUIController.mButtonActorCount[mButtonNumber] = 0;
    }

    // Reset the variable used to store the top actor's name
    strcpy(GEUIController.mButtonTopActorCName[mButtonNumber], "");

    // Allocate memory for the array of actors in the current mouse actor position
    GEUIController.mButtonActors[mButtonNumber] = malloc(count * sizeof *actors);
    GEUIController.mButtonActorCount[mButtonNumber] = count; // Store the count of actors

    if (GEUIController.mButtonActors[mButtonNumber]) // If the memory allocation succeeded
    {
        // Copy the contents of the array returned by getAllActorsInCollision to
        // the allocated chunk of memory
        memcpy(GEUIController.mButtonActors[mButtonNumber], actors, count * sizeof *actors);
    }
    else // If the memory allocation failed
        { DEBUG_MSG_FROM("memory allocation failed", "updateMouseButtonDown"); return 2; }

    GEUIController.activeButton = mButtonNumber;

    // Of the clicked actors, find the one with the highest z depth
    if (GEUIController.mButtonActorCount[mButtonNumber] > 0)
        strcpy(GEUIController.mButtonTopActorCName[mButtonNumber],
            GEUIController.mButtonActors[mButtonNumber][GEUIController.mButtonActorCount[mButtonNumber] - 1].clonename);

    // Iterate through the array of actors and send an activation event to each of them
    for (i = 0; i < GEUIController.mButtonActorCount[mButtonNumber]; i ++)
    {
        SendActivationEvent(GEUIController.mButtonActors[mButtonNumber][i].clonename);
    }

    return 0;
}

void updateMouseButtonUp(enum mouseButtonsEnum mButtonNumber)
{
    int i;

    GEUIController.mButtonState[mButtonNumber] = GEUI_MOUSE_UP;
    GEUIController.activeButton = mButtonNumber;

    // If a top actor exists
    if (strlen(GEUIController.mButtonTopActorCName[mButtonNumber]) > 0)
        // Send activation event to the top actor
        SendActivationEvent(GEUIController.mButtonTopActorCName[mButtonNumber]);

    // Reset the variable used to store the top actor's name
    strcpy(GEUIController.mButtonTopActorCName[mButtonNumber], "");

    // If there is an array actors stored for the mouse button
    if (GEUIController.mButtonActors[mButtonNumber])
    {
        // Iterate through the array of actors and send an activation event to each of them
        for (i = 0; i < GEUIController.mButtonActorCount[mButtonNumber]; i ++)
        {
            SendActivationEvent(GEUIController.mButtonActors[mButtonNumber][i].clonename);
        }

        free(GEUIController.mButtonActors[mButtonNumber]);
        GEUIController.mButtonActors[mButtonNumber] = NULL;
        GEUIController.mButtonActorCount[mButtonNumber] = 0;
    }
}

void doKeyDown(WindowItem *item, int key)
{
    if (item)
    {
        switch (item->type)
        {
            case GEUI_Button:
                if (key == KEY_RETURN || key == KEY_SPACE)
                {
                    colorClones("a_gui",
                        item->data.button.bTileStartIndex,
                        item->data.button.bTileEndIndex, item->parent->style.buttonPressedColor);
                    item->data.button.state = 1;
                }
            break;
            case GEUI_InputInt:
                handleIntInput(&item->data.inputInt, key);

                if (key == KEY_RETURN || key == KEY_TAB)
                {
                    enforceIntLimits(&item->data.inputInt);
                }

                refreshText(&item->data.inputInt.text);
                updateCaretPosition(&item->data.inputInt.caret);
                {
                    char temp[256];
                    sprintf(temp, "int value: %d", item->data.inputInt.value);
                    DEBUG_MSG_FROM(temp, "inputInt doKeyDown");
                }
            break;
            case GEUI_InputText:
                handleTextInput(&item->data.inputText, key);
                refreshText(&item->data.inputText.text);
                updateCaretPosition(&item->data.inputText.caret);
            break;
        }
    }

    switch (key)
    {
        case KEY_TAB:
            focusNextItemInWindow();
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
                    long start = item->data.button.bTileStartIndex;
                    long end   = item->data.button.bTileEndIndex;

                    if (item->data.button.actionFunction)
                        item->data.button.actionFunction(item->parent, item);

                    colorClones("a_gui", start, end, item->parent->style.buttonColor);
                    item->data.button.state = 0;
                }
            break;
        }
    }
}
