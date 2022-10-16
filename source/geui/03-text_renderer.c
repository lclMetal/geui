#define CAPACITY_STEP 256

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

#define SKIP_ESCAPE_SEQUENCE_MARKER 1
#define ESCAPE_SEQUENCE_CHARACTER_LENGTH 1
#define SKIP_COLOR_ESCAPE_SEQUENCE 2
#define SKIP_COLOR_VALUES 5
#define SKIP_COLOR_CHANGE 7
#define COLOR_ESCAPE_SEQUENCE_LENGTH 7
#define SKIP_ENDING_ESCAPE_SEQUENCE 2
#define ENDING_ESCAPE_SEQUENCE_LENGTH 2

#define ASCII_START 33
#define NULL_ACTOR getclone("-")

#define ABSOLUTE False
#define RELATIVE True

typedef enum { False, True } bool;

#define CHARS 94

typedef struct
{
    int letterSpacing;
    int wordSpacing;
    int lineSpacing;
    int baselineOffset;
    int indentation;
    char fontName[256];
    int fontCharWidths[CHARS];
}Font;

typedef struct LetterCloneindexStruct
{
    int index;
    struct LetterCloneindexStruct *next;
}LetterCloneindex;

typedef struct
{
    bool rendered;
    int lastRenderFrame;

    int beginX;
    int beginY;
    bool relative;
    int alignment;

    int textAreaLeftX;
    int textAreaTopY;
    int textAreaRightX;
    int textAreaBottomY;
    int textAreaScrollPosition;
    int textAreaScrollUpLimit;
    int textAreaScrollDownLimit;
    double textAreaHeightPercent;
    double textAreaScrollPercent;
    bool fitInArea;

    size_t capacity;
    char *pString;

    LetterCloneindex *letterIndexHead;
    LetterCloneindex *letterIndexTail;

    int width;
    int height;

    double zDepth;
    char parentCName[256];

    Font *pFont;
    Color color;
}Text;

char typeActorName[2][256] = {"a_typeActor", "a_typeActor2"};

// Functions for use
void fitTextInWidth(Text *pText, int widthLimit);
void fitTextInArea(Text *pText, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);
void scrollTextByAmount(Text *pText, int scroll);
void setTextScrollByPercent(Text *pText, double scrollPercent);
void readFontDataFile(char *fileName, Font *fontData);
size_t calculateRequiredCapacity(size_t len);
void updateTextDimensions(Text *pText);
Text createText(const char *string, Font *pFont, const char *parentCName, bool relative, int startX, int startY);
void setTextAlignment(Text *pText, int alignment);
void setTextColor(Text *pText, Color color);
void setTextZDepth(Text *pText, double zDepth);
void setTextParent(Text *pText, char *parentCName, bool keepCurrentPosition);
void setTextPosition(Text *pText, int posX, int posY);
void concatenateText(Text *pText, char *string);
void concatenateCharToText(Text *pText, char c);
void setTextText(Text *pText, char *string);
void refreshText(Text *pText);
void eraseText(Text *pText);
void destroyText(Text *pText);

