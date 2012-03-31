#include "components.h"

void
Timer::ResetTimer(void)
{
  struct timezone tz;

  gettimeofday(&initialtime, &tz);
  timeoffset = 0;
  currenttime = 0;
}

long
Timer::GetTimer(void)
{
  struct timeval  tv;
  struct timezone tz;
  long sec, usec;

  gettimeofday(&tv, &tz);
  sec = tv.tv_sec - initialtime.tv_sec;
  if (tv.tv_usec < initialtime.tv_usec)
    usec = 1000000;
  else
    usec = 0;
  usec += tv.tv_usec - initialtime.tv_usec;
  return (timeoffset+sec*1000+usec/1000);
}

void
Timer::SetTimer(long tim)
{
  struct timezone tz;

  gettimeofday(&initialtime, &tz);
  timeoffset = tim;
  currenttime = 0;
}

TimerW::TimerW(Widget parent, Widget top, char *s1, char *s2,
               char *s3, char *s4)
{
#if USE_MOTIF
    XmString ms;

    ms = XmStringCreateLocalized(s4);
    label[3] = XtVaCreateManagedWidget("label3", xmLabelWidgetClass, parent,
                                       XmNtopAttachment,    XmATTACH_WIDGET,
                                       XmNrightAttachment,  XmATTACH_FORM,
                                       XmNleftAttachment,   XmATTACH_NONE,
                                       XmNbottomAttachment, XmATTACH_NONE,
                                       XmNalignment,        XmALIGNMENT_END,
                                       XmNlabelString,      ms,
                                       XmNtopWidget,        top,
                                       XmNrecomputeSize,    False,
                                       NULL);
    XmStringFree(ms);

    ms = XmStringCreateLocalized(s3);
    label[2] = XtVaCreateManagedWidget("label2", xmLabelWidgetClass, parent,
                                       XmNtopAttachment,    XmATTACH_WIDGET,
                                       XmNrightAttachment,  XmATTACH_WIDGET,
                                       XmNleftAttachment,   XmATTACH_NONE,
                                       XmNbottomAttachment, XmATTACH_NONE,
                                       XmNalignment,        XmALIGNMENT_END,
                                       XmNlabelString,      ms,
                                       XmNtopWidget,        top,
                                       XmNrightWidget,      label[3],
                                       XmNmarginLeft,       12,
                                       XmNrecomputeSize,    False,
                                       NULL);
    XmStringFree(ms);

    ms = XmStringCreateLocalized(s2);
    label[1] = XtVaCreateManagedWidget("label1", xmLabelWidgetClass, parent,
                                       XmNtopAttachment,    XmATTACH_WIDGET,
                                       XmNrightAttachment,  XmATTACH_WIDGET,
                                       XmNleftAttachment,   XmATTACH_NONE,
                                       XmNbottomAttachment, XmATTACH_NONE,
                                       XmNalignment,        XmALIGNMENT_END,
                                       XmNlabelString,      ms,
                                       XmNtopWidget,        top,
                                       XmNrightWidget,      label[2],
                                       XmNrecomputeSize,    False,
                                       NULL);
    XmStringFree(ms);

    str1 = XmStringCreateLocalized(s1);
    label[0] = XtVaCreateManagedWidget("label0", xmLabelWidgetClass, parent,
                                       XmNtopAttachment,    XmATTACH_WIDGET,
                                       XmNrightAttachment,  XmATTACH_WIDGET,
                                       XmNleftAttachment,   XmATTACH_FORM,
                                       XmNbottomAttachment, XmATTACH_NONE,
                                       XmNalignment,        XmALIGNMENT_END,
                                       XmNlabelString,      str1,
                                       XmNtopWidget,        top,
                                       XmNrightWidget,      label[1],
                                       XmNrecomputeSize,    True,
                                       NULL);

#else /* USE_MOTIF */
    /* Don't use "top" with Athena Widget */
    label[0] = XtVaCreateManagedWidget("label0", labelWidgetClass, parent,
                                       XtNtop,         XawChainTop,
                                       XtNbottom,      XawChainTop,
                                       XtNleft,        XawChainLeft,
                                       XtNright,       XawChainLeft,
                                       XtNresizable,   True,
                                       XtNborderWidth, 0,
                                       XtNlabel,       s1,
                                       NULL);
    label[1] = XtVaCreateManagedWidget("label1", labelWidgetClass, parent,
                                       XtNfromHoriz,   label[0],
                                       XtNtop,         XawChainTop,
                                       XtNbottom,      XawChainTop,
                                       XtNleft,        XawChainLeft,
                                       XtNright,       XawChainLeft,
                                       XtNborderWidth, 0,
                                       XtNlabel,       s2,
                                       NULL);
    label[2] = XtVaCreateManagedWidget("label2", labelWidgetClass, parent,
                                       XtNfromHoriz,   label[1],
                                       XtNtop,         XawChainTop,
                                       XtNbottom,      XawChainTop,
                                       XtNleft,        XawChainLeft,
                                       XtNright,       XawChainLeft,
                                       XtNborderWidth, 0,
                                       XtNlabel,       s3,
                                       NULL);
    label[3] = XtVaCreateManagedWidget("label3", labelWidgetClass, parent,
                                       XtNfromHoriz,   label[2],
                                       XtNtop,         XawChainTop,
                                       XtNbottom,      XawChainTop,
                                       XtNleft,        XawChainLeft,
                                       XtNright,       XawChainLeft,
                                       XtNborderWidth, 0,
                                       XtNlabel,       s4,
                                       NULL);
    str1 = s1;
#endif /* USE_MOTIF */
    dateMode = 0;
    countNow = 0;
    ResetTimer();
}

