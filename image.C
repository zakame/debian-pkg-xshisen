#include <stdlib.h>
#include "components.h"

Pixmap
ResizePixmap(Pixmap original, GC gc,
             unsigned int new_width, unsigned int new_height)
{
    XImage *orig_image, *new_image;
    unsigned int orig_width, orig_height;
    unsigned int dummy, depth;
    int    dummy_x, dummy_y;
    int    bitmap_pad;
    Window win;
    Display *disp = XtDisplay(toplevel);
    int     scno = XDefaultScreen(disp);
    int     x, y, xo, yo;
    double  x_scale_factor, y_scale_factor;
    Pixmap  new_pixmap;

    XGetGeometry(disp, original, &win,
                 &dummy_x, &dummy_y, &orig_width, &orig_height, &dummy, &depth);
    orig_image = XGetImage(disp, original,
                           0, 0, orig_width, orig_height, AllPlanes, ZPixmap);
    bitmap_pad = (depth>16)?32:((depth>8)?16:8);
    new_image = XCreateImage(disp, XDefaultVisual(disp, scno), depth, ZPixmap,
                             0, NULL, new_width, new_height, bitmap_pad, 0);
    new_image->data = (char *)malloc(new_image->bytes_per_line * new_height);
    x_scale_factor = ((double)orig_width - 1.0)/ ((double)new_width - 1.0);
    y_scale_factor = ((double)orig_height- 1.0)/ ((double)new_height - 1.0);
    for(y=0; y<new_height; y++) {
        yo = (int)(0.5 + y * y_scale_factor);
        if (yo >= orig_height)
            yo = orig_height-1;
        for(x=0; x<new_width; x++) {
            xo = (int)(0.5 + x * x_scale_factor);
            if (xo >= orig_width)
                xo = orig_width-1;
            XPutPixel(new_image, x, y, XGetPixel(orig_image, xo, yo));
        }
    }
    new_pixmap = XCreatePixmap(disp, win, new_width, new_height, depth);
    XPutImage(disp, new_pixmap, gc, new_image, 0, 0, 0, 0, new_width, new_height);
    XDestroyImage(new_image);
    return new_pixmap;
}

Pixmap
MakeHalfBrightPixmap(Pixmap original, GC gc)
{
    XImage *orig_image, *new_image;
    unsigned int width, height;
    unsigned int dummy, depth;
    int    dummy_x, dummy_y;
    int    bitmap_pad;
    Window win;
    Display *disp = XtDisplay(toplevel);
    int     scno = XDefaultScreen(disp);
    int     x, y;
    Pixmap  new_pixmap;

    XGetGeometry(disp, original, &win,
                 &dummy_x, &dummy_y, &width, &height, &dummy, &depth);
    orig_image = XGetImage(disp, original,
                           0, 0, width, height, AllPlanes, ZPixmap);
    bitmap_pad = (depth>16)?32:((depth>8)?16:8);
    new_image = XCreateImage(disp, XDefaultVisual(disp, scno), depth, ZPixmap,
                             0, NULL, width, height, bitmap_pad, 0);
    new_image->data = (char *)malloc(new_image->bytes_per_line * height);
    for(y=0; y<height; y++) {
        for(x=0; x<width; x++) {
            if ((x+y)%2 == 0)
                XPutPixel(new_image, x, y, BlackPixel(disp, scno));
            else
                XPutPixel(new_image, x, y, XGetPixel(orig_image, x, y));
        }
    }
    new_pixmap = XCreatePixmap(disp, win, width, height, depth);
    XPutImage(disp, new_pixmap, gc, new_image, 0, 0, 0, 0, width, height);
    XDestroyImage(new_image);
    return new_pixmap;
}
