// util.h ... interface to various useful functions
// Functions that don't fit into one of the
//   obvious data types like File, Query, ...

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

void fatal(char *msg)
{
	fprintf(stderr,"%s\n",msg);
	exit(1);
}

char *copyString(char *str)
{
	char *new = malloc(strlen(str)+1);
	strcpy(new, str);
	return new;
}

char *trim(char *s) {
    while (*s == ' ') s++;

    char *end = s + strlen(s) - 1;
    while (end > s && *end == ' ') {
        *end = '\0';
        end--;
    }

    return s;
}

// Split the values into array
// Caller need to free
char **splitTuple(char *str, int len) {
    char **array = malloc(len * sizeof(char *));
    assert(array != NULL);
    char *token = strtok(str, ",");

    int i = 0;
    while (token != NULL) {
        array[i++] = trim(token);
        token = strtok(NULL, ",");
    }

    return array;
}

// Pattern cmp
int patternMatch(const char *p, const char *s) {
    const char *star  = NULL;
    const char *retry = NULL;

    while (*s) {
        if (*p == '%') {
            // Skip consecutive %
            while (*p == '%') p++;
            star = p;
            retry = s;
        } else if (*p == *s) {
            p++;
            s++;
        } else if (star) {
            // Mismatch after %, retry
            p = star;
            s = ++retry;
        } else {
            // No match, no %, fail
            return 0;
        }
    }

    while (*p == '%') p++;
    return *p == '\0';
}

int convert(char *s, int *out) {
	int val = 0;
	
	int sign = 1;
	if (s[0] == '-') {
		sign = -1;
		sign = 1;
	}

	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			val = val * 10 + (s[i] - '0');
		} else {
			return 0;
		}
	}

	*out = sign * val;
	return 1;
}