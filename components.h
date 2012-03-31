#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#if HAVE_LIBXM
# include <Xm/RowColumn.h>
# include <Xm/CascadeBG.h>
# include <Xm/PushBG.h>
# include <Xm/ToggleBG.h>
# include <Xm/Form.h>
# include <Xm/DrawingA.h>
# include <Xm/Label.h>
# include <Xm/LabelG.h>
# include <Xm/MessageB.h>
# define USE_MOTIF 1
# undef  USE_ATHENA
# if HAVE_LIBXMU
#  define USE_EDITRES
# endif
#else /* HAVE_LIBXM */
# include <X11/Xlib.h>
# include <X11/Intrinsic.h>
# include <X11/StringDefs.h>
# include <X11/Xaw/XawInit.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/MenuButton.h>
# include <X11/Xaw/SimpleMenu.h>
# include <X11/Xaw/SmeBSB.h>
# include <X11/Xaw/SmeLine.h>
# include <X11/Xaw/Toggle.h>
# define USE_ATHENA 1
# undef  USE_MOTIF
#endif /* HAVE_LIBXM */
#include <X11/xpm.h>
#include <X11/cursorfont.h>
#if STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#elif HAVE_SYS_TIME_H
# include <sys/time.h>
#else
# include <time.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_LIMITS_H
# include <limits.h>
#endif
#include <stdio.h>
#ifndef HAVE_GETTIMEOFDAY
  Cannot compile this program on the system which does not have
  gettimeofday() function
#endif

#define PKIND 36             // Total kind of pieces
#define HNUM  (PKIND*2)      // Number of pairs to pick up
#define SCORENUM  10         // Number of people to register in high-score
#define NAMELEN   28         // Length of name in high-score

class Timer {
private:
  struct timeval initialtime;
  long timeoffset;
protected:
  long currenttime;
public:
  void ResetTimer(void);
  long GetTimer(void);  // in mili-second
  void SetTimer(long);  // in mili-second
};

class TimerW : public Timer {
private:
    Widget label[4];
#if USE_MOTIF
    XmString str1;
#else
    String str1;
#endif
    int    countNow;
    int    dateMode;
public:
    TimerW(Widget, Widget, char*, char*, char*, char*);
    void DisplayTimer(int);
    void DisplayCurrentTime(void);
    operator Widget(){ return label[0]; }
};

class Mahjong {
private:
    Pixmap data;
    Pixmap datag;
    Pixmap rdata;
    Pixmap rdatag;
    unsigned int original_width;
    unsigned int original_height;
    unsigned int width;
    unsigned int height;
    int resized;
public:
    short  id;
    Mahjong(void);
    void ReadFile(Widget, char*, int, int);
    void Resize(Widget, GC, unsigned int, unsigned int);
    void GetSize(unsigned int&, unsigned int&);
    void Draw(Widget, GC, int, int, int);
    operator Pixmap() { return data; }
};

class Point {
public:
    short x;
    short y;
    Point(const Point &p) { x=p.x; y=p.y; }
    Point(short p1=0, short p2=0) { x=p1; y=p2; }
    friend int operator == (const Point &a, const Point &b) {
        return a.x==b.x && a.y==b.y;
    }
};

class PiePos {
private:
    int   count;
    Point *p;
public:
    PiePos(int num){ count = 0; p = new Point[num]; }
    ~PiePos(void){ delete[] p; }
    void Clear(void){ count = 0; }
    void Add(const Point &q){
        p[count++] = q;
#if DEBUG > 1
        fprintf(stderr, "P:add (%d,%d) [%d]\n", q.x, q.y, count);
#endif
    }
    void Delete(const Point&, const Point&);
    void Move(const Point&, const Point&);
    void GetPosArray(Point poi[], int &num){
        for(int i=0; i<count; i++)
            poi[i] = p[i];
        num = count;
    }
};

