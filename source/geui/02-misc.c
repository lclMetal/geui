#define HALF_WIDTH (width / 2)
#define HALF_HEIGHT (height / 2)
#define HALF_WIDTH_ACTOR(X) (X->width / 2)
#define HALF_HEIGHT_ACTOR(X) (X->height / 2)

int safeStrcat(char *destination, const char *source, int maxLen);
char *addFileExtension(char *fileName, char *fileExtension);
int actorExists(char *actorName);
int actorExists2(Actor *a);
Actor *gc2(char *actorName, long cloneNum);
void destroyClone(const char *actorName, long cloneNum);
void destroyClones(const char *actorName, long startIndex, long endIndex);
void changeParentOfClones(const char *actorName, long startIndex, long endIndex, const char *parentName);
void disableMouseEvents(char *actorName);
void enableMouseEvents(char *actorName);
int mouseOver(void);
int mouseOverRegion(void);
int mouseOverPartOfRegion(int startX, int startY, int endX, int endY);
int mouseOverActor(const char *actorName);
int mouseOverClones(const char *actorName, long startIndex, long endIndex);

int safeStrcat(char *destination, const char *source, int maxLen)
{
    int dLen = strlen(destination);                   // Current length of destination string
    int sLen = strlen(source);                        // Current length of source string
    int catLen = sLen - (dLen + sLen - (maxLen - 1)); // Calculate how many characters at most
                                                      // can be concatenated to destination

    strncat(destination, source, catLen); // Concatenate as much as possible

    return (catLen < sLen); // Return 1 if some of the source string had to be left out
}

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
    // GE uses the cloneindex -1 to indicate that an actor is not a valid, existing actor.
    // The actor count needs to be checked because:
    //
    // 1. IF an actor with the given base name exists in the project
    // 2. AND the lowest indexed clone (= the actor itself, if no clones present)
    //      in editor mode has "Create at startup" set to "No"
    // 3. AND the actor currently has no alive instances in the game
    //
    // ..getclone will return an invalid Actor* that has the cloneindex of that specific
    // clone / actor over in editor mode despite it not existing in the game.
    // And that would break this function without the ActorCount check.
    Actor *a = getclone(actorName);
    return (ActorCount(a->name) > 0 && a->cloneindex != -1);
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
    // GE uses the cloneindex -1 to indicate that an actor is not a valid, existing actor.
    // The actor count needs to be checked because:
    //
    // 1. IF an actor with the given base name exists in the project
    // 2. AND the lowest indexed clone (= the actor itself, if no clones present)
    //      in editor mode has "Create at startup" set to "No"
    // 3. AND the actor currently has no alive instances in the game
    //
    // ..getclone will return an invalid Actor* that has the cloneindex of that specific
    // clone / actor over in editor mode despite it not existing in the game.
    // And that would break this function without the ActorCount check.
    return (ActorCount(a->name) > 0 && a->cloneindex != -1);
}

Actor *gc2(char *actorName, long cloneNum)
{
    char cName[256];
    sprintf(cName, "%s.%d", actorName, cloneNum);
    return getclone(cName);
}

void destroyClone(const char *actorName, long cloneNum)
{
    char cName[256];
    sprintf(cName, "%s.%d", actorName, cloneNum);
    DestroyActor(cName);
}

void destroyClones(const char *actorName, long startIndex, long endIndex)
{
    long i;
    char cName[256];

    if (startIndex < 0 || endIndex < 0) return;

    for (i = startIndex; i <= endIndex; i ++)
    {
        sprintf(cName, "%s.%d", actorName, i);
        DestroyActor(cName);
    }
}

void changeParentOfClones(const char *actorName, long startIndex, long endIndex, const char *parentName)
{
    long i;
    char cName[256];

    if (startIndex < 0 || endIndex < 0) return;

    for (i = startIndex; i <= endIndex; i ++)
    {
        sprintf(cName, "%s.%d", actorName, i);
        ChangeParent(cName, parentName);
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

int mouseOverClones(const char *actorName, long startIndex, long endIndex)
{
    long i;
    char cName[256];

    if (startIndex < 0 || endIndex < 0) return 0;

    for (i = startIndex; i <= endIndex; i ++)
    {
        sprintf(cName, "%s.%d", actorName, i);
        if (mouseOverActor(cName)) return 1;
    }

    return 0;
}
