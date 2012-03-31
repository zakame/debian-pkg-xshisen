#include "components.h"

#define CLASS_NAME "XShisen"
#define MIN_WIN_WID 320
#define MIN_WIN_HEI 200
#define NUM_GAME      3      // Number of game variation (size)
#define XMARGIN       1      // Horizontal dots between pieces
#define YMARGIN       1      // Vertical dots between pieces
#define MESSAGE_SYMBOL  ((Pixmap)Mp[18])  // Symbol Pixmap for message dialog

class Body : private Timer, public History {
private:
    Widget body;
    unsigned short xsize, ysize;
    Mahjong ***pie;
    Mahjong ***initialp;
    PiePos  *piepos[PKIND];
    unsigned char **pstatus;
        // 0: removed, 1: exist, 2: just clicked
    short rest;   // number of pieces left
    unsigned char pause;  // Game is pausing when pause=1
    unsigned char status;
        // 0: normal, 1: one clicked, 2: pair selected (not cleared yet)
        // 3: no game
    unsigned char gravity;
    unsigned char clearqueue;
    Point clicked_p;  // position of clicked piece when status==1
    Point xp1, xp2;
        // Positions of pieces to be getting
    Point vp1, vp2;
        // Positions to connect line
    Point hint_p1, hint_p2;  // Position of hint
    short hintNum, currentHintNum, initialHintNum;
    Point *hintArray;
    GC  gcl, gcc;
    short xoff, yoff;
    short xstep, ystep;
    short xmarg, ymarg;
    short currentWidth, currentHeight;
    unsigned char demoMode;
    unsigned char helpUsed;
    short timeOut;
    int reset(int);
    void refreshhint(void);
    void getnexthint(void);
    void fallpiece(int);
    void insertpiece(const Point&);
    void removePair(void);
    int can_pass(int, int);
    void get_hrange(int, int, int&, int&);
    void get_vrange(int, int, int&, int&);
    int sweep_horiz(Point, Point, int&);
    int sweep_vert(Point, Point, int&);
public:
    int game;
    Body(int, int, int, Widget, Widget);
    ~Body(void);
    void ChangeGame(int, int, int);
    void SetGC(void);
    void SetGeometry(int, int);
    int  GetGeometry(int&, int&);
    void SetSize(int, int);
    void Sort(void);  // for Opening
    void Sort2(void); // for "Click Trial"
    void Restart(void);
    void Shuffle(void);
    void GetPos(int, int, int&, int&);
    void DrawOne(int, int);
    void DrawCol(int);
    void DrawAll(void);
    void DrawRegion(int, int, int, int);
    void GetPiece(int, int);
    void CancelPiece(int, int);
    void Back(void);
    int GetRest(void);
    int Check(const Point&, const Point&);
    int Active(void);
    void HourlyPatrol(void);
    void LinkLine(void);
    void ClearLine(void);
    void ClearLineAll(void);
    void Hint(void);
    int TogglePause(void);
    void ToggleClickTrial(void);
    void ToggleGravity(void);
    void WriteRcFile(void);
    void LogGiveUp(void);
    void GameOver(void);
    void SetDemo(int);
    void Robot(void);
    operator Widget(){ return body; }
};

struct GlobRes {
    char    *aboutString;
    char    *tedumari;
    int     Width;
    int     Height;
    char    *timeFormat;
    char    *displayFormat1;
    char    *displayFormat2;
    char    *scoreFile;
    char    *personalScoreFile;
    Boolean scoreOnly;
    Pixel   connLineColor;
    int     connLineTime;
    int     connLineWidth;
    Boolean autoDemo;
    char    *libDir;
    char    *scoreDir;
    float   magFactor;
    Boolean fitPixmap;
    int     colorCloseness;
    int     gameSize;
    Boolean trialMode;
    Boolean gravityMode;
    int     idleTime;
    char    *kanjiCode;
    Boolean kanjiConv;
    int     imageSet;
};

void MenuAC(Widget, XEvent*, String*, Cardinal*);
void PickupPieceAC(Widget, XEvent*, String*, Cardinal*);
void CancelPieceAC(Widget, XEvent*, String*, Cardinal*);
void PopDownTO(XtPointer, XtIntervalId*);
void ScoreRegisterCB(Widget, int);
void ResizeCB(void);
void AutoDemoCB(void);
void ExitCB(void);
void GetGameSize(int, int&, int&);
void GetBoardSizeFromGameSize(int, int, int&, int&);
void SetGameStart(void);

extern Mahjong      Mp[];
extern Body         *bd;
extern Score        *sc;
extern MenuBar      *mb;
extern TimerW       *tm;
extern GlobRes      globRes;
extern Dimension    max_win_wid;
extern Dimension    max_win_hei;
extern char         *rcfile;
