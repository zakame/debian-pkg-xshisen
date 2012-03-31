#include <pwd.h>
#include "components.h"
#include "kconv.h"

XtResource Score::resources[] = {
    { "scoreTitle",  "ScoreTitle",  XtRString, sizeof(char *),
      0*sizeof(char *), XtRString, (XtPointer)"XShisen High Score\n\n" },
    { "scoreFormat", "ScoreFormat", XtRString, sizeof(char *),
      1*sizeof(char *), XtRString, (XtPointer)"%2d  %2.2d:%2.2d:%2.2d  %-28s  %s %s\n" },
    { "strPeriod", "StrPeriod", XtRString, sizeof(char *),
      2*sizeof(char *), XtRString, (XtPointer)"Last %d days" },
    { "averagePeriod", "AveragePeriod", XtRString, sizeof(char *),
      3*sizeof(char *), XtRString, (XtPointer)"14" },
    { "strPlayed", "StrPlayed", XtRString, sizeof(char *),
      4*sizeof(char *), XtRString, (XtPointer)"Played" },
    { "strCompleted", "StrCompleted", XtRString, sizeof(char *),
      5*sizeof(char *), XtRString, (XtPointer)"Completed" },
    { "strTedumari", "StrTedumari", XtRString, sizeof(char *),
      6*sizeof(char *), XtRString, (XtPointer)"Deadlocked" },
    { "strSearched", "StrSearched", XtRString, sizeof(char *),
      7*sizeof(char *), XtRString, (XtPointer)"Search used" },
    { "strGiveUp", "StrGiveUp", XtRString, sizeof(char *),
      8*sizeof(char *), XtRString, (XtPointer)"Given up" },
    { "strTotal", "StrTotal", XtRString, sizeof(char *),
      9*sizeof(char *), XtRString, (XtPointer)"Total" },
    { "strAverage", "StrAverage", XtRString, sizeof(char *),
      10*sizeof(char *), XtRString, (XtPointer)"Whole Average" },
    { "strGames", "StrGames", XtRString, sizeof(char *),
      11*sizeof(char *), XtRString, (XtPointer)"games" },
    { "personalTitle", "PersonalTitle", XtRString, sizeof(char *),
      12*sizeof(char *), XtRString, (XtPointer)"Personal Statistics for %s" }
};

void
ScoreRecord::SetDefault(void)
{
    strcpy(name, "                            ");
    hour = 99;
    min  = 99;
    sec  = 99;
    strcpy(date, "00-00-00");
    strcpy(time, "00:00:00");
}

void
ScoreRecord::ReadField(FILE *fp, int kanjiCode, int &offset)
{
    char buffer[64];

    if (fread((void*)buffer,  sizeof(char), 64, fp) != 64 ||
        buffer[0] == '\0') {
        SetDefault();
        return;
    }
    for(int i=0; i<64; i++) {
        buffer[i] = ((buffer[i] - offset - i) & 0xff);
    }
    offset += 64;
    strncpy(name, buffer, 28);
    name[28] = '\0';
    switch(kanjiCode) {
    case 1:
        strcpy(name, sjis_to_euc(name));
        break;
    case 2:
        strcpy(name, sjis_to_jis(name));
        break;
    }
    hour = atoi(&buffer[35]);
    min  = atoi(&buffer[38]);
    sec  = atoi(&buffer[41]);
    strncpy(date, &buffer[44], 8);
    date[8] = '\0';
    strncpy(time, &buffer[53], 8);
    time[8] = '\0';
    if (date[0] == '1') {
        for(int i=1; i<8; i++) {
            date[i-1] = date[i];
        }
        date[7] = date[6] == '0' ? '1' : '0';
    }
}

void
ScoreRecord::WriteField(FILE *fp, int kanjiCode, int &cr_offset, int cr)
{
    char buffer[64];

    switch(kanjiCode) {
    case 1:
        strcpy(buffer, euc_to_sjis(name));
        break;
    case 2:
        strcpy(buffer, jis_to_sjis(name));
        break;
    default:
        strcpy(buffer, name);
        break;
    }
    buffer[28] = ' ';
    strncpy(&buffer[29], "X" XSHISEN_VERSION, 5);
    buffer[34] = ' ';
    sprintf(&buffer[35], "%2.2d:%2.2d:%2.2d", hour, min, sec);
    buffer[43] = ' ';
    strncpy(&buffer[44], date, 8);
    buffer[52] = ' ';
    strncpy(&buffer[53], time, 8);
    buffer[61] = ' ';
    buffer[62] = '\r';
    buffer[63] = '\n';
    if (cr) {
        for(int i=0; i<64; i++) {
            buffer[i] = ((buffer[i] + cr_offset + i) & 0xff);
        }
        cr_offset += 64;
    }
    fwrite((void*)buffer, sizeof(char), 64, fp);
}