// Internal system functions
int fromASCII(int num);
Color extractHexColor(char *pString);
int calculateTextHeight(Text *pText);
int calculateTextWidth(Text *pText);
int calculateStringWidth(Font *pFont, const char *string);
int getSpecialCharWidth(Text *pText, int pos);
int getCharWidth(Text *pText, int pos);
int isIndentationAllowed(Text *pText);
int getPrevTabStop(Text *pText, int lineWidth);
int getNextTabStopOffset(Text *pText, int lineWidth);
int strCharsLeft(const char *str, int pos);
int isPrintable(const char *str, int pos);
int getLetterSpacing(Text *pText, int lineWidth, int charWidth, int pos);
int calculateLineSubRangeWidth(Text *pText, int start, int end);
int calculateLineTailWidth(Text *pText, int start);
int parseToNextLineStart(Text *pText, int startPosition, Color *pTempColor, bool *pCustomColorPrint);
void replaceAllInstancesOfChar(char *pString, char replace, char new);
void handleEscapeSequences(Text *pText, int *iterator, int *pPrevX, int *pPrevY, Color *pTempColor, bool *pCustomColorPrint, bool *firstOnLine);
void skipEscapeSequences(char *pString, int *iterator, Color *pTempColor, bool *pCustomColorPrint);
void renderText(Text *pText);
int renderCharacter(Text *pText, char printChar, Color color, bool relative, int xPos, int yPos, bool firstOnLine);
LetterCloneindex *newLetterCloneindex(int index);
void storeLetterCloneindex(Text *pText, int index);
void freeLetterCloneindexList(Text *pText);
void execOnLCList(Text *pText, LetterCloneindex *list, LetterCloneindex *(*pFunc)(Text *, LetterCloneindex *));
LetterCloneindex *freeLC(Text *pText, LetterCloneindex *this);
LetterCloneindex *setLCZDepth(Text *pText, LetterCloneindex *this);
LetterCloneindex *setLCParent(Text *pText, LetterCloneindex *this);
LetterCloneindex *eraseLC(Text *pText, LetterCloneindex *this);

int fromASCII(int num)
{
    int value = num - ASCII_START;

    if (value < 0 || value > CHARS - 1) return '?' - ASCII_START;
    else return value;
}

Color extractHexColor(char *pString)
{
    Color tempColor;
    unsigned int rValue;
    unsigned int gValue;
    unsigned int bValue;

    sscanf(pString, "%02x%02x%02x", &rValue, &gValue, &bValue);

    tempColor = createRGB(rValue, gValue, bValue, 1.0);

    return tempColor;
}

int calculateTextHeight(Text *pText)
{
    int i;
    int lineCount = 1;
    int len;
    char *pString = pText->pString;

    len = strlen(pString);

    for (i = 0; i < len; i ++)
        if (pString[i] == '\n' || pString[i] == '\v') lineCount ++;

    return pText->pFont->lineSpacing * lineCount;
}

int calculateTextWidth(Text *pText)
{
    int i;
    int len;
    int lineWidth;
    int maximumLineWidth;
    char *pString = pText->pString;

    maximumLineWidth = calculateLineTailWidth(pText, 0);
    len = strlen(pString);

    for (i = 0; i < len; i ++)
    {
        if (pString[i] == '\n' || pString[i] == '\v')
        {
            lineWidth = calculateLineTailWidth(pText, i + 1);

            if (lineWidth > maximumLineWidth) maximumLineWidth = lineWidth;
        }
    }

    return maximumLineWidth;
}

int calculateStringWidth(Font *pFont, const char *string)
{
    Text tempText = createText(string, pFont, "(none)", False, 0, 0);
    int result = tempText.width;

    destroyText(&tempText);

    return result;
}

int getSpecialCharWidth(Text *pText, int pos)
{
    int letterNum = 0;
    int len = strlen(pText->pString);

    if (pos < len - 1)
    {
        letterNum = fromASCII(pText->pString[pos]);

        switch (pText->pString[pos])
        {
            case '$':   return pText->pFont->fontCharWidths[letterNum];
            case '_':   return pText->pFont->wordSpacing;
            default:    return 0;
        }
    }

    return 0;
}

int getCharWidth(Text *pText, int pos)
{
    int letterNum = fromASCII(pText->pString[pos]);

    switch (pText->pString[pos])
    {
        case '\n':
        case '\v':
        case '\t':  return 0;
        case ' ':   return pText->pFont->wordSpacing;
        case '$':   return getSpecialCharWidth(pText, pos + 1);
        default:    return pText->pFont->fontCharWidths[letterNum];
    }
}

int isIndentationAllowed(Text *pText)
{
    return (pText->pFont->indentation && pText->alignment == ALIGN_LEFT);
}

int getPrevTabStop(Text *pText, int lineWidth)
{
    int tab = pText->pFont->indentation;
    return ceil(lineWidth / tab) * tab;
}

