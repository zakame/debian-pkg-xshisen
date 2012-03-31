#include <locale.h>
#include "body.h"
#include "fallback.h"
#include "kconv.h"
#if USE_EDITRES
#include <X11/Xmu/Editres.h>
#endif

Widget       toplevel;
XtAppContext app_context;
Body         *bd;
Score        *sc;
MenuBar      *mb;
TimerW       *tm;
Dimension    max_win_wid;
Dimension    max_win_hei;
char         *rcfile;

static Window iconW;
static XtWorkProcId workproc_id = 0;
static XtIntervalId timeout_id = 0;
static int icon_state;
static struct timeval idletime;

static XtActionsRec actions[] = {
    { "menu", MenuAC },
    { "PickupPiece", PickupPieceAC },
    { "CancelPiece", CancelPieceAC }
};
static XrmOptionDescRec options[] = {
    { "-fn",     "*fontList",         XrmoptionSepArg, NULL },
    { "-score",  "*scoreOnly",        XrmoptionNoArg,  (XPointer)"True" },
    { "-ctime",  "*connectLineTime",  XrmoptionSepArg,  NULL },
    { "-cwidth", "*connectLineWidth", XrmoptionSepArg,  NULL },
    { "-demo",   "*autoDemo",         XrmoptionNoArg,  (XPointer)"True" },
    { "-mag",    "*magnifyFactor",    XrmoptionSepArg,  NULL },
    { "-regular","*gameSize",         XrmoptionNoArg,  (XPointer)"0" },
    { "-large",  "*gameSize",         XrmoptionNoArg,  (XPointer)"1" },
    { "-huge",   "*gameSize",         XrmoptionNoArg,  (XPointer)"2" },
    { "-trial",  "*trialMode",        XrmoptionNoArg,  (XPointer)"True" },
    { "-gravity","*gravityMode",      XrmoptionNoArg,  (XPointer)"True" },
    { "-KCONV",  "*kanjiConvert",     XrmoptionNoArg,  (XPointer)"True" }
};

static XtResource gres[] = {
    { "aboutString", "AboutString", XtRString, sizeof (char *),
      0, XtRString, (XtPointer)"XShisen " XSHISEN_VERSION " by Masaoki Kobayashi" },
    { "tedumari", "Tedumari", XtRString, sizeof (char *),
      offsetof(GlobRes, tedumari), XtRString, (XtPointer)"You can get no more pieces." },
    { "width", "Width", XtRInt, sizeof (int),
      offsetof(GlobRes, Width), XtRString, (XtPointer)"640" },
    { "height", "Height", XtRInt, sizeof (int),
      offsetof(GlobRes, Height), XtRString, (XtPointer)"400" },
    { "timeFormat", "TimeFormat", XtRString, sizeof (char *),
      offsetof(GlobRes, timeFormat), XtRString, (XtPointer)"Your time is %2.2d:%2.2d." },
    { "displayFormat1", "DisplayFormat1", XtRString, sizeof (char *),
      offsetof(GlobRes, displayFormat1), XtRString, (XtPointer)"Rest: " },
    { "displayFormat2", "DisplayFormat2", XtRString, sizeof (char *),
      offsetof(GlobRes, displayFormat2), XtRString, (XtPointer)"Time: " },
    { "scoreFile", "ScoreFile", XtRString, sizeof (char *),
      offsetof(GlobRes, scoreFile), XtRString, (XtPointer)"%s/xshisen.scores" },
    { "personalScore", "PersonalScore", XtRString, sizeof (char *),
      offsetof(GlobRes, personalScoreFile), XtRString, (XtPointer)".xshisen.scores" },
    { "scoreOnly", "ScoreOnly", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, scoreOnly), XtRString, (XtPointer)"False" },
    { "connectLineColor", "ConnectLineColor", XtRPixel, sizeof (Pixel),
      offsetof(GlobRes, connLineColor), XtRString, (XtPointer)"blue" },
    { "connectLineTime", "ConnectLineTime", XtRInt, sizeof (int),
      offsetof(GlobRes, connLineTime), XtRString, (XtPointer)"1000" },
    { "connectLineWidth", "ConnectLineWidth", XtRInt, sizeof (int),
      offsetof(GlobRes, connLineWidth), XtRString, (XtPointer)"6" },
    { "autoDemo", "AutoDemo", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, autoDemo), XtRString, (XtPointer)"False" },
    { "libraryDirectory", "LibraryDirectory", XtRString, sizeof (char *),
      offsetof(GlobRes, libDir), XtRString, (XtPointer)LIB_DIR },
    { "scoreDirectory", "ScoreDirectory", XtRString, sizeof (char *),
      offsetof(GlobRes, scoreDir), XtRString, (XtPointer)DAT_DIR },
    { "magnifyFactor", "MagnifyFactor", XtRFloat, sizeof (float),
      offsetof(GlobRes, magFactor), XtRString, (XtPointer)"1.0" },
    { "fitPixmap", "FitPixmap", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, fitPixmap), XtRString, (XtPointer)"True" },
    { "colorCloseness", "ColorCloseness", XtRInt, sizeof (int),
      offsetof(GlobRes, colorCloseness), XtRString, (XtPointer)"40000" },
    { "gameSize", "GameSize", XtRInt, sizeof (int),
      offsetof(GlobRes, gameSize), XtRString, (XtPointer)"0" },
    { "trialMode", "TrialMode", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, trialMode), XtRString, (XtPointer)"False" },
    { "gravityMode", "GravityMode", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, gravityMode), XtRString, (XtPointer)"False" },
    { "idleTime", "IdleTime", XtRInt, sizeof (int),
      offsetof(GlobRes, idleTime), XtRString, (XtPointer)"100000" },
    { "kanjiCode", "KanjiCode", XtRString, sizeof (char *),
      offsetof(GlobRes, kanjiCode), XtRString, (XtPointer)KANJICODE },
    { "kanjiConvert", "KanjiConvert", XtRBoolean, sizeof (Boolean),
      offsetof(GlobRes, kanjiConv), XtRString, (XtPointer)"False" },
    { "imageSet", "ImageSet", XtRInt, sizeof (int),
      offsetof(GlobRes, imageSet), XtRString, (XtPointer)"1" }
};
GlobRes globRes;

