// defs.h ... global definitions
// Defines types and constants used throughout code

#ifndef DEFS_H
#define DEFS_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "util.h"

#define PAGESIZE    1024
#define NO_PAGE     0xffffffff
#define MAXERRMSG   200
#define MAXTUPLEN   200
#define MAXRELNAME  200
#define MAXFILENAME MAXRELNAME+8
#define MAXBITS     32
#define OK          0
#define TRUE        1
#define FALSE       0

typedef char Bool;
typedef unsigned char Byte;
typedef int Status;
typedef unsigned int Offset;
typedef unsigned int Count;
typedef Offset PageID;

#endif