int getNextTabStopOffset(Text *pText, int lineWidth)
{
    int tab = pText->pFont->indentation;

    if (isIndentationAllowed(pText))
        return getPrevTabStop(pText, lineWidth) + tab - lineWidth;

    return 0;
}

int strCharsLeft(const char *str, int pos)
{
    return strlen(str) - pos - 1;
}

int isPrintable(const char *str, int pos)
{
    char c = str[pos];
    int len = strlen(str);

    switch (c)
    {
        case ' ':
        case '\t':
        case '\n':
        case '\v': return 0;

        case '$':
            return (strCharsLeft(str, pos) > 0 && str[pos + 1] == '$');

        default: return 1;
    }

}

int getLetterSpacing(Text *pText, int lineWidth, int charWidth, int pos)
{
    if (isPrintable(pText->pString, pos))
        return pText->pFont->letterSpacing * (lineWidth > charWidth);
    else
        return 0;
}

int calculateLineSubRangeWidth(Text *pText, int start, int end)
{
    int i           = 0;
    int lineWidth   = 0;
    int curCharW    = 0;

    for (i = start; i < end; i++)
    {
        lineWidth += curCharW = getCharWidth(pText, i);
        lineWidth += getLetterSpacing(pText, lineWidth, curCharW, i);

        switch (pText->pString[i])
        {
            case '$':   skipEscapeSequences(pText->pString, &i, NULL, NULL); break;
            case '\t':  lineWidth += getNextTabStopOffset(pText, lineWidth); break;
            case '\n':
            case '\v':  return lineWidth;
            default:    break;
        }
    }

    return lineWidth;
}

int calculateLineTailWidth(Text *pText, int start)
{
    return calculateLineSubRangeWidth(pText, start, strlen(pText->pString));
}

int parseToNextLineStart(Text *pText, int startPosition, Color *pTempColor, bool *pCustomColorPrint)
{
    int i;
    int len;
    char *pString = pText->pString;

    len = strlen(pString);

    for (i = startPosition; i < len; i ++)
    {
        if (pString[i] == '$')
            skipEscapeSequences(pText->pString, &i, pTempColor, pCustomColorPrint);

        if (pString[i] == '\n' || pString[i] == '\v')
        {
            if (i < len - 1) return i + 1;
            else return i;
        }

    }

    return -1;
}

void replaceAllInstancesOfChar(char *pString, char replace, char new)
{
    int i;
    int len = strlen(pString);

    for (i = 0; i < len; i ++)
    {
        if (pString[i] == replace)
            pString[i] = new;
    }
}

void fitTextInWidth(Text *pText, int widthLimit)
{
    int i;
    char *tempLine;
    int lineWidth = 0;
    int lineCharCount = 0;
    int maximumLineWidth = 0;
    int maximumLineCharCount = 0;
    int currentLineStart = 0;
    int len;
    char *pString = pText->pString;

    replaceAllInstancesOfChar(pString, '\n', ' ');

    len = strlen(pString);

    for (i = 0; i < len; i ++)
    {
        if (pString[i] == ' ' || pString[i] == '\v' || i == len -  1)
        {
            int charCount = i - currentLineStart + (i == len - 1);

            tempLine = calloc(charCount + 1, sizeof(char));
            strncpy(tempLine, &pString[currentLineStart], charCount);

            lineWidth = calculateLineSubRangeWidth(pText, currentLineStart, currentLineStart + charCount);
            lineCharCount = strlen(tempLine);

            if (maximumLineWidth == 0 && i < len - 1)
            {
                maximumLineWidth = lineWidth;
                maximumLineCharCount = lineCharCount;
            }

            if (lineWidth > maximumLineWidth && lineWidth <= widthLimit)
            {
                maximumLineWidth = lineWidth;
                maximumLineCharCount = lineCharCount;
            }

            if (lineWidth > widthLimit && maximumLineCharCount > 0)
            {
                int tempCharCount = currentLineStart + maximumLineCharCount;

                pString[tempCharCount] = '\n';

                currentLineStart = tempCharCount + 1;
                i = currentLineStart - 1;

                maximumLineWidth = 0;
                maximumLineCharCount = 0;
            }
            else if (pString[i] == '\v')
            {
                currentLineStart = i + 1;

                maximumLineWidth = 0;
                maximumLineCharCount = 0;
            }

            free(tempLine);
        }
    }

    pText->width = calculateTextWidth(pText);
    pText->height = calculateTextHeight(pText);
}

