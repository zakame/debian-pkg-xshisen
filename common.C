#include "components.h"

void
PopDownCB(Widget w, XtPointer client_data)
{
#if USE_MOTIF
    XtUnmanageChild(w);
#else /* USE_MOTIF */
    Widget shell = (Widget)client_data;
    XtPopdown(shell);
#endif /* USE_MOTIF */
}

void
Message(Pixmap symbol, const char *message, int timeout,
        XtTimerCallbackProc timeout_proc, XtCallbackProc additional_callback)
{
#if USE_MOTIF
    static Widget w;
    static int created = 0;
    XmString str;

    if (!created) {
        w = XmCreateMessageDialog(toplevel, "message", NULL, 0);
        XtAddCallback(w, XmNokCallback, (XtCallbackProc)PopDownCB, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(w, XmDIALOG_HELP_BUTTON));
        created = 1;
    }
    str = XmStringCreateLtoR((char *)message, XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(w,
                  XmNmessageString, str,
                  XmNsymbolPixmap, symbol,
                  NULL);
    XmStringFree(str);
    if (additional_callback)
        XtAddCallback(w, XmNokCallback, additional_callback, NULL);
    if (timeout > 0)
        XtAppAddTimeOut(app_context, timeout, timeout_proc, w);
    XtManageChild(w);
#else /* USE_MOTIF */
    static Widget shell, mf, mfl, pix, label, button;
    static int created = 0;

    if (!created) {
        Position x, y;
        XtVaGetValues(toplevel, XtNx, &x, XtNy, &y, NULL);
        shell = XtVaCreatePopupShell("message", transientShellWidgetClass, toplevel,
                                     XtNx, x+10,
                                     XtNy, y+10,
                                     NULL);
        mf = XtVaCreateManagedWidget("mf", formWidgetClass, shell,
                                     XtNborderWidth, 0,
                                     NULL);
        mfl = XtVaCreateManagedWidget("mfl", formWidgetClass, mf,
                                      XtNborderWidth, 0,
                                      XtNresizable,   True,
                                      XtNtop,         XawChainTop,
                                      XtNbottom,      XawChainBottom,
                                      XtNleft,        XawChainLeft,
                                      XtNright,       XawChainRight,
                                      NULL);
        pix = XtVaCreateManagedWidget("pix", labelWidgetClass, mfl,
                                      XtNborderWidth, 0,
                                      XtNbitmap,      symbol,
                                      XtNresizable,   True,
                                      XtNtop,         XawChainTop,
                                      XtNbottom,      XawChainTop,
                                      XtNleft,        XawChainLeft,
                                      XtNright,       XawChainLeft,
                                      NULL);
        label = XtVaCreateManagedWidget("label", labelWidgetClass, mfl,
                                        XtNborderWidth, 0,
                                        XtNfromHoriz,   pix,
                                        XtNresizable,   True,
                                        XtNtop,         XawChainTop,
                                        XtNbottom,      XawChainBottom,
                                        XtNleft,        XawChainLeft,
                                        XtNright,       XawChainRight,
                                        NULL);
        button = XtVaCreateManagedWidget("button", commandWidgetClass, mf,
                                         XtNfromVert, mfl,
                                         XtNlabel,    "OK",
                                         XtNtop,      XawChainBottom,
                                         XtNbottom,   XawChainBottom,
                                         XtNleft,     XawChainLeft,
                                         NULL);
        XtAddCallback(button, XtNcallback, (XtCallbackProc)PopDownCB, shell);
    }
    XtVaSetValues(pix,
                  XtNbackgroundPixmap, symbol,
                  NULL);
    XtVaSetValues(label,
                  XtNlabel, message,
                  NULL);
    if (additional_callback)
        XtAddCallback(button, XtNcallback, additional_callback, NULL);
    if (timeout > 0)
        XtAppAddTimeOut(app_context, timeout, timeout_proc, shell);
    XtPopup(shell, XtGrabNone);
#endif /* USE_MOTIF */
}
