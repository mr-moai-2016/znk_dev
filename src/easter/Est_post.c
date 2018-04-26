#include <Est_post.h>
#include <Est_base.h>
#include <Est_config.h>
#include <Est_ui.h>
#include <Est_hint_manager.h>

#include <Rano_log.h>
#include <Rano_htp_modifier.h>
#include <Rano_htp_boy.h>
#include <Rano_post.h>

#include <Znk_htp_util.h>
#include <Znk_htp_post.h>
#include <Znk_cookie.h>
#include <Znk_stdc.h>
#include <Znk_str_ex.h>

#define IS_OK( val ) (bool)( (val) != NULL )

typedef struct {
	RanoCGIEVar* evar_; 
	ZnkHtpHdrs   htp_hdrs_;
	RanoModule   mod_; 
	ZnkStr       given_authentick_key_;
} EstPostInfo;

static void
initHtpHdr_forPost( ZnkHtpHdrs htp_hdrs, const char* hostname, const char* req_urp, const char* ua, ZnkVarpAry cookie, RanoCGIEVar* evar )
{
	ZnkVarp varp;
	ZnkSRef sref = { 0 };
	Znk_UNUSED(varp);

	RanoLog_printf( "initHtpHdr_forPost : hostname=[%s] req_urp=[%s]\n", hostname, req_urp );

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Host",   Znk_strlen_literal("Host"),
			hostname, Znk_strlen(hostname), true );

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"User-Agent", Znk_strlen_literal("User-Agent"),
			ua, Znk_strlen(ua), true );

	ZnkSRef_set_literal( &sref, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept", Znk_strlen_literal("Accept"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "ja,en-US;q=0.7,en;q=0.3" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Language", Znk_strlen_literal("Accept-Language"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "gzip, deflate" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Accept-Encoding", Znk_strlen_literal("Accept-Encoding"),
			sref.cstr_, sref.leng_, true );

	ZnkSRef_set_literal( &sref, "http://www.google.com" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Referer", Znk_strlen_literal("Referer"),
			sref.cstr_, sref.leng_, true );
	if( varp ){
		ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
		ZnkStr_set( val, "http://" );
		ZnkStr_add( val, hostname );
		ZnkStr_add( val, req_urp );
	}

	{
		ZnkStr     cok_stmt = ZnkStr_new( "" );
		ZnkCookie_extend_toCookieStatement( cookie, cok_stmt );
		varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
				"Cookie",   Znk_strlen_literal("Cookie"),
				ZnkStr_cstr(cok_stmt), ZnkStr_leng(cok_stmt), true );
		ZnkStr_delete( cok_stmt );
	}

	ZnkSRef_set_literal( &sref, "keep-alive" );
	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Connection", Znk_strlen_literal("Connection"),
			sref.cstr_, sref.leng_, true );

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Content-Type", Znk_strlen_literal("Content-Type"),
			evar->content_type_, Znk_strlen(evar->content_type_), true );

	varp = ZnkHtpHdrs_regist( htp_hdrs->vars_,
			"Content-Length", Znk_strlen_literal("Content-Length"),
			evar->content_length_, Znk_strlen(evar->content_length_), true );

}


/***
 * Security�̖����ǂ��������邩�ł��邪�A
 * �Ƃ肠�����L���b�V���폜�ȂǂƓ��l�Ɏw�肳�ꂽ�f�B���N�g���z���ȊO�ł̃t�@�C���w��͖����Ƃ݂Ȃ����ƂőΏ�����.
 */