void
Score::retry_button(Widget w, XtPointer client_data)
{
    Score *p = (Score *)client_data;
#if USE_MOTIF
    XtUnmanageChild(w);
#else /* USE_MOTIF */
    XtPopdown(p->mdialog);
#endif /* USE_MOTIF */
    p->Register();
}

void
Score::abandon_button(Widget w, XtPointer client_data)
{
    Score *p = (Score *)client_data;
#if USE_MOTIF
    XtUnmanageChild(w);
#else /* USE_MOTIF */
    XtPopdown(p->mdialog);
#endif /* USE_MOTIF */
    p->DisplayScore(p->game);
}

void
Score::problem(void)
{
#if USE_MOTIF
    if (!mdialog_exist) {
        mdialog = XmCreateWarningDialog(toplevel, "mdialog", NULL, 0);
        XtAddCallback(mdialog, XmNokCallback,
                      (XtCallbackProc)retry_button,
                      (XtPointer)this);
        XtAddCallback(mdialog, XmNcancelCallback,
                      (XtCallbackProc)abandon_button,
                      (XtPointer)this);
        mdialog_exist = 1;
        XtUnmanageChild(XmMessageBoxGetChild(mdialog, XmDIALOG_HELP_BUTTON));
    }
    XtManageChild(mdialog);
#else /* USE_MOTIF */
    if (!mdialog_exist) {
        Widget form, label, b1, b2;
        Position x, y;
        XtVaGetValues(toplevel, XtNx, &x, XtNy, &y, NULL);
        mdialog = XtVaCreatePopupShell("mdialog_popup", transientShellWidgetClass,
                                       toplevel,
                                       XtNx, x+10,
                                       XtNy, y+10,
                                       NULL);
        form = XtVaCreateManagedWidget("mdialog", formWidgetClass, mdialog,
                                       NULL);
        label = XtVaCreateManagedWidget("label", labelWidgetClass, form,
                                        XtNresizable, True,
                                        NULL);
        b1 = XtVaCreateManagedWidget("ok_button", commandWidgetClass, form,
                                     XtNfromVert, label,
                                     NULL);
        b2 = XtVaCreateManagedWidget("cancel_button", commandWidgetClass, form,
                                     XtNfromVert,  label,
                                     XtNfromHoriz, b1,
                                     NULL);
        XtAddCallback(b1, XtNcallback,
                      (XtCallbackProc)retry_button,
                      (XtPointer)this);
        XtAddCallback(b2, XtNcallback,
                      (XtCallbackProc)abandon_button,
                      (XtPointer)this);
        mdialog_exist = 1;
    }
    XtPopup(mdialog, XtGrabNone);
#endif /* USE_MOTIF */
}

void
Score::readfile(void)
{
    FILE *fp = fopen(filename, "r");
    int offset;

    if (fp == NULL) {
        for(int i=0; i<SCORENUM; i++) {
            rec[i].SetDefault();
        }
        return;
    }
    if (fseek(fp, 1344*game+704, SEEK_SET) != 0)
        fseek(fp, 0, SEEK_END);
    offset = 1;
    for(int i=0; i<SCORENUM; i++){
        rec[i].ReadField(fp, kanjiCode, offset);
    }
    fclose(fp);
}

int
Score::writefile(void)
{
    static const char filler[] = "\032"
        "                                                               ";
    FILE *fp = fopen(filename, "r+");
    int offset;
    int i;

    if (fp == NULL) {
        problem();
        return 1; // fail!
    }
    offset = 1;
    fseek(fp, 1344*game, SEEK_SET);
    for(i=0; i<SCORENUM; i++) {
        rec[i].WriteField(fp, kanjiCode, offset);
    }
    fwrite((void*)filler, sizeof(filler)-1, 1, fp);
    for(i=0; i<SCORENUM; i++) {
        rec[i].WriteField(fp, kanjiCode, offset, 1);
    }
    fclose(fp);
    return 0; // no problem
}

