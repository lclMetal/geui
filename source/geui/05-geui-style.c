typedef struct StyleStruct
{
    char guiAnim[100];

    short tileWidth;
    short tileHeight;

    Font *titleFont;
    Font *labelFont;
    Font *textFont;

    short padding;

    Color titleBgColor;
    Color windowBgColor;
    Color inputBgColor;

    Color titleColor;
    Color labelColor;
    Color textColor;

    Color buttonColor;
    Color buttonHilitColor;
    Color buttonPressedColor;
}Style;

Style defStyle;
Style *setDimensions;

Style createStyle(const char guiAnim[100], Font *titleFont, Font *labelFont, Font *textFont,
                  short padding, Color titleBgColor, Color windowBgColor, Color inputBgColor,
                  Color titleColor, Color labelColor, Color textColor, Color buttonColor,
                  Color buttonHilitColor, Color buttonPressedColor);

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

Style createStyle(const char guiAnim[100], Font *titleFont, Font *labelFont, Font *textFont,
                  short padding, Color titleBgColor, Color windowBgColor, Color inputBgColor,
                  Color titleColor, Color labelColor, Color textColor, Color buttonColor,
                  Color buttonHilitColor, Color buttonPressedColor)
{
    Style new;

    strcpy(new.guiAnim, guiAnim);
    new.titleFont = titleFont;
    new.labelFont = labelFont;
    new.textFont = textFont;
    new.padding = padding;
    new.titleBgColor = titleBgColor;
    new.windowBgColor = windowBgColor;
    new.inputBgColor = inputBgColor;
    new.titleColor = titleColor;
    new.labelColor = labelColor;
    new.textColor = textColor;
    new.buttonColor = buttonColor;
    new.buttonHilitColor = buttonHilitColor;
    new.buttonPressedColor = buttonPressedColor;

    return new;
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