static bool
replaceUploadFile_forMPFD( const ZnkVarpAry hpvs, const char* query_post_var_name, const char* new_filename )
{
	const size_t size = ZnkVarpAry_size( hpvs );
	uint8_t buf[ 4096 ] = { 0 };
	size_t idx;
	ZnkVarp varp;

	for( idx=0; idx<size; ++idx ){
		varp = ZnkVarpAry_at( hpvs, idx );
		if( ZnkS_eq( ZnkVar_name_cstr( varp ), query_post_var_name ) && varp->type_ == ZnkHtpPostVar_e_BinaryData ){
			/* ���ɑ��݂����ꍇ�͒u�������� */
			ZnkFile fp = Znk_fopen( new_filename, "rb" );
			size_t readed_byte = 0;
			const char* q = Znk_strrchr( new_filename, '/' );
			q = q ? q + 1 : new_filename;
			//RanoLog_printf( "replaceUploadFile_forMPFD : q=[%s]\n", q );
			ZnkStr_assign( ZnkVar_misc( varp ), 0, q, Znk_NPOS );
			if( fp ){
				ZnkBfr bfr = ZnkVar_bfr( varp );
				ZnkBfr_clear( bfr );
				while( true ){
					readed_byte = Znk_fread( buf, sizeof(buf), fp );
					if( readed_byte == 0 ){
						break;
					}
					ZnkBfr_append_dfr( bfr, buf, readed_byte );
				}
				return true;
			}
		}
	}

	{
		/* ���݂��Ȃ��ꍇ�͐V���ɒǉ� */
		ZnkFile fp = Znk_fopen( new_filename, "rb" );
		size_t readed_byte = 0;
		if( fp ){
			ZnkBfr bfr;
			/* �V�K�ǉ� */
			const char* q = Znk_strrchr( new_filename, '/' );
			q = q ? q + 1 : new_filename;
			//RanoLog_printf( "replaceUploadFile_forMPFD : q=[%s]\n", q );
			varp = ZnkVarp_create( "", "", 0, ZnkPrim_e_Bfr, NULL );
			varp->type_ = ZnkHtpPostVar_e_BinaryData;
			ZnkStr_assign( varp->name_, 0, query_post_var_name, Znk_NPOS );
			ZnkStr_assign( ZnkVar_misc( varp ), 0, q, Znk_NPOS );
			ZnkVarpAry_push_bk( hpvs, varp );

			bfr = ZnkVar_bfr( varp );
			while( true ){
				readed_byte = Znk_fread( buf, sizeof(buf), fp );
				if( readed_byte == 0 ){
					break;
				}
				ZnkBfr_append_dfr( bfr, buf, readed_byte );
			}
			return true;
		}
	}
	return false;
}

static bool
doHyperUpload( ZnkVarpAry post_vars,
		const char* new_filename, const char* upfile_varname, const char* ua, const char* evar_http_cookie, RanoModule mod )
{
	bool   is_updated = false;
	ZnkStr result_filename = ZnkStr_new( "result.dat" );
	bool   is_result = false;

	/***
	 * http:// �܂��� https:// �܂��� // �ȊO�͔F�߂Ȃ�.
	 * ����file:// �� ���[�J���t�@�C���ւ̑��΂���уt���p�X�̎w��͔F�߂Ȃ�.
	 * �����́u�Y�t�t�@�C���v���w�肷�ׂ��ł���A���̕������Z�L�����e�B�I�Ɍ��S�ł���.
	 */
	if( ZnkS_isBegin( new_filename, "http://" ) || ZnkS_isBegin( new_filename, "https://" ) || ZnkS_isBegin( new_filename, "//" ) ){
		int         status_code = -1;
		char        hostname[ 1024 ] = "";
		ZnkStr      req_urp = ZnkStr_new( "" );
		const char* src     = EstBase_getHostnameAndRequrp_fromEstVal( hostname, sizeof(hostname), req_urp, new_filename );
		const char* unesc_req_urp = ZnkStr_cstr( req_urp );
		ZnkVarpAry  cookie  = ZnkVarpAry_create( true );
		const char* parent_proxy  = EstConfig_parent_proxy();

		Znk_UNUSED( src );
		is_result = EstBase_download( hostname, unesc_req_urp, "upload",
				ua, cookie, evar_http_cookie,
				parent_proxy,
				result_filename, NULL, mod, &status_code );

		ZnkVarpAry_destroy( cookie );
		ZnkStr_delete( req_urp );

		if( is_result && status_code == 200 && replaceUploadFile_forMPFD( post_vars, upfile_varname, ZnkStr_cstr(result_filename) ) ){
			is_updated = true;
		}
	}

	ZnkStr_delete( result_filename );
	return is_updated;
}

