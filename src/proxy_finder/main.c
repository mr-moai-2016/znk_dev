#include "proxy_finder.h"
#include "cgi_helper.h"
#include <Rano_cgi_util.h>
#include <Rano_html_ui.h>

#include <Znk_varp_ary.h>
#include <Znk_str_path.h>
#include <Znk_htp_util.h>

#include <stdio.h>
#include <string.h>


static bool
getProxy( ZnkVarpAry post_vars, ZnkMyf conf_myf, const char* moai_dir, ZnkStr ermsg )
{
	bool        result          = false;
	ZnkStr      result_filename = ZnkStr_new( "" );
	const char* target          = NULL;
	ZnkVarpAry  target_vars     = NULL;
	ZnkVarp     varp            = NULL;

	/***
	 * post_vars����target�ϐ��𓾂āA���̒l���擾���܂�.
	 */
	varp = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
	if( varp ){
		target      = ZnkVar_cstr( varp );
		target_vars = ZnkMyf_find_vars( conf_myf, target );
	}
	ZnkStr_addf( ermsg, "Report : target is [%s].\n", target );

	/* Phase-1 : download cache */
	if( target_vars ){
		const char* hostname      = NULL;
		const char* url_underpath = NULL;

		/***
		 * �ݒ�t�@�C��(conf_myf)����ȉ��̍��ڂ��擾���܂�.
		 */
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "hostname", false );
		if( varp ){
			hostname = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "url_underpath", false );
		if( varp ){
			url_underpath = ZnkVar_cstr( varp );
		}

		/***
		 * http://hostname/url_underpath �֐ڑ����A����HTML�t�@�C���𓾂܂�.
		 * ���������ꍇ�A���ꂪ�L���b�V���Ƃ��ĕۑ�����A���̃L���b�V���̃t�@�C������
		 * result_filename�Ɋi�[����܂�.
		 */
		result = ProxyFinder_download( result_filename,
				hostname, url_underpath, target, ermsg );
		if( result ){
			ZnkStr_addf( ermsg, "Report : download cache file as filename=[%s].\n", ZnkStr_cstr(result_filename) );
		}
	}

	/* Phase-2 : analysis */
	if( target_vars && result ){
		char parent_proxy_filename[ 256 ] = "";
		const char* ip_ptn_begin   = "IP_PTN_BEGIN";   /* dummy pattern */
		const char* ip_ptn_end     = "IP_PTN_END";     /* dummy pattern */
		const char* port_ptn_begin = "PORT_PTN_BEGIN"; /* dummy pattern */
		const char* port_ptn_end   = "PORT_PTN_END";   /* dummy pattern */
		bool        is_hex_port    = false;

		/***
		 * �ŏI�I�Ȍ���(Moai���g�p����parent_proxy.txt)���������΃p�X�ƂȂ�܂�.
		 */
		Znk_snprintf( parent_proxy_filename, sizeof(parent_proxy_filename),
				"%sparent_proxy.txt", moai_dir );

		/***
		 * �ݒ�t�@�C��(conf_myf)����ȉ��̍��ڂ��擾���܂�.
		 */
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "ip_ptn_begin", false );
		if( varp ){
			ip_ptn_begin = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "ip_ptn_end", false );
		if( varp ){
			ip_ptn_end = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "port_ptn_begin", false );
		if( varp ){
			port_ptn_begin = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "port_ptn_end", false );
		if( varp ){
			port_ptn_end = ZnkVar_cstr( varp );
		}
		varp = ZnkVarpAry_findObj_byName_literal( target_vars, "is_hex_port", false );
		if( varp ){
			is_hex_port = ZnkS_eq( ZnkVar_cstr( varp ), "true" );
		}

		/**
		 * ip_ptn_begin �� ip_ptn_end �̊Ԃɂ��镶�����IP,
		 * port_ptn_begin �� port_ptn_end �̊Ԃɂ��镶�����port�ԍ��Ƃ݂Ȃ��ĉ��.
		 * �܂��Aport��16�i���ŋL�q����Ă���T�C�g�̏ꍇ��is_hex_port��true�Ɏw�肷��.
		 */
		ZnkStr_addf( ermsg, "Report : Now, we will analysis downloaded file=[%s].\n", ZnkStr_cstr(result_filename) );
		ProxyFinder_analysis( ZnkStr_cstr(result_filename), parent_proxy_filename,
				ip_ptn_begin,   ip_ptn_end,
				port_ptn_begin, port_ptn_end,
				is_hex_port, ermsg );
	}
	ZnkStr_delete( result_filename );
	return result;
}

