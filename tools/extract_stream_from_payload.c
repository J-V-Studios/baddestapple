/* Decode committed badapple_data.h and dump tiles + stream for re-encoding A/B tests. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BADATA_DECORATOR const
#include "../playback/ba_play.h"

int main( int argc, char ** argv )
{
	const char * out_tiles = ( argc > 1 ) ? argv[1] : "tiles-64x48x8.dat";
	const char * out_stream = ( argc > 2 ) ? argv[2] : "stream_stripped.dat";

	ba_play_context ctx;
	if( ba_play_setup( &ctx ) != 0 )
	{
		fprintf( stderr, "ba_play_setup failed\n" );
		return 1;
	}

	/* Glyphs: float tiles[TILE_COUNT][BLOCKSIZE][BLOCKSIZE] as used by streamrecomp/videocomp. */
	float tiles[TILE_COUNT][BLOCKSIZE][BLOCKSIZE];
	memset( tiles, 0, sizeof( tiles ) );
	for( int g = 0; g < TILE_COUNT; g++ )
	{
		for( int lx = 0; lx < BLOCKSIZE; lx++ )
		{
			graphictype col = ctx.glyphdata[g][lx];
			for( int ly = 0; ly < BLOCKSIZE; ly++ )
			{
#if defined( VPX_GREY3 )
				/* 2 bits per pixel packed: MSB grey plane in high byte half. */
				int bit_hi = ( col >> ( ly + BLOCKSIZE ) ) & 1;
				int bit_lo = ( col >> ly ) & 1;
				float v = 0.0f;
				if( bit_hi && bit_lo ) v = 1.0f;
				else if( bit_hi || bit_lo ) v = 0.5f;
				else v = 0.0f;
#else
				int bit = ( col >> ly ) & 1;
				float v = bit ? 1.0f : 0.0f;
#endif
				tiles[g][ly][lx] = v;
			}
		}
	}

	FILE * ft = fopen( out_tiles, "wb" );
	if( !ft ) { perror( out_tiles ); return 1; }
	if( fwrite( tiles, sizeof( tiles ), 1, ft ) != 1 ) { perror( "write tiles" ); return 1; }
	fclose( ft );

	uint32_t stream[FRAMECT][BLKY][BLKX];
	memset( stream, 0, sizeof( stream ) );

	for( int f = 0; f < FRAMECT; f++ )
	{
		if( ba_play_frame( &ctx ) != 0 )
		{
			fprintf( stderr, "ba_play_frame failed at %d\n", f );
			return 1;
		}
		for( int y = 0; y < BLKY; y++ )
			for( int x = 0; x < BLKX; x++ )
				stream[f][y][x] = ctx.curmap[y * BLKX + x];
		if( ( f % 500 ) == 0 )
			fprintf( stderr, "decoded frame %d/%d\n", f, FRAMECT );
	}

	FILE * fs = fopen( out_stream, "wb" );
	if( !fs ) { perror( out_stream ); return 1; }
	if( fwrite( stream, sizeof( stream ), 1, fs ) != 1 ) { perror( "write stream" ); return 1; }
	fclose( fs );

	printf( "Wrote %s (%zu bytes) and %s (%zu bytes)\n",
		out_tiles, sizeof( tiles ), out_stream, sizeof( stream ) );
	return 0;
}
