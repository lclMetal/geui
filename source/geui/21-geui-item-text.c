WindowItem *addText(Panel *panel, char tag[256], char *string, short maxWidth);
void buildText(WindowItem *ptr);

WindowItem *addText(Panel *panel, char tag[256], char *string, short maxWidth)
{
    WindowItem *ptr = initNewItem(GEUI_Text, panel, tag);
    if (!ptr) { DEBUG_MSG_FROM("item is NULL", "addText"); return NULL; }

    ptr->data.text = createText(string, panel->parent->style->textFont, "(none)", ABSOLUTE, 0, 0);
    setTextColor(&ptr->data.text, panel->parent->style->textColor);
    setTextZDepth(&ptr->data.text, DEFAULT_ITEM_ZDEPTH);

    if (maxWidth > 0)
        fitTextInWidth(&ptr->data.text, maxWidth);

    ptr->layout.width = ptr->data.text.width;
    ptr->layout.height = max(ptr->data.text.height + ptr->data.text.pFont->baselineOffset, ptr->parent->style->tileHeight);

    return addItemToWindow(ptr);
}

void buildText(WindowItem *ptr)
{
    if (ptr->type != GEUI_Text) { DEBUG_MSG_FROM("item is not a valid Text item", "buildText"); return; }

    setTextZDepth(&ptr->data.text, DEFAULT_ITEM_ZDEPTH);
    // TODO: layout / positioning
    setTextPosition(&ptr->data.text,
        ptr->layout.startx + ptr->parent->style->padding,
        ptr->layout.starty + ptr->parent->style->tileHeight * 0.5 + ceil(ptr->data.text.pFont->baselineOffset * 0.5));
    refreshText(&ptr->data.text);
}

// This function allows editing the text of a text type
// window item. Notice that using the function currently
// erases the existing text if the window is open and
// does not re-render the text automatically. At this
// point it's advisable to only use the function to change
// the text content _before_ the window is opened.
void setTextContent(WindowItem *ptr, char *string)
{
    if (ptr->type != GEUI_Text) { DEBUG_MSG_FROM("item is not a valid Text item", "setTextContent"); return; }

    setTextText(&ptr->data.text, string);
}
