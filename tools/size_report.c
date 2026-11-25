/* Report packed asset sizes from badapple_data.h (+ optional song header). */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define BADATA_DECORATOR const
#include "../vpxtest/badapple_data.h"

/* Song size from committed playback header when available. */
#ifndef SKIP_SONG
#include <string.h>
/* Approximate: parse is hard; include the huffman song if present via sizeof in a stub. */
#endif

int main( void )
{
	size_t tile_class = sizeof( ba_exportbinclass );
#ifdef USE_TILE_CLASSES
	size_t tile_prob = sizeof( ba_chancetable_glyph_dual );
#else
	size_t tile_prob = sizeof( ba_chancetable_glyph );
#endif
#ifdef RUNCODES_CONTINUOUS
	size_t run_prob = sizeof( ba_vpx_probs_by_tile_run_continuous );
#else
	size_t run_prob = sizeof( ba_vpx_probs_by_tile_run );
#endif
	size_t glyphs = sizeof( ba_glyphdata ) + sizeof( ba_vpx_glyph_probability_run_0_or_1 );
	size_t payload = sizeof( ba_video_payload );

	/* Song: committed playback header size of note tables is reported separately by song build.
	   Use fixed 673 from upstream when header not linked; try to detect ba_audio via optional include. */
	size_t sound = 673;

	size_t total = tile_class + tile_prob + run_prob + glyphs + payload + sound;

	printf( "=== baddestapple size report ===\n" );
	printf( "RES %dx%d block %d tiles %d classes %d runcodes %d\n",
		RESX, RESY, BLOCKSIZE, TILE_COUNT, USE_TILE_CLASSES, RUNCODES_CONTINUOUS );
	printf( " +Tile Class: %6zu bytes\n", tile_class );
	printf( " + Tile Prob: %6zu bytes\n", tile_prob );
	printf( " +  Run Prob: %6zu bytes\n", run_prob );
	printf( " +COMPGlyphs: %6zu bytes\n", glyphs );
	printf( "    Payload : %6zu bytes\n", payload );
	printf( "   Sound    : %6zu bytes (nominal / from song build)\n", sound );
	printf( " Total: %6zu Bytes (assets)\n", total );
	printf( " Upstream reference total: 62781\n" );
	printf( " Delta vs upstream: %+zd\n", (ssize_t)total - 62781 );
	return 0;
}
