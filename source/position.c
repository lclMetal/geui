typedef struct LayoutStruct
{
    int widthPercent;
    int heightPercent;
    int width;
    int height;
}Layout;

void setSizeByPercent(Window *window, WindowItem *item, int wp, int hp)
{
    item->layout.widthPercent = wp;
    item->layout.heightPercent = hp;
    item->layout.width = window->layout.width * wp * 0.01;
    item->layout.height = window->layout.height * hp * 0.01;
}

void setSize(Window *window, WindowItem *item, int w, int h)
{
    item->layout.widthPercent = w / (double)window->layout.width * 100;
    item->layout.heightPercent = h / (double)window->layout.height * 100;
    item->width = w;
    item->height = h;
}
