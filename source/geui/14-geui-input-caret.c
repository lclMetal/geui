void setCaretBlinkRate(BlinkingCaret *caret, float blinkRate);

void initializeCaret(BlinkingCaret *caret)
{
    caret->pText = NULL;
    caret->state = 0;
    strcpy(caret->actorCName, "");
    setCaretBlinkRate(caret, GEUI_DEFAULT_CARET_BLINK_RATE);
}

void setCaretActor(BlinkingCaret *caret, char *actorCName, Text *pText, Color color)
{
    strcpy(caret->actorCName, actorCName);
    caret->pText = pText;
    colorActorByName(actorCName, color);
}

void eraseCaret(BlinkingCaret *caret)
{
    DestroyActor(caret->actorCName);
    strcpy(caret->actorCName, "");
}

void setCaretBlinkRate(BlinkingCaret *caret, float blinkRate)
{
    caret->blinkRate = blinkRate;
    caret->timer = 0.0f;
}

void showCaret(BlinkingCaret *caret)
{
    VisibilityState(caret->actorCName, ENABLE);
}

void hideCaret(BlinkingCaret *caret)
{
    VisibilityState(caret->actorCName, DISABLE);
}

void resetCaretBlink(BlinkingCaret *caret)
{
    caret->timer = 0.0f;
    caret->state = 0;
    transp = caret->state;
}

void doCaretBlink(BlinkingCaret *caret)
{
    caret->timer += caret->blinkRate / (float)max(real_fps, 1);

    if (caret->timer >= 1.0f)
    {
        caret->state = !caret->state;
        caret->timer = 0.0f;
    }

    transp = caret->state;
}

void updateCaretPosition(BlinkingCaret *caret)
{
    Actor *a = getclone(caret->actorCName);
    updateTextDimensions(caret->pText);
    a->x = caret->pText->beginX + caret->pText->width + caret->pText->pFont->letterSpacing + ceil(a->width * 0.5);
    a->y = caret->pText->beginY;
    resetCaretBlink(caret);
}
