#include <stdio.h>
#include <stdlib.h>
#include "body.h"

void
PiePos::Delete(const Point &p1, const Point &p2)
{
    int diff = 0;
#if DEBUG
    fprintf(stderr, "Piepos::Delete ");
    for(int xx=0; xx<count; xx++)
        fprintf(stderr, "(%d,%d)", p[xx].x, p[xx].y);
    fprintf(stderr, "\n");
#endif
    for(int i=0; i<count; i++) {
        if (diff)
            p[i-diff] = p[i];
        if (p[i]==p1 || p[i]==p2)
            diff++;
    }
#if DEBUG
    if (diff != 2)
        fprintf(stderr, "!!! Delete: No match (%d,%d)(%d,%d)\n",p1.x,p1.y,p2.x,p2.y);
#endif
    if (diff)
        count -= 2;
}

void
PiePos::Move(const Point &from, const Point &to)
{
    for(int i=0; i<count; i++) {
        if (p[i]==from)
            p[i] = to;
    }
}

Body::Body(int g, int x, int y, Widget parent, Widget top)
    : History(x*y/2), game(g), xsize(x), ysize(y)
{
    int i, num_each;

    pie      = new (Mahjong** [xsize]);
    initialp = new (Mahjong** [xsize]);
    pstatus  = new (unsigned char* [xsize]);
    for(i=0; i<xsize; i++) {
        pie[i]      = new (Mahjong* [ysize]);
        initialp[i] = new (Mahjong* [ysize]);
        pstatus[i]  = new unsigned char [ysize];
    }
    num_each = x*y/PKIND;
    for(i=0; i<PKIND; i++) {
        piepos[i] = new PiePos(num_each);
    }
    clearqueue = 0;
    hintNum    = 0;
    pause      = 0;
    if (game >= NUM_GAME*2)
        gravity = 1;
    else
        gravity = 0;
#if USE_MOTIF
    body = XtVaCreateManagedWidget("drawing", xmDrawingAreaWidgetClass, parent,
                                   XmNtopAttachment,    XmATTACH_WIDGET,
                                   XmNrightAttachment,  XmATTACH_FORM,
                                   XmNleftAttachment,   XmATTACH_FORM,
                                   XmNbottomAttachment, XmATTACH_FORM,
                                   XmNtopWidget,        top,
                                   XmNwidth,            globRes.Width,
                                   XmNheight,           globRes.Height,
                                   NULL);
    XtAddCallback(body, XmNexposeCallback, (XtCallbackProc)ExposeCB, NULL);
    XtAddCallback(body, XmNresizeCallback, (XtCallbackProc)ResizeCB, NULL);
#else /* USE_MOTIF */
    body = XtVaCreateManagedWidget("simple", simpleWidgetClass, parent,
                                   XtNfromVert,    top,
                                   XtNtop,         XawChainTop,
                                   XtNbottom,      XawRubber,
                                   XtNleft,        XawRubber,
                                   XtNright,       XawRubber,
                                   XtNwidth,       globRes.Width,
                                   XtNheight,      globRes.Height,
                                   XtNborderWidth, 0,
                                   XtNresizable,   True,
                                   NULL);
    XtAddEventHandler(body, ExposureMask, False,
                      (XtEventHandler)ExposeCB, NULL);
    XtAddEventHandler(body, ConfigureNotify, False,
                      (XtEventHandler)ResizeCB, NULL);
#endif /* USE_MOTIF */
}

void
Body::SetGC(void)
{
    Colormap cmap = XDefaultColormapOfScreen(XtScreen(body));
    XColor  c1;
    XGCValues gcv;

    gcv.line_width = globRes.connLineWidth;
    gcv.foreground = globRes.connLineColor;
    gcv.join_style = JoinRound;
    gcl = XCreateGC(XtDisplay(body), XtWindow(body),
                    GCForeground | GCLineWidth | GCJoinStyle, &gcv);
    XtVaGetValues(body, XtNbackground, &(c1.pixel), NULL);
    gcv.foreground = c1.pixel;
    gcc = XCreateGC(XtDisplay(body), XtWindow(body),
                    GCLineWidth | GCForeground | GCJoinStyle, &gcv);
}

