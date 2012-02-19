#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include "mp3.h"
#include "mad_config.h"
#include "mad_decoder.h"

/*
 * This small program tests that my offset addition to libMAD actually works.
 * Erez Volk 2007-05-30
 */

static void dump_file( const char *filename );
static void test_file( const char *filename );
static void test_mad( FILE *f );
static void test_mp3f( FILE *f );

static void get_offsets( FILE *f );

static enum mad_flow cb_input( void *context, struct mad_stream *stream );
static enum mad_flow cb_header( void *context, struct mad_header const *header );
static enum mad_flow cb_accept_header( void *context, struct mad_header const *header );
static enum mad_flow cb_dump( void *context, struct mad_header const *header, struct mad_pcm *pcm );

static void cb_samples( const int *channels[MP3F_MAX_CHANNELS],
		unsigned num_samples,
		void *context );

#define BUFFER_SIZE 2048
static unsigned char the_buffer[BUFFER_SIZE];

#define MAX_OFFSETS 4096
static unsigned long offsets[MAX_OFFSETS];
static unsigned num_offsets;

int main( int argc, char *argv[] )
{
	int i;

	if ( argc <= 1 )
	{
		printf( "Usage: %s MP3_FILE [MP3_FILE...]\n", argv[0] );
		return -1;
	}

	if ( argc > 2 && !strcmp( argv[1], "-dump" ) )
		dump_file( argv[2] );
	else for ( i = 1; i < argc; ++ i )
		test_file( argv[i] );

	return 0;
}

typedef struct {
	FILE *f;
	unsigned done;
	unsigned left;
	FILE *f2;
} DUMP_CONTEXT;

static void dump_file( const char *filename )
{
	enum { MAX_FRAMES = 16 };
	FILE *f;
	unsigned i;

	printf( "Dumping file: \"%s\"...\n", filename );
	if ( (f = fopen( filename, "rb" )) == NULL ) {
		printf( "  Cannot open file.\n" );
		return;
	}

	get_offsets( f );

	printf( "  Header offsets: %lu, %lu, %lu, %lu, ... %lu\n",
		offsets[0], offsets[1], offsets[2], offsets[3],
		offsets[num_offsets - 1] );

	for ( i = 0; i < MAX_FRAMES; ++ i )
	{
		char dumpname[128];
		struct mad_decoder d;
		DUMP_CONTEXT c = { f, i, MAX_FRAMES - i };

		sprintf( dumpname, "%02u.dump", i );
		printf( "  Creating %s...\n", dumpname );
		c.f2 = fopen( dumpname, "w" );

		fseek( f, offsets[i], SEEK_SET );
		mad_decoder_init( &d, &c, cb_input, cb_accept_header, NULL, cb_dump, NULL, NULL );
		mad_decoder_run( &d, MAD_DECODER_MODE_SYNC );
		mad_decoder_finish( &d );

		fclose( c.f2 );
	}

	fclose( f );
}

static void test_file( const char *filename )
{
	FILE *f;

	printf( "Testing file: \"%s\"...\n", filename );

	if ( (f = fopen( filename, "rb" )) == NULL ) {
		printf( "  Cannot open file.\n" );
		return;
	}

	test_mad( f );
	test_mp3f( f );

	fclose( f );
}

static void test_mad( FILE *f )
{
	enum { MAX_BAD = 16 };
	unsigned i, bad;

	get_offsets( f );

	if ( num_offsets >= MAX_OFFSETS )
		printf( "  Reached maximum number of headers (%u)\n", MAX_OFFSETS );
	else
		printf( "  Number of headers found: %u\n", num_offsets );
	printf( "  Header offsets: %lu, %lu, %lu, %lu, ... %lu\n",
		offsets[0], offsets[1], offsets[2], offsets[3],
		offsets[num_offsets - 1] );
	printf( "  Checking header validity...\n" );
	for ( i = bad = 0; i < num_offsets && bad < MAX_BAD; ++ i )
	{
		unsigned char header[2] = { 0, 0 };
		if ( i > 0 && offsets[i] <= offsets[i - 1] )
		{
			printf( "  Invalid offset table.\n" );
			return;
		}
		fseek( f, offsets[i], SEEK_SET );
		fread( header, 1, sizeof(header), f );
		if ( (header[0] != 0xFF) || ((header[1] & 0xE0) != 0xE0) )
		{
			++ bad;
			printf( "  ERROR: No header at file offset %lu\n", offsets[i] );
		}
		else if ( bad > 0 )
			printf( "  Good header at file offset %lu\n", offsets[i] );
	}
	if ( bad == 0 )
		printf( "  All offsets have valid MP3 headers\n" );
}

static void get_offsets( FILE *f )
{
	struct mad_decoder d;

	/* Make sure there are no leftovers */
	memset( &d, time(NULL), sizeof(d) );

	num_offsets = 0;

	mad_decoder_init( &d, &f, cb_input, cb_header, NULL, NULL, NULL, NULL );
	if ( mad_decoder_run( &d, MAD_DECODER_MODE_SYNC ) != 0 )
	{
		printf( "  Error scanning file.\n" );
		return;
	}

	mad_decoder_finish( &d );
}

static enum mad_flow cb_input( void *context, struct mad_stream *stream )
{
	FILE *f = *(FILE **)context;
	unsigned char *data = NULL;
	unsigned nthrown = 0, ncopied = 0, size = 0;
	unsigned offset;
	size_t nread = 0;

