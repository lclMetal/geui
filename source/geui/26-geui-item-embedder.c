WindowItem *addEmbedder(Panel *panel, char tag[256], const char *actorName);
void buildEmbedder(WindowItem *ptr);

WindowItem *addEmbedder(Panel *panel, char tag[256], const char *actorName)
{
    Actor *actor;
    WindowItem *ptr = initNewItem(GEUI_Embedder, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addEmbedder"); return NULL; }

    if (!actorExists2(actor = getclone(actorName)))
    {
        DEBUG_MSG_FROM("actor doesn't exist", "addEmbedder");
        free(ptr);
        return NULL;
    }

    strcpy(ptr->data.embedder.actorCName, actor->clonename);
    VisibilityState(ptr->data.embedder.actorCName, DISABLE);

    ptr->layout.width = actor->width;
    ptr->layout.height = actor->height;

    return addItemToWindow(ptr);
}

void buildEmbedder(WindowItem *ptr)
{
    Actor *actor;
    if (ptr->type != GEUI_Embedder) { DEBUG_MSG_FROM("item is not a valid Embedder item", "buildEmbedder"); return; }

    if (!actorExists2(actor = getclone(ptr->data.embedder.actorCName))) { DEBUG_MSG_FROM("actor doesn't exist", "buildEmbedder"); return; }

    ChangeZDepth(ptr->data.embedder.actorCName, DEFAULT_ITEM_ZDEPTH);
    ChangeParent(ptr->data.embedder.actorCName, "(none)");
    actor->x = 0;
    actor->y = 0;
    ChangeParent(ptr->data.embedder.actorCName, ptr->parent->parentCName);
    actor->x = ptr->layout.startx + ptr->parent->style.padding + actor->width / 2;
    actor->y = ptr->layout.starty + ptr->parent->style.padding + actor->height / 2;
    VisibilityState(ptr->data.embedder.actorCName, ENABLE);

    // {
        // char temp[256];
        // sprintf(temp, "%d, %d - %d, %d", ptr->layout.startx + ptr->parent->style.padding + actor->width / 2,
                                // ptr->layout.starty + ptr->parent->style.padding + actor->height / 2,
                                // (int)actor->x,
                                // (int)actor->y);
        // DEBUG_MSG_FROM(temp, "buildEmbedder");
    // }
}