Body::~Body(void)
{
    XFreeGC(XtDisplay(body), gcl);
    XFreeGC(XtDisplay(body), gcc);
    for(int i=0; i<ysize; i++) {
        delete[] pie[i];
        delete[] initialp[i];
        delete[] pstatus[i];
    }
    for(int j=0; j<PKIND; j++) {
        delete piepos[j];
    }
    delete[] pie;
    delete[] initialp;
    delete[] pstatus;
    XtDestroyWidget(body);
}

void
Body::ChangeGame(int g, int x, int y)
{
    int i, num_each;

    xsize = x;
    ysize = y;
    game  = g;
    for(i=0; i<ysize; i++) {
        delete[] pie[i];
        delete[] initialp[i];
        delete[] pstatus[i];
    }
    for(i=0; i<PKIND; i++) {
        delete piepos[i];
    }
    delete[] pie;
    delete[] initialp;
    delete[] pstatus;

    pie      = new (Mahjong** [xsize]);
    initialp = new (Mahjong** [xsize]);
    pstatus  = new (unsigned char* [xsize]);
    for(i=0; i<xsize; i++) {
        pie[i]      = new (Mahjong* [ysize]);
        initialp[i] = new (Mahjong* [ysize]);
        pstatus[i]  = new unsigned char [ysize];
    }
    num_each = x*y/PKIND;
    for(i=0; i<PKIND; i++) {
        piepos[i] = new PiePos(num_each);
    }
    ChangeHistorySize(xsize*ysize/2);
}

int
Body::reset(int value)
{
    int i, j;
    for(i=0; i<xsize; i++)
        for(j=0; j<ysize; j++)
            pstatus[i][j] = value;
    for(i=0; i<PKIND; i++)
        piepos[i]->Clear();
    for(i=0; i<xsize; i++)
        for(j=0; j<ysize; j++)
            piepos[pie[i][j]->id]->Add(Point(i, j));
    refreshhint();
    ResetHistory();
    if (hintNum < 5)
        return -1;  // This configuration may be problematic
    else
        return 0;   // Good initial configuration!
}

int
Body::can_pass(int i, int j)
{
    if (i<-1 || i>xsize || j<-1 || j>ysize)
        return 0;
    else if (i==-1 || i==xsize)
        return 1;
    else if (j==-1 || j==ysize)
        return 1;
    else if (pstatus[i][j]==0)
        return 1;
    else if (i==xp1.x && j==xp1.y)
        return 1;
    else if (i==xp2.x && j==xp2.y)
        return 1;
    else
        return 0;
}

void
Body::get_hrange(int i, int j, int &is, int &ie)
{
    is = ie = i;
    while(can_pass(is-1, j)) is--;
    while(can_pass(ie+1, j)) ie++;
}

void
Body::get_vrange(int i, int j, int &js, int &je)
{
    js = je = j;
    while(can_pass(i, js-1)) js--;
    while(can_pass(i, je+1)) je++;
}

void
Body::refreshhint(void)
{
    Point (*p)[2], *t;
    int   c = 0, dummy, num;
    int   max_c;

    max_c = xsize*ysize*(xsize*ysize/PKIND - 1)/2;
    p = new Point[max_c][2];
    t = new Point[xsize*ysize/PKIND];

    if (hintNum == 0)
        delete[] hintArray;
    for(int i=0; i<PKIND; i++) {
        piepos[i]->GetPosArray(t, num);
        for(int j=0; j<num; j++)
            for(int k=j+1; k<num; k++)
                if (pstatus[t[j].x][t[j].y] == 1 &&
                    pstatus[t[k].x][t[k].y] == 1) {
                    xp1 = t[j];
                    xp2 = t[k];
                    if ((sweep_vert(t[j], t[k], dummy) ||
                         sweep_horiz(t[j], t[k], dummy))) {
                        p[c][0] = t[j];
                        p[c][1] = t[k];
                        c++;
                    }
                }
    }
#if DEBUG
    fprintf(stderr, "Removalbe pairs: %d\n",c);
#endif
    hintNum = c;
    currentHintNum = 0;
    if (c != 0) {
        hintArray = new Point [c*2];
        memcpy(hintArray, p, c*2*sizeof(Point));
    }
    delete[] p;
    delete[] t;
}

