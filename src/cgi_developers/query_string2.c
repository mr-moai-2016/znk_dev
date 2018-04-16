#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <stdio.h>
#include <string.h>

static void
show_result( const ZnkVarpAry post_vars )
{
	/***
	 * Output HTTP Header for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );
	printf( "\r\n" );

	/***
	 * Output HTML for CGI
	 */
	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "<pre>\n" );
	printf( "PostVars from Query String: \n" );
	{
		size_t i; 
		for( i=0; i<ZnkVarpAry_size(post_vars); ++i ){
			ZnkVarp     var  = ZnkVarpAry_at( post_vars, i );
			const char* name = ZnkVar_name_cstr(var);
			const char* val  = ZnkVar_cstr(var);
			printf( "%s = [%s]\n", name, val );
		}
	}
	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( int argc, char **argv )
{
	/***
	 * CGI�ɂ�������ϐ����擾���܂�.
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * URL�ɂ����� Query String ���w�肳��Ă���ꍇ�͂�����擾���܂�.
	 * �w�肳��Ă��Ȃ��ꍇ�A���̊֐��͋󕶎���Ԃ��܂�.
	 */
	const char* query_string = RanoCGIUtil_getQueryString( evar );

	/***
	 * ZnkVarpAry�͕ϐ��̔z����i�[���邽�߂̔ėp�̃f�[�^�\���ł�.
	 * �����p���āA�ȉ��ł� POST�ϐ��Q�S�̂��i�[���܂�.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * Windows�ɂ�����W������(0)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
	 */
	Znk_Internal_setMode( 0, true );
	/***
	 * Windows�ɂ�����W���o��(1)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
	 */
	Znk_Internal_setMode( 1, true );

	/***
	 * query_string �̓��e�͈ȉ��̂悤�� & �ŋ�؂���ϐ��w��̘A���ƂȂ��Ă���͂��ł�.
	 *
	 * name1=val1&name2=val2&name3=val3&...
	 *
	 * ����� & �����Ɋւ��ĕ���(split)���āA���ʂ�post_vars �Ɋi�[���܂�.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );

	/***
	 * post_vars�̓��e�̏o�͂ł�.
	 * �����Ȃ邽�߁A�T�u���[�`�������܂�.
	 */
	show_result( post_vars );

	/***
	 * ZnkVarpAry�͎g�p��������K�v������܂�.
	 */
	ZnkVarpAry_destroy( post_vars );

	/***
	 * RanoCGIEVar* �͎g�p��������K�v������܂�.
	 */
	RanoCGIEVar_destroy( evar );
	return 0;
}