static bool
func_proc_post_vars( ZnkVarpAry post_vars, void* arg, const char* content_type, bool* is_updated )
{
	EstPostInfo* post_info = Znk_force_ptr_cast( EstPostInfo*, arg );
	RanoCGIEVar* evar                 = post_info->evar_;
	ZnkHtpHdrs   htp_hdrs             = post_info->htp_hdrs_;
	RanoModule   mod                  = post_info->mod_;
	ZnkStr       given_authentick_key = post_info->given_authentick_key_;
	bool         is_authenticated     = false;
	const char*  moai_authentic_key   = EstConfig_authenticKey();
	
	if( ZnkStr_eq( given_authentick_key, moai_authentic_key ) ){
		is_authenticated = true;
	} else if( ZnkStr_empty( given_authentick_key ) ){
		ZnkVarp authkey = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false );
		if( authkey ){
			if( ZnkS_eq( ZnkVar_cstr(authkey), moai_authentic_key ) ){
				is_authenticated = true;
			}
		}
	}

	/***
	 * �O�̂���Moai_AuthenticKey�̂ݎ����.
	 */
	{
		const size_t idx = ZnkVarpAry_findIdx_byName( post_vars, "Moai_AuthenticKey", Znk_NPOS, false );
		if( idx != Znk_NPOS ){
			ZnkVarpAry_erase_byIdx( post_vars, idx );
			*is_updated = true;
		}
	}

	/***
	 * Easter Hyper-Upload
	 */
	if( is_authenticated ){
		bool    result = false;
		ZnkVarp hyper_upload_path = NULL;
		ZnkVarp hyper_upload_var  = NULL;
		/***
		 * ���̋@�\�� Authentic ��UI����̓��e�Ɍ���L��.
		 */
		if( IS_OK( hyper_upload_path = ZnkVarpAry_find_byName_literal( post_vars, "est_hyper_upload_path", false ) )){
			if( IS_OK( hyper_upload_var = ZnkVarpAry_find_byName_literal( post_vars, "est_hyper_upload_var", false ) )){
				const char* new_filename = ZnkVar_cstr( hyper_upload_path );
				if( !ZnkS_empty( new_filename ) ){
					const char* upfile_varname = ZnkVar_cstr(hyper_upload_var);
					ZnkVarp     ua_var         = ZnkHtpHdrs_find_literal( htp_hdrs->vars_, "User-Agent" );
					const char* ua             = ua_var ? ZnkHtpHdrs_val_cstr(ua_var,0) : "Firesexy";
					result = doHyperUpload( post_vars, new_filename, upfile_varname, ua, evar->http_cookie_, mod );
					if( !result ){
						/***
						 * Download�Ɏ��s�����Ǝv����.
						 * ���̏ꍇ�͉��������G���[���b�Z�[�W���o���ׂ�.
						 */
						RanoCGIMsg_initiate( true, NULL );
						RanoCGIMsg_begin();
						Znk_printf( "Easter Hyper Post : Error : [%s] �̃_�E�����[�h�Ɏ��s���܂���.\n", new_filename );
						RanoCGIMsg_end();
						RanoCGIMsg_finalize();
						return false;
					}
					if( result ){
						*is_updated = true;
					}
				}
			}
		}
		/***
		 * est_hyper_upload_path, est_hyper_upload_var �̂ݎ����.
		 */
		if( hyper_upload_path ){
			ZnkVarpAry_erase( post_vars, hyper_upload_path );
			*is_updated = true;
		}
		if( hyper_upload_var ){
			ZnkVarpAry_erase( post_vars, hyper_upload_var );
			*is_updated = true;
		}
	}
	return true;
}

