#include <Rano_cgi_util.h>
#include <Znk_varp_ary.h>
#include <Znk_htp_util.h>
#include <stdio.h>
#include <string.h>

static bool
saveAsResultFile( const char* ext, const uint8_t* data, size_t data_size )
{
	char    result_filename[ 256 ] = "";
	ZnkFile fp = NULL;
	size_t  write_size = 0;
	Znk_snprintf( result_filename, sizeof(result_filename), "result_file.%s", ext );
	fp = Znk_fopen( result_filename, "wb" );
	if( fp ){
		write_size =  Znk_fwrite( data, data_size, fp );
		Znk_fclose( fp );
	}
	return (bool)( write_size == data_size );
}

static void
show_result( const ZnkVarpAry post_vars )
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

	printf( "Moai CGI Post for multipart/form-data.<br> <br>\n" );
	printf( "<form action=post2.cgi method=POST enctype=multipart/form-data>\n" );
	printf( "<b>E-mail</b><input type=text name=email size=28><br>\n" );
	printf( "<b>�R�����g</b><textarea name=com cols=48 rows=\"4\"></textarea><br>\n" );
	printf( "<b>�Y�tFile</b><input type=file name=upfile ><br>\n" );
	printf( "<br>\n" );
	printf( "<input type=submit value=\"�ԐM����\"><br>\n" );
	printf( "</form>\n" );
	printf( "<ul>\n" );
	printf( "<li>�Y�tFile : GIF, JPG, PNG, WEBM. 2000KB�܂�</li>\n" );
	printf( "</ul>\n" );

	printf( "<pre>\n" );
	printf( "PostVars:.\n" );

	{
		const char* ext = NULL;
		bool is_image = false;
		/* ZnkStr��libZnk���񋟂��镶����^�ł�. */
		ZnkStr msg = ZnkStr_new( "" );
		size_t i; 
		
		for( i=0; i<ZnkVarpAry_size(post_vars); ++i ){
			/* Post�ϐ�������o���܂�.
			* ���̕ϐ��̖��O�����Ă����܂�. */
			ZnkVarp     var  = ZnkVarpAry_at( post_vars, i );
			const char* name = ZnkVar_name_cstr(var);
			
			if( ZnkVar_prim_type( var ) == ZnkPrim_e_Bfr ){
				/* ZnkVarp�̃^�C�v��ZnkPrim_e_Bfr(�C�ӂ�Byte��).
				* ����͓Y�t�t�@�C�����Ӗ����܂�. */
				const char*    filename  = ZnkVar_misc_cstr( var, "filename" );
				const uint8_t* data      = ZnkVar_data( var );
				const size_t   data_size = ZnkVar_data_size( var );
				
				/* ZnkS_get_extension �̖߂�l�̓t�@�C���̊g���q�ƂȂ�܂�. */
				/* ���̎Q�Ɛ�́Apost_vars�̎�������������ɂ����Ă͗L���ł�. */
				ext = ZnkS_get_extension( filename, '.' );
				
				if( data_size > 2000000 ){ /* 2000KB�ȏ� */
					is_image = false;
					ZnkStr_addf( msg, "Binary : %s orinal filename=[%s] : Error : This is too large.\n", name, filename );
				} else {
					/* �����񂪓��������̔�r���s���܂�. */
					/* �������啶���������̋�ʂ͂��܂���. */
					if(  ZnkS_eqCase( ext, "jpg" )
					  || ZnkS_eqCase( ext, "png" )
					  || ZnkS_eqCase( ext, "gif" )
					  || ZnkS_eqCase( ext, "webm" )
					){
						/* �摜�t�@�C���̊g���q�ł������ꍇ�͂��̃f�[�^��result_file.ext �Ƃ��ĕۑ����܂�. */
						ZnkStr_addf( msg, "Image : data_size=[%zu]\n", data_size );
						if( saveAsResultFile( ext, data, data_size ) ){
							is_image = true;
							ZnkStr_addf( msg, "Image : %s orinal filename=[%s]\n", name, filename );
						} else {
							is_image = false;
							ZnkStr_addf( msg, "Image : %s orinal filename=[%s] : Error : fwrite error.\n", name, filename );
						}
					} else {
						is_image = false;
						ZnkStr_addf( msg, "Binary : %s orinal filename=[%s]\n", name, filename );
					}
				}
			} else {
				/* ZnkVarp�̃^�C�v�����̑�(Str).
				* �����input�^�O��textarea�^�O���ɗR�����镶����f�[�^. */
				const char* val = ZnkVar_cstr(var);
				ZnkStr_addf( msg, "%s = [%s] (size=[%u])\n", name, val, ZnkVar_str_leng(var) );
			}
		}
		
		/* XSS�΍�. msg���ɂ����؂�HTML�^�O�̌��ʂ�ł������܂�. */
		ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
		if( is_image ){
			/* �摜�t�@�C���ł������ꍇ��img�^�O�ɂ�肻���\�����܂�. */
			ZnkStr_addf( msg, "<br>\n" );
			ZnkStr_addf( msg, "<img src=\"./result_file.%s\">\n", ext );
			ZnkStr_addf( msg, "<br>\n" );
		}
		fputs( ZnkStr_cstr(msg), stdout );
		/* ZnkStr�͎g�p�������Ȃ���΂Ȃ�܂���. */
		ZnkStr_delete( msg );
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
	 * CGI�ɂ�����t�H�[���̓��e�f�[�^��post_vars�ւƎ擾���܂�.
	 * �܂����̊֐��ɂ����Ă͑�1����evar�̎w����K�v�ƂȂ�܂�.
	 */
	RanoCGIUtil_getPostedFormData( evar, post_vars, NULL, NULL, NULL, NULL, true );

	show_result( post_vars );

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
