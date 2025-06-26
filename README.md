# miniDB

---

### Introduction
A simple implementation of a linear-hashed file structure that uses multi-attribute hashing

#### Create

Creates MALH (multi-attribute linear-hashed) files by accepting 4 command line arguments:
- the name of the relation
- the number of attributes
- the initial number of data pages (rounded up to nearest 2n)
- the multi-attribute hashing choice vector

Example:
```shell
$ ./create R 4 6 "0,0:0,1:1,0:1,1:2,0:3,0"
```
makes a table called abc with 4 attributes and 8 inital data pages (6 will be rounded up to the nearest 2n)

The choice vector (4th argument):
- bit 0 from attribute 0 produces bit 0 of the MA hash value
- bit 1 from attribute 0 produces bit 1 of the MA hash value
- bit 0 from attribute 1 produces bit 2 of the MA hash value
- bit 1 from attribute 1 produces bit 3 of the MA hash value
- bit 0 from attribute 2 produces bit 4 of the MA hash value
- bit 0 from attribute 3 produces bit 5 of the MA hash value

#### Insert

Reads tuples, one per line, from std input and inserts them into the relation.

The bucket where the tuple is stored is determined by the appropriate number of bits of the combined hash value. If the relation has $2^d$ data pages, then $d$ bits are used. If the specified data page is full, then the tuple is inserted into an overflow page of that data page.

Example:

```shell
$ echo "100,xyz,abc" | ./insert R
# prints the hash value for the tuple
```

#### Query

Run selection and projection queries over a given relation. It supports wildcard and pattern matching, finds all tuples in either the data pages or overflow pages that match the query, as well as flexible attribute projection without **distinct**

Example:

```shell
$ ./query [-v] 'a1,a2,...' from RelName where 'v1,v2,...'
```
- **'a1,a2,...' (or '\*')**: a sequence of 1-based attribute indexes used for projection, can be '\*' to indicate all attributes. The minimal 'a' value is '0'
- **'v1,v2,...'**: a sequence of attribute values used for selection

Each $v_i$ in the selection tuple can be:
- **Literal value**: A specific value that must match exactly in the corresponding attribute position. (e.g., 'xyz' matches 'xyz', '64' matches '64')
- **Single question mark '?'**: Matches any literal value in the corresponding attribute position. (e.g., '?' matches 'xyz', '?' matches '64')
- **Pattern string containing '%'**: A string that includes one or more'%', where each '%' matches zero or more characters.

```shell
$ ./query '*' from R where '?,?,?'
# matches any tuple in R

$ ./query '3, 1' from R where '10,?,?'
# projects attribute 1 and 3 in order from tuples where attribute 0 is 10

$ ./query '*' from R where '?,%xz%,?'
# matches any tuple where attribute 1 contains 'xz'
```

#### Status

Check the status of the files for table R with stats command:
```shell
$ ./stats R
# prints out the stats of R
```

#### dump

Show tuples, bucket-by-bucket
```shell
$ ./dump [RelName]
```
#### gendata

Use `gendata` to generate large numbers of tuples, and insert them as:

```shell
$ ./gendata 800 3 100 | ./insert R
```

Insert 800 tuples into the table, with ID values starting at 100.

#### clean

Remove `Rel.data` `Rel.info` `Rel.ovflow`. If no argument provided, remove every existing relation.


```shell
$ ./clean [RelName]
```

---

### Setting Up

To build the executables from the code:
```shell
$ cd src
$ make
```

The following executables should be generated in `/src`:
- `create`
- `dump`
- `insert`
- `query`
- `stats`

---

### Troubleshooting
- Check `make` file
- Check `gcc` version