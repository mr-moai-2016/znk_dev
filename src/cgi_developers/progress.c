#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_thread.h>
#include <Znk_dir.h>
#include <stdio.h>
#include <string.h>

static size_t st_count = 0;

static void
show_result( RanoCGIEVar* evar, const ZnkVarpAry post_vars )
{
	ZnkBird bird = ZnkBird_create( "#[", "]#" );

	if( !RanoCGIUtil_printTemplateHTML( evar, bird, "publicbox/progress.html" ) ){
		ZnkStr dir = ZnkStr_new( "" );
		printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		printf( "<html><body>\n" );
		printf( "file type=[%d]\n", ZnkDir_getType( "publicbox/progress.html" ) );
		printf( "Cannot open template HTML file. [%s]\n", ZnkDir_getCurrentDir(dir) );
		printf( "</body></html>\n" );
		fflush( stdout );
		ZnkStr_delete( dir );
	}
	ZnkBird_destroy( bird );
}

static void
work( void )
{
	while( st_count <= 100 ){
		ZnkFile fp = Znk_fopen( "publicbox/state.dat", "wb" );
		if( fp ){
			Znk_fprintf( fp, "%zu", st_count );
			++st_count;
			Znk_fclose( fp );
		}
		ZnkThread_sleep( 100 );
	}
}

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * Get query_string;
	 */
	const char* query_string = RanoCGIUtil_getQueryString( evar );

	/***
	 * Create PostVars.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 0, true );
	Znk_Internal_setMode( 1, true );

	/***
	 * Get post_vars from query_string and posted form data.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );
	RanoCGIUtil_getPostedFormData( evar, post_vars, NULL, NULL, NULL, NULL, true );

	show_result( evar, post_vars );
	/***
	 * Broken-Pipeを強制的に引き起こし、Moai CGI側でのReadループを強制終了させる.
	 */
	Znk_fflush( Znk_stdout() );
	Znk_fclose( Znk_stdout() );
	work();

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
