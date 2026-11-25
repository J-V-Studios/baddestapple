include ../common/make.inc

CFLAGS:=-O2 -g -I../common -I../playback -I../vpxtest -DRESX=$(RESX) -DRESY=$(RESY) -DBLOCKSIZE=$(BLOCKSIZE) -DFRAMECT=$(FRAMECT) -DTARGET_GLYPH_COUNT=$(TARGET_GLYPH_COUNT)

all: size_report extract_stream_from_payload

size_report: size_report.c ../vpxtest/badapple_data.h ../common/bacommon.h
	gcc $(CFLAGS) -o $@ size_report.c

extract_stream_from_payload: extract_stream_from_payload.c ../playback/ba_play.h ../vpxtest/badapple_data.h
	gcc $(CFLAGS) -o $@ extract_stream_from_payload.c

baseline.txt: size_report
	./size_report | tee baseline.txt

clean:
	rm -f size_report extract_stream_from_payload baseline.txt

.PHONY: all clean
