#include "body.h"

void
GetGameSize(int state, int &xsize, int &ysize)
{
    switch(state%NUM_GAME) {
    case 1:
        xsize = 24;
        ysize = 12;
        break;
    case 2:
        xsize = 36;
        ysize = 16;
        break;
    default: // expected 0
        xsize = 18;
        ysize =  8;
        break;
    }
}

void
GetBoardSizeFromGameSize(int xsize, int ysize, int &width, int &height)
{
    if (globRes.fitPixmap) {
        // When fitPixmap is True, Width and Height resources are ignored,
        // but magFactor is still valid.
        unsigned int w, h;
        Mp[0].GetSize(w, h);
        width  = (w + 2*XMARGIN) * (xsize+2);
        height = (h + 2*YMARGIN) * (ysize+2);
        width  = (int) (width  * globRes.magFactor);
        height = (int) (height * globRes.magFactor);
    }

    if (width < MIN_WIN_WID)
        width = MIN_WIN_WID;
    if (width > max_win_wid)
        width = max_win_wid;
    if (height< MIN_WIN_HEI)
        height= MIN_WIN_HEI;
    if (height> max_win_hei)
        height= max_win_hei;
}

void
SetGameStart(void)
{
    bd->GameOver();
    mb->DisableRestart();
    bd->Sort();
    if (globRes.autoDemo) {
        XtAppAddTimeOut(app_context, 2000,
                        (XtTimerCallbackProc)AutoDemoCB, NULL);
        mb->DemoMode();
        bd->SetDemo(1);
        bd->Shuffle();
    } else {
        bd->SetDemo(0);
    }
}