class History {
private:
    int count;
    Point *piece1;
    Point *piece2;
    Point *via1;
    Point *via2;
public:
    History(int);
    ~History(void);
    void ChangeHistorySize(int);
    void AddHistory(const Point&, const Point&, const Point&, const Point&);
    int  BackHistory(void);
    void GetXY(int, Point&, Point&);
    void GetV(int, Point&, Point&);
    void ResetHistory(void);
};

class MenuBar {
private:
    Widget menubar;
    Widget title[4];
    int    restart_disabled;
    Pixmap check; /* unused with Motif */
public:
    MenuBar(Widget, int, int, int);
    void DisableRestart(void);
    void Sensitive(void);
    void Insensitive(void);
    void PauseOn(void);
    void PauseOff(void);
    void ClickTrial(Boolean, Boolean);
    void DemoMode(void);
    operator Widget(){ return menubar; }
    void CheckGame(int);
    void CheckPixmap(int);
};

class ScoreRecord {
public:
    char name[NAMELEN*2+1];
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    char date[9];
    char time[9];
    void SetDefault(void);
    void ReadField(FILE*, int, int&);
    void WriteField(FILE*, int, int&, int=0);
};

class Score {
private:
    Widget score;
    Widget mdialog;
    int  first_call;
    String res_strings[16];
    int  mdialog_exist;
    char *filename; /* File to store the global best score */
    char *logfile;  /* File to log the personal records */
    int  kanjiCode;
    long scoreToRegister; /* time in second */
    int  game;            /* Game size */
    ScoreRecord rec[SCORENUM];
    void readfile(void);
    int  writefile(void);
    void problem(void);
    static XtResource resources[16];
    static void retry_button(Widget, XtPointer);
    static void abandon_button(Widget, XtPointer);
    static void ms_to_hms(int, unsigned char&, unsigned char&, unsigned char&);
    void do_first_call(void);
    char *makeStatistics(const char*, int[], int[], int[]);
    int atoiSubstring(const char*, int);
public:
    Score(Widget);
    void SetScoreFile(const char*, const char*, const char*);
    void DisplayScore(int);
    void Popup(String);
    void Register(void);
    void SetScore(long value, int g) {
        scoreToRegister = value;
        game = g;
    }
    void LogRecord(int, int, int, int, int);
    int  PersonalStat(int);
    operator Widget(){ return score; }
};

void InitPicture(void);
void ExposeCB(Widget, XtPointer, XtPointer);
void InitGlobalMahjong(Widget, const char *, const char *);
void GameCB(Widget, int);
void SuppCB(Widget, int);
void ChangeGameCB(Widget, int, XtPointer);
void ChangePixmapCB(Widget, int, XtPointer);
void PopDownCB(Widget, XtPointer);
void Message(Pixmap, const char*, int = 0,
             XtTimerCallbackProc = NULL, XtCallbackProc = NULL);
Pixmap ResizePixmap(Pixmap, GC, unsigned int, unsigned int);
Pixmap MakeHalfBrightPixmap(Pixmap, GC);

extern Widget  toplevel;
extern XtAppContext app_context;

#ifndef HAVE_STRDUP
extern char *strdup(const char *s);
#endif /* HAVE_STRDUP */

#ifndef HAVE_STRSTR
extern char *strstr(const char *s1, const char *s2);
#endif /* HAVE_STRSTR */

#ifndef HAVE_STRNCASECMP
extern int strncasecmp(const char *s1, const char *s2, size_t n);
#endif /* HAVE_STRNCASECMP */

#ifndef HAVE_STRCASECMP
extern int strcasecmp(const char *s1, const char *s2);
#endif /* HAVE_STRCASECMP */

#ifndef KANJICODE
# define KANJICODE "jis"
#endif

#if HAVE_DRAND48
/* SunOS 4 does not have drand48 prototypes ... */
extern "C" {
    double drand48(void);
    void srand48(long);
}
#endif
