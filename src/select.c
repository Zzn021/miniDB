// select.c ... select scan functions
// Manage creating and using Selection objects

#include "defs.h"
#include "select.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"
#include "hash.h"
#include "util.h"

struct SelectionRep {
    // Info about rel
	Reln    rel;            // need to remember Relation info
	Bits    known;          // the hash value from MAH
	Bits    unknown;        // the unknown bits from MAH
    // Info about page
	Page    curpage;        // current page in scan
                            // Need to be freed
    PageID  curpageID;      // current page id
	int     is_ovflow;      // are we in the overflow pages?
    // For get nextTuple
	Offset  curtupOffset;   // offset of current tuple within page
    Bits    *buckets;       // All the pages we need to go through.
                            // Need to be freed
    int     bucketIndex;    // the current bucket index [0..nBuckets-1]
    int     nBuckets;       // The size of the pages
    int     count;          // The tuples being read       
    // Pattern
    Tuple   pattern;        // The pattern to match
                            // Need to be freed
};

// Helpers
int hasValue(char *str);
Bits *computePage(Bits known, Bits unknown, int nStars, Bits lowerBits, int *nBuckets, Reln r);
Bits getMaskNumber(Bits page, Reln r);
void getNextPage(Selection q);

Selection startSelection(Reln r, char *q)
{
    Selection new = malloc(sizeof(struct SelectionRep));
    assert(new != NULL);

    char *matchTuple = copyString(q);
    Tuple tuple = (Tuple) matchTuple;

    char **values = splitTuple(q, nattrs(r));

    Bits knownMask = 0;
    Bits unknownMask = 0;

    ChVecItem *cvs = chvec(r);
    int nStars = 0;
    for (int i = 0; i < 32; i++) {
        int attrNum = cvs[i].att;
        int bitPos = cvs[i].bit;

        if (hasValue(values[attrNum])) {
            Bits hash = hash_any((unsigned char *)values[attrNum],strlen(values[attrNum])); 
            if (bitIsSet(hash, bitPos)) {
                knownMask = setBit(knownMask, i);
            } else {
                knownMask = unsetBit(knownMask, i);
            }
        } else {
            unknownMask = setBit(unknownMask, i);
            nStars++;
        }
    }

    // The max lower bits we need
    Bits lowStars = getMaskNumber(unknownMask, r);

    // Compute page
    int nBuckets;
    Bits *buckets = computePage(knownMask, unknownMask, nStars, lowStars, &nBuckets, r);

    // Get the current page;
    PageID pid = getMaskNumber(buckets[0], r);
    Page cur = getPage(dataFile(r), pid);
    
    int index = 0;
    
    free(values);
    
    // Set all values
    new->rel = r;
    new->known = knownMask;
    new->unknown = unknownMask;
    
    new->curpage = cur;
    new->curpageID = pid;
    new->is_ovflow = 0;
    
    new->curtupOffset = 0;
    new->buckets = buckets;
    new->bucketIndex = index;
    new->nBuckets = nBuckets;
    new->count = 0;
    
    new->pattern = tuple;
    
    return new;
}

// get next tuple during a scan

Tuple getNextTuple(Selection q)
{
    // END OF ALL
    if (q->curpage == NULL) {
        return NULL;
    }
    
    // Current Page
    Page p = q->curpage;
    // Starting position
    char *base = pageData(p);
    
    int free = pageFreeSpace(p);
    int pageSize = PAGESIZE - free;
    int nTuple = pageNTuples(p);
    
    while (q->count < nTuple && q->curtupOffset < pageSize) {
        Tuple t = (Tuple)(base + q->curtupOffset);
        int len = tupLength(t);
        
        char  buf[MAXTUPLEN];
        tupleString(t, buf);
        
        q->curtupOffset += len + 1;
        q->count++;
        
        // Match
        if (tupleMatch(q->rel, t, q->pattern)) {
            return t;
        }
    }
    
    getNextPage(q);
    return getNextTuple(q);
}

// clean up a SelectionRep object and associated data

void closeSelection(Selection q)
{
    free(q->curpage);
    free(q->pattern);
    free(q->buckets);
    free(q);
}

// Check if an vi is known or unknown
int hasValue(char *str) {
    if (strcmp(str, "?") == 0) {
        return 0;
    } else if (strstr(str, "%") != NULL) {
        return 0;
    } else {
        return 1;
    }
}

// Compute pages
Bits *computePage(Bits known, Bits unknown, int nStars, Bits lowerBits, int *nBuckets, Reln r) {
    // The wild card position       
    int starPos[32];
    int k = 0;
    for (int i = 0; i < 32; i++) {
        if (lowerBits & (1u << i)) {
            starPos[k] = i;
            k++;
        }
    }

    // Total size
    int size = 1 << k;
    if (size >= npages(r)) {
        size = npages(r);
    }
    *nBuckets = size;

    Bits *pages = malloc(sizeof(Bits) * size);
    assert(pages != NULL);

    for (int mask = 0; mask < size; ++mask) {
        Bits value =known;
        for (int j = 0; j < k; ++j) {
            Bits bitMask = 1u << starPos[j];
            if (mask & (1 << j)) {
                value |= bitMask;
            } else {
                value &= ~bitMask;
            }
        }
        pages[mask] = value;
    }

    char buf1[MAXBITS+5];
    char buf2[MAXBITS+5];
    bitsString(unknown, buf1);
    bitsString(known, buf2);
    
    for (int i=0;i<size;i++) {
        char t[MAXBITS+5];
        bitsString(pages[i], t);
    }

    return pages;
    // return NULL;
}

Bits getMaskNumber(Bits page, Reln r) {
    int lowerBits = (splitp(r) == 0) ? depth(r) : depth(r) + 1;
    Bits lowMask = (lowerBits == 32) ? ~0u : ((1u << lowerBits) - 1);
    Bits number = page & lowMask;
    return number;
}

void getNextPage(Selection q) {
    // If current page has overflow go to overflow
    // Else go to next bucket

    Reln r = q->rel;
    Page old = q->curpage;
    
    PageID ovID = pageOvflow(old);
    // free(old);
    if (ovID != NO_PAGE) {
        
        q->curpage = getPage(ovflowFile(r), ovID);

        q->curtupOffset = 0;
        q->curpageID = ovID;
        q->is_ovflow = 1;
        q->count = 0;

        return;
    }

    // Go to the next bucket
    q->is_ovflow = 0;
    q->bucketIndex++;
    if (q->bucketIndex >= q->nBuckets) {
        // End of buckets
        q->curpage = NULL;
        return;
    }
    
    PageID pid = getMaskNumber(q->buckets[q->bucketIndex], r);
    if (pid >= npages(r)) {
        pid =  pid % npages(r);
    }
    
    q->curpage = getPage(dataFile(r), pid);
    q->curpageID = pid;
    q->curtupOffset = 0;
    q->count = 0;
}