Score::Score(Widget parent)
{
#if USE_MOTIF
    score = XmCreateMessageDialog(parent, "score", NULL, 0);
    XtAddCallback(score, XmNokCallback, (XtCallbackProc)PopDownCB, NULL);
    XtUnmanageChild(XmMessageBoxGetChild(score, XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(score, XmDIALOG_HELP_BUTTON));
#else /* USE_MOTIF */
    Widget form, label, button;
    score = XtVaCreatePopupShell("score", transientShellWidgetClass, parent,
                                 XtNallowShellResize, True,
                                 NULL);
    form = XtVaCreateManagedWidget("mf", formWidgetClass, score,
                                   NULL);
    label = XtVaCreateManagedWidget("label", labelWidgetClass, form,
                                    XtNresizable, True,
                                    XtNtop,       XawChainTop,
                                    XtNbottom,    XawChainBottom,
                                    XtNleft,      XawRubber,
                                    XtNright,     XawRubber,
                                    NULL);
    button = XtVaCreateManagedWidget("ok_button", commandWidgetClass, form,
                                     XtNfromVert, label,
                                     XtNtop,      XawChainTop,
                                     XtNbottom,   XawChainTop,
                                     XtNleft,     XawChainLeft,
                                     XtNright,    XawChainLeft,
                                     XtNlabel,    "OK",
                                     NULL);
    XtAddCallback(button, XtNcallback, (XtCallbackProc)PopDownCB, (XtPointer)score);
#endif /* USE_MOTIF */
    mdialog_exist = 0;
    first_call = 1;
}

void
Score::Popup(String str)
{
#if USE_MOTIF
    XmString mstr = XmStringCreateLtoR(str, XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues(score,
                  XmNmessageString, mstr,
                  NULL);
    XmStringFree(mstr);
    XtManageChild(score);
#else /* USE_MOTIF */
    XtVaSetValues(XtNameToWidget(score, "*label"),
                  XtNlabel, str,
                  NULL);
    XtPopup(score, XtGrabNone);
#endif /* USE_MOTIF */
}

void
Score::SetScoreFile(const char *scorefile, const char *kcode, const char *personal)
{
    char *home;

    filename = strdup(scorefile);
    home = getenv("HOME");
    if (home == NULL)
        home = ".";
    logfile = new char[strlen(home)+strlen(personal)+2];
    sprintf(logfile, "%s/%s", home, personal);

    if (strcasecmp(kcode, "euc") == 0)
        kanjiCode = 1;
    else if (strcasecmp(kcode, "jis") == 0)
        kanjiCode = 2;
    else
        kanjiCode = 0;
}

void
Score::do_first_call(void)
{
    XtVaGetApplicationResources(score, (XtPointer)res_strings,
                                resources, XtNumber(resources), NULL);
    first_call = 0;
#if !USE_MOTIF
    Position x, y;
    XtVaGetValues(toplevel, XtNx, &x, XtNy, &y, NULL);
    XtVaSetValues(score, XtNx, x+10, XtNy, y+10, NULL);
#endif /* !USE_MOTIF */
}

void
Score::DisplayScore(int kind_of_game)
{
    char format[SCORENUM*80+20], buff[SCORENUM*80];
    long prev_time, this_time;
    int  num;

    game = kind_of_game;
    if (first_call) {
        do_first_call();
    }
    // Always read the latest high score
    readfile();
    sprintf(format, res_strings[0]);
    prev_time = -1;
    for(int i=0; i<SCORENUM; i++) {
        this_time = rec[i].hour * 3600 + rec[i].min * 60 + rec[i].sec;
        if (this_time == prev_time)
            num++;
        else
            num = 0;
        prev_time = this_time;
        sprintf(buff, res_strings[1], i+1-num, rec[i].hour, rec[i].min, rec[i].sec,
                rec[i].name, rec[i].date, rec[i].time);
        strcat(format, buff);
    }
    Popup(format);
}

// Convert milisecond data into hour:minute:second
void
Score::ms_to_hms(int ms, unsigned char &h, unsigned char &m, unsigned char &s)
{
    h = ms/3600000;
    m = ms/60000 - h*60;
    s = (ms/1000)%60;
}

// Call Score::SetScore formerly!
void
Score::Register(void)
{
    unsigned char m, s, h;
    int    s1, i, mybest;
    int    inspos;
    time_t t;
    struct tm *tp;
    struct passwd *pw;
    char   namebuf[128], myname[NAMELEN+1], gecos[128], *po;

    s1 = scoreToRegister / 1000;
    ms_to_hms(scoreToRegister, h, m, s);
    pw = getpwuid(getuid());
    strcpy(gecos, pw->pw_gecos);
    if ((po = strchr(gecos, ',')) != NULL)
        *po = 0;
    sprintf(namebuf, "%-8.8s (%s)", pw->pw_name, gecos);
    sprintf(myname, "%-28.28s", namebuf);
    // Always read the latest high score
    readfile();

    // Get my best score in past
    mybest = SCORENUM;
#if !ALLOW_DUPSCORE
    for(i=0; i<SCORENUM; i++) {
        if (strcmp(myname, rec[i].name) == 0) {
            mybest = i;
            break;
        }
    }
#endif /* !ALLOW_DUPSCORE */

    // Check if this score values as high score
    inspos = SCORENUM;
    for(i=0; i<SCORENUM; i++) {
        int sx = rec[i].hour * 3600 + rec[i].min * 60 + rec[i].sec;
        if (s1 < sx) {
            inspos = i;
            break;
        }
    }
    if (inspos == SCORENUM || mybest < inspos) {
        return;  // Not high score!
    }

    for(i=mybest; i>inspos; i--) {
        if (i>=SCORENUM) continue;
        rec[i] = rec[i-1];
    }

    rec[inspos].hour = h;
    rec[inspos].min  = m;
    rec[inspos].sec  = s;
    strcpy(rec[inspos].name, myname);
    time(&t);
    tp = localtime(&t);
    sprintf(rec[i].date, "%2.2d-%2.2d-%2.2d", tp->tm_year%100, tp->tm_mon+1, tp->tm_mday);
    sprintf(rec[i].time, "%2.2d:%2.2d:%2.2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
    if (writefile() == 0)
        DisplayScore(game);
}

void
Score::LogRecord(int flag, int result, int gamesize, int level, int rest)
// flag: 0=giveup 1=finished 2=finish+help 3=tedumari
{
    FILE       *stream;
    time_t     t;
    struct tm  *tp;
    int        check_digit;

    stream = fopen(logfile, "a");
    if (stream == NULL)
        return; /* Just do not record, no notifying */
    time(&t);
    tp = localtime(&t);
    check_digit = flag*3 + gamesize + level*7 + (tp->tm_year%100)*3 + rest*7
        + tp->tm_mon*7 + tp->tm_mday*3 + tp->tm_hour*7 + tp->tm_min*3 + tp->tm_sec;
    check_digit %= 857;
    fprintf(stream, "%2.2d%2.2d%8.8d%2.2d%1.1d%2.2d%4.4d%2.2d%3.3d%3.3d%2.2d%2.2d\n",
            flag,
            tp->tm_sec, result, tp->tm_min, gamesize, tp->tm_year%100, level, tp->tm_mon,
            check_digit, rest, tp->tm_mday, tp->tm_hour);
    fclose(stream);
}

int
Score::PersonalStat(int kind_of_game)
{
    FILE *s;
    char buf[4096], *bufp, xb[128];
    char scorebuf[SCORENUM*2+1][128];
    int  sec, min, hour, day, month, year;
    int  g, f, c1, c2, l, t;
    int  rest;
    int  w1[2][4], w2[2][4], w3[2][4]; // for average
    int  i;
    struct tm tb;
    unsigned char t1, t2, t3;
    time_t    tm1, tm2;
    struct passwd *pw;

    game = kind_of_game;
    time(&tm2);  // Time of now
    if (first_call) {
        do_first_call();
    }
    tm2 -= 86400 * atoi(res_strings[3]);
    s = fopen(logfile, "r");
    if (s == NULL)
        return -1;
    for(i=0; i<SCORENUM*2; i++)
        strcpy(scorebuf[i], "99:99:99 (  0) [00-00-00 00:00:00]");
    memset(w1, 0, 2*4*sizeof(int));
    memset(w2, 0, 2*4*sizeof(int));
    memset(w3, 0, 2*4*sizeof(int));
    while(fgets(buf, 128, s) != NULL) {
        switch(strlen(buf)) {
        case 31:
            // Just for compatibility with xshisen 1.10
            f     = atoiSubstring(buf +  0, 2);
            sec   = atoiSubstring(buf +  2, 2);
            t     = atoiSubstring(buf +  4, 8);
            min   = atoiSubstring(buf + 12, 2);
            g     = atoiSubstring(buf + 14, 1);
            year  = atoiSubstring(buf + 15, 2);
            l     = atoiSubstring(buf + 17, 4);
            month = atoiSubstring(buf + 21, 2);
            c1    = atoiSubstring(buf + 23, 3);
            day   = atoiSubstring(buf + 26, 2);
            hour  = atoiSubstring(buf + 28, 2);
            rest = -1;
            c2 = f*3 + g + l*7 + year*3 + month*7 + day*3 + hour*7 + min*3 + sec;
            break;
        case 32:
            // Just for compatibility with xshisen 1.10, Y2K
            f     = atoiSubstring(buf +  0, 2);
            sec   = atoiSubstring(buf +  2, 2);
            t     = atoiSubstring(buf +  4, 8);
            min   = atoiSubstring(buf + 12, 2);
            g     = atoiSubstring(buf + 14, 1);
            year  = atoiSubstring(buf + 15, 3) - 100;
            l     = atoiSubstring(buf + 18, 4);
            month = atoiSubstring(buf + 22, 2);
            c1    = atoiSubstring(buf + 24, 3);
            day   = atoiSubstring(buf + 27, 2);
            hour  = atoiSubstring(buf + 29, 2);
            rest = -1;
            c2 = f*3 + g + l*7 + (year+100)*3 + month*7 + day*3 + hour*7 + min*3 + sec
;
            break;
         case 34:
            // This is usual format
            f     = atoiSubstring(buf +  0, 2);
            sec   = atoiSubstring(buf +  2, 2);
            t     = atoiSubstring(buf +  4, 8);
            min   = atoiSubstring(buf + 12, 2);
            g     = atoiSubstring(buf + 14, 1);
            year  = atoiSubstring(buf + 15, 2);
            l     = atoiSubstring(buf + 17, 4);
            month = atoiSubstring(buf + 21, 2);
            c1    = atoiSubstring(buf + 23, 3);
            rest  = atoiSubstring(buf + 26, 3);
            day   = atoiSubstring(buf + 29, 2);
            hour  = atoiSubstring(buf + 31, 2);
            c2 = f*3 + g + l*7 + year*3 + month*7 + day*3 + hour*7 + min*3
                + sec + rest*7;
            break;
         case 35: 
            // This is usual format, Y2K
            f     = atoiSubstring(buf +  0, 2);
            sec   = atoiSubstring(buf +  2, 2);
            t     = atoiSubstring(buf +  4, 8);
            min   = atoiSubstring(buf + 12, 2);
            g     = atoiSubstring(buf + 14, 1);
            year  = atoiSubstring(buf + 15, 3) - 100;
            l     = atoiSubstring(buf + 18, 4);
            month = atoiSubstring(buf + 22, 2);
            c1    = atoiSubstring(buf + 24, 3);
            rest  = atoiSubstring(buf + 27, 3);
            day   = atoiSubstring(buf + 30, 2);
            hour  = atoiSubstring(buf + 32, 2);
            c2 = f*3 + g + l*7 + (year+100)*3 + month*7 + day*3 + hour*7 + min*3
                + sec + rest*7;
            break;
        default:
            // Someone edited this file?
            continue;
        }
        c2 %= 857;
        if (c1 != c2 || g != game)
            continue; // Check digit was incorrect, or other game size
        if (f < 0 || f > 3)
            continue; // Invalid value
        l /= 2;
        ms_to_hms(t, t1, t2, t3);
        if (f == 1) {  // Only completed game can be included
            sprintf(scorebuf[SCORENUM*2],
                    "%2.2d:%2.2d:%2.2d (%3d) [%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d]",
                    t1, t2, t3, l, year, month+1, day, hour, min, sec);
            for(i=SCORENUM*2; i>0 && strcmp(scorebuf[i-1],scorebuf[i])>0; i--) {
                char p[128];
                strcpy(p, scorebuf[i-1]);
                strcpy(scorebuf[i-1], scorebuf[i]);
                strcpy(scorebuf[i], p);
            }
        }
        tb.tm_isdst = 0;
        tb.tm_sec   = sec;
        tb.tm_min   = min;
        tb.tm_hour  = hour;
        tb.tm_mday  = day;
        tb.tm_mon   = month;
        tb.tm_year  = year > 69 ? year : year + 100;
        tm1 = mktime(&tb);  // Time of the score
        w1[0][f] += t;
        w2[0][f]++;
        w3[0][f] += l;
        if (tm1 >= tm2) { // if the time of the score is newer than specified days ago
            w1[1][f] += t;
            w2[1][f]++;
            w3[1][f] += l;
        }
    }
    fclose(s);
    bufp = buf;
    *bufp = '\0';
    pw = getpwuid(getuid());
    sprintf(bufp, res_strings[12], pw->pw_name);
    bufp += strlen(bufp);
    for(i=0; i<SCORENUM; i++) {
        sprintf(bufp, "%2d ", i+1);
        bufp += 3;
        strcpy(bufp, scorebuf[i]);
        bufp += strlen(bufp);
        sprintf(bufp, "    %2d ", SCORENUM+i+1);
        bufp += 7;
        strcpy(bufp, scorebuf[SCORENUM+i]);
        bufp += strlen(bufp);
        *bufp = '\n';
        bufp++;
    }
    *bufp = '\n'; bufp++;
    *bufp = '\n'; bufp++;
    sprintf(xb, res_strings[2], atoi(res_strings[3]));
    strcpy(bufp, makeStatistics(xb, w1[1], w2[1], w3[1]));
    bufp += strlen(bufp);
    strcpy(bufp, makeStatistics(res_strings[9], w1[0], w2[0], w3[0]));
    bufp += strlen(bufp);
    *bufp = '\0';
    Popup(buf);
}

int
Score::atoiSubstring(const char *str, int length)
{
    char b[16];
    strncpy(b, str, length);
    *(b+length) = '\0';
    return atoi(b);
}

char
*Score::makeStatistics(const char *title, int rtime[], int rcount[], int rlev[])
{
    int x;
    double y;
    unsigned char t1, t2, t3;
    static char b[1024], *bp;

    bp = b;
    x = rcount[0] + rcount[1] + rcount[2] + rcount[3];
    sprintf(bp, "%-20.20s: %6d %s\n", title, x, res_strings[11]);
    bp += strlen(bp);
    if (rcount[1]) {
        x = rtime[1] / rcount[1];
        y = (double)rlev[1] / (double)rcount[1];
        ms_to_hms(x, t1, t2, t3);
        bp += strlen(bp);
        sprintf(bp, "    %-14.14s: %2.2d:%2.2d:%2.2d (%4.1f) %6d %s\n",
                res_strings[5], t1, t2, t3, y, rcount[1], res_strings[11]);
        bp += strlen(bp);
    }
    if (rcount[3]) {
        x = rtime[3] / rcount[3];
        y = (double)rlev[3] / (double)rcount[3];
        ms_to_hms(x, t1, t2, t3);
        bp += strlen(bp);
        sprintf(bp, "    %-14.14s: %2.2d:%2.2d:%2.2d (%4.1f) %6d %s\n",
                res_strings[6], t1, t2, t3, y, rcount[3], res_strings[11]);
        bp += strlen(bp);
    }
    if (rcount[2]) {
        x = rtime[2] / rcount[2];
        y = (double)rlev[2] / (double)rcount[2];
        ms_to_hms(x, t1, t2, t3);
        bp += strlen(bp);
        sprintf(bp, "    %-14.14s: %2.2d:%2.2d:%2.2d (%4.1f) %6d %s\n",
                res_strings[7], t1, t2, t3, y, rcount[2], res_strings[11]);
        bp += strlen(bp);
    }
    if (rcount[0]) {
        x = rtime[0] / rcount[0];
        y = (double)rlev[0] / (double)rcount[0];
        ms_to_hms(x, t1, t2, t3);
        bp += strlen(bp);
        sprintf(bp, "    %-14.14s: %2.2d:%2.2d:%2.2d (%4.1f) %6d %s\n",
                res_strings[8], t1, t2, t3, y, rcount[0], res_strings[11]);
        bp += strlen(bp);
    }
    *bp = '\0';
    return b;
}
