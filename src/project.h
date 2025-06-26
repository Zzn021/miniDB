// project.h ... interface to query scan functions
// See project.c for details of Projection type and functions

#ifndef PROJECTION_H
#define PROJECTION_H 1

typedef struct ProjectionRep *Projection;

#include "reln.h"
#include "tuple.h"

Projection startProjection(Reln r, char *attrstr);
void projectTuple(Projection p, Tuple t, char *buf);
void closeProjection(Projection p);

#endif
