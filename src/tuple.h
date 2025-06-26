// tuple.h ... interface to functions on Tuples
// A Tuple is just a '\0'-terminated C string
// Consists of "val_1,val_2,val_3,...,val_n"

#ifndef TUPLE_H
#define TUPLE_H 1

typedef char *Tuple;

#include "reln.h"
#include "bits.h"

int tupLength(Tuple t);
Tuple readTuple(Reln r, FILE *in);
Bits tupleHash(Reln r, Tuple t);
void tupleVals(Tuple t, char **vals);
void freeVals(char **vals, int nattrs);
Bool tupleMatch(Reln r, Tuple pt, Tuple t);
void tupleString(Tuple t, char *buf);
void freeTuple(Tuple t); //** release memory used for tuple

#endif
