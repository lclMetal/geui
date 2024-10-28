WindowItem *addDataBind(Panel *panel, char tag[256], void *data);
int updateDataBind(Panel *panel, char tag[256], void *data);
void *getBoundData(Panel *panel, char tag[256]);

WindowItem *addDataBind(Panel *panel, char tag[256], void *data)
{
    WindowItem *ptr = initNewItem(GEUI_DataBind, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addDataBind"); return NULL; }

    ptr->data.dataBind = data;

    return addItemToWindow(ptr);
}

int updateDataBind(Panel *panel, char tag[256], void *data)
{
    WindowItem *ptr = getItemFromPanelByTag(panel, tag);

    if (!ptr) { DEBUG_MSG_FROM("item not found", "getBoundData"); return 1; }
    if (ptr->type != GEUI_DataBind) { DEBUG_MSG_FROM("item is not a valid DataBind item", "getBoundData"); return 2; }

    ptr->data.dataBind = data;

    return 0;
}

void *getBoundData(Panel *panel, char tag[256])
{
    WindowItem *ptr = getItemFromPanelByTag(panel, tag);

    if (!ptr) { DEBUG_MSG_FROM("item not found", "getBoundData"); return NULL; }
    if (ptr->type != GEUI_DataBind) { DEBUG_MSG_FROM("item is not a valid DataBind item", "getBoundData"); return NULL; }

    return ptr->data.dataBind;
}
