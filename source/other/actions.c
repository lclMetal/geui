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

void printItems(GUIAction *action)
{
    printItemsInPanel(&action->window->root);
}

void setRandomColor(GUIAction *action)
{
    colorActorByName("f", createRGB(rand(256), rand(256), rand(256), 1));
}

void toggleTransparency(GUIAction *action)
{
    if (getclone(action->window->parentCName)->transp > 0)
        getclone(action->window->parentCName)->transp = 0;
    else
        getclone(action->window->parentCName)->transp = 0.5;

    if (getclone(getWindowByTag("tag")->parentCName)->transp > 0)
        getclone(getWindowByTag("tag")->parentCName)->transp = 0;
    else
        getclone(getWindowByTag("tag")->parentCName)->transp = 0.5;


    if (getclone(getWindowByTag("win3")->parentCName)->transp > 0)
        getclone(getWindowByTag("win3")->parentCName)->transp = 0;
    else
        getclone(getWindowByTag("win3")->parentCName)->transp = 0.5;
}

void movePacman(GUIAction *action)
{
    getclone("pac")->xvelocity = 0.5;
}

void exit(GUIAction *action)
{
    quitGEUI();
    ExitGame();
}

void exitos(GUIAction *action)
{
    quitGEUI();
    ExitGame();
}

void teleportActor(GUIAction *action)
{
    Actor *a = getBoundData(getWindowRootPanel(action->window), "actorToTp");
    a = getclone(a->clonename);
    a->xscreen = rand(view.width - 100) + 50;
    a->yscreen = rand(view.height - 100) + 50;

    {
        char temp[256];
        sprintf(temp, "%s %d %d %d", a->clonename, a->cloneindex, (int)a->xscreen, (int)a->yscreen);
        DEBUG_MSG_FROM(temp, "teleportActor");
    }
}