static void
initrandom(void)
{
    struct timeval  tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
#if HAVE_DRAND48
    srand48(tv.tv_usec);
#elif HAVE_RANDOM
    srandom(tv.tv_usec);
#elif HAVE_RAND
    srand(tv.tv_usec);
#else
    This line will cause error because all the random number
    generating functions are not available!
#endif
}

static Boolean
refreshtimer(XtPointer p)
{
    if (bd->Active()) {
        bd->HourlyPatrol();
        tm->DisplayTimer(bd->GetRest());
    } else {
        tm->DisplayCurrentTime();
    }
    // This is a way not to consume too much CPU time
    // just for such a silly game...
#if HAVE_USLEEP
    usleep(idletime.tv_usec);
#elif HAVE_SELECT
    select(0, NULL, NULL, NULL, &idletime);
#endif

    if (icon_state)
        return True;
    else
        return False;
}

static void
changeiconwindow(void)
{
#if DEBUG
    fprintf(stderr, "Change icon window!\n");
#endif
    XSetWindowBackgroundPixmap(XtDisplay(toplevel), iconW, (Pixmap)Mp[rand()%36]);
    XClearArea(XtDisplay(toplevel), iconW, 0, 0, 0, 0, False);
    timeout_id = XtAppAddTimeOut(app_context, 5000,
                                 (XtTimerCallbackProc)changeiconwindow, NULL);
}

static void
seticonwindow(void)
{
    Pixel  black;
    unsigned int w, h;

    Mp[0].GetSize(w, h);
    black = XBlackPixelOfScreen(XtScreen(toplevel));
    iconW = XCreateSimpleWindow(XtDisplay(toplevel),
                                XRootWindowOfScreen(XtScreen(toplevel)),
                                0, 0, w, h, 1, black, black);
    XtVaSetValues(toplevel,
                  XtNbackground, black,
                  XtNiconWindow, iconW,
                  NULL);
}

void
InitPicture()
{
    char *lib_directory;
    char subDir[16];

    if ((lib_directory = getenv("XSHISENLIB")) == NULL)
        lib_directory = globRes.libDir;
    sprintf(subDir, "s%d", globRes.imageSet);
    InitGlobalMahjong(toplevel, lib_directory, subDir);
}

