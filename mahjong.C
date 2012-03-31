#include <stdio.h>
#include "components.h"

Mahjong Mp[36];

Mahjong::Mahjong(void)
{
    data = 0;
    datag = 0;
    rdata = 0;
    rdatag = 0;
}

// filename:  regular piece xpm data
// num:       ID for object instance
// closeness: Color closeness for xpm library
void
Mahjong::ReadFile(Widget w, char *filename, int num, int closeness)
{
    XpmAttributes attributes;
    int result;
    Pixmap mask;
    Colormap colormap = XDefaultColormapOfScreen(XtScreen(w));
    GC gc;
    XGCValues gcv;

    if (rdata != 0 && data != rdata) {
        XFreePixmap(XtDisplay(w), rdata);
        XFreePixmap(XtDisplay(w), rdatag);
    }
    if (data != 0) {
        XFreePixmap(XtDisplay(w), data);
        XFreePixmap(XtDisplay(w), datag);
    }
    attributes.valuemask = XpmColormap | XpmSize | XpmCloseness;
    attributes.colormap  = colormap;
    attributes.exactColors = False;
    attributes.closeness = closeness;
    id = num;
    result = XpmReadFileToPixmap(XtDisplay(w), XtWindow(w), filename,
                                  &data, &mask, &attributes);
    width  = original_width  = attributes.width;
    height = original_height = attributes.height;
    if (result != XpmSuccess && result != XpmColorError) {
        fprintf(stderr, "XpmReadFileToPixmap failed ");
        switch(result) {
        case XpmOpenFailed:
            fprintf(stderr, "(Cannot open xpm files)\n");
            break;
        case XpmFileInvalid:
            fprintf(stderr, "(xpm file may be broken)\n");
            break;
        case XpmNoMemory:
            fprintf(stderr, "(memory exhausted)\n");
            break;
        case XpmColorFailed:
            fprintf(stderr, "(color allocation failed [closeness:%d])\n",
                    closeness);
            break;
        default:
            fprintf(stderr, "(reason unknown)\n");
            break;
        }
        fprintf(stderr, " %s\n", filename);
        exit(1);
    } else {
        attributes.valuemask = XpmColormap;
        gcv.foreground = WhitePixelOfScreen(XtScreen(w));
        gc = XCreateGC(XtDisplay(w), XtWindow(w), GCForeground, &gcv);
        datag = MakeHalfBrightPixmap(data, gc);
        result = XpmSuccess;
        XFreeGC(XtDisplay(w), gc);
    }
    rdata  = data;
    rdatag = datag;
    resized = 0;
}

void
Mahjong::Resize(Widget w, GC gc, unsigned int new_width, unsigned int new_height)
{
#if DEBUG > 2
    fprintf(stderr, "piece[%2.2d] resize (%d,%d)->(%d,%d)\n",
            id, width, height, new_width, new_height);
#endif
    if (new_width == width && new_height == height) {
        return;
    }

    if (resized) {
        XFreePixmap(XtDisplay(w), rdata);
        XFreePixmap(XtDisplay(w), rdatag);
    }

    if (new_width == original_width && new_height == original_height) {
        rdata  = data;
        rdatag = datag;
        resized = 0;
    }
    else {
        rdata  = ResizePixmap(data,  gc, new_width, new_height);
        rdatag = MakeHalfBrightPixmap(rdata, gc);
        resized = 1;
    }
    width  = new_width;
    height = new_height;
}

void
Mahjong::GetSize(unsigned int &w, unsigned int &h)
{
    w = width;
    h = height;
}

void
Mahjong::Draw(Widget w, GC gc, int x, int y, int d)
{
    switch(d) {
    case 1:
        XCopyArea(XtDisplay(w), rdata,  XtWindow(w), gc, 0, 0, width, height, x, y);
        break;
    case 2:
        XCopyArea(XtDisplay(w), rdatag, XtWindow(w), gc, 0, 0, width, height, x, y);
        break;
    }
}