void
EstPost_parsePostAndCookieVars( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg, ZnkHtpHdrs htp_hdrs,
		ZnkStr pst_str, /* XSS-safe */ const char* hostname, const char* req_urp, const char* target, RanoModule mod )
{
	static const char* varname_of_urlenc_body = NULL;
	bool   is_unescape_val = true;

	bool result = false;
	const char* tmpdir = EstConfig_getTmpDirCommon( true );

	ZnkBfr in_bfr  = ZnkBfr_create_null();
	ZnkStr given_authentick_key = ZnkStr_new( "" );
	size_t      content_length = 0;
	const char* content_type   = evar->content_type_;
	EstPostInfo post_info = { 0 };
	RanoCGIUtilFnca_procPostVars fnca_proc_post_vars = {
		func_proc_post_vars, NULL,
	};

	/***
	 * ���ۂɎg�p����boundary��Content-Type���ɋL�ڂ���Ă��镶����̑O��
	 * ����� -- ��t���������̂��g�p���Ȃ���΂Ȃ�Ȃ�.
	 */
	ZnkStr src_boundary = ZnkStr_new( "--" );
	ZnkStr dst_boundary = ZnkStr_new( "--" );

	ZnkS_getSzU( &content_length, evar->content_length_ );
	RanoCGIUtil_getStdInStr( in_bfr, content_length );

#if 0
	target = EstConfig_getTargetAndModule( &mod, hostname );
	if( target == NULL ){
		goto FUNC_END;
	}
#endif

	{
		post_info.evar_     = evar;
		post_info.htp_hdrs_ = htp_hdrs,
		post_info.mod_      = mod;
		post_info.given_authentick_key_ = given_authentick_key,
		fnca_proc_post_vars.arg_ = &post_info;
	}

	/***
	 * Moai_AuthenticKey �̂݊m�F����.
	 * ���̌�� URL QueryString�Ɋ܂܂��Moai/Easter�n�Ŏg�p����PostVar�����ׂăN���A���Ă����A
	 * �K�v��PostVar�����ēx�\�z����.
	 */
	{
		ZnkVarp authkey = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false );
		if( authkey ){
			ZnkStr_set( given_authentick_key, ZnkVar_cstr(authkey) );
		}
		ZnkVarpAry_clear( post_vars );
	}

	/***
	 * HtpHdr���ł����グ��.
	 */
	{
		ZnkVarpAry cookie   = ZnkVarpAry_create( true );
		char cookie_filename[ 256 ] = "";
		const char* ua = "Firesexy";

		Znk_snprintf( cookie_filename, sizeof(cookie_filename), "%s%s/cookie.txt", tmpdir, target );
		ZnkCookie_load( cookie, cookie_filename );
		initHtpHdr_forPost( htp_hdrs, hostname, req_urp, ua, cookie, evar );

		ZnkVarpAry_destroy( cookie );
	}

	ZnkHtpHdrs_scanContentTypeVal( content_type, src_boundary );

	/***
	 * boundary���Ɍ��߂邽�߁ARanoHtpModifier_modifySendHdrs�͐旧���čs���Ă����K�v������.
	 */
	if( mod ){
		ZnkMyf ftr_send = RanoModule_ftrSend( mod );
		const ZnkVarpAry ftr_vars = ZnkMyf_find_vars( ftr_send, "header_vars" );
		if( ftr_vars ){
			const ZnkVarp ua_var = ZnkVarpAry_find_byName( ftr_vars, "User-Agent", Znk_strlen_literal("User-Agent"), false );
			if( RanoHtpModifier_modifySendHdrs( htp_hdrs->vars_, ZnkVar_cstr(ua_var), pst_str ) ){
				ZnkStr_add( pst_str, "  RanoHtpModifier_modifySendHdrs is true\n" );
			} else {
				ZnkStr_add( pst_str, "  RanoHtpModifier_modifySendHdrs is false\n" );
			}
		}
	}
	ZnkHtpHdrs_scanContentType( htp_hdrs->vars_, dst_boundary );

	result = RanoCGIUtil_getPostVars_fromInBfr( post_vars, mod, htp_hdrs,
			in_bfr, content_length,
			content_type, ZnkStr_cstr(src_boundary), ZnkStr_cstr(dst_boundary),
			pst_str, varname_of_urlenc_body, is_unescape_val, &fnca_proc_post_vars );
	Znk_UNUSED(result);

	/***
	 * Header and Cookie filtering
	 * PostVar filtering�ɂ�����on_post�֐��̌Ăяo���ŁAheader��cookie_vars�̒l��
	 * �Q��/���H����悤�ȏ����ɂ��Ή����邽�߁A���� filtering�������ŌĂяo��.
	 */
	if( mod ){
		bool is_all_replace = true;
		ZnkVarpAry extra_vars     = NULL;
		RanoModule_filtHtpHeader(  mod, htp_hdrs->vars_ );
		RanoModule_filtCookieVars( mod, htp_hdrs->vars_, is_all_replace, extra_vars );
	}