void fitTextInArea(Text *pText, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY)
{
    int boxWidth = bottomRightX - topLeftX;
    int boxHeight = bottomRightY - topLeftY;

    pText->textAreaLeftX = topLeftX;
    pText->textAreaTopY = topLeftY;
    pText->textAreaRightX = bottomRightX;
    pText->textAreaBottomY = bottomRightY;
    pText->fitInArea = True;

    switch (pText->alignment)
    {
        case ALIGN_LEFT: pText->beginX = topLeftX; break;
        case ALIGN_CENTER: pText->beginX = topLeftX + ceil(boxWidth * 0.5); break;
        case ALIGN_RIGHT: pText->beginX = bottomRightX; break;
    }

    pText->beginY = topLeftY + ceil(pText->pFont->lineSpacing * 0.5) + pText->pFont->baselineOffset;
    pText->textAreaScrollPosition = pText->beginY;
    pText->textAreaScrollPercent = 1.0;

    fitTextInWidth(pText, boxWidth);
    pText->textAreaScrollUpLimit = min(pText->beginY, pText->beginY - ceil((pText->height - boxHeight) / (double)pText->pFont->lineSpacing) * pText->pFont->lineSpacing);
    pText->textAreaScrollDownLimit = pText->beginY;
    pText->textAreaHeightPercent = min((double)boxHeight / (double)pText->height, 1.0);
}

void scrollTextByAmount(Text *pText, int scroll)
{
    int tempScrollHeight = pText->textAreaScrollDownLimit - pText->textAreaScrollUpLimit;

    if ((scroll < 0 && pText->textAreaScrollPosition + scroll >= pText->textAreaScrollUpLimit) ||
        (scroll > 0 && pText->textAreaScrollPosition + scroll <= pText->textAreaScrollDownLimit))
    {
        pText->textAreaScrollPosition += scroll;
        pText->textAreaScrollPercent = (double)(pText->textAreaScrollPosition - pText->textAreaScrollUpLimit) / (double)tempScrollHeight;
        refreshText(pText);
    }
    else if (scroll < 0 && pText->textAreaScrollPosition != pText->textAreaScrollUpLimit)
    {
        pText->textAreaScrollPosition = pText->textAreaScrollUpLimit;
        pText->textAreaScrollPercent = (double)(pText->textAreaScrollPosition - pText->textAreaScrollUpLimit) / (double)tempScrollHeight;
        refreshText(pText);
    }
    else if (scroll > 0 && pText->textAreaScrollPosition != pText->textAreaScrollDownLimit)
    {
        pText->textAreaScrollPosition = pText->textAreaScrollDownLimit;
        pText->textAreaScrollPercent = (double)(pText->textAreaScrollPosition - pText->textAreaScrollUpLimit) / (double)tempScrollHeight;
        refreshText(pText);
    }
}

void setTextScrollByPercent(Text *pText, double scrollPercent)
{
    int tempScrollHeight = pText->textAreaScrollDownLimit - pText->textAreaScrollUpLimit;
    double limitedValue = min(1.0, max(0.0, scrollPercent));

    pText->textAreaScrollPosition = pText->textAreaScrollUpLimit + (limitedValue * (double)tempScrollHeight);
    pText->textAreaScrollPercent = limitedValue;

    refreshText(pText);
}

// TODO: convert to function that returns error codes for how the reading went
void readFontDataFile(char *fileName, Font *fontData)
{
    FILE *f = fopen(addFileExtension(fileName, "fdf"), "r+b");

    if (f)
    {
        fread(fontData, sizeof *fontData, 1, f);
        fclose(f);
    }
}

size_t calculateRequiredCapacity(size_t len)
{
    return ((len / CAPACITY_STEP) + 1) * (size_t)CAPACITY_STEP;
}

