#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <stdio.h>
#include <string.h>

static void
show_result( const ZnkVarpAry post_vars )
{
	/***
	 * Output HTML for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
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

	printf( "Moai CGI Post test.\n" );
	printf( "<form action=\"/cgis/cgi_developers/post1.cgi\" method=\"POST\">\n" );
	printf( "<b>E-mail</b><input type=text name=email size=\"28\"><br>\n" );
	printf( "<b>コメント</b><textarea name=com cols=\"48\" rows=\"4\"></textarea><br>\n" );
	printf( "<input type=submit value=\"返信する\">\n" );
	printf( "</form>\n" );

	printf( "<pre>\n" );
	printf( "PostVars:.\n" );
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

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * Get query_string;
	 */
	const char* query_string = evar->query_string_ ? evar->query_string_ : "";

	/***
	 * Create post_vars.
	 */
	ZnkVarpAry post_vars = ZnkVarpAry_create( true );

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 0, true );
	Znk_Internal_setMode( 1, true );

	/***
	 * Get post_vars from query_string.
	 */
	RanoCGIUtil_splitQueryString( post_vars, query_string, Znk_NPOS, false );

	/***
	 * CGIにおけるフォームの投稿データをpost_varsへと取得します.
	 * またこの関数においては第1引数evarの指定も必要となります.
	 */
	RanoCGIUtil_getPostedFormData( evar, post_vars, NULL, NULL, NULL, NULL, true );

	show_result( post_vars );

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