#if 0
	/* Javascript ��ł�cookie�ݒ�Ȃǂł��蓾��. */
	if( evar->http_cookie_ ){
		ZnkVarp varp = ZnkHtpHdrs_find_literal( htp_hdrs->vars_, "Cookie" );
		ZnkStr str = ZnkHtpHdrs_val( varp, 0 );
		ZnkStr_addf( str, "; %s", evar->http_cookie_ );
	}
#endif

	ZnkStr_delete( given_authentick_key );

	ZnkStr_delete( src_boundary );
	ZnkStr_delete( dst_boundary );
	ZnkBfr_destroy( in_bfr );
}

void
EstPost_procPost( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg,
		EstPostFuncT_filterMain filterMain )
{
	const char* tmpdir = EstConfig_getTmpDirPID( true );
	struct ZnkHtpHdrs_tag htp_hdrs_instance = { 0 };
	ZnkHtpHdrs htp_hdrs = &htp_hdrs_instance;
	ZnkStr pst_str  = ZnkStr_new( "" );
	ZnkStr hostname = ZnkStr_new( "" );
	ZnkStr req_urp  = ZnkStr_new( "" );
	ZnkStr src      = ZnkStr_new( "" );
	const char* target = NULL;
	RanoModule  mod    = NULL;

	ZnkHtpHdrs_compose( htp_hdrs );

	RanoLog_printf( "procPost : est_val=[%s]\n", est_val );
	{
		char hostname_buf[ 1024 ] = "";
		ZnkStr_set( src, EstBase_getHostnameAndRequrp_fromEstVal( hostname_buf, sizeof(hostname_buf), req_urp, est_val ) );
		ZnkStr_set( hostname, hostname_buf );
	}

	target = EstConfig_getTargetAndModule( &mod, ZnkStr_cstr(hostname) );
	if( target == NULL ){
		goto FUNC_END;
	}

	EstPost_parsePostAndCookieVars( evar, post_vars, est_val, msg, htp_hdrs,
			pst_str, ZnkStr_cstr(hostname), ZnkStr_cstr(req_urp), target, mod );

	/***
	 * confirm�p
	 * ���b�Z�[�W���e�𐳊m�Ȃ��̂Ƃ��邽�߁Afiltering�����Ȃǂ��S�ďI�������
	 * �S�����\�z���Ȃ���΂Ȃ�Ȃ�.
	 */
#if 1
	{
		RanoLog_printf( "%s", ZnkStr_cstr(pst_str) );
	}
#endif

	{
		RanoTextType txt_type = RanoText_Binary;
		char content_type[ 1024 ] = "application/octet-stream";
		const char* parent_proxy  = EstConfig_parent_proxy();
		bool result = false;
		ZnkVarpAry  cookie        = ZnkVarpAry_create( true );
		ZnkStr result_filename = ZnkStr_newf( "./%sresult.dat", tmpdir );
		const char* cachebox = "./cachebox/";
		ZnkStr console_msg = ZnkStr_new( "" );

		{
			result = RanoHtpBoy_do_post( ZnkStr_cstr(hostname), ZnkStr_cstr(req_urp), target,
					htp_hdrs, post_vars,
					parent_proxy,
					tmpdir, cachebox, result_filename );
			if( !result ){
				ZnkStr_addf( msg,
						"  RanoHtpBoy_do_post : result=[%d] hostname=[%s] req_urp=[%s]<br>"
						"                     : target=[%s] parent_proxy=[%s] tmpdir=[%s]<br>"
						"                     : result_filename=[%s].<br>",
						result, ZnkStr_cstr(hostname), ZnkStr_cstr(req_urp), target, parent_proxy, tmpdir, ZnkStr_cstr(result_filename) );
			}

		}

		EstConfig_readRecvHdrSetCookie( mod, target );
		RanoHtpBoy_readRecvHdrVar( "Content-Type", content_type, sizeof(content_type), target, tmpdir );
		ZnkVarpAry_destroy( cookie );
		//Znk_printf( "ResultFile=[%s] src=[%s]\n", result_file, ZnkStr_cstr(src) );

		txt_type = RanoCGIUtil_getTextType( content_type );
		switch( txt_type ){
		case RanoText_HTML:
		case RanoText_CSS:
			ZnkStr_add( console_msg, "est_post success.\n" );
			EstBase_addConsoleMsg_HttpCookie( console_msg, evar->http_cookie_ );
			filterMain( ZnkStr_cstr(result_filename), ZnkStr_cstr(src), target, txt_type, console_msg, false );
			break;
		case RanoText_JS:
			//filterJS( ZnkStr_cstr(result_filename), ZnkStr_cstr(src), target, txt_type );
			break;
		default:
			break;
		}
		RanoCGIUtil_displayBinary( ZnkStr_cstr(result_filename), content_type );

		ZnkStr_delete( result_filename );
		ZnkStr_delete( console_msg );
	}

FUNC_END:
	ZnkStr_delete( pst_str );
	ZnkStr_delete( hostname );
	ZnkStr_delete( req_urp );
	ZnkStr_delete( src );
	ZnkHtpHdrs_dispose( htp_hdrs );
}