Text createText(const char *string, Font *pFont, const char *parentCName, bool relative, int startX, int startY)
{
    Text temp;
    char const *strPtr = NULL;
    char strError[100] = "Warning: invalid value for argument 'string' in createText.";

    temp.rendered = False;
    temp.lastRenderFrame = -1;
    temp.beginX = startX;
    temp.beginY = startY + pFont->baselineOffset;
    temp.relative = relative;
    temp.alignment = ALIGN_LEFT;
    temp.zDepth = 0.5;
    strcpy(temp.parentCName, parentCName);
    temp.pFont = pFont;
    temp.color = createRGB(255, 255, 255, 1.0);
    temp.textAreaLeftX = 0;
    temp.textAreaTopY = 0;
    temp.textAreaRightX = 0;
    temp.textAreaBottomY = 0;
    temp.textAreaScrollPosition = 0;
    temp.textAreaScrollUpLimit = 0;
    temp.textAreaScrollDownLimit = 0;
    temp.textAreaHeightPercent = 0.0;
    temp.textAreaScrollPercent = 1.0;
    temp.fitInArea = False;
    temp.letterIndexHead = NULL;
    temp.letterIndexTail = NULL;

    strPtr = string;
    if (strPtr == NULL)
        strPtr = strError;

    temp.capacity = calculateRequiredCapacity(strlen(strPtr));
    temp.pString = calloc(temp.capacity + 1, sizeof(char));

    if (temp.pString)
        strcpy(temp.pString, strPtr);
    else
        temp.capacity = 0;

    temp.width = calculateTextWidth(&temp);
    temp.height = calculateTextHeight(&temp);

    return temp;
}

void updateTextDimensions(Text *pText)
{
    pText->width = calculateTextWidth(pText);
    pText->height = calculateTextHeight(pText);
}

void execOnLCList(Text *pText, LetterCloneindex *list, LetterCloneindex *(*pFunc)(Text *, LetterCloneindex *))
{
    LetterCloneindex *iterator = list, *next;

    while (iterator)
    {
        next = (*pFunc)(pText, iterator);
        iterator = next;
    }
}

LetterCloneindex *newLetterCloneindex(int index)
{
    LetterCloneindex *new = malloc(sizeof *new);

    if (!new) return NULL;

    new->index = index;
    new->next = NULL;

    return new;
}

void storeLetterCloneindex(Text *pText, int index)
{
    LetterCloneindex *new = newLetterCloneindex(index);

    if (!pText->letterIndexHead && !pText->letterIndexTail)
    {
        pText->letterIndexHead = pText->letterIndexTail = new;
    }
    else
    {
        pText->letterIndexTail->next = new;
        pText->letterIndexTail = new;
    }
}

LetterCloneindex *freeLC(Text *pText, LetterCloneindex *this)
{
    LetterCloneindex *next = this->next;
    free(this);
    return next;
}

void freeLetterCloneindexList(Text *pText)
{
    execOnLCList(pText, pText->letterIndexHead, &freeLC);
    pText->letterIndexHead = NULL;
    pText->letterIndexTail = NULL;
}

void setTextAlignment(Text *pText, int alignment)
{
    if (pText->alignment != alignment)
    {
        pText->alignment = alignment;

        if (pText->fitInArea) fitTextInArea(pText, pText->textAreaLeftX, pText->textAreaTopY, pText->textAreaRightX, pText->textAreaBottomY);
    }
}

void setTextColor(Text *pText, Color color)
{
    pText->color = color;
}

LetterCloneindex *setLCZDepth(Text *pText, LetterCloneindex *this)
{
    char actorName[256];
    sprintf(actorName, "%s.%i", typeActorName[pText->lastRenderFrame], this->index);
    ChangeZDepth(actorName, pText->zDepth);
    return this->next;
}

void setTextZDepth(Text *pText, double zDepth)
{
    pText->zDepth = zDepth;
    execOnLCList(pText, pText->letterIndexHead, &setLCZDepth);
}

