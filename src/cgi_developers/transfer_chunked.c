#include "cgi_util.h"
#include <stdio.h>

static void
show_result( void )
{
	/***
	 * Output HTML for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* �u���E�U�ɃL���b�V�������Ȃ��B */
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );
	printf( "\r\n" );

	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Moai CGI Transfer chunked test.<br>" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk1 fflush<br>\n" );
	fflush( stdout );
	/* �o�b�t�@�ɉ����Ȃ��ꍇ�͉����N����Ȃ�.
	 * (0-size��chunk�����M����邱�Ƃ͂Ȃ�) */
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk2 fflush<br>\n" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "C" );
	/* fflush �����s���ȂƁA�ʏ�͒��r���[�ȏ�Ԃ�chunk�͑��M����Ȃ� */
	CGIUtil_sleep( 1000 );

	printf( "hunk3 fflush<br>\n" );
	fflush( stdout );
	CGIUtil_sleep( 1000 );

	printf( "Chunk4<br>\n" ); /* ���s����ꂽ�����ł�chunk�͑��M����Ȃ� */
	CGIUtil_sleep( 1000 );

	printf( "End." );
	printf( "</body></html>\n" );
	/* �Ō��fflush�͕ʂɂȂ��Ƃ��v���Z�X�̏I���Ŏ����I�ɑ��M����� */
}

int main( void )
{
	static const int is_binary_mode = 1;
	/***
	 * Windows�ɂ�����W���o��(1)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
	 */
	CGIUtil_Internal_setMode( 1, is_binary_mode );

	show_result();
	return 0;
}