static void
makeTargetSelectBarUI( ZnkStr ans, ZnkMyf conf_myf, const char* target_current_val )
{
	const size_t  size = ZnkMyf_numOfSection( conf_myf );
	size_t        idx;
	ZnkMyfSection sec  = NULL;
	const char*   sec_name = NULL;
	ZnkVarpAry    vars = NULL;
	ZnkVarp       varp = NULL;
	const char*   hostname = NULL;
	ZnkStrAry     val_list  = ZnkStrAry_create( true );
	ZnkStrAry     name_list = ZnkStrAry_create( true );

	for( idx=0; idx<size; ++idx ){
		sec = ZnkMyf_atSection( conf_myf, idx );
		if( ZnkMyfSection_type( sec ) == ZnkMyfSection_e_Vars ){
			vars = ZnkMyfSection_vars( sec );
			sec_name = ZnkMyfSection_name( sec );
			varp = ZnkVarpAry_findObj_byName_literal( vars, "hostname", false );
			if( varp ){
				hostname = ZnkVar_cstr( varp );
				ZnkStrAry_push_bk_cstr( name_list, hostname, Znk_NPOS );
				ZnkStrAry_push_bk_cstr( val_list,  sec_name, Znk_NPOS );
				if( target_current_val == NULL ){
					target_current_val = sec_name;
				}
			}
		}
	}
	ZnkStr_add( ans, "" );
	RanoHtmlUI_makeSelectBox( ans,
			"target", target_current_val, true,
			val_list, name_list );

	ZnkStrAry_destroy( val_list );
	ZnkStrAry_destroy( name_list );
}

