CC=gcc
CFLAGS=-Wall -Werror -g -std=c99
LDLIBS = -lm

OBJS=select.o project.o page.o reln.o tuple.o util.o chvec.o hash.o bits.o
BINS=create dump insert query stats gendata

all : $(BINS)

create: create.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

dump: dump.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

insert: insert.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

query: query.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

stats: stats.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

gendata: gendata.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

create.o: create.c defs.h
dump.o: dump.c defs.h reln.h page.h
insert.o: insert.c defs.h reln.h tuple.h
query.o: query.c defs.h select.h project.h tuple.h reln.h chvec.h hash.h bits.h
stats.o: stats.c defs.h reln.h
gendata.o: gendata.c defs.h

bits.o: bits.c bits.h
chvec.o: chvec.c defs.h chvec.h reln.h
hash.o: hash.c defs.h hash.h bits.h
page.o: page.c defs.h bits.h
select.o: select.c defs.h select.h reln.h tuple.h bits.h hash.h
project.o: project.c defs.h project.h reln.h tuple.h util.h
reln.o: reln.c defs.h reln.h page.h tuple.h chvec.h hash.h bits.h
tuple.o: tuple.c defs.h tuple.h reln.h chvec.h hash.h bits.h util.h
util.o: util.c

defs.h: util.h

db:
	rm -f R.*
	./create R 3 5 ""
	./gendata 1000 3 1234 | ./insert R

clean:
	rm -f $(BINS) *.o
