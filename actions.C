#include "body.h"

void
PickupPieceAC(Widget w, XEvent *event, String *params, Cardinal *num)
{
    XButtonEvent *e = &(event->xbutton);
    if (e->type != ButtonPress) return;
    bd->GetPiece(e->x, e->y);
#if DEBUG
    fprintf(stderr, "PickupPieceAC: (%d,%d) %d\n", e->x, e->y, e->button);
#endif
}

void
CancelPieceAC(Widget w, XEvent *event, String *params, Cardinal *num)
{
    XButtonEvent *e = &(event->xbutton);
    if (e->type != ButtonPress) return;
    bd->CancelPiece(e->x, e->y);
#if DEBUG
    fprintf(stderr, "CancelPieceAC: (%d,%d) %d\n", e->x, e->y, e->button);
#endif
}

void
MenuAC(Widget w, XEvent *event, String *params, Cardinal *num)
{
#if DEBUG
    fprintf(stderr, "MenuAC: (%d,%d)\n",atoi(params[0]),atoi(params[1]));
#endif
    if (*num < 2)
        GameCB(w, 4);
    switch(params[0][0]) {
    case '0':
        GameCB(w, atoi(params[1]));
        break;
    case '1':
        SuppCB(w, atoi(params[1]));
        break;
    case '2':
        ChangeGameCB(w, atoi(params[1]), NULL);
        break;
    }
}