void
Body::getnexthint(void)
{
    hint_p1 = hintArray[currentHintNum*2];
    hint_p2 = hintArray[currentHintNum*2+1];
    currentHintNum++;
    if (currentHintNum >= hintNum)
        currentHintNum = 0;
}

void
Body::fallpiece(int col)
{
    int j, j0;
    int flag = 0;

    for(j0=ysize-1; j0>=0; j0--) {
        if (pstatus[col][j0] != 1)
            break;
    }
    // At this point, j0 is the lowest empty piece in the specified
    // column.  This means that the pieces existing above here must
    // be felt downto here.
    for(j=j0-1; j>=0; j--) {
        if (pstatus[col][j] == 1) {
            unsigned char c;
            Mahjong *m;
            c = pstatus[col][j];
            m = pie[col][j];
            pstatus[col][j]  = pstatus[col][j0];
            pie[col][j]      = pie[col][j0];
            pstatus[col][j0] = c;
            pie[col][j0]     = m;
            piepos[m->id]->Move(Point(col, j), Point(col, j0));
            j0--;
            flag = 1;
        }
    }
    /* if (flag)
        DrawCol(col); */
}

void
Body::insertpiece(const Point &p)
{
    int j, j0;

    if (pstatus[p.x][p.y] == 1) {
        for(j0=p.y; j0>=0; j0--) {
            if (pstatus[p.x][j0] != 1)
                break;
        }
        // At this point, j0 is the lowest empty place.
        for(j=j0+1; j<=p.y; j++) {
            unsigned char c;
            Mahjong *m;
            c = pstatus[p.x][j];
            m = pie[p.x][j];
            pstatus[p.x][j]  = pstatus[p.x][j0];
            pie[p.x][j]      = pie[p.x][j0];
            pstatus[p.x][j0] = c;
            pie[p.x][j0]     = m;
            piepos[m->id]->Move(Point(p.x, j), Point(p.x, j0));
            j0++;
        }
    }
    pstatus[p.x][p.y] = 1;
    piepos[pie[p.x][p.y]->id]->Add(p);
    DrawCol(p.x);
}

void
Body::removePair(void)
{
    AddHistory(xp1, xp2, vp1, vp2);
    rest -= 2;
    pstatus[xp1.x][xp1.y] = 0;
    pstatus[xp2.x][xp2.y] = 0;
    clearqueue++;
    LinkLine();
    piepos[pie[xp1.x][xp1.y]->id]->Delete(xp1, xp2);
    if (gravity) {
        fallpiece(xp1.x);
        if (xp1.x != xp2.x)
            fallpiece(xp2.x);
    }
    refreshhint();
    ResetTimer();
    status = 0;
}

int
Body::GetRest(void)
{
    return rest;
}

void
Body::SetGeometry(int width, int height)
{
#if DEBUG
    fprintf(stderr, "SetGeometry(%d,%d)\n", width, height);
#endif
    XtVaSetValues(toplevel,
                  XtNallowShellResize, True,
                  NULL);
    XtVaSetValues(body,
                  XtNwidth,  (Dimension)width,
                  XtNheight, (Dimension)height,
                  NULL);
    XtVaSetValues(toplevel,
                  XtNallowShellResize, False,
                  NULL);
    SetSize(width, height);
}

// This returns 1 if the widget size is changed
int
Body::GetGeometry(int &width, int &height)
{
    Dimension w, h;
    XtVaGetValues(body,
                  XtNwidth,  &w,
                  XtNheight, &h,
                  NULL);
    width  = w;
    height = h;
    if (width != currentWidth || height != currentHeight)
        return 1;
    else
        return 0;
}

