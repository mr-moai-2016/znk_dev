#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <stdio.h>
#include <string.h>

static void
show_result( const RanoCGIEVar* evar )
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

	printf( "Moai CGI Enviroment Variables:<br>\n" );

	printf( "<pre>\n" );
	printf( "server_name=[%s]\n",     evar->server_name_ );
	printf( "server_port=[%s]\n",     evar->server_port_ );
	printf( "content_type=[%s]\n",    evar->content_type_ );
	printf( "content_length=[%s]\n",  evar->content_length_ );
	printf( "remote_addr=[%s]\n",     evar->remote_addr_ );
	printf( "remote_host=[%s]\n",     evar->remote_host_ );
	printf( "remote_port=[%s]\n",     evar->remote_port_ );
	printf( "request_method=[%s]\n",  evar->request_method_ );
	printf( "query_string=[%s]\n",    evar->query_string_ );
	printf( "http_cookie=[%s]\n",     evar->http_cookie_ );
	printf( "http_user_agent=[%s]\n", evar->http_user_agent_ );
	printf( "http_accept=[%s]\n",     evar->http_accept_ );
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
	 * Windows�ɂ�����W���o��(1)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
	 */
	Znk_Internal_setMode( 1, true );

	/***
	 * ���ϐ��Qevar�̓��e�̏o�͂ł�.
	 * �����Ȃ邽�߁A�T�u���[�`�������܂�.
	 */
	show_result( evar );

	/***
	 * RanoCGIEVar* �͎g�p��������K�v������܂�.
	 */
	RanoCGIEVar_destroy( evar );
	return 0;
}
