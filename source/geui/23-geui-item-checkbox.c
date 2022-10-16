WindowItem *addCheckbox(Panel *panel, char tag[256], bool state);
void buildCheckbox(WindowItem *ptr);

WindowItem *addCheckbox(Panel *panel, char tag[256], bool state)
{
    WindowItem *ptr = initNewItem(GEUI_Checkbox, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addCheckbox"); return NULL; }

    ptr->focusable = True;
    ptr->data.checkbox.state = state;
    ptr->data.checkbox.tileIndex = -1;

    ptr->layout.width = ptr->parent->style.tileWidth;
    ptr->layout.height = ptr->parent->style.tileHeight;

    return addItemToWindow(ptr);
}

void buildCheckbox(WindowItem *ptr)
{
    Actor *a = CreateActor("a_gui", ptr->parent->style.guiAnim, ptr->parent->parentCName, "(none)", 0, 0, true);
    a->x = ptr->layout.startx + ptr->parent->style.tileWidth / 2;
    a->x += ptr->parent->style.padding;
    a->y = ptr->layout.starty + ptr->parent->style.tileHeight / 2;
    a->y += ptr->parent->style.padding;
    ChangeZDepth(a->clonename, DEFAULT_ITEM_ZDEPTH);
    colorActor(a, ptr->parent->style.buttonColor);
    a->animpos = 24 + (ptr->data.checkbox.state == True);
    a->myWindow = ptr->parent->index;
    a->myPanel  = ptr->myPanel->index;
    a->myIndex  = ptr->index;
    ptr->data.checkbox.tileIndex = a->cloneindex;
}