void
Body::SetSize(int x, int y)
{
#if DEBUG
    fprintf(stderr, "SetSize(%d,%d)\n",x,y);
#endif
    // This function must be called after realized!
    Cursor cursor = XCreateFontCursor(XtDisplay(body), XC_watch);
    XDefineCursor(XtDisplay(body), XtWindow(body), cursor);
    xstep = x / (2+xsize);
    ystep = y / (2+ysize);
    xmarg = XMARGIN;
    ymarg = YMARGIN;
    xoff  = (x - xstep * xsize) / 2;
    yoff  = (y - ystep * ysize) / 2;
    currentWidth  = x;
    currentHeight = y;
    for(int i=0; i<PKIND; i++)
        Mp[i].Resize(toplevel, gcl, xstep-xmarg*2, ystep-ymarg*2);
    XClearArea(XtDisplay(body), XtWindow(body), 0, 0, x, y, True);
    XUndefineCursor(XtDisplay(body), XtWindow(body));
}

void
Body::Sort(void)
{
    int num_each = xsize*ysize/PKIND;
    int c = 0;

    for(int d=0; d<ysize/4; d++) {
        for(int i=0; i<xsize; i++) {
            for(int j=0; j<4; j++) {
                pie[i][d*4+j] = &Mp[c/num_each];
                c++;
            }
        }
    }
    rest = xsize*ysize;
    status = 3;
    reset(1);
}

// Sort function for "click trial"
void
Body::Sort2(void)
{
    int num_each, num_block, basex, basey;
    int blockx, blocky;
    int x1, y1, x2, y2;

    num_each  = xsize*ysize/PKIND;
    num_block = num_each / 2;
    if (xsize%num_block == 0) {
        blockx = xsize / num_block;
        blocky = ysize;
    } else {
        blockx = xsize * 2 / num_block;
        blocky = ysize / 2;
    }
    for(int i=0; i<num_block; i++) {
        basex = (i * blockx) % xsize;
        basey = ysize * i / blocky / num_block;
        basey *= blocky;
        for(int j=0; j<PKIND; j++) {
            x1 = basex + j/blocky;
            y1 = basey + j%blocky;
            x2 = basex + blockx - 1 - j/blocky;
            y2 = basey + blocky - 1 - j%blocky;
            pie[x1][y1] = &Mp[j];
            pie[x2][y2] = &Mp[j];
        }
    }
    rest = xsize*ysize;
    status = 0;
    clearqueue = 0;
    if (demoMode)
        helpUsed = 1;  // Never be a high score!
    else
        helpUsed = 0;
    reset(1);
}

void
Body::Restart(void)
{
    for(int ky=0; ky<ysize; ky++)
        for(int kx=0; kx<xsize; kx++)
            pie[kx][ky] = initialp[kx][ky];
    rest = xsize*ysize;
    status = 0;
    clearqueue = 0;
    helpUsed = 1;
    reset(1);
}

void
Body::Shuffle(void)
{
    int  i, a1, a2, b1, b2;
    Mahjong *p;
    int num_shuffle = xsize*ysize*2;

    // Check if current game is "Click Trial"
    if (game / NUM_GAME == 1) {
        Sort2();
        goto done;
    }
    // Loop until the good initial configuration found
    while(1) {
        for(i=0; i<num_shuffle; i++) {
#if HAVE_DRAND48
            a1 = (int)(drand48() * xsize);
            b1 = (int)(drand48() * ysize);
            a2 = (int)(drand48() * xsize);
            b2 = (int)(drand48() * ysize);
#elif HAVE_RANDOM
            a1 = random() % xsize;
            b1 = random() % ysize;
            a2 = random() % xsize;
            b2 = random() % ysize;
#elif HAVE_RAND
            a1 = (rand()>>2) % xsize;
            b1 = (rand()>>3) % ysize;
            a2 = (rand()>>4) % xsize;
            b2 = (rand()>>5) % ysize;
#else
            This line will cause error because all the random number
            generating functions are not available!
#endif
            p = pie[a1][b1];
            pie[a1][b1] = pie[a2][b2];
            pie[a2][b2] = p;
        }
        rest = xsize*ysize;
        status = 0;
        clearqueue = 0;
        if (demoMode)
            helpUsed = 1;  // Never be a high score!
        else
            helpUsed = 0;
        if (reset(1) == 0) {
            break;
        }
    }
  done:
    initialHintNum = hintNum;
    for(int ky=0; ky<ysize; ky++)
        for(int kx=0; kx<xsize; kx++)
            initialp[kx][ky] = pie[kx][ky];
}