void
TimerW::DisplayTimer(int count)
{
    char buffer[128];
    long t;

    if (dateMode) {
        XtManageChild(label[1]);
        XtManageChild(label[2]);
        XtManageChild(label[3]);
        dateMode = 0;
#if USE_MOTIF
        XtVaSetValues(label[0],
                      XmNlabelString,      str1,
                      XmNrightAttachment,  XmATTACH_WIDGET,
                      XmNrightWidget,      label[1],
                      NULL);
#else /* USE_MOTIF */
        XtVaSetValues(label[0],
                      XtNlabel, str1,
                      NULL);
#endif /* USE_MOTIF */
    }
    if (count != countNow) {
        sprintf(buffer, "%3.3d", count);
#if USE_MOTIF
        XmString s = XmStringCreateLocalized(buffer);
        XtVaSetValues(label[1], XmNlabelString, s, NULL);
        XmStringFree(s);
#else /* USE_MOTIF */
        XtVaSetValues(label[1], XtNlabel, buffer, NULL);
#endif /* USE_MOTIF */
        countNow = count;
    }
    t = GetTimer() / 1000;
    if (t > currenttime) {
        sprintf(buffer, "%2.2d:%2.2d:%2.2d", t/3600, (t/60)%60, t%60);
#if USE_MOTIF
        XmString s = XmStringCreateLocalized(buffer);
        XtVaSetValues(label[3], XmNlabelString, s, NULL);
        XmStringFree(s);
#else /* USE_MOTIF */
        XtVaSetValues(label[3], XtNlabel, buffer, NULL);
#endif /* USE_MOTIF */
        currenttime = t;
    }
}

void
TimerW::DisplayCurrentTime(void)
{
    time_t   t;
    static time_t t_before = 0;
    char     *timestr, *p;

    if (!dateMode) {
        XtUnmanageChild(label[1]);
        XtUnmanageChild(label[2]);
        XtUnmanageChild(label[3]);
        dateMode = 1;
#if USE_MOTIF
        XtVaSetValues(label[0],
                      XmNrightAttachment,  XmATTACH_FORM,
                      NULL);
#endif /* USE_MOTIF */
    }
    t = time(NULL);
    if (t == t_before)
        return;
    t_before = t;
    timestr = ctime(&t);
    if ((p = strchr(timestr, '\n')) != NULL)
        *p = '\0';
#if USE_MOTIF
    XmString s = XmStringCreateLocalized(timestr);
    XtVaSetValues(label[0],
                  XmNlabelString, s,
                  NULL);
    XmStringFree(s);
#else /* USE_MOTIF */
    XtVaSetValues(label[0], XtNlabel, timestr, NULL);
#endif /* USE_MOTIF */
}
