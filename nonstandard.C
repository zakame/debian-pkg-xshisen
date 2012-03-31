#include "components.h"

#ifndef HAVE_STRDUP
char *strdup(const char *s)
{
    char *p = malloc((strlen(s)+1)*sizeof(char));
    if (p)
        strcpy(p, s);
    return p;
}
#endif /* HAVE_STRDUP */

#ifndef HAVE_STRSTR
char *strstr(const char *s1, const char *s2)
{
    char *p;
    int  l1 = strlen(s1);
    int  l2 = strlen(s2);
    if (l1 < l2)
        return NULL;
    for(p = s1; p <= s1+l1-l2; p++)
        if (strncmp(p, s2, l2) == 0)
            return p;
    return NULL;
}
#endif /* HAVE_STRSTR */

#ifndef HAVE_STRNCASECMP
# if STDC_HEADERS
#  define Toupper(c) (toupper(c))
# else /* STDC_HEADERS */
#  define Toupper(c) ((c)&50)
# endif /* STDC_HEADERS */
int strncasecmp(const char *s1, const char *s2, size_t n)
{
    register int i;
    for(i=0; i<n; i++) {
        register char p1 = toupper(s1[i]), p2 = toupper(s2[i]);
        if (p1 == '\0') {
            if (p2 == '\0')
                return 0;
            else
                return 1;
        } else if (p2 == '\0') {
            return -1;
        } else if (p1 < p2) {
            return 1;
        } else if (p1 > p2) {
            return -1;
        }
    }
    return 0;
}
#endif /* HAVE_STRNCASECMP */

#ifndef HAVE_STRCASECMP
#ifndef INT_MAX
# define INT_MAX (sizeof(int)==4 ? 2147483647 : 32767)
#endif
int strcasecmp(const char *s1, const char *s2)
{
    return strncasecmp(s1, s2, INT_MAX);
}
#endif /* HAVE_STRCASECMP */
