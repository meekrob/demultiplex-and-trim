#!/usr/bin/env python
import sys
import demultiplex
import cProfile
cProfile.run("demultiplex.do_main('/Volumes/homes/erinn/02_POSTDOC/Erin/2_COMPUTATIONAL/7_RAWSEQFILES/4_barcode_files/barcode_index_AR100.txt', '/Volumes/homes/erinn/02_POSTDOC/Erin/2_COMPUTATIONAL/7_RAWSEQFILES/mAR100_JM127_5493_NoIndex_L007_R01_001.fastq.gz')")

