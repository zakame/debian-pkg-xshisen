#include "body.h"

void
InitGlobalMahjong(Widget w, const char *directory, const char *subdir)
{
  static const char *files[PKIND] = {
    "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8", "p9",
    "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9",
    "m1", "m2", "m3", "m4", "m5", "m6", "m7", "m8", "m9",
    "ton", "nan", "sha", "pei", "haku", "hatsu", "chun",
    "hana", "hishi" };
  char buffer[1024];

  for(int i=0; i<PKIND; i++) {
    sprintf(buffer, "%s/%s/%s.xpm", directory, subdir, files[i]);
    Mp[i].ReadFile(w, buffer, i, globRes.colorCloseness);
  }
}
