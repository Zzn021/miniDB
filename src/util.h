// util.h ... interface to various useful functions
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...

#ifndef UTIL_H
#define UTIL_H 1

void fatal(char *);
char *copyString(char *);
char *trim(char *s);
char **splitTuple(char *str, int len);
int patternMatch(char *p, char *t);
int convert(char *s, int *out);

#endif