void
Body::DrawOne(int i, int j)
{
    if (pstatus[i][j] == 1 && !pause)
        pie[i][j]->Draw(body, gcl, i*xstep+xoff+xmarg, j*ystep+yoff+ymarg, 1);
    else if (pstatus[i][j] == 2 && !pause)
        pie[i][j]->Draw(body, gcl, i*xstep+xoff+xmarg, j*ystep+yoff+ymarg, 2);
    else
        XClearArea(XtDisplay(body), XtWindow(body),
                   i*xstep+xoff+xmarg, j*ystep+yoff+ymarg,
                   xstep, ystep, False);
}

void
Body::DrawCol(int i)
{
    XClearArea(XtDisplay(body), XtWindow(body), i*xstep+xoff+xmarg, yoff+ymarg,
               xstep, ystep*ysize, False);
    for(int j=0; j<ysize; j++)
        DrawOne(i, j);
}

void
Body::DrawAll(void)
{
    XClearArea(XtDisplay(body), XtWindow(body), 0, 0,
               xoff+xstep*(2+xsize), yoff+ystep*(2+ysize), False);
    for(int i=0; i<xsize; i++)
        for(int j=0; j<ysize; j++)
            DrawOne(i, j);
}

void
Body::GetPos(int x, int y, int &i, int &j)
{
    i = x-xoff-xmarg;
    if (i<0) i = -1;
    else i /= xstep;
    j = y-yoff-ymarg;
    if (j<0) j = -1;
    else j /= ystep;
}

void
Body::DrawRegion(int x, int y, int w, int h)
{
    int is, js, ie, je;
    GetPos(x, y, is, js);
    GetPos(x+w, y+h, ie, je);

    for(int i=is; i<=ie; i++)
        for(int j=js; j<=je; j++)
            if (i>=0 && i<xsize && j>=0 && j<ysize)
                DrawOne(i, j);
}

int
Body::sweep_horiz(Point p1, Point p2, int &ir)
// p1 and p2 must not be reference because this function may swap these values.
{
    int i1, j1, i2, j2;
    int is1, ie1, is2, ie2;
    int is, ie, i, j;
    int value = 1000;  // let it enough big (smaller value is better)

    if (p1.y > p2.y) {  // make always p1.y<p2.y
        Point pp = p1;
        p1 = p2;
        p2 = pp;
    }
    i1 = p1.x;
    j1 = p1.y;
    i2 = p2.x;
    j2 = p2.y;
    get_hrange(i1, j1, is1, ie1);
    get_hrange(i2, j2, is2, ie2);
    is = (is1 > is2) ? is1 : is2;
    ie = (ie1 < ie2) ? ie1 : ie2;
    for(i=is; i<=ie; i++) {
        int flag = 1;
        for(j=j1; j<=j2; j++)
            if (!can_pass(i, j)) {
                flag = 0;
                break;
            }
        if (flag==1) { // 取れる!
            int this_value = abs(i-i1) + abs(i-i2);
            if (this_value < value) {
                value = this_value;
                ir = i;
            }
        }
    }
    if (value < 1000)
        return 1;
    else
        return 0;
}

