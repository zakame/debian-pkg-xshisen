#include "components.h"

MenuBar::MenuBar(Widget parent, int radio_selection, int other_mode, int pixmap_selection)
{
    Boolean trial_mode, gravity_mode;

    switch(other_mode) {
    case 0:
        trial_mode   = False;
        gravity_mode = False;
        break;
    case 1:
        trial_mode   = True;
        gravity_mode = False;
        break;
    case 2:
        trial_mode   = False;
        gravity_mode = True;
        break;
    default:
        fprintf(stderr, "Error: unknown game mode (%d)\n", other_mode);
        exit(1);
    }
#if USE_MOTIF
    XmString str[20];
    char button_name[9];

    menubar = XmVaCreateSimpleMenuBar(parent, "mb",
                                      XmVaCASCADEBUTTON, NULL, '\0',
                                      XmVaCASCADEBUTTON, NULL, '\0',
                                      XmVaCASCADEBUTTON, NULL, '\0',
                                      XmVaCASCADEBUTTON, NULL, '\0',
                                      NULL);
    XtVaSetValues(menubar,
                  XmNtopAttachment,   XmATTACH_FORM,
                  XmNrightAttachment, XmATTACH_FORM,
                  XmNleftAttachment,  XmATTACH_FORM,
                  NULL);
    title[0] = XmVaCreateSimplePulldownMenu(menubar,
                                            "pd0", 0, (XtCallbackProc)GameCB,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaSEPARATOR,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaSEPARATOR,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            XmVaSEPARATOR,
                                            XmVaPUSHBUTTON,  NULL, '\0', NULL, NULL,
                                            NULL);
    title[1] = XmVaCreateSimplePulldownMenu(menubar,
                                            "pd1", 1, (XtCallbackProc)SuppCB,
                                            XmVaPUSHBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaPUSHBUTTON, NULL, '\0', NULL, NULL,
                                            NULL);
    title[2] = XmVaCreateSimplePulldownMenu(menubar,
                                            "pd2", 2, (XtCallbackProc)ChangeGameCB,
                                            XmVaRADIOBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaRADIOBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaRADIOBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaSEPARATOR,
                                            XmVaCHECKBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaCHECKBUTTON, NULL, '\0', NULL, NULL,
                                            XmNradioBehavior,  False,
                                            XmNradioAlwaysOne, False,
                                            NULL);
    title[3] = XmVaCreateSimplePulldownMenu(menubar,
                                            "pd3", 3, (XtCallbackProc)ChangePixmapCB,
                                            XmVaRADIOBUTTON, NULL, '\0', NULL, NULL,
                                            XmVaRADIOBUTTON, NULL, '\0', NULL, NULL,
                                            XmNradioBehavior,  False,
                                            XmNradioAlwaysOne, False,
                                            NULL);
    XtManageChild(menubar);
#else /* USE_MOTIF */
    char button_name[9];
    Widget menushell0, menushell1, menushell2, menushell3;
    static char check_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x20, 0x00, 0x20,
        0x00, 0x10, 0x00, 0x18, 0x02, 0x0c, 0x04, 0x06, 0x08, 0x07, 0x98, 0x03,
        0xf0, 0x01, 0xe0, 0x00, 0x40, 0x00, 0x00, 0x00};

    menubar = XtVaCreateManagedWidget("mb", boxWidgetClass, parent,
                                      XtNorientation, XtEhorizontal,
                                      XtNtop,         XawChainTop,
                                      XtNbottom,      XawChainTop,
                                      XtNleft,        XawChainLeft,
                                      XtNright,       XawChainLeft,
                                      XtNborderWidth, 0,
                                      NULL);
    title[0] = XtVaCreateManagedWidget("pd0", menuButtonWidgetClass, menubar,
                                       NULL);
    title[1] = XtVaCreateManagedWidget("pd1", menuButtonWidgetClass, menubar,
                                       NULL);
    title[2] = XtVaCreateManagedWidget("pd2", menuButtonWidgetClass, menubar,
                                       NULL);
    title[3] = XtVaCreateManagedWidget("pd3", menuButtonWidgetClass, menubar,
                                       NULL);
    menushell0 = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, title[0],
                                      NULL);
    menushell1 = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, title[1],
                                      NULL);
    menushell2 = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, title[2],
                                      NULL);
    menushell3 = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, title[3],
                                      NULL);
    XtVaCreateManagedWidget("button_0", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_1", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_2", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("sep_0", smeLineObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_3", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_4", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("sep_1", smeLineObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_5", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("sep_2", smeLineObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_6", smeBSBObjectClass, menushell0,
                            NULL);
    XtVaCreateManagedWidget("button_0", smeBSBObjectClass, menushell1,
                            NULL);
    XtVaCreateManagedWidget("button_1", smeBSBObjectClass, menushell1,
                            NULL);
    XtVaCreateManagedWidget("button_0", smeBSBObjectClass, menushell2,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("button_1", smeBSBObjectClass, menushell2,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("button_2", smeBSBObjectClass, menushell2,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("sep_0", smeLineObjectClass, menushell2,
                            NULL);
    XtVaCreateManagedWidget("button_3", smeBSBObjectClass, menushell2,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("button_4", smeBSBObjectClass, menushell2,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("button_0", smeBSBObjectClass, menushell3,
                            XtNleftMargin, 16,
                            NULL);
    XtVaCreateManagedWidget("button_1", smeBSBObjectClass, menushell3,
                            XtNleftMargin, 16,
                            NULL);
    XtAddCallback(XtNameToWidget(menushell0, "button_0"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)0);
    XtAddCallback(XtNameToWidget(menushell0, "button_1"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)1);
    XtAddCallback(XtNameToWidget(menushell0, "button_2"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)2);
    XtAddCallback(XtNameToWidget(menushell0, "button_3"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)3);
    XtAddCallback(XtNameToWidget(menushell0, "button_4"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)4);
    XtAddCallback(XtNameToWidget(menushell0, "button_5"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)5);
    XtAddCallback(XtNameToWidget(menushell0, "button_6"), XtNcallback,
                  (XtCallbackProc)GameCB, (XtPointer)6);
    XtAddCallback(XtNameToWidget(menushell1, "button_0"), XtNcallback,
                  (XtCallbackProc)SuppCB, (XtPointer)0);
    XtAddCallback(XtNameToWidget(menushell1, "button_1"), XtNcallback,
                  (XtCallbackProc)SuppCB, (XtPointer)1);
    XtAddCallback(XtNameToWidget(menushell2, "button_0"), XtNcallback,
                  (XtCallbackProc)ChangeGameCB, (XtPointer)0);
    XtAddCallback(XtNameToWidget(menushell2, "button_1"), XtNcallback,
                  (XtCallbackProc)ChangeGameCB, (XtPointer)1);
    XtAddCallback(XtNameToWidget(menushell2, "button_2"), XtNcallback,
                  (XtCallbackProc)ChangeGameCB, (XtPointer)2);
    XtAddCallback(XtNameToWidget(menushell2, "button_3"), XtNcallback,
                  (XtCallbackProc)ChangeGameCB, (XtPointer)3);
    XtAddCallback(XtNameToWidget(menushell2, "button_4"), XtNcallback,
                  (XtCallbackProc)ChangeGameCB, (XtPointer)4);
    XtAddCallback(XtNameToWidget(menushell3, "button_0"), XtNcallback,
                  (XtCallbackProc)ChangePixmapCB, (XtPointer)0);
    XtAddCallback(XtNameToWidget(menushell3, "button_1"), XtNcallback,
                  (XtCallbackProc)ChangePixmapCB, (XtPointer)1);
    check = XCreateBitmapFromData(XtDisplay(menubar),
                                  XRootWindowOfScreen(XtScreen(menubar)),
                                  check_bits, 16, 16);
#endif /* USE_MOTIF */

    // Set one item on the radio button
    CheckGame(radio_selection);
    CheckPixmap(pixmap_selection);
    // "Restart" and "Pause" is initially disabled.
    DisableRestart();
    // Set the "Click Trial" check button
    ClickTrial(trial_mode, gravity_mode);
}

void
MenuBar::DisableRestart(void)
{
    XtVaSetValues(XtNameToWidget(title[0], "*button_1"),
                  XtNsensitive, False,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[0], "*button_2"),
                  XtNsensitive, False,
                  NULL);
    restart_disabled = 1;
}

void
MenuBar::Sensitive(void)
{
    XtVaSetValues(XtNameToWidget(title[1], "*button_0"),
                  XtNsensitive, True,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[1], "*button_1"),
                  XtNsensitive, True,
                  NULL);
    if (restart_disabled) {
        XtVaSetValues(XtNameToWidget(title[0], "*button_1"),
                      XtNsensitive, True,
                      NULL);
        XtVaSetValues(XtNameToWidget(title[0], "*button_2"),
                      XtNsensitive, True,
                      NULL);
        restart_disabled = 0;
    }
}

void
MenuBar::Insensitive(void)
{
    XtVaSetValues(XtNameToWidget(title[1], "*button_0"),
                  XtNsensitive, False,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[1], "*button_1"),
                  XtNsensitive, False,
                  NULL);
}

void
MenuBar::PauseOn(void)
{
    XtVaSetValues(XtNameToWidget(title[0], "*button_0"),
                  XtNsensitive, False,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[0], "*button_1"),
                  XtNsensitive, False,
                  NULL);
    Insensitive();
}

void
MenuBar::PauseOff(void)
{
    XtVaSetValues(XtNameToWidget(title[0], "*button_0"),
                  XtNsensitive, True,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[0], "*button_1"),
                  XtNsensitive, True,
                  NULL);
    Sensitive();
}

void
MenuBar::ClickTrial(Boolean click, Boolean gravity)
{
#if USE_MOTIF
    XtVaSetValues(XtNameToWidget(title[2], "*button_3"),
                  XmNset, click,
                  NULL);
    XtVaSetValues(XtNameToWidget(title[2], "*button_4"),
                  XmNset, gravity,
                  NULL);
#else /* USE_MOTIF */
    XtVaSetValues(XtNameToWidget(title[2], "*button_3"),
                  XtNleftBitmap, (click ? check : 0),
                  NULL);
    XtVaSetValues(XtNameToWidget(title[2], "*button_4"),
                  XtNleftBitmap, (gravity ? check : 0),
                  NULL);
#endif /* USE_MOTIF */
}

void
MenuBar::DemoMode(void)
{
    Arg arg;

    // make only "exit" button remain active
    XtSetArg(arg, XtNsensitive, False);
    XtSetValues(XtNameToWidget(title[0], "*button_0"), &arg, 1);
    XtSetValues(XtNameToWidget(title[0], "*button_1"), &arg, 1);
    XtSetValues(XtNameToWidget(title[0], "*button_2"), &arg, 1);
    XtSetValues(XtNameToWidget(title[1], "*button_0"), &arg, 1);
    XtSetValues(XtNameToWidget(title[1], "*button_1"), &arg, 1);
}

void
MenuBar::CheckGame(int which)
{
    Arg arg_off, arg_on;
#if USE_MOTIF
    XtSetArg(arg_off, XmNset, False);
    XtSetArg(arg_on,  XmNset, True);
#else /* USE_MOTIF */
    /* Athena widget sets contains the 'Toggle' widget, but it
       is unusable from the simple menubar widget.  I have to
       get it toggled by myself. */
    XtSetArg(arg_off, XtNleftBitmap, NULL);
    XtSetArg(arg_on,  XtNleftBitmap, check);
#endif
    switch(which) {
    case 0:
        XtSetValues(XtNameToWidget(title[2], "*button_0"), &arg_on,  1);
        XtSetValues(XtNameToWidget(title[2], "*button_1"), &arg_off, 1);
        XtSetValues(XtNameToWidget(title[2], "*button_2"), &arg_off, 1);
        break;
    case 1:
        XtSetValues(XtNameToWidget(title[2], "*button_0"), &arg_off, 1);
        XtSetValues(XtNameToWidget(title[2], "*button_1"), &arg_on,  1);
        XtSetValues(XtNameToWidget(title[2], "*button_2"), &arg_off, 1);
        break;
    case 2:
        XtSetValues(XtNameToWidget(title[2], "*button_0"), &arg_off, 1);
        XtSetValues(XtNameToWidget(title[2], "*button_1"), &arg_off, 1);
        XtSetValues(XtNameToWidget(title[2], "*button_2"), &arg_on,  1);
        break;
    }
}

void
MenuBar::CheckPixmap(int which)
{
    Arg arg_off, arg_on;
#if USE_MOTIF
    XtSetArg(arg_off, XmNset, False);
    XtSetArg(arg_on,  XmNset, True);
#else /* USE_MOTIF */
    /* Athena widget sets contains the 'Toggle' widget, but it
       is unusable from the simple menubar widget.  I have to
       get it toggled by myself. */
    XtSetArg(arg_off, XtNleftBitmap, NULL);
    XtSetArg(arg_on,  XtNleftBitmap, check);
#endif
    switch(which) {
    case 0:
        XtSetValues(XtNameToWidget(title[3], "*button_0"), &arg_on,  1);
        XtSetValues(XtNameToWidget(title[3], "*button_1"), &arg_off, 1);
        break;
    case 1:
        XtSetValues(XtNameToWidget(title[3], "*button_0"), &arg_off, 1);
        XtSetValues(XtNameToWidget(title[3], "*button_1"), &arg_on,  1);
        break;
    }
}
