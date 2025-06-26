// tuple.c ... functions on tuples

#include "defs.h"
#include "tuple.h"
#include "reln.h"
#include "hash.h"
#include "chvec.h"
#include "bits.h"
#include "util.h"

// return number of bytes/chars in a tuple

int tupLength(Tuple t)
{
	return strlen(t);
}

// reads/parses next tuple in input

Tuple readTuple(Reln r, FILE *in)
{
	char line[MAXTUPLEN];
	if (fgets(line, MAXTUPLEN-1, in) == NULL)
		return NULL;
	line[strlen(line)-1] = '\0';
	char *c; int nf = 1;
	for (c = line; *c != '\0'; c++)
		if (*c == ',') nf++;
	// invalid tuple
	if (nf != nattrs(r)) return NULL;
	return copyString(line); // needs to be free'd sometime
}

// extract values into an array of strings

void tupleVals(Tuple t, char **vals)
{
	char *c = t, *c0 = t;
	int i = 0;
	for (;;) {
		while (*c != ',' && *c != '\0') c++;
		if (*c == '\0') {
			// end of tuple; add last field to vals
			vals[i++] = copyString(c0);
			break;
		}
		else {
			// end of next field; add to vals
			*c = '\0';
			vals[i++] = copyString(c0);
			*c = ',';
			c++; c0 = c;
		}
	}
}

// release memory used for separate attribute values

void freeVals(char **vals, int nattrs)
{
	int i;
    // release memory used for each attribute
	for (i = 0; i < nattrs; i++) free(vals[i]);
    // release memory used for pointer array
    free(vals);
}

// hash a tuple using the choice vector

Bits tupleHash(Reln r, Tuple t)
{
	// printChVec(cvs); // for debug
	char buf[MAXBITS+5];  //*** for debug

	// Combined Bit
	Bits combined = 0;

	// Choice vectors
	ChVecItem *cvs = chvec(r);

	// Number of attributes
	Count nvals = nattrs(r);
    char **vals = malloc(nvals*sizeof(char *));
    assert(vals != NULL);

	// Parse the tuple into its individual attribute strings.
    tupleVals(t, vals);

	// Take an attribute and its length to produce a 32-bit hash
	// Hash all attributes and store in the array
	Bits attrHashs[nvals];
	for (int i = 0; i < nvals; i++) {
		attrHashs[i] = hash_any((unsigned char *)vals[i],strlen(vals[i]));
	}

	// Loop cvs and fill the combined hash
	for (int i = 0; i < 32; i++) {
		int attrNum = cvs[i].att;
		int bitPos = cvs[i].bit;

		// printf("attrNum: %d, bitPos: %d", attrNum, bitPos); // Debug
		// Hash
		Bits hash = attrHashs[attrNum];
		// Set on the combined hash
		if (bitIsSet(hash, bitPos)) {
			combined = setBit(combined, i);
		} else {
			combined = unsetBit(combined, i);
		}
	}

	bitsString(combined,buf);  //*** for debug
	// printf("hash(%s) = %s\n", vals[0], buf);  //*** for debug

    freeVals(vals,nvals);
	return combined;
}

// compare two tuples (allowing for "unknown" values)
Bool tupleMatch(Reln r, Tuple pt, Tuple t)
{
	Count na = nattrs(r);

	char **pv = malloc(na*sizeof(char *));
	assert(pv != NULL);
	tupleVals(pt, pv);

	char **tv = malloc(na*sizeof(char *));
	assert(tv != NULL);
	tupleVals(t, tv);

	Bool match = TRUE;
	for (int i = 0; i < na; i++) {
		char *pat = trim(tv[i]);
		// Wld Card
		if (strcmp(pat, "?") == 0) {
			continue;
		}
		// Exact Match
		if (strcmp(pv[i], pat) == 0) {
			continue;
		}
		// Pattern
		if (patternMatch(pat, pv[i]) == 1) {
			continue;
		}
		match = FALSE;
		break;
	}

	freeVals(pv,na); 
	freeVals(tv,na);
	return match;
}

// puts printable version of tuple in user-supplied buffer

void tupleString(Tuple t, char *buf)
{
	strcpy(buf,t);
}

// release memory used for tuple
void freeTuple(Tuple t)
{
    if (t != NULL) {
        free(t);
    }
}