LetterCloneindex *setLCParent(Text *pText, LetterCloneindex *this)
{
    char actorName[256];
    sprintf(actorName, "%s.%i", typeActorName[pText->lastRenderFrame], this->index);
    ChangeParent(actorName, pText->parentCName);
    return this->next;
}

void setTextParent(Text *pText, char *parentCName, bool keepCurrentPosition)
{
    int parentExists;
    Actor *pParent;
    Actor *pPrevParent;

    pParent = getclone(parentCName);
    pPrevParent = getclone(pText->parentCName);

    if (!(parentExists = actorExists2(pParent)))
        strcpy(pText->parentCName, "(none)");
    else
        strcpy(pText->parentCName, parentCName);

    execOnLCList(pText, pText->letterIndexHead, &setLCParent);
    pText->relative = (parentExists) ? True : False;

    if (keepCurrentPosition)
    {
        int parentX = pParent->x * actorExists2(pParent);
        int parentY = pParent->y * actorExists2(pParent);
        int prevParentX = pPrevParent->x * actorExists2(pPrevParent);
        int prevParentY = pPrevParent->y * actorExists2(pPrevParent);

        pText->beginX = prevParentX + pText->beginX - parentX;
        pText->beginY = prevParentY + pText->beginY - parentY;
    }
}

void setTextPosition(Text *pText, int posX, int posY)
{
    pText->beginX = posX;
    pText->beginY = posY + pText->pFont->baselineOffset;
}

void concatenateText(Text *pText, char *string)
{
    size_t len = strlen(pText->pString) + strlen(string);

    if (len <= pText->capacity)
    {
        strcat(pText->pString, string);
        return;
    }
    else
    {
        size_t reqCapacity = calculateRequiredCapacity(len);
        pText->capacity = reqCapacity;
        pText->pString = realloc(pText->pString, pText->capacity + 1 * sizeof(char));
    }

    if (pText->pString)
        strcat(pText->pString, string);
    else
        pText->capacity = 0;
}

void concatenateCharToText(Text *pText, char c)
{
    char temp[2];

    if (c == '\b')
    {
        pText->pString[strlen(pText->pString) - (strlen(pText->pString) > 0)] = '\0';
        return;
    }
    else if (c == '$')
    {
        concatenateText(pText, "$$");
        return;
    }

    sprintf(temp, "%c", c);
    concatenateText(pText, temp);
}

void setTextText(Text *pText, char *string)
{
    size_t reqCapacity;
    eraseText(pText);

    if (pText->capacity != (reqCapacity = calculateRequiredCapacity(strlen(string))))
    {
        if (pText->pString)
            free(pText->pString);

        pText->capacity = reqCapacity;
        pText->pString = calloc(pText->capacity + 1, sizeof(char));
    }

    if (pText->pString)
        strcpy(pText->pString, string);
    else
        pText->capacity = 0;
}

void refreshText(Text *pText)
{
    renderText(pText);
}

LetterCloneindex *eraseLC(Text *pText, LetterCloneindex *this)
{
    char actorName[256];
    sprintf(actorName, "%s.%i", typeActorName[pText->lastRenderFrame], this->index);
    DestroyActor(actorName);
    this->index = -1;
    return this->next;
}

void eraseText(Text *pText)
{
    if (pText && pText->pString && pText->rendered && pText->lastRenderFrame > -1)
    {
        execOnLCList(pText, pText->letterIndexHead, &eraseLC);
        pText->rendered = False;
        pText->lastRenderFrame = -1;
        freeLetterCloneindexList(pText);
    }
}

void destroyText(Text *pText)
{
    eraseText(pText);

    if (pText->pString)
    {
        free(pText->pString);
        pText->pString = NULL;
    }
}

