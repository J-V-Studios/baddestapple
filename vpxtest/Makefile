all : vpxtest vpxtest_headless

include ../common/make.inc

CFLAGS:=-O2 -g -DRESX=$(RESX) -DRESY=$(RESY) -DBLOCKSIZE=$(BLOCKSIZE) -Wno-unused-result -I../common

vpxtest : vpxtest.c ../common/bacommon.h
	gcc -o $@ $< $(LDFLAGS) $(CFLAGS) -lm

vpxtest_headless : vpxtest.c ../common/bacommon.h
	gcc -o $@ $< -lm -DHEADLESS $(CFLAGS) -lm

test.gif badapple_data.h : vpxtest
	#./vpxtest ../comp2/stream-$(RESX)x$(RESY)x$(BLOCKSIZE).dat ../comp2/tiles-$(RESX)x$(RESY)x$(BLOCKSIZE).dat test.gif
	./vpxtest ../streamrecomp/stream_stripped.dat ../comp2/tiles-$(RESX)x$(RESY)x$(BLOCKSIZE).dat test.gif

# Fast A/B encode without X11/GIF (writes badapple_data.h + size summary on stdout)
encode_headless : vpxtest_headless
	./vpxtest_headless ../streamrecomp/stream_stripped.dat ../comp2/tiles-$(RESX)x$(RESY)x$(BLOCKSIZE).dat /dev/null 2>&1 | tee encode_report.txt

test.mp4 : test.gif
	ffmpeg -loglevel quiet -y -f f32le -ar 48000 -ac 1 -i ../song/track-float-48000.dat -r 30 -i test.gif -af adelay=1350 -vf "scale=trunc(iw/2)*4:trunc(ih/2)*4" -r 30 -crf 15 test.mp4


clean :
	rm -rf vpxtest vpxtest_headless encode_report.txt