void
statewatcher(Widget w, caddr_t unused, XEvent *event)
{
    if (event->type == MapNotify) {
        // de-iconified
#if DEBUG
        fprintf(stderr, "Deiconified.\n");
#endif
        workproc_id = XtAppAddWorkProc(app_context, (XtWorkProc)refreshtimer, NULL);
        icon_state = 0;
        if (timeout_id)
            XtRemoveTimeOut(timeout_id);
    }
    else if (event->type == UnmapNotify) {
        // iconified
#if DEBUG
        fprintf(stderr, "Iconified.\n");
#endif
        changeiconwindow();
        icon_state = 1;
    }
}

static void
kanjiconvert(const char *operation)
{
    char buffer[100], *p;
    char *(*codeconv)(const char*);

    strcpy(buffer, operation);
    if (strchr(buffer, '-') == NULL) {
        strcat(buffer, "-" KANJICODE);
    }
    if (strncasecmp(buffer, "jis-euc", 7) == 0)
        codeconv = jis_to_euc;
    else if (strncasecmp(buffer, "jis-sjis", 8) == 0)
        codeconv = jis_to_sjis;
    else if (strncasecmp(buffer, "euc-jis", 7) == 0)
        codeconv = euc_to_jis;
    else if (strncasecmp(buffer, "euc-sjis", 8) == 0)
        codeconv = euc_to_sjis;
    else if (strncasecmp(buffer, "sjis-jis", 8) == 0)
        codeconv = sjis_to_jis;
    else if (strncasecmp(buffer, "sjis-euc", 8) == 0)
        codeconv = sjis_to_euc;
    else {
        // fprintf(stderr, "Don't know kanji conversion \"%s\"\n", buffer);
        // exit(1);
        codeconv = NULL;
    }
    while(fgets(buffer, 100, stdin) != NULL) {
        // If the string "XSHISEN_VERSION" is found, it should be replaced
        // with the XSHISEN_VERSION value.
        if ((p = strstr(buffer, "XSHISEN_VERSION")) != NULL) {
            int len1 = strlen(XSHISEN_VERSION);
            char *p2 = p + strlen("XSHISEN_VERSION");
            strncpy(p, XSHISEN_VERSION, len1);
            p += len1;
            memmove(p, p2, 100 - (p2 - buffer));
        }
        if (codeconv == NULL) {
            fputs(buffer, stdout);
        } else {
            fputs(codeconv(buffer), stdout);
        }
    }
}

static void
usage(const char *myname)
{
    printf("%s " XSHISEN_VERSION " by Masaoki Kobayashi\n\n", CLASS_NAME);
    printf("Usage: %s [options]\n", myname);
    printf("  -fn font   Sets fonts for menubar and time counter.\n");
    printf("  -score     Popup high-score dialog and exit.\n");
    printf("  -ctime N   Set the time that connection line remains (msec).\n");
    printf("  -cwidth N  Set the connection line width (pixel).\n");
    printf("  -demo      Auto demo mode for understanding rules.\n");
    printf("  -mag F     Magnify window F times from original.\n");
    printf("  -regular   Play regular game (default).\n");
    printf("  -large     Play 2x game.\n");
    printf("  -huge      Play 4x game.\n");
    printf("  -trial     Play as \"Click Trial\".\n");
    printf("  -gravity   Play with gravity.\n");
    printf("\nAll other regular toolkit options are accepted.\n");
    exit(2);
}

