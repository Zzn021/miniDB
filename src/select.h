// select.h ... interface to query scan functions

#ifndef SELECTION_H
#define SELECTION_H 1

typedef struct SelectionRep *Selection;

#include "reln.h"
#include "tuple.h"

Selection startSelection(Reln, char *);
Tuple getNextTuple(Selection);
void closeSelection(Selection);

#endif
