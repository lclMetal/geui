WindowItem *addPanel(Panel *panel, char tag[256]);
void buildPanel(WindowItem *ptr);

WindowItem *addPanel(Panel *panel, char tag[256])
{
    WindowItem *ptr = initNewItem(GEUI_Panel, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addPanel"); return NULL; }

    ptr->data.panel = malloc(sizeof *ptr->data.panel);
    if (!ptr->data.panel)
    {
        free(ptr);
        DEBUG_MSG_FROM("memory allocation failed", "addPanel");
        return NULL;
    }

    ptr->data.panel->index = panel->parent->pIndex++;
    ptr->data.panel->iIndex = 0;
    ptr->data.panel->rows = 0;
    ptr->data.panel->cols = 0;
    ptr->data.panel->width = 0;
    ptr->data.panel->height = 0;
    ptr->data.panel->parent = panel->parent;

    return addItemToWindow(ptr);
}

void buildPanel(WindowItem *ptr)
{
    if (ptr->type != GEUI_Panel) { DEBUG_MSG_FROM("item is not a valid Panel item", "buildPanel"); return; }

    buildItems(ptr->data.panel);
}
