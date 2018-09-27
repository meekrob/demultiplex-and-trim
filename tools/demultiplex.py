#!/usr/bin/env python
import sys

only=0
DEBUG = only > 0

class fastq:
    def __init__(self, infile):
        self.identifier = infile.readline().strip()
        if not self.identifier: raise ValueError
        self.basecall = infile.readline().strip()
        if not self.basecall: raise ValueError
        infile.readline() # just skip plus sign
        #self.plus_sign = infile.readline().strip()
        #if not self.plus_sign: raise ValueError
        self.quality = infile.readline().strip()
        if not self.quality: raise ValueError
        
        if DEBUG: print >>sys.stderr,  "I just read a single fastq:", self.__str__()

    def __str__(self):
        return "\n".join([ self.identifier, self.basecall, self.plus_sign, self.quality])

    def write(self, outfile):
        print >>outfile, self.identifier
        print >>outfile, self.basecall
        print >>outfile, "+"
        print >>outfile, self.quality

def hamming(seq1, seq2):
    d = 0
    for a,b in zip(seq1,seq2):
        if a != b:
            d += 1

    return d

def get_barcode_index(seq, barcodes, mismatches_allowed=1):
    # return -1 if no barcode matches the sequence 
    # with the number of mismatches allowed
    for i, (name, barcode) in enumerate(barcodes):
        dist = hamming(seq,barcode)
        if DEBUG:
            print >>sys.stderr, "\t", seq, "-", barcode, "=", dist
        if dist <= mismatches_allowed:
            return i

    return -1

def read_barcode_file(filename):
    obj = []
    max_len = 0
    for line in open(filename):
        prefix, seq = line.strip().split()
        max_len = max( max_len, len(seq) )
        obj.append( (prefix,seq) )
        
    return obj, max_len

def main():
    do_main(sys.argv[1], sys.argv[2])

def do_main(barcode_file, fastq_file):
    #barcode_file = sys.argv[1]
    barcodes, max_len = read_barcode_file(barcode_file)
    outfiles = []
    for prefix,barcode in barcodes:
        outfile = file(prefix + '.fastq', 'w')
        outfiles.append(outfile)

    unmatched_outfile = file('unmatched.fastq', 'w')

    counts = [0] * len(outfiles)
    unmatched_count = 0
        
    print barcodes

    table = {}
    # read fastq file
    import gzip

    if fastq_file.endswith('.gz'):
        infile = gzip.open(fastq_file, "r")
        print >>sys.stderr, "opening gzip format"
    else:
        infile = file(fastq_file)
    #infile = open( sys.argv[2] )
    nseq = 0
    while True:
        if DEBUG:
            print >>sys.stderr, "========================= %d =============================" % nseq
        try:
            if DEBUG: print >>sys.stderr, "I am about to read a single fastq"
            seq = fastq(infile)
        except:
            break

        nseq += 1
        seq_to_match = seq.basecall[:max_len]

        if DEBUG:
            print >>sys.stderr, "looking to match: `%s'" % seq_to_match
        if seq_to_match not in table:
            table[seq_to_match] = get_barcode_index(seq_to_match, barcodes)

        idx = table[seq_to_match]
        if DEBUG:
            print >>sys.stderr, "Match results: %d" % idx

        if idx > -1:
            #print >> outfiles[ idx ], seq
            seq.write(outfiles[idx])
            counts[ idx ] += 1
        else:
            #print >> unmatched_outfile, seq
            seq.write(unmatched_outfile)
            unmatched_count += 1

        if DEBUG:
            print >>sys.stderr, "here comes the table", table

        #elif nseq % 50000 == 0:
        elif 0:
            print >> sys.stderr, "\r",
            for i in range( len( barcodes ) ):
                print >> sys.stderr, barcodes[i][0] + ":", counts[i], ";", # prefix: count
            print >> sys.stderr, "unmatched:", unmatched_count, "total seqs:", nseq,

        if only and nseq >= only:
            break
                
    print >> sys.stderr
    print >> sys.stderr, "\rFinal Counts"
    for i in range( len( barcodes ) ):
        print >> sys.stderr, barcodes[i][0] + ":", counts[i]
    print >> sys.stderr, "unmatched:", unmatched_count
    # close output files
    unmatched_outfile.close()
    for f in outfiles: f.close()
    
if __name__ == "__main__": main()
