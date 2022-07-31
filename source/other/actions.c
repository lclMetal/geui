void close(Window *win, WindowItem *item)
{
    if (item->type == GEUI_Button)
        closeWindow(win);
}

void printItemsInPanel(Panel *panel)
{
    char temp[256];
    WindowItem *iterator = panel->iList;

    while (iterator)
    {
        sprintf(temp, "%-20s\t[%d]", iterator->tag, iterator->index);
        DEBUG_MSG_FROM(temp, "printItemsInPanel");
        if (iterator->type == GEUI_Panel)
        {
            printItemsInPanel(iterator->data.panel);
        }

        iterator = iterator->next;
    }
}

void printItemList(Window *win, WindowItem *item)
{
    printItemsInPanel(&win->mainPanel);
}

void openOtherWindow(Window *win, WindowItem *item)
{
    openWindow("second");
}

void setRandomColor(Window *win, WindowItem *item)
{
    colorActorByName("f", createRGB(rand(256), rand(256), rand(256), 1));
}

void toggleTransparency(Window *win, WindowItem *item)
{
    if (getclone(win->parentCName)->transp > 0)
        getclone(win->parentCName)->transp = 0;
    else
        getclone(win->parentCName)->transp = 0.5;

    if (getclone(getWindowByTag("tag")->parentCName)->transp > 0)
        getclone(getWindowByTag("tag")->parentCName)->transp = 0;
    else
        getclone(getWindowByTag("tag")->parentCName)->transp = 0.5;


    if (getclone(getWindowByTag("win3")->parentCName)->transp > 0)
        getclone(getWindowByTag("win3")->parentCName)->transp = 0;
    else
        getclone(getWindowByTag("win3")->parentCName)->transp = 0.5;
}

void movePacman(Window *win, WindowItem *item)
{
    getclone("pac")->xvelocity = 0.5;
}

void exit(Window *win, WindowItem *item)
{
    quitGEUI();
    ExitGame();
}

void exitos(Window *win, WindowItem *item)
{
    quitGEUI();
    ExitGame();
}
