#!/usr/bin/env python
import sys

def top_ten_tags(tags):
    taglist = tags.items()
    taglist.sort(lambda a,b: -1*cmp(a[1],b[1]))
    outstr = "" 
    for i in range(10):
        outstr += "%s:%d, " % (taglist[i][0],taglist[i][1])

    return outstr

class fastq:
    def __init__(self, infile):
        self.identifier = infile.readline().strip()
        self.basecall = infile.readline().strip()
        self.plus_sign = infile.readline().strip()
        self.quality = infile.readline().strip()

    def __str__(self):
        return "\n".join([ self.identifier, self.basecall, self.plus_sign, self.quality])

tags = {}

import gzip

if sys.argv[1].endswith('.gz'):
    infile = gzip.open(sys.argv[1], "r")
    print >>sys.stderr, "opening gzip format"
else:
    infile = file(sys.argv[1])

i = 0
while True:
    try:
        seq = fastq(infile)
        i += 1
        try:
            a,b = seq.identifier.split()
        except:
            print >>sys.stderr, "sequence (%d) problem with splitting `%s' into two" % (i, seq.identifier)
            continue
        parts = b.split(":")
        tag = parts[-1]
        if tag not in tags: tags[tag] = 0
        tags[tag] += 1
        if i % 10000 == 0:
            print >>sys.stderr, "\r", top_ten_tags(tags),

    except EOFError:
        break
print >>sys.stderr
print  >>sys.stderr, tags