void
EstPost_procHyperPost( RanoCGIEVar* evar, ZnkVarpAry post_vars, const char* est_val, ZnkStr msg )
{
	struct ZnkHtpHdrs_tag htp_hdrs_instance = { 0 };
	ZnkHtpHdrs htp_hdrs = &htp_hdrs_instance;
	ZnkStr pst_str  = ZnkStr_new( "" );
	ZnkStr hostname = ZnkStr_new( "" );
	ZnkStr req_urp  = ZnkStr_new( "" );
	ZnkStr src      = ZnkStr_new( "" );
	ZnkStr original_url = ZnkStr_new( "" );
	const char* target = NULL;
	RanoModule  mod    = NULL;

	ZnkHtpHdrs_compose( htp_hdrs );

	ZnkStr_addf( msg, "procHyperPost : est_val=[%s]\n", est_val );
	{
		char hostname_buf[ 1024 ] = "";
		ZnkStr_set( src, EstBase_getHostnameAndRequrp_fromEstVal( hostname_buf, sizeof(hostname_buf), req_urp, est_val ) );
		ZnkStr_set( hostname, hostname_buf );
	}

	target = EstConfig_getTargetAndModule( &mod, ZnkStr_cstr(hostname) );
	if( target == NULL ){
		goto FUNC_END;
	}

	{
		ZnkVarp varp = ZnkVarpAry_find_byName_literal( post_vars, "est_original_url", false );
		if( varp ){
			ZnkStr_set( original_url, ZnkVar_cstr(varp) );
		}
	}

	EstPost_parsePostAndCookieVars( evar, post_vars, est_val, msg, htp_hdrs,
			pst_str, ZnkStr_cstr(hostname), ZnkStr_cstr(req_urp), target, mod );

	/***
	 * confirm�p
	 * ���b�Z�[�W���e�𐳊m�Ȃ��̂Ƃ��邽�߁Afiltering�����Ȃǂ��S�ďI�������
	 * �S�����\�z���Ȃ���΂Ȃ�Ȃ�.
	 */
	{
		ZnkStr str = ZnkStr_new( "" );
		size_t idx;
		const size_t size = ZnkVarpAry_size( post_vars );
		ZnkBird bird = ZnkBird_create( "#[", "]#" );
		ZnkStr  val  = ZnkStr_new( "" );
		ZnkStr  input_hiddens = ZnkStr_new( "" );
		ZnkVarp varp = NULL;
		bool has_binary_attachment = false;
		bool has_title = false;

		ZnkStr_add( str, "HTTP Header Confirmation.\n" );
		RanoCGIUtil_addHdrVarsStr( str, htp_hdrs->vars_ ); /* XSS-safe */
		ZnkStr_add( str, "\n" );

		ZnkBird_regist( bird, "xhr_dmz",  EstConfig_XhrDMZ() );
		ZnkBird_regist( bird, "post_dst", ZnkStr_cstr(src) );
		if( ZnkS_isBegin( evar->content_type_, "multipart/form-data;" ) ){
			ZnkBird_regist( bird, "post_enctype", "multipart/form-data" );
		} else {
			ZnkBird_regist( bird, "post_enctype", "application/x-www-form-urlencoded" );
		}

		{
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };
			ZnkSRef_set_literal( &old_ptn, "\n" );
			ZnkSRef_set_literal( &new_ptn, "<br>\n" );
			ZnkStrEx_replace_BF( str,     0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
			ZnkStrEx_replace_BF( pst_str, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
		}
		ZnkBird_regist( bird, "hdr_vars_str", ZnkStr_cstr(str) );
		ZnkBird_regist( bird, "pst_vars_str", ZnkStr_cstr(pst_str) );
		ZnkBird_regist( bird, "Moai_AuthenticKey", EstConfig_authenticKey() );

		if( ZnkVarpAry_find_byName_literal( post_vars, "resto", false ) ){
			ZnkBird_regist( bird, "send_button_name", "�ԐM����" );
		} else {
			ZnkBird_regist( bird, "send_button_name", "�X���b�h�𗧂Ă�" );
		}

		for( idx=0; idx<size; ++idx ){
			const char* key  = NULL;
			varp = ZnkVarpAry_at( post_vars, idx );
			key  = ZnkVar_name_cstr( varp );
			if( EstConfig_isInputHiddensPostVarNames( target, key ) ){
				/* this is hiddens post var */
				if( ZnkVar_prim_type( varp ) == ZnkPrim_e_Str ){
					ZnkStr_set( val, ZnkVar_cstr( varp ) );
					ZnkHtpURL_negateHtmlTagEffection( val ); /* for XSS */
					ZnkStr_addf( input_hiddens, "<input type=hidden name=%s value=\"%s\">\n", key, ZnkStr_cstr(val) );
				}
			} else {
				/* this is not hiddens post var */
				if( ZnkS_eq( key, "sub" ) ){
					has_title = true;
				}
				if( ZnkVar_prim_type( varp ) == ZnkPrim_e_Str ){
					ZnkStr_set( val, ZnkVar_cstr( varp ) );
				} else if( ZnkVar_prim_type( varp ) == ZnkPrim_e_Bfr ){
					ZnkStr_set( val, ZnkVar_misc_cstr( varp, "filename" ) ); /* filename */
					has_binary_attachment = true;
				}
				ZnkHtpURL_negateHtmlTagEffection( val ); /* for XSS */
				ZnkBird_regist( bird, key, ZnkStr_cstr(val) );
			}
		}
		ZnkBird_regist( bird, "input_hiddens", ZnkStr_cstr(input_hiddens) );

		if( ZnkStr_empty(original_url) ){
			ZnkBird_regist( bird, "original_url", "Unknown" );
			ZnkBird_regist( bird, "summary",      "" );
		} else {
			char cache_path[ 256 ] = "";
			ZnkBird_regist( bird, "original_url", ZnkStr_cstr(original_url) );
			Znk_snprintf( cache_path, sizeof(cache_path), "cachebox/%s/%s", target, ZnkStr_cstr(original_url) );
			EstUI_getSummary( bird, cache_path );
		}

		{
			ZnkStr hint_table = EstHint_getHintTable( "hyper_post" );
			if( hint_table ){
				ZnkBird_regist( bird, "hint_table", ZnkStr_cstr(hint_table) );
			} else {
				ZnkBird_regist( bird, "hint_table", "" );
			}
		}

		ZnkHtpURL_negateHtmlTagEffection( msg ); /* for XSS */
		{
			ZnkSRef old_ptn = { 0 };
			ZnkSRef new_ptn = { 0 };
			ZnkSRef_set_literal( &old_ptn, "\n" );
			ZnkSRef_set_literal( &new_ptn, "<br>\n" );
			ZnkStrEx_replace_BF( msg, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 
		}
		ZnkBird_regist( bird, "msg",           ZnkStr_cstr(msg) );

		if( has_binary_attachment ){
			if( has_title ){
				RanoCGIUtil_printTemplateHTML( evar, bird, "templates/post_futaba.html" );
			} else {
				RanoCGIUtil_printTemplateHTML( evar, bird, "templates/post_futaba_img_thre.html" );
			}
		} else {
			/* futaba img �^�C�v�Ƃ݂Ȃ� */
			RanoCGIUtil_printTemplateHTML( evar, bird, "templates/post_futaba_img.html" );
		}
		ZnkBird_destroy( bird );

		ZnkStr_delete( input_hiddens );
		ZnkStr_delete( val );
		ZnkStr_delete( str );
	}

FUNC_END:
	ZnkStr_delete( pst_str );
	ZnkStr_delete( hostname );
	ZnkStr_delete( req_urp );
	ZnkStr_delete( src );
	ZnkStr_delete( original_url );
	ZnkHtpHdrs_dispose( htp_hdrs );
}