int
Body::sweep_vert(Point p1, Point p2, int &jr)
// p1 and p2 must not be reference because this function may swap these values.
{
    int i1, j1, i2, j2;
    int js1, je1, js2, je2;
    int js, je, i, j;
    int value = 1000;  // let it enough big (smaller value is better)

    if (p1.x > p2.x) {  // make always p1.x<p2.x
        Point pp = p1;
        p1 = p2;
        p2 = pp;
    }
    i1 = p1.x;
    j1 = p1.y;
    i2 = p2.x;
    j2 = p2.y;
    get_vrange(i1, j1, js1, je1);
    get_vrange(i2, j2, js2, je2);
    js = (js1 > js2) ? js1 : js2;
    je = (je1 < je2) ? je1 : je2;
    for(j=js; j<=je; j++) {
        int flag = 1;
        for(i=i1; i<=i2; i++)
            if (!can_pass(i, j)) {
                flag = 0;
                break;
            }
        if (flag==1) { // 取れる!
            int this_value = abs(j-j1) + abs(j-j2);
            if (this_value < value) {
                value = this_value;
                jr = j;
            }
        }
    }
    if (value < 1000)
        return 1;
    else
        return 0;
}

// (i1,j1) と (i2,j2) の組み合わせが「取れる」かどうか、チェックする
int
Body::Check(const Point &p1, const Point &p2)
{
    int ir, jr;

    xp1 = p1;
    xp2 = p2;
    if (p1==p2 || pie[p1.x][p1.y] != pie[p2.x][p2.y])
        return 0;
    if (sweep_vert(p1, p2, jr)) {
        vp1 = p1;
        vp2 = p2;
        vp1.y = vp2.y = jr;
        return 1;
    }
    else if (sweep_horiz(p1, p2, ir)) {
        vp1 = p1;
        vp2 = p2;
        vp1.x = vp2.x = ir;
        return 1;
    }
    else
        return 0;
}

void
Body::GetPiece(int x, int y)
{
    int   xi, yi;
    Point here_p;

#if DEBUG
    if (x < 10 && !demoMode) {
        rest = 0;
        return;
    }
#endif
    GetPos(x, y, xi, yi);
    if (xi<0 || xi>=xsize || yi<0 || yi>=ysize)
        return;
    here_p = Point(xi, yi);
    switch(status) {
    case 0: // First piece
        if (pstatus[xi][yi] == 1) {
            // button1 is clicked as first piece
            status = 1;
            clicked_p = here_p;
            pstatus[xi][yi] = 2;
            DrawOne(xi, yi);
        }
        break;
    case 1: // Second piece
        if (pstatus[xi][yi] != 1)  // Avoid to get zombie piece!
            break;
        if (Check(here_p, clicked_p)) {
            // It can be removed!
            pstatus[xi][yi] = 2;
            DrawOne(xi, yi);
            removePair();
                // No parameters because it uses xp1, xp2,
                // vp1 and vp2 member variables.
        }
        break;
    case 4:  // After hint
        Check(hint_p1, hint_p2);  // Just to get vp1 and vp2
        removePair();
        break;
    }
}

void
Body::CancelPiece(int x, int y)
{
    int xi, yi;

    GetPos(x, y, xi, yi);
    if (xi<0 || xi>=xsize || yi<0 || yi>=ysize)
        return;
    switch(status) {
    case 1:  // Cancel (button3)
        status = 0;
        pstatus[clicked_p.x][clicked_p.y] = 1;
        DrawOne(clicked_p.x, clicked_p.y);
        break;
    case 4:  // Cancel after hint
        status = 0;
        pstatus[hint_p1.x][hint_p1.y] = 1;
        pstatus[hint_p2.x][hint_p2.y] = 1;
        DrawOne(hint_p1.x, hint_p1.y);
        DrawOne(hint_p2.x, hint_p2.y);
        break;
    }
}

