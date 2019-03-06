#define HALF_WIDTH (width / 2)
#define HALF_HEIGHT (height / 2)
#define HALF_WIDTH_ACTOR(X) (X->width / 2)
#define HALF_HEIGHT_ACTOR(X) (X->height / 2)

char *addFileExtension(char *fileName, char *fileExtension);
int actorExists(char *actorName);
int actorExists2(Actor *a);
Actor *gc2(char *actorName, int cloneNum);
void destroyClone(const char *actorName, int cloneNum);
void destroyClones(const char *actorName, int startIndex, int endIndex);
void disableMouseEvents(char *actorName);
void enableMouseEvents(char *actorName);
int mouseOver(void);
int mouseOverRegion(void);
int mouseOverPartOfRegion(int startX, int startY, int endX, int endY);
int mouseOverActor(const char *actorName);
int mouseOverClones(const char *actorName, int startIndex, int endIndex);

char tempFullFileName[256];
char *addFileExtension(char *fileName, char *fileExtension)
{
    char *fileExtensionPos;

    fileExtensionPos = strchr(fileName, '.');

    if (fileExtensionPos == NULL) sprintf(tempFullFileName, "%s.%s", fileName, fileExtension);
    else
    {
        int actualFileNameLength = strlen(fileName) - strlen(fileExtensionPos);

        strncpy(tempFullFileName, fileName, actualFileNameLength);
        sprintf(tempFullFileName + actualFileNameLength, ".%s", fileExtension);
    }

    return tempFullFileName;
}

int actorExists(char *actorName)
{
    return (getclone(actorName)->cloneindex != -1);
}

// ----- WARNING! -----
//
// Using this function on a stale pointer is undefined behavior!
// Use this function ONLY for checking the result of getclone() right after fetching it.
//
// If you didn't fully understand the above warnings, DO NOT USE THIS FUNCTION!
// Use the above version instead (actorExists())
//
// This function only exists for optimizing performance in cases where a fresh actor pointer
// is already at hand.
int actorExists2(Actor *a)
{
    return (a->cloneindex != -1);
}

Actor *gc2(char *actorName, int cloneNum)
{
    char cName[256];
    sprintf(cName, "%s.%i", actorName, cloneNum);
    return getclone(cName);
}

void destroyClone(const char *actorName, int cloneNum)
{
    char cName[256];
    sprintf(cName, "%s.%i", actorName, cloneNum);
    DestroyActor(cName);
}

void destroyClones(const char *actorName, int startIndex, int endIndex)
{
    int i;
    char cName[256];

    for (i = startIndex; i <= endIndex; i ++)
    {
        sprintf(cName, "%s.%i", actorName, i);
        DestroyActor(cName);
    }
}

void disableMouseEvents(char *actorName)
{
    EventDisable(actorName, EVENTMOUSEBUTTONDOWN);
    EventDisable(actorName, EVENTMOUSEBUTTONUP);
    EventDisable(actorName, EVENTMOUSEENTER);
    EventDisable(actorName, EVENTMOUSELEAVE);
}

void enableMouseEvents(char *actorName)
{
    EventEnable(actorName, EVENTMOUSEBUTTONDOWN);
    EventEnable(actorName, EVENTMOUSEBUTTONUP);
    EventEnable(actorName, EVENTMOUSEENTER);
    EventEnable(actorName, EVENTMOUSELEAVE);
}

int mouseOver(void)
{
    return (xmouse >= xscreen - HALF_WIDTH &&
            xmouse <  xscreen - HALF_WIDTH + width &&
            ymouse >= yscreen - HALF_HEIGHT &&
            ymouse <  yscreen - HALF_HEIGHT + height);
}

int mouseOverRegion(void)
{
    return (xmouse >= xscreen && xmouse < xscreen + width &&
            ymouse >= yscreen && ymouse < yscreen + height);
}

int mouseOverPartOfRegion(int startX, int startY, int endX, int endY)
{
    return (xmouse >= xscreen + startX && xmouse < xscreen + width - endX &&
            ymouse >= yscreen + startY && ymouse < yscreen + height - endY);
}

int mouseOverActor(const char *actorName)
{
    Actor *actor;

    if (!actorExists2(actor = getclone(actorName))) return 0;

    return (xmouse >= actor->xscreen - HALF_WIDTH_ACTOR(actor) &&
            xmouse <  actor->xscreen - HALF_WIDTH_ACTOR(actor) + actor->width &&
            ymouse >= actor->yscreen - HALF_HEIGHT_ACTOR(actor) &&
            ymouse <  actor->yscreen - HALF_HEIGHT_ACTOR(actor) + actor->height);
}

int mouseOverClones(const char *actorName, int startIndex, int endIndex)
{
    int i;
    char cName[256];

    for (i = startIndex; i <= endIndex; i ++)
    {
        sprintf(cName, "%s.%i", actorName, i);
        if (mouseOverActor(cName)) return 1;
    }

    return 0;
}
