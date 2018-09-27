#!/usr/bin/env python
import sys

class fastq:
    def __init__(self, infile):
        self.identifier = infile.readline().strip()
        self.basecall = infile.readline().strip()
        self.plus_sign = infile.readline().strip()
        self.quality = infile.readline().strip()
        self.tag = self.basecall[0:7]

    def __str__(self):
        return "\n".join([ self.identifier, self.basecall, self.plus_sign, self.quality])

    
barcodes = {
    'CACGTCG': 'AR100',
    'AGATGGT': 'AR101',
    'TCAGGAC': 'AR102',
    'ATAGACA': 'AR103',
    'CGAGAAG': 'AR104',
    'TGCTAAT': 'AR105',
    'GTTAATC': 'AR106',
    'GGCTTCA': 'AR107'
}

if len(sys.argv) > 1:
    infile = open(sys.argv[1])
else:
    infile = sys.stdin

count = {}
unmatched = 0
nseq = 0
while True:
    try:
        seq = fastq(infile)
    except EOFError:
        break

    if seq.tag in barcodes:
        label = barcodes[ seq.tag ]
        if label not in count: count[ label ] = 0
        count[ label ] += 1
    else:
        unmatched += 1

    nseq += 1
    print "\r", nseq,

print

infile.close()

print count
print "unmatched", unmatched
