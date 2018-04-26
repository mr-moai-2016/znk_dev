#include "cgi_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char*
rejectHtmlTag( const char* cstr )
{
	/***
	 * �ꉞ�Œ����XSS�΍�͂��Ă���.
	 * �{���͂����Ɛ^�ʖڂɖ������������s���ׂ��ł����A����͏��F�T���v���Ȃ̂ŋɗ͊ȒP�����邽�߁A
	 * HTML�^�O���܂މ\�������鎞�_��NULL��Ԃ��Ēe���Ƃ����җ�Ȑ����������đΏ����Ă��܂�.
	 */
	if( cstr && ( strchr( cstr, '<' ) || strchr( cstr, '>' ) ) ){
		return NULL;
	}
	return cstr;
}

static void
show_result( CGIEVar* evar, const char* query_string )
{
	/***
	 * Output HTTP Header for CGI
	 */
	printf( "Content-type: text/html; charset=Shift_JIS\r\n" );
	/* �u���E�U�ɃL���b�V�������Ȃ��B */
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

	printf( "Moai CGI Post test.\n" );
	printf( "<form action=\"post1.cgi\" method=\"POST\">\n" );
	printf( "<b>E-mail</b><input type=text name=email size=\"28\"><br>\n" );
	printf( "<b>�R�����g</b><textarea name=com cols=\"48\" rows=\"4\"></textarea><br>\n" );
	printf( "<input type=submit value=\"�ԐM����\">\n" );
	printf( "</form>\n" );

	printf( "<pre>\n" );
	printf( "URL Query String Tokens: \n" );
	/***
	 * ����͏��F�T���v���ł��邽�߁A���܂�_����l������ƃR�[�h���ώG�ɂȂ�߂��Ė{�����ڂ₯�Ă��܂��܂�.
	 * �����ł́A�ϐ��̍ő吔�A�l�o�b�t�@����̓o�b�t�@�̍ő�T�C�Y�Ȃǂ��Œ艻���Ă���܂�.
	 */
	{
		size_t i = 0; 
		for( i=0; i<64; ++i ){
			char key[ 256 ] = ""; 
			char val[ 256 ] = ""; 
			int result = CGIUtil_getQueryStringToken( query_string, i,
					key, sizeof(key),
					val, sizeof(val) );
			if( result == 0 ){
				break;
			}
			printf( "%s = [%s]\n", rejectHtmlTag(key), rejectHtmlTag(val) );
		}
	}

	printf( "\n" );

	/***
	 * CGI�ɂ�����t�H�[���̓��e�f�[�^��post_vars�ւƎ擾���܂�.
	 * �܂����̊֐��ɂ����Ă͑�1����evar�̎w����K�v�ƂȂ�܂�.
	 */
	printf( "StdIn Posting Data: \n" );
	{
		char stdin_bfr[ 4096 ] = "";
		char* end = NULL;
		size_t content_length = strtoul( evar->content_length_, &end, 10 );
		printf( "Content-Length = [%u]\n", (unsigned int)content_length );
		if( evar->content_length_ != end ){
			size_t i = 0; 
			CGIUtil_getStdInStr( stdin_bfr, sizeof(stdin_bfr), content_length );
			printf( "Original data = [%s]\n", rejectHtmlTag(stdin_bfr) );
			printf( "Tokens: \n" );
			for( i=0; i<64; ++i ){
				char key[ 256 ] = ""; 
				char val[ 256 ] = ""; 
				int result = CGIUtil_getQueryStringToken( stdin_bfr, i,
						key, sizeof(key),
						val, sizeof(val) );
				if( result == 0 ){
					break;
				}
				printf( "%s = [%s]\n", rejectHtmlTag(key), rejectHtmlTag(val) );
			}
		} else {
			printf( "Error : content_length is not found.\n" );
		}
	}

	printf( "</pre>" );

	printf( "</body></html>\n" );
	fflush( stdout );
}

int main( void )
{
	/***
	 * CGI�ɂ�������ϐ����擾���܂�.
	 */
	CGIEVar* evar = CGIEVar_create();

	/***
	 * URL�ɂ����� Query String ���w�肳��Ă���ꍇ�͂�����擾���܂�.
	 * �w�肳��Ă��Ȃ��ꍇ�A���̊֐��͋󕶎���Ԃ��܂�.
	 */
	const char* query_string = CGIUtil_getQueryString( evar );

	{
		static const int is_binary_mode = 1;
		/***
		 * Windows�ɂ�����W������(0)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
		 */
		CGIUtil_Internal_setMode( 0, is_binary_mode );
		/***
		 * Windows�ɂ�����W���o��(1)�ɂ����� \n => \r\n �����ϊ��𖳌��ɂ��܂�.
		 */
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}

	/***
	 * query_string �̓��e�͈ȉ��̂悤�� & �ŋ�؂���ϐ��w��̘A���ƂȂ��Ă���͂��ł�.
	 *
	 * name1=val1&name2=val2&name3=val3&...
	 *
	 * ����� & �����Ɋւ��ĕ���(split)���āA���ʂ��o�͂��܂�.
	 * �܂��W�����͂��CGI�ɂ�����t�H�[���̓��e�f�[�^���擾���āA���ʂ��o�͂��܂�.
	 * �����Ȃ邽�߁A�T�u���[�`�������܂�.
	 */
	show_result( evar, query_string );

	/***
	 * CGIEVar* �͎g�p��������K�v������܂�.
	 */
	CGIEVar_destroy( evar );
	return 0;
}