void handleEscapeSequences(Text *pText, int *iterator, int *pPrevX, int *pPrevY, Color *pTempColor, bool *pCustomColorPrint, bool *firstOnLine)
{
    int len = strlen(pText->pString);

    if (*iterator < len - 1) // If there's still enough characters left for an escape sequence
    {
        switch (pText->pString[*iterator + 1])
        {
            case '$':
                *iterator += SKIP_ESCAPE_SEQUENCE_MARKER;

               if (!*pCustomColorPrint)
                    *pPrevX = renderCharacter(pText, pText->pString[*iterator], pText->color, pText->relative, *pPrevX, *pPrevY, *firstOnLine);
                else
                    *pPrevX = renderCharacter(pText, pText->pString[*iterator], *pTempColor, pText->relative, *pPrevX, *pPrevY, *firstOnLine);

                *firstOnLine = False;
            break;

            case '_':
                (*iterator) ++;
                *pPrevX += pText->pFont->wordSpacing;
            break;

            case 'c': // If there's is still enough characters left for a color sequence
            case 'C':
                if (*iterator < len - COLOR_ESCAPE_SEQUENCE_LENGTH)
                {
                    *iterator += SKIP_COLOR_ESCAPE_SEQUENCE;
                    *pTempColor = extractHexColor(&pText->pString[*iterator]);
                    *pCustomColorPrint = True;
                    *iterator += SKIP_COLOR_VALUES;
                }
                else
                    *iterator += len - *iterator; // Skip to the end of the string
            break;

            case 'x': // Handle ending escape sequences
            case 'X':
                if (*iterator < len - ENDING_ESCAPE_SEQUENCE_LENGTH)
                {
                    switch (pText->pString[*iterator + 2])
                    {
                        case 'c': // Custom color end
                        case 'C':
                            *pCustomColorPrint = False;
                        break;
                    }

                    *iterator += SKIP_ENDING_ESCAPE_SEQUENCE;
                }
                else
                    *iterator += len - *iterator; // Skip to the end of the string
            break;
        }
    }
}

void skipEscapeSequences(char *pString, int *iterator, Color *pTempColor, bool *pCustomColorPrint)
{
    int len = strlen(pString);

    if (*iterator < len - 1) // If there's still enough characters left for an escape sequence
    {
        switch (pString[*iterator + 1])
        {
            case '$':
                *iterator += SKIP_ESCAPE_SEQUENCE_MARKER;
            break;

            case '_':
                (*iterator) ++;
            break;

            case 'c': // If there's is still enough characters left for a color sequence
            case 'C':
                if (*iterator < len - COLOR_ESCAPE_SEQUENCE_LENGTH)
                {
                    *iterator += SKIP_COLOR_ESCAPE_SEQUENCE;
                    if (pTempColor) *pTempColor = extractHexColor(&pString[*iterator]);
                    if (pCustomColorPrint) *pCustomColorPrint = True;
                    *iterator += SKIP_COLOR_VALUES;
                }
                else
                    *iterator += len - *iterator; // Skip to the end of the string
            break;

            case 'x': // Handle ending escape sequences
            case 'X':
                if (*iterator < len - ENDING_ESCAPE_SEQUENCE_LENGTH)
                {
                    switch (pString[*iterator + 2])
                    {
                        case 'c': // Custom color end
                        case 'C':
                            if (pCustomColorPrint) *pCustomColorPrint = False;
                        break;
                    }

                    *iterator += SKIP_ENDING_ESCAPE_SEQUENCE;
                }
                else
                    *iterator += len - *iterator; // Skip to the end of the string
            break;
            default: break;
        }
    }
}

int getLineStartX(Text *pText, int pos)
{

}

