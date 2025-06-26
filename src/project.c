// project.c ... project scan functions
// Manage creating and using Projection objects

#include <stdbool.h>
#include "defs.h"
#include "project.h"
#include "reln.h"
#include "tuple.h"
#include "util.h"



struct ProjectionRep {
	Reln    rel;       // Relation info
    int     *order;    // The order to print the attribute
                       // Need to be freed
    int     nAttr;     // Number of Attributes need to be projected;
    int     totalAttr; // Number of Attribute in the Relation;
};

// take a string of 1-based attribute indexes (e.g. "1,3,4")
// set up a ProjectionRep object for the Projection
Projection startProjection(Reln r, char *attrstr)
{
    Projection new = malloc(sizeof(struct ProjectionRep));
    assert(new != NULL);
    
    // Split the attrstr:
    int nAttrs = nattrs(r);
    new->totalAttr = nAttrs;

    int *order = malloc(sizeof(int) * nAttrs);

    // Print all attributes
    if (strcmp(attrstr, "*") == 0) {
        for (int i = 0; i < nAttrs; i++) {
            order[i] = i;
        }
        new->nAttr = nAttrs;

    // Print selected attributes        
    } else {
        int i = 0;
        char *each = strtok(attrstr, ",");
        while (each != NULL) {
            each = trim(each);
            int val;
            if (convert(each, &val)) {
                if (val > 0 && val <= nAttrs) {
                    order[i] = val - 1;
                    i++;
                } else {
                    fatal("Invalid attrstr for projection\n");
                }
            }
            each = strtok(NULL, ",");
        }
        if (i > nAttrs) {
            fatal("Invalid number of attrstr for projection\n");
        }

        new->nAttr = i;
    }
    new->order = order;
    
    return new;
}

void projectTuple(Projection p, Tuple t, char *buf)
{
    char **tupleVal = malloc(p->totalAttr * sizeof(char *));
    tupleVals(t, tupleVal);

    char *out = buf;
    for (int i = 0; i < p->nAttr; i++) {
        const char *s = tupleVal[p->order[i]];
        size_t len = strlen(s);

        memcpy(out, s, len);
        out += len;

        if (i < p->nAttr - 1) {
            *out++ = ',';
        }
    }
    *out = '\0';

    free(tupleVal);
}

void closeProjection(Projection p)
{
    free(p);
}