int
main(int argc, char **argv)
{
    char         *dat_directory;
    char         *scorefile;
    char         *home;
    Display      *disp;
    int          initial_game_state;
    int          num_piece_x, num_piece_y;
    Widget       topform, base;
    XrmDatabase  rdb1, rdb2;

    setlocale(LC_ALL, "");
    XtSetLanguageProc(NULL, NULL, NULL);

    initrandom();
    toplevel = XtVaAppInitialize(&app_context, CLASS_NAME,
                                 options, XtNumber(options),
                                 &argc, argv,
                                 fallback_resources,
                                 XtNminWidth,         MIN_WIN_WID,
                                 XtNminHeight,        MIN_WIN_HEI,
                                 XtNallowShellResize, True,
                                 NULL);
    disp = XtDisplay(toplevel);

    // Override resources with $HOME/.xshisenrc file which is
    // created by Body::WriteRcFile()
    rdb1 = XrmGetDatabase(disp);
    home = getenv("HOME");
    rcfile = new char [strlen(home) + 12];
    sprintf(rcfile, "%s/.xshisenrc", home);
    rdb2 = XrmGetFileDatabase(rcfile);
    XrmMergeDatabases(rdb2, &rdb1);
    XrmSetDatabase(disp, rdb1);
    XtVaGetApplicationResources(toplevel, (XtPointer)&globRes,
                                gres, XtNumber(gres), NULL);

    if (globRes.kanjiConv) {
        kanjiconvert(argv[1]);
        exit(0);
    }
    if (argc != 1)
        usage(argv[0]);

    XtAddEventHandler(toplevel, StructureNotifyMask, False,
                      (XtEventHandler)statewatcher,
                      (Opaque)NULL);
#if USE_EDITRES
    XtAddEventHandler(toplevel, (EventMask)0, True,
                      (XtEventHandler)_XEditResCheckMessages,
                      (Opaque)NULL);
#endif
    XtAppAddActions(app_context, actions, XtNumber(actions));

    globRes.Width  = (int)(globRes.Width  * globRes.magFactor);
    globRes.Height = (int)(globRes.Height * globRes.magFactor);

    initial_game_state = globRes.gameSize;
    if (globRes.trialMode)
        initial_game_state += NUM_GAME;
    else if (globRes.gravityMode)
        initial_game_state += 2*NUM_GAME;
    idletime.tv_sec  = 0;
    idletime.tv_usec = globRes.idleTime;
    max_win_wid = DisplayWidth (disp, XDefaultScreen(disp));
    max_win_hei = DisplayHeight(disp, XDefaultScreen(disp));
    XtVaSetValues(toplevel,
                  XtNmaxWidth,         max_win_wid,
                  XtNmaxHeight,        max_win_hei,
                  NULL);

    if ((dat_directory = getenv("XSHISENDAT")) == NULL)
        dat_directory = globRes.scoreDir;

    sc = new Score(toplevel);
    scorefile = new char[strlen(dat_directory)+strlen(globRes.scoreFile)+1];
    sprintf(scorefile, globRes.scoreFile, dat_directory);
    sc->SetScoreFile(scorefile, globRes.kanjiCode, globRes.personalScoreFile);
    delete[] scorefile;

    if (globRes.scoreOnly) {
        sc->DisplayScore(initial_game_state);
#if USE_MOTIF
        XtAddCallback(*sc, XmNokCallback, (XtCallbackProc)ExitCB, NULL);
#else
        XtAddCallback(XtNameToWidget(*sc, "*ok_button"),
                      XtNcallback, (XtCallbackProc)ExitCB, NULL);
#endif
    }
    else {
        GetGameSize(initial_game_state, num_piece_x, num_piece_y);
#if USE_MOTIF
        topform = XtVaCreateManagedWidget("form", xmFormWidgetClass, toplevel,
                                          NULL);
        mb = new MenuBar(topform, initial_game_state % NUM_GAME,
                         initial_game_state / NUM_GAME, globRes.imageSet - 1);
        base = topform;
#else
        topform = XtVaCreateManagedWidget("form", formWidgetClass, toplevel,
                                          XtNdefaultDistance, 0,
                                          NULL);
        mb = new MenuBar(topform, initial_game_state % NUM_GAME,
                         initial_game_state / NUM_GAME, globRes.imageSet - 1);
        base = XtVaCreateManagedWidget("sform", formWidgetClass, topform,
                                       XtNfromVert,        (Widget)*mb,
                                       XtNleft,            XawChainLeft,
                                       XtNright,           XawChainRight,
                                       XtNtop,             XawChainTop,
                                       XtNbottom,          XawChainBottom,
                                       XtNresizable,       True,
                                       XtNborderWidth,     0,
                                       XtNdefaultDistance, 0,
                                       NULL);
#endif
        tm = new TimerW(base, *mb,
                        globRes.displayFormat1, "000",
                        globRes.displayFormat2, "00:00:00");
        bd = new Body(initial_game_state, num_piece_x, num_piece_y, base, *tm);
        workproc_id = XtAppAddWorkProc(app_context, (XtWorkProc)refreshtimer, NULL);
        XtRealizeWidget(toplevel);
        // InitGlobalMahjong must be called after realized because it
        // requires the real Window.
        InitPicture();
        seticonwindow();
        bd->SetGC();
        GetBoardSizeFromGameSize(num_piece_x, num_piece_y,
                                 globRes.Width, globRes.Height);
        bd->SetGeometry(globRes.Width, globRes.Height);
        SetGameStart();
    }

    XtAppMainLoop(app_context);
}
