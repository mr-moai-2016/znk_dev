#include "cgi_util.h"
#include <stdio.h>

static void
show_result( const char* query_string )
{
	/***
	 * Output HTTP Header for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* ブラウザにキャッシュさせない。 */
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
	printf( "Query String Tokens: \n" );
	{
		size_t i = 0; 
		char key[ 256 ] = ""; 
		char val[ 256 ] = ""; 
		for( i=0; /* infinity */ ; ++i ){
			int result = CGIUtil_getQueryStringToken( query_string, i,
					key, sizeof(key),
					val, sizeof(val) );
			if( result == 0 ){
				break;
			}
			printf( "%s = [%s]\n", key, val );
		}
	}
	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( void )
{
	/***
	 * CGIにおける環境変数を取得します.
	 */
	CGIEVar* evar = CGIEVar_create();

	/***
	 * URLにおける Query String が指定されている場合はそれを取得します.
	 * 指定されていない場合、この関数は空文字を返します.
	 */
	const char* query_string = CGIUtil_getQueryString( evar );

	{
		static const int is_binary_mode = 1;
		/***
		 * Windowsにおける標準入力(0)における \n => \r\n 自動変換を無効にします.
		 */
		CGIUtil_Internal_setMode( 0, is_binary_mode );
		/***
		 * Windowsにおける標準出力(1)における \n => \r\n 自動変換を無効にします.
		 */
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}

	/***
	 * query_string の内容は以下のように & で区切られる変数指定の連続となっているはずです.
	 *
	 * name1=val1&name2=val2&name3=val3&...
	 *
	 * これを & 文字に関して分割(split)して、結果を出力します.
	 * 長くなるため、サブルーチン化します.
	 */
	show_result( query_string );

	/***
	 * CGIEVar* は使用後解放する必要があります.
	 */
	CGIEVar_destroy( evar );
	return 0;
}