	if (feof (f))
		return MAD_FLOW_STOP;

	if (stream -> next_frame) {
		nthrown = stream -> next_frame - the_buffer; 
		ncopied = BUFFER_SIZE - nthrown;
		memmove (the_buffer, stream -> next_frame, ncopied);
	}

	data = the_buffer + ncopied;
	size = BUFFER_SIZE - ncopied;

	offset = ftell (f) - ncopied;
	if (size > 0)
		nread = fread (data, 1, size, f);

	mad_stream_buffer_offset (stream, the_buffer, nread + ncopied, offset);
	stream -> this_offset = offset;
	return MAD_FLOW_CONTINUE;
}

static enum mad_flow cb_header( void *context, struct mad_header const *header )
{
	FILE *f = *(FILE **)context;
	unsigned long foff = ftell( f );
	if ( foff > header->offset + BUFFER_SIZE )
		printf( "  ??? %lu <-> %lu\n", foff, header->offset );
	offsets[num_offsets] = header->offset;
	if ( ++ num_offsets >= MAX_OFFSETS )
		return MAD_FLOW_STOP;
	return MAD_FLOW_CONTINUE;
}

static enum mad_flow cb_accept_header( void *context, struct mad_header const *header )
{
	(void)context;
	(void)header;
	return MAD_FLOW_CONTINUE;
}

static enum mad_flow cb_dump( void *context, struct mad_header const *header, struct mad_pcm *pcm )
{
	DUMP_CONTEXT *c = (DUMP_CONTEXT *)context;
	FILE *f = c->f2;
	unsigned i, j, length = pcm->length;
	const mad_fixed_t *samples = pcm->samples[0];
	(void)header;

	fprintf( f, "FRAME %u, OFFSET %lu (expected %lu)\n",
		       	c->done, header->offset, offsets[c->done] );
	for ( i = 0; i < length; i += 8 ) {
		for ( j = i; i < length && j < i + 8; ++ j )
			fprintf( f, "%9i ", samples[j] );
		fprintf( f, "\n" );
	}
	fprintf( f, "\n" );

	++ c->done;
	-- c->left;
	return c->left == 0 ? MAD_FLOW_STOP : MAD_FLOW_CONTINUE;
}

static void test_mp3f( FILE *f )
{
	enum { FIRST = 15000, SECOND = 2000, TOTAL = FIRST + SECOND };
	enum { MID_OFFSET = 16000, MID_SIZE = 800 };
	static int x[TOTAL], y[TOTAL], z[MID_SIZE];
	int *p;
	MP3_FILE mp3f = mp3f_new();

	mp3f_set_file( mp3f, f );
	if ( mp3f_analyze( mp3f ) )
	{
		printf( "  MP3F: Channels = %u\n", mp3f_channels( mp3f ) );
		printf( "  MP3F: Frequency = %u\n", mp3f_frequency( mp3f ) );
		printf( "  MP3F: Samples = %lu\n", (unsigned long)mp3f_samples( mp3f ) );
		printf( "  MP3F: Time => %.2lfs\n",
				(double)mp3f_samples(mp3f) / (double)mp3f_frequency(mp3f) );
	}
	else	
		printf( "  MP3F: Cannot analyze\n" );

	memset( x, 0x22, sizeof(x) );
	memset( y, 0x22, sizeof(y) );
	memset( z, 0x22, sizeof(z) );

	mp3f_set_file( mp3f, f );
	p = x;
	mp3f_set_callback( mp3f, cb_samples, &p );
	mp3f_read( mp3f, TOTAL );

	mp3f_set_file( mp3f, f );
	p = y;
	mp3f_set_callback( mp3f, cb_samples, &p );
	mp3f_read( mp3f, FIRST );
	mp3f_read( mp3f, SECOND );

	mp3f_set_file( mp3f, f );
	p = z;
	mp3f_set_callback( mp3f, cb_samples, &p );
	if ( mp3f_seek( mp3f, MID_OFFSET ) )
		mp3f_read( mp3f, MID_SIZE );
	else
		printf( "  MP3F: Seek failed\n" );

	if ( memcmp( x, y, FIRST * sizeof(int) ) != 0 )
		printf( "  MP3F: Difference in first samples!\n" );
	else
		printf( "  MP3F: First samples OK\n" );

	if ( memcmp( x + FIRST, y + FIRST, SECOND * sizeof(int) ) != 0 )
		printf( "  MP3F: Difference in later samples!\n" );
	else
		printf( "  MP3F: Later samples OK\n" );

	if ( memcmp( x + MID_OFFSET, z, MID_SIZE * sizeof(int) ) != 0 )
		printf( "  MP3F: Seek doesn't work\n" );
	else
		printf( "  MP3F: Seek works\n" );

	mp3f_delete( mp3f );
}

static void cb_samples( const int *channels[MP3F_MAX_CHANNELS],
		unsigned num_samples,
		void *context )
{
	int **pp = (int **)context;
	int *p = *pp;
	unsigned i;

	for ( i = 0; i < num_samples; ++ i )
		p[i] = channels[0][i];
	*pp += num_samples;
}

/* Needed for libMP3 */
void *Melder_malloc( unsigned size ) { return malloc( size ); }
void _Melder_free( void **p ) { free( *p ); }

