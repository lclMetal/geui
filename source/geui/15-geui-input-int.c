#define GEUI_INT_STRING_LENGTH 30

int GEUI_INT_MIN_VALUE = 0;
int GEUI_INT_MAX_VALUE = 0;

IntInputField test;

void updateIntText(IntInputField *field);

void initLimValues()
{
    // algorithm from: https://www.geeksforgeeks.org/computing-int_max-int_min-bitwise-operations/
    unsigned int notZero = ~0; // there's a tilde before the 0, it's just not visible in GE - don't remove
    GEUI_INT_MAX_VALUE = notZero = notZero >> 1;
    GEUI_INT_MIN_VALUE = ~notZero; // there's a tilde before notZero, it's just not visible in GE - don't remove
}

short countDigits(int num)
{
    if (num == 0) return 1;
    return (short)log10(abs(num)) + 1;
}

int calculateRequiredWidthInPixels(IntInputField *field)
{
    int i;
    char temp[GEUI_INT_STRING_LENGTH] = "";
    int minValDigits = countDigits(field->minValue) + (field->minValue < 0);
    int maxValDigits = countDigits(field->maxValue) + (field->maxValue < 0);
    int result = (int)min(GEUI_INT_STRING_LENGTH - 1, max(minValDigits, maxValDigits));

    for (i = 0; i < (int)min(GEUI_INT_STRING_LENGTH - 1, max(minValDigits, maxValDigits)); i++)
    {
        temp[i] = '0';
    }

    temp[i] = '\0';

    return calculateStringWidth(field->text.pFont, temp);
}

void setIntLimits(IntInputField *field, int minLim, int maxLim)
{
    if (!field) return;

    field->minValue = (int)min(minLim, maxLim);
    field->maxValue = (int)max(minLim, maxLim);
    field->intSign = 1;

    if (minLim > maxLim)
    {
        DEBUG_MSG_FROM("Limit min value can't be greater than limit max value! "
                       "Auto-swapped values to fix the issue.", "setIntLimits");
    }

    if (field->minValue <= 0 && field->maxValue >= 0)
    {
        field->value  = 0; // set value to 0
        field->typedZero = 1; // make 0 value shown
    }
    else
    {
        field->value = field->minValue;
        if (field->minValue < 0)
            field->intSign = -1;
    }

    updateIntText(field);
}

void enforceIntLimits(IntInputField *field)
{
    if (field->value > field->maxValue)
        field->value = field->maxValue;

    if (field->value < field->minValue)
        field->value = field->minValue;

    updateIntText(field);
}

int validateIntInput(IntInputField *field, int input)
{
    int val = input;
    int maxval = field->maxValue;
    int minval = field->minValue;
    short valdigits = countDigits(val);
    short maxPositiveDigits;
    short maxNegativeDigits;
    short maxdigits = max(countDigits(maxval), countDigits(minval));

    if (minval >= 0 && maxval >= 0) maxPositiveDigits = maxdigits;
    else if (minval < 0 && maxval < 0) maxNegativeDigits = maxdigits;
    else if (minval < 0 && maxval >= 0)
    {
        maxPositiveDigits = countDigits(maxval);
        maxNegativeDigits = countDigits(minval);
    }
                                                                // e.g.:    val lim
    if (val >= 0 && val > maxval && valdigits >= maxPositiveDigits)     //  15  +13
        val = maxval;
    else if (val < 0 && val > maxval && valdigits >= maxNegativeDigits) //  -2  -5
        val = maxval;
    else if (val >= 0 && val < minval && valdigits >= maxPositiveDigits)//  13  +15
        val = minval;
    else if (val < 0 && val < minval && valdigits >= maxNegativeDigits) //  -5  -2
        val = minval;

    return val;
}

void handleIntInput(IntInputField *field, int key)
{
    char temp[256];
    short numConvert;
    double tempValue = abs(field->value); // remove the sign to simplify calculations

    if (!field) return;

    // basic number input
    if (key >= KEY_0 && key <= KEY_9 || key >= KEY_PAD_0 && key <= KEY_PAD_9)
    {
        if (key >= KEY_0 && key <= KEY_9)
            numConvert = key - KEY_0;
        else
            numConvert = key - KEY_PAD_0;

        if (field->intSign != -1 && tempValue * 10.0 + (double)numConvert <= (double)GEUI_INT_MAX_VALUE ||
            field->intSign == -1 && (tempValue * 10.0 + (double)numConvert) * (double)field->intSign >= (double)GEUI_INT_MIN_VALUE)
        {
            tempValue *= 10; // multiply current value by 10 to move all digits one place to the left
            tempValue += numConvert; // add the number that was typed
            tempValue *= field->intSign; // add back the sign

            // if value is 0, negative sign is not set and number typed was 0, set 0 to be visible
            field->typedZero = tempValue == 0 && field->intSign != -1 && numConvert == 0;
            // validate input (enforceIntLimits is still needed later on, as all limits can't be
            // enforced here) and set the value
            field->value = validateIntInput(field, tempValue);
        }
    }
    else if (key == KEY_BACKSPACE)
    {
        // if at value zero but a negative sign remains and this field can also
        // take positive values
        if (field->value == 0 && field->maxValue >= 0)
        {
            if (field->intSign == -1)
                field->intSign = 1; // remove the negative sign
            else if (field->typedZero)
                field->typedZero = 0; // stop showing 0 value
        }

        tempValue /= 10; // divide by ten to remove the smallest digit
        tempValue *= field->intSign; // add back the sign
        field->value = tempValue; // set the value
    }
    else if (key == KEY_MINUS || key == KEY_PAD_MINUS)
    {
        // if at value zero and no negative sign present, and this field can take
        // values below zero
        if (field->value == 0 && field->intSign == 1 && field->minValue < 0)
        {
            field->intSign = -1; // add the negative sign
            field->typedZero = 0; // hide 0 value
        }
    }

    updateIntText(field);
}

void updateIntText(IntInputField *field)
{
    char temp[GEUI_INT_STRING_LENGTH];

    if (!field) return;

    if (field->value != 0)
        sprintf(temp, "%d", field->value); // print number
    else if (field->value == 0 && field->intSign == -1)
        sprintf(temp, "-"); // print only negative sign
    else if (field->value == 0 && field->typedZero)
        sprintf(temp, "0"); // print 0 only if explicitly typed
    else
        strcpy(temp, "\0"); // print nothing

    setTextText(&field->text, temp);
}

/*
typedef struct inputFieldStruct
{
    short  fieldType;
    short  textMaxLength;
    int    intMinValue;
    int    intMaxValue;
    double realMinValue;
    double realMaxValue;

    short  intSign;
    short  typedZero;

    char  *textValue;
    int    intValue;
    char   intText[30];
    double realValue;
}InputField;
*/