static void
parseMainPostVars( RanoCGIEVar* evar, const ZnkVarpAry post_vars )
{
	const char* moai_dir = NULL;
	const char* authentic_key = NULL;
	bool        is_authenticated = false;

	const char* template_html_file = "templates/proxy_finder.html";
	ZnkBird     bird     = ZnkBird_create( "#[", "]#" );
	ZnkStr      ermsg    = ZnkStr_new( "" );
	ZnkMyf      conf_myf = ZnkMyf_create();
	ZnkVarp     command  = NULL;

	/***
	 * CGIHelper_initEsscential�ł́Amoai_dir, Moai authentic_key���擾���܂�.
	 * �܂�post_vars�ɐݒ肳�ꂽMoai_AuthenticKey��authentic_key�̒l���r���邱�Ƃ��
	 * Moai�F�؂ɐ����������ۂ��̐^�U�l������is_authenticated�̒l���擾���܂�.
	 */
	if( !CGIHelper_initEsscential( post_vars,
			&moai_dir, &authentic_key, &is_authenticated,
			ermsg ) ){
		goto FUNC_END;
	}

	/***
	 * ProxyFinder�ݒ�t�@�C���̃��[�h.
	 * ����͔F�؂���Ă��Ȃ��ꍇ�ł����[�h�������Ă���܂�.
	 */
	if( !ZnkMyf_load( conf_myf, "proxy_finder.myf" ) ){
		/* error */
		ZnkStr_addf( ermsg, "Error : proxy_finder.myf does not exist.\n" );
		goto FUNC_END;
	}

	/***
	 * �R�}���h�̏���.
	 * post_vars��command���w�肳��Ă���ꍇ�A���̒l�ɉ����Ď��s����R�}���h�������s���܂�.
	 */
	command = ZnkVarpAry_findObj_byName_literal( post_vars, "command", false );
	if( command ){
		if( ZnkS_eq( ZnkVar_cstr(command), "get_proxy" ) && is_authenticated ){
			/***
			 * ���̃R�}���h�Ɋւ��Ă͔F�؂��K�v�ł���Ƃ��܂�.
			 */
			ZnkStr_addf( ermsg, "Report : OK. This request is authenticated.\n" );
			getProxy( post_vars, conf_myf, moai_dir, ermsg );
		} else {
			ZnkStr_addf( ermsg, "Report : Error : This request is not authenticated.\n" );
		}
	} else {
		/***
		 * �����R�}���h�����s�����A�P���ɃX�^�[�g�ƂȂ�UI��\������ꍇ.
		 * ���̏ꍇ�͔F�؂���Ă��Ȃ��ꍇ�ł��\���\�Ƃ��Ă����܂�.
		 */
	}

FUNC_END:
	{
		const char* target_current_val = NULL;
		ZnkStr  ui   = ZnkStr_new( "" );
		ZnkVarp varp = ZnkVarpAry_findObj_byName_literal( post_vars, "target", false );
		if( varp ){
			target_current_val = ZnkVar_cstr( varp );
		}
		makeTargetSelectBarUI( ui, conf_myf, target_current_val );
		ZnkBird_regist( bird, "target_select_bar", ZnkStr_cstr(ui) );
		ZnkStr_delete( ui );
	}
	/***
	 * ���K��UI�ɂ̂�Moai_AuthenticKey�̒l�𖄂ߍ��݂܂�.
	 * template_html_file�̒��g�́A�Ⴆ�Έȉ��̂悤�ɂ��� form ���� post �����ۂ�
	 * ����Moai_AuthenticKey��post_vars�ϐ��Ɋ܂܂��悤�ɂ��Ă����܂�.
	 *
	 * <input type=hidden name=Moai_AuthenticKey value="#[Moai_AuthenticKey]#">
	 *
	 * XhrDMZ���Ńu���E�U�ɂĎ��s�����Javascript�́A����UI��HTML�ɑ΂���XHtmlRequest�𔭍s���邱�Ƃ��ł����A
	 * �]����Moai_AuthenticKey�̒l�����[�U�ȊO�������I�ɂ͓ǂނ��Ƃ��ł��Ȃ��Ƃ����d�g�݂ɂȂ��Ă��܂�.
	 *
	 * �܂� ermsg�̒l�𖄂ߍ��ނ��ƂŖ�肪���������ꍇ�ɂ��̌�����e�ՂɒǐՂł��܂�.
	 * template_html_file�̒��g�́A�Ⴆ�Έȉ��̂悤�� #[ermsg]# �Ƃ����������K���ȏꏊ�ɖ��ߍ���ł����܂�.
	 *
	 * <b>System Console:</b><br>#[ermsg]#<br>
	 *
	 * CGIHelper_printTemplateHTML �ł͂��̂悤�Ȗ��ߍ��݂�W�J���鏈�����s���Ă��܂�.
	 */
	if( !CGIHelper_printTemplateHTML( evar, bird, template_html_file, authentic_key, ermsg ) ){
		printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
		printf( "<html><body>\n" );
		printf( "Cannot open template HTML file. [%s]\n", template_html_file );
		printf( "</body></html>\n" );
		fflush( stdout );
	}

	ZnkBird_destroy( bird );
	ZnkMyf_destroy( conf_myf );
	ZnkStr_delete( ermsg );
}

int main( int argc, char** argv )
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

	/***
	 * MoaiCGI�̃��C������.
	 */
	parseMainPostVars( evar, post_vars );

	ZnkVarpAry_destroy( post_vars );
	RanoCGIEVar_destroy( evar );
	return 0;
}
