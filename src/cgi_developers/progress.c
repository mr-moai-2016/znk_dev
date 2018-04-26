#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_thread.h>
#include <stdio.h>
#include <string.h>

static size_t st_count = 0;

static void
very_long_work( void )
{
	while( st_count <= 100 ){
		ZnkFile fp = Znk_fopen( "../publicbox/state.dat", "wb" );
		if( fp ){
			Znk_fprintf( fp, "%zu", st_count );
			++st_count;
			Znk_fclose( fp );
		}
		ZnkThread_sleep( 100 ); /* 100�~���bSleep���� */
	}
}

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 1, true );

	/* �܂�Javascript progress.js���܂�HTML��\��������. */
	{
		ZnkBird bird = ZnkBird_create( "#[", "]#" );
		RanoCGIUtil_printTemplateHTML( evar, bird, "../publicbox/progress.html" );
		ZnkBird_destroy( bird );
	}

	Znk_fflush( Znk_stdout() );
	/* Broken-Pipe�������I�Ɉ����N�����AWeb Server���ł�Read���[�v�������I��������. */
	Znk_fclose( Znk_stdout() );

	/* ���Ԃ̂����鏈��. */
	very_long_work();

	RanoCGIEVar_destroy( evar );
	return 0;
}