void
Body::Back(void)
{
    Point b1, b2;

    // Don't back from initial state
    if (rest == xsize*ysize)
        return;
    // At first, clear the all lines existing
    ClearLineAll();
    GetXY(-1, b1, b2);
    if (BackHistory()) {
        rest +=2;
        if (gravity) {
            if (b1.y < b2.y) {
                // This order is very important here. The piece at
                // the lower position must be inserted first.
                insertpiece(b2);
                insertpiece(b1);
            } else {
                insertpiece(b1);
                insertpiece(b2);
            }
        } else {
            piepos[pie[b1.x][b1.y]->id]->Add(b1);
            piepos[pie[b1.x][b1.y]->id]->Add(b2);
            pstatus[b1.x][b1.y] = 1;
            pstatus[b2.x][b2.y] = 1;
            DrawOne(b1.x, b1.y);
            DrawOne(b2.x, b2.y);
        }
        refreshhint();
    }
}

int
Body::Active(void)
{
    if (status != 3 && pause == 0)
        return 1;
    else
        return 0;
}

void
Body::HourlyPatrol(void)
{
    if (clearqueue > 0 && GetTimer()>=globRes.connLineTime)
        ClearLine();
    if (rest == 0) {
        char b[128];
        long result;

        sc->SetScore(result = tm->GetTimer(), game);
        sprintf(b, globRes.timeFormat, result/60000, (result/1000)%60);
        if (helpUsed) {
            if (!globRes.autoDemo)
                sc->LogRecord(2, result, game, initialHintNum, 0);
            Message(MESSAGE_SYMBOL, b, timeOut, PopDownTO);
        }
        else {
            if (!globRes.autoDemo)
                sc->LogRecord(1, result, game, initialHintNum, 0);
            Message(MESSAGE_SYMBOL, b, timeOut, PopDownTO,
                    (XtCallbackProc)ScoreRegisterCB);
        }
        GameOver();
    }
    else if (hintNum == 0) {
        if (!globRes.autoDemo)
            sc->LogRecord(3, tm->GetTimer(), game, initialHintNum, rest);
        Message(MESSAGE_SYMBOL, globRes.tedumari, timeOut, PopDownTO);
        GameOver();
    }
}

void
Body::GameOver(void)
{
    ClearLineAll();
    status = 3;
    mb->Insensitive();
}

void
Body::LogGiveUp(void)  // Log the time even you give up the previous game!
{
    if (status != 3) {
        long result = tm->GetTimer();
        if (!globRes.autoDemo)
            sc->LogRecord(0, result, game, initialHintNum, rest);
    }
}

void
Body::SetDemo(int demo)
{
    if (demo) {
        timeOut  = 5000;
        demoMode = 1;
    } else {
        timeOut  = 0;
        demoMode = 0;
    }
}

void
Body::Robot(void)
{
    if (hintNum > 0 && clearqueue == 0 && rest !=0) {
        Hint();
        GetPiece(xoff+xstep/2, yoff+ystep/2);
    }
}

void
Body::LinkLine(void)
{
    Point np1, np2;
    int ti1, ti2, tj1, tj2;
    int vi1, vi2, vj1, vj2;
    XPoint pts[4];

    GetXY(0 - clearqueue, np1, np2);
    ti1 = np1.x; tj1 = np1.y; ti2 = np2.x; tj2 = np2.y;
    GetV(0 - clearqueue, np1, np2);
    vi1 = np1.x; vj1 = np1.y; vi2 = np2.x; vj2 = np2.y;
    pts[0].x = xoff+xmarg+ti1*xstep+xstep/2;
    pts[0].y = yoff+ymarg+tj1*ystep+ystep/2;
    pts[1].x = xoff+xmarg+vi1*xstep+xstep/2;
    pts[1].y = yoff+ymarg+vj1*ystep+ystep/2;
    pts[2].x = xoff+xmarg+vi2*xstep+xstep/2;
    pts[2].y = yoff+ymarg+vj2*ystep+ystep/2;
    pts[3].x = xoff+xmarg+ti2*xstep+xstep/2;
    pts[3].y = yoff+ymarg+tj2*ystep+ystep/2;
    XDrawLines(XtDisplay(body), XtWindow(body), gcl, pts, 4, CoordModeOrigin);
}

