#include "body.h"

void
PopDownTO(XtPointer data, XtIntervalId *id)
{
    Widget w = (Widget)data;
    XtUnmanageChild(w);
    bd->Shuffle();
    tm->ResetTimer();
    bd->DrawAll();
}

void
ScoreRegisterCB(Widget w, int index)
{
#if DEBUG
    fprintf(stderr, "ScoreRegisterCB\n");
#endif
    sc->Register();
#if USE_MOTIF
    XtRemoveCallback(w, XmNokCallback, (XtCallbackProc)ScoreRegisterCB, NULL);
#else /* USE_MOTIF */
    XtRemoveCallback(w, XtNcallback, (XtCallbackProc)ScoreRegisterCB, NULL);
#endif /* USE_MOTIF */
}

void
ResizeCB(void)
{
    int wid, hei;

    if (bd->GetGeometry(wid, hei))
        bd->SetSize(wid, hei);
#if DEBUG
    fprintf(stderr, "ResizeCB: (%d,%d)\n", wid, hei);
#endif
}

void
ExposeCB(Widget w, XtPointer tag, XtPointer p)
{
#if USE_MOTIF
    XExposeEvent *e = &(((XmDrawingAreaCallbackStruct *)p)->event->xexpose);
#else /* USE_MOTIF */
    XExposeEvent *e = &(((XEvent *)p)->xexpose);
#endif /* USE_MOTIF */
#if DEBUG
    fprintf(stderr, "ExposeCB: (%d,%d) -> (%d,%d)\n", e->x, e->y, e->width, e->height);
#endif
    bd->DrawRegion(e->x, e->y, e->width, e->height);
}

void
GameCB(Widget w, int index)
{
    static long pausedTime;
    int pauseStatus;
#if DEBUG
    fprintf(stderr, "GameCB: %d\n", index);
#endif
    switch(index) {
    case 0: /* Start */
        bd->LogGiveUp();
        bd->Shuffle();
        bd->DrawAll();
        mb->Sensitive();
        tm->ResetTimer();
        break;
    case 1: /* Restart */
        bd->LogGiveUp();
        bd->Restart();
        bd->DrawAll();
        mb->Sensitive();
        tm->ResetTimer();
        break;
    case 2: /* Pause */
        pauseStatus = bd->TogglePause();
        if (pauseStatus == 1) { // Paused
            mb->PauseOn();
            pausedTime = tm->GetTimer();
        } else if (pauseStatus == 0) {  // Pause cancelled
            mb->PauseOff();
            tm->SetTimer(pausedTime);
        }
        break;
    case 3: /* Score */
        sc->DisplayScore(bd->game);
        break;
    case 4: /* Personal Score */
        sc->PersonalStat(bd->game);
        break;
    case 5: /* About */
        Message(MESSAGE_SYMBOL, globRes.aboutString);
        break;
    case 6: /* Exit */
        exit(0);
    }
}

void
SuppCB(Widget w, int index)
{
#if DEBUG
    fprintf(stderr, "SuppCB: %d\n", index);
#endif
    switch(index) {
    case 0: /* Hint */
        bd->Hint();
        break;
    case 1: /* Back */
        bd->Back();
        break;
    }
}

void
ChangeGameCB(Widget w, int index, XtPointer p)
{
    int xsize, ysize;
    int trial;

#if DEBUG
    fprintf(stderr, "ChangeGameCB: %d\n", index);
#endif
    if (index >= NUM_GAME) {
        if (index == NUM_GAME) {
            bd->ToggleClickTrial();
        } else {
            bd->ToggleGravity();
        }
        SetGameStart();
        bd->DrawAll();
    } else {
        mb->CheckGame(index);
        trial = bd->game / NUM_GAME;
        if (index == bd->game % NUM_GAME)
            return;
        index += trial * NUM_GAME;
        GetGameSize(index, xsize, ysize);
        bd->ChangeGame(index, xsize, ysize);
        GetBoardSizeFromGameSize(xsize, ysize, globRes.Width, globRes.Height);
        bd->SetGeometry(globRes.Width, globRes.Height);
        SetGameStart();
    }
    bd->WriteRcFile();
}

void
ChangePixmapCB(Widget w, int index, XtPointer p)
{
    char subdir[16];
extern Widget toplevel;

#if DEBUG
    fprintf(stderr, "ChangePixmapCB: %d\n", index);
#endif
    mb->CheckPixmap(index);
    if (globRes.imageSet != index + 1) {
        int xsize, ysize;
	globRes.imageSet = index + 1;
        InitPicture();
        GetGameSize(bd->game%NUM_GAME, xsize, ysize);
        GetBoardSizeFromGameSize(xsize, ysize, globRes.Width, globRes.Height);
        bd->SetGeometry(globRes.Width, globRes.Height);
        bd->WriteRcFile();
        SetGameStart();
    }
}

void
AutoDemoCB(void)
{
    bd->Robot();
    XtAppAddTimeOut(app_context, 2000,
                    (XtTimerCallbackProc)AutoDemoCB, NULL);
}

void
ExitCB(void)
{
    exit(0);
}
