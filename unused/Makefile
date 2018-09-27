CFLAGS=-std=c99 -Wall -O3 # -g #-O3


# for test runs
SPLIT=fastx_barcode_splitter.pl
RAWSEQFILES=/Volumes/homes/erinn/02_POSTDOC/Erin/2_COMPUTATIONAL/7_RAWSEQFILES
AR100=$(RAWSEQFILES)/mAR100_JM127_5493_NoIndex_L007_R01_001.fastq.gz 
AR100_barcode=$(RAWSEQFILES)/4_barcode_files/barcode_index_AR100.txt

testrun: demultiplex_and_trim
	time cat mAR100_all.fastq | ./demultiplex_and_trim /dev/stdin $(AR100_barcode)
	#time cat mAR100_all.fastq | $(SPLIT) --bcfile $(AR100_barcode) --suffix .fastq --prefix 'pl' --bol

run: demultiplex_and_trim
	time gzcat $(AR100) | ./demultiplex_and_trim /dev/stdin $(AR100_barcode)


default: demultiplex_and_trim

test_linked_line: linked_line.o

read_fours: linked_line.o

demultiplex_and_trim: linked_line.o

clean:
	rm *.o test_linked_line read_fours