void renderText(Text *pText)
{
    int i;
    int len = strlen(pText->pString);
    int currentLine = 0;
    int prevX;
    int prevY;
    Font *pFont = pText->pFont;

    Color tempColor;
    bool printWithCustomColor = False;
    bool firstOnLine = True;

    if (pText->rendered)
        eraseText(pText);

    prevX = getLineStartX(pText, 0);
    switch (pText->alignment)
    {
        case ALIGN_LEFT: prevX = pText->beginX; break;
        case ALIGN_CENTER: prevX = pText->beginX - calculateLineTailWidth(pText, 0) * 0.5; break;
        case ALIGN_RIGHT: prevX = pText->beginX - calculateLineTailWidth(pText, 0); break;
    }

    prevY = (pText->fitInArea) ? pText->textAreaScrollPosition : pText->beginY;

    for (i = 0; i < len; i ++)
    {
        if (pText->fitInArea)
        {
            int skipTo;

            if (prevY < pText->textAreaTopY + pFont->lineSpacing * 0.5)
            {
                skipTo = parseToNextLineStart(pText, i, &tempColor, &printWithCustomColor);

                if (skipTo > -1)
                {
                    i = skipTo - 1;
                    currentLine ++;
                    firstOnLine = True;

                    switch (pText->alignment)
                    {
                        case ALIGN_LEFT: prevX = pText->beginX; break;
                        case ALIGN_CENTER: prevX = pText->beginX - calculateLineTailWidth(pText, skipTo) * 0.5; break;
                        case ALIGN_RIGHT: prevX = pText->beginX - calculateLineTailWidth(pText, skipTo); break;
                    }

                    prevY += pFont->lineSpacing;
                    continue;
                }
            }
            else if (prevY > pText->textAreaBottomY - pFont->lineSpacing * 0.5) break;
        }

        switch (pText->pString[i])
        {
            case ' ': prevX += pFont->wordSpacing; break;

            case '\n':
            case '\v':
                currentLine ++;
                firstOnLine = True;

                switch (pText->alignment)
                {
                    case ALIGN_LEFT: prevX = pText->beginX; break;
                    case ALIGN_CENTER: prevX = pText->beginX - calculateLineTailWidth(pText, i + 1) * 0.5; break;
                    case ALIGN_RIGHT: prevX = pText->beginX - calculateLineTailWidth(pText, i + 1); break;
                }

                prevY += pFont->lineSpacing;
            break;
            case '\t':
                if (pFont->indentation != 0 && pText->alignment == ALIGN_LEFT)
                {
                    int tempIndentation = ceil((prevX - pText->beginX) / pFont->indentation) * pFont->indentation + pFont->indentation + pText->beginX;
                    prevX = tempIndentation;
                }
            break;
            case '$': // Handle custom escape sequences
                handleEscapeSequences(pText, &i, &prevX, &prevY, &tempColor, &printWithCustomColor, &firstOnLine);
            break;

            default:
                if (!printWithCustomColor)
                    prevX = renderCharacter(pText, pText->pString[i], pText->color, pText->relative, prevX, prevY, firstOnLine);
                else
                    prevX = renderCharacter(pText, pText->pString[i], tempColor, pText->relative, prevX, prevY, firstOnLine);

                firstOnLine = False;
            break;
        }
    }

    pText->rendered = True;
    pText->lastRenderFrame = frame % 2;
}

int renderCharacter(Text *pText, char printChar, Color color, bool relative, int xPos, int yPos, bool firstOnLine)
{
    Actor *a, *pParent = getclone(pText->parentCName);
    int letterNum = fromASCII(printChar);
    double prevX = xPos;
    Font *pFont = pText->pFont;

    prevX += floor(pFont->fontCharWidths[letterNum] * 0.5) + pFont->letterSpacing * (!firstOnLine);

    if (!pText->fitInArea || (pText->fitInArea && yPos >= pText->textAreaTopY + pFont->lineSpacing * 0.5 && yPos <= pText->textAreaBottomY - pFont->lineSpacing * 0.5))
    {
        if (pText->relative && actorExists2(pParent))
            a = CreateActor(typeActorName[frame % 2], pFont->fontName, pText->parentCName, "(none)", prevX, yPos, true);
        else
            a = CreateActor(typeActorName[frame % 2], pFont->fontName, "(none)", "(none)", prevX, yPos, true);

        storeLetterCloneindex(pText, a->cloneindex);

        if (actorExists2(pParent))
            ChangeParent(a->clonename, pText->parentCName);

        colorActor(a, color);
        ChangeZDepth(a->clonename, pText->zDepth);

        a->animpos = letterNum;
    }

    prevX += ceil(pFont->fontCharWidths[letterNum] * 0.5);

    return prevX;
}
