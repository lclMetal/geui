#define GEUI_BUTTON_STRETCH 1
#define GEUI_BUTTON_TEXT_ALIGN_LEFT 2
#define GEUI_TITLE_CENTERED 1

typedef struct StyleStruct
{
    char guiAnim[100];

    short tileWidth;
    short tileHeight;

    Font *titleFont;
    Font *labelFont;
    Font *textFont;

    short padding;
    short focusWidth;
    short titleProperties;
    short buttonProperties;
    float buttonPadding;

    Color titleBgColor;
    Color windowBgColor;
    Color inputBgColor;

    Color titleColor;
    Color labelColor;
    Color textColor;

    Color buttonColor;
    Color buttonHilitColor;
    Color buttonPressedColor;

    Color focusColor;
}Style;

Style defStyle;
Style *setDimensions;

void getTileDimensions(Style *style)
{
    Actor*a=CreateActor("a_gui", style->guiAnim, "(none)", "(none)", 0, 0, true);
    setDimensions = style;
    SendActivationEvent(a->clonename);
    DestroyActor(a->clonename);
}

void setTileDimensions()
{
    (*setDimensions).tileWidth = width;
    (*setDimensions).tileHeight = height;
    setDimensions = NULL;
}

/*Style createStyle(const char guiAnim[100], Font *titleFont, Font *labelFont, Font *textFont,
                  short padding, short focusWidth, short titleProperties, short buttonProperties, float buttonPadding, Color titleBgColor,
                  Color windowBgColor, Color inputBgColor, Color titleColor, Color labelColor, Color textColor,
                  Color buttonColor, Color buttonHilitColor, Color buttonPressedColor,
                  Color focusColor)
{
    Style new;

    strcpy(new.guiAnim, guiAnim);
    new.titleFont = titleFont;
    new.labelFont = labelFont;
    new.textFont = textFont;
    new.padding = padding;
    new.focusWidth = focusWidth;
    new.titleProperties = titleProperties;
    new.buttonProperties = buttonProperties;
    new.buttonPadding = buttonPadding;
    new.titleBgColor = titleBgColor;
    new.windowBgColor = windowBgColor;
    new.inputBgColor = inputBgColor;
    new.titleColor = titleColor;
    new.labelColor = labelColor;
    new.textColor = textColor;
    new.buttonColor = buttonColor;
    new.buttonHilitColor = buttonHilitColor;
    new.buttonPressedColor = buttonPressedColor;
    new.focusColor = focusColor;

    return new;
}*/

void initStyle(Style *style)
{
    if (style)
    {
        *style = defStyle;
    }
}

void setStyleGuiSheet(Style *style, const char guiAnim[100])
{
    if (style)
    {
        strcpy(style->guiAnim, guiAnim);
    }
}

void setStylePadding(Style *style, short padding)
{
    if (style)
    {
        style->padding = padding;
    }
}

void setStyleWindowColor(Style *style, Color windowBgColor)
{
    if (style)
    {
        style->windowBgColor = windowBgColor;
    }
}

void setStyleInputColor(Style *style, Color inputBgColor)
{
    if (style)
    {
        style->inputBgColor = inputBgColor;
    }
}

void setStyleFocus(Style *style, short focusWidth, Color focusColor)
{
    if (style)
    {
        style->focusWidth = focusWidth;
        style->focusColor = focusColor;
    }
}

void setStyleTitleStyle(Style *style, Font *titleFont, short titleProperties, Color titleColor, Color titleBgColor)
{
    if (style)
    {
        style->titleFont = titleFont;
        style->titleProperties = titleProperties;
        style->titleColor = titleColor;
        style->titleBgColor = titleBgColor;
    }
}

void setStyleLabelStyle(Style *style, Font *labelFont, Color labelColor)
{
    if (style)
    {
        style->labelFont = labelFont;
        style->labelColor = labelColor;
    }
}

void setStyleTextStyle(Style *style, Font *textFont, Color textColor)
{
    if (style)
    {
        style->textFont = textFont;
        style->textColor = textColor;
    }
}

void setStyleButtonStyle(Style *style, short buttonProperties, float buttonPadding, Color buttonColor, Color buttonHilitColor, Color buttonPressedColor)
{
    if (style)
    {
        style->buttonProperties = buttonProperties;
        style->buttonPadding = buttonPadding;
        style->buttonColor = buttonColor;
        style->buttonHilitColor = buttonHilitColor;
        style->buttonPressedColor = buttonPressedColor;
    }
}

//This function defines and creates a new style
//windowActor - the name of the actor to be used as the background of the windows
//titleActor - the name of the actor to be used as the title text for the windows
//textActor - the name of the actor to be used as the text actor for the windows' contents
//buttonActor - the name of the actor to be used as the button actor for the windows
//windowAnim - the animation of the windowActor that should be used for the windows
//buttonAnim - the animation of the buttonActor that should be used for the buttons
//borderSize - the amount of pixels for the left, right and bottom borders of the windows
//topBarSize - the amount of pixels for the top border bar of the windows
//bgColor - the color to be used on the windowActor
//titleColor - the color to be used on the titleActor
//textColor - the color to be used on the textActor
//buttonColor - the color to be used on the buttonActor when the button is in idle state
//buttonHilitColor - the color to be used on the buttonActor when the button is highlighted
//buttonPressedColor - the color to be used on the buttonActor when the button is pressed
/*Style createStyle(char *windowActor, char *titleActor, char *textActor, char *buttonActor,
                  char *windowAnim, char *buttonAnim, int borderSize, int topBarSize,
                  Color bgColor, Color titleColor, Color textColor, Color buttonColor,
                  Color buttonHilitColor, Color buttonPressedColor)
{
    Style ptr;

    strcpy(ptr.windowActor, windowActor);
    strcpy(ptr.titleActor, titleActor);
    strcpy(ptr.textActor, textActor);
    strcpy(ptr.buttonActor, buttonActor);

    strcpy(ptr.windowAnim, windowAnim);
    strcpy(ptr.buttonAnim, buttonAnim);

    ptr.borderSize = borderSize;
    ptr.topBarSize = topBarSize;

    ptr.bgColor = bgColor;
    ptr.titleColor = titleColor;
    ptr.textColor = textColor;
    ptr.buttonColor = buttonColor;
    ptr.buttonHilitColor = buttonHilitColor;
    ptr.buttonPressedColor = buttonPressedColor;

    return ptr;
}*/