void
Body::ClearLine(void)
{
    Point np1, np2;
    int ti1, ti2, tj1, tj2;
    int vi1, vi2, vj1, vj2;
    XPoint pts[4];

    GetXY(0 - clearqueue, np1, np2);
    ti1 = np1.x; tj1 = np1.y; ti2 = np2.x; tj2 = np2.y;
    GetV(0 - clearqueue, np1, np2);
    vi1 = np1.x; vj1 = np1.y; vi2 = np2.x; vj2 = np2.y;
    pts[0].x = xoff+xmarg+ti1*xstep+xstep/2;
    pts[0].y = yoff+ymarg+tj1*ystep+ystep/2;
    pts[1].x = xoff+xmarg+vi1*xstep+xstep/2;
    pts[1].y = yoff+ymarg+vj1*ystep+ystep/2;
    pts[2].x = xoff+xmarg+vi2*xstep+xstep/2;
    pts[2].y = yoff+ymarg+vj2*ystep+ystep/2;
    pts[3].x = xoff+xmarg+ti2*xstep+xstep/2;
    pts[3].y = yoff+ymarg+tj2*ystep+ystep/2;
    XDrawLines(XtDisplay(body), XtWindow(body), gcc, pts, 4, CoordModeOrigin);
    if (gravity) {
        DrawCol(ti1);
        if (ti1 != ti2)
            DrawCol(ti2);
    } else {
        DrawOne(ti1, tj1);
        DrawOne(ti2, tj2);
    }
    clearqueue--;
}

void
Body::ClearLineAll(void)
{
    while(clearqueue > 0)
        ClearLine();
}

void
Body::Hint(void)
{
    if (status == 4) {
        status = 0;
        pstatus[hint_p1.x][hint_p1.y] = 1;
        pstatus[hint_p2.x][hint_p2.y] = 1;
        DrawOne(hint_p1.x, hint_p1.y);
        DrawOne(hint_p2.x, hint_p2.y);
    }
    else if (status == 1) {
        status = 0;
        pstatus[clicked_p.x][clicked_p.y] = 1;
        DrawOne(clicked_p.x, clicked_p.y);
    }
    getnexthint();
    helpUsed = 1;

    pstatus[hint_p1.x][hint_p1.y] = 2;
    pstatus[hint_p2.x][hint_p2.y] = 2;
    DrawOne(hint_p1.x, hint_p1.y);
    DrawOne(hint_p2.x, hint_p2.y);
    status = 4;
}

int
Body::TogglePause(void)
{
    if (status == 3) // Pause at no game is problematic
        return -1;
    if (pause == 0) {
        pause = 1;
    } else if (pause == 1) {
        pause = 0;
    }
    DrawAll();
    return pause;
}

void
Body::ToggleClickTrial(void)
{
    if (game / NUM_GAME != 1) {
        game = game % NUM_GAME + NUM_GAME;
        mb->ClickTrial(True, False);
    } else {
        game %= NUM_GAME;
        mb->ClickTrial(False, False);
    }
    gravity = 0;
}

void
Body::ToggleGravity(void)
{
    if (game / NUM_GAME != 2) {
        game = game % NUM_GAME + NUM_GAME*2;
        mb->ClickTrial(False, True);
        gravity = 1;
    } else {
        game %= NUM_GAME;
        mb->ClickTrial(False, False);
        gravity = 0;
    }
}

void
Body::WriteRcFile(void)
{
    FILE *file;
    const char *tr, *gr;

    file = fopen(rcfile, "w");
    if (file) {
        fprintf(file, CLASS_NAME "*gameSize: %d\n", game%NUM_GAME);
        switch(game / NUM_GAME) {
        case 1: // Click Trial
            tr = "true";
            gr = "false";
            break;
        case 2: // Gravity
            tr = "false";
            gr = "true";
            break;
        default: // Normal
            tr = "false";
            gr = "false";
            break;
        }
        fprintf(file, CLASS_NAME "*trialMode: %s\n", tr);
        fprintf(file, CLASS_NAME "*gravityMode: %s\n", gr);
	fprintf(file, CLASS_NAME "*imageSet: %d\n", globRes.imageSet);
        fclose(file);
    }
}
