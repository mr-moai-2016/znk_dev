#include "Moai_post.h"
#include "Moai_io_base.h"
#include "Moai_server_info.h"

#include <Rano_log.h>
#include <Rano_htp_modifier.h>
#include <Rano_post.h>
#include <Rano_cgi_util.h>

#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_bfr.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_htp_hdrs.h>
#include <Znk_htp_post.h>
#include <string.h>


static bool st_post_confirm = true;


static void
addHdrFirstLine( ZnkStr str, const ZnkStrAry hdr1st )
{
	ZnkStr_addf( str, "  RequestLine[%s][%s][%s]\n",
			ZnkStrAry_at_cstr(hdr1st,0),
			ZnkStrAry_at_cstr(hdr1st,1),
			ZnkStrAry_at_cstr(hdr1st,2) );
}

Znk_INLINE void
appendBfr_byCStr( ZnkBfr bfr, const char* cstr )
{
	const size_t leng = strlen( cstr );
	ZnkBfr_append_dfr( bfr, (uint8_t*)cstr, leng );
}


/*****************************************************************************/

MoaiPostMode
MoaiPost_decidePostMode2( ZnkMyf hosts, const char* dst_hostname )
{
	MoaiPostMode post_mode = MoaiPostMode_e_SendDirect;
	ZnkStrAry   post_confirm_hosts = ZnkMyf_find_lines( hosts, "post_confirm_hosts" );

	if( ZnkStrAry_find_isMatch( post_confirm_hosts, 0, dst_hostname, Znk_NPOS, ZnkS_isMatchSWC ) != Znk_NPOS ){
		post_mode = MoaiPostMode_e_Confirm;
	}
	if( !st_post_confirm ){
		post_mode = MoaiPostMode_e_SendDirect;
	}
	return post_mode;
}
bool
MoaiPost_isPostConfirm( void )
{
	return st_post_confirm;
}
void
MoaiPost_setPostConfirm( bool post_confirm )
{
	st_post_confirm = post_confirm;
}

bool
MoaiPost_parsePostAndCookieVars( ZnkSocket sock, MoaiFdSet mfds,
		ZnkStr str,
		const size_t hdr_size, ZnkHtpHdrs htp_hdrs,
		size_t content_length, ZnkBfr stream,
		ZnkVarpAry post_vars,
		const RanoModule mod, bool is_authenticated )
{
	bool result = false;
	const char* content_type = NULL;
	ZnkStr pst_str = ZnkStr_new( "" );
	ZnkBfr in_bfr  = ZnkBfr_create_null();
	static const char* varname_of_urlenc_body = "Moai_HtpPostTextBody";
	static const bool  is_unescape_val = true;

	/***
	 * ���ۂɎg�p����boundary��Content-Type���ɋL�ڂ���Ă��镶����̑O��
	 * ����� -- ��t���������̂��g�p���Ȃ���΂Ȃ�Ȃ�.
	 */
	ZnkStr src_boundary = ZnkStr_new( "--" );
	ZnkStr dst_boundary = ZnkStr_new( "--" );

	content_type = ZnkHtpHdrs_scanContentType( htp_hdrs->vars_, src_boundary );
	if( content_type == NULL ){
		/***
		 * Content-Type �����݂��Ȃ�.
		 * ���̂悤��POST�͂ǂ������ׂ���?
		 * �����̏ꍇ�A�{���G���[�Ƃ��ׂ��Ǝv���邪�A
		 * �����ł͂Ƃ肠���� application/x-www-form-urlencoded �Ƃ��Ď�舵��.
		 */
		content_type = "application/x-www-form-urlencoded";
	}

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

	/***
	 * Note. body���ɂ���.
	 *
	 * 1. GET�̏ꍇ
	 *   body���͑��݂��Ȃ�.
	 *
	 * 2. POST(multipart/form-data)�̏ꍇ
	 *   ���݂���.
	 *   boundary������ŕ��������`���ł���.
	 *
	 * 3. POST(application/x-www-form-urlencoded)�̏ꍇ
	 *   ���݂���.
	 *   ���̏ꍇ�AURL�� ? �ɑ�������������ꍇ�́A���ϐ� QUERY_STRING �Ƃ��Ď擾�ł��邪�A
	 *   �����body���Ƃ͂Ȃ�Ȃ�.
	 *   ����Aform�^�O���Ŏw�肵��input��textarea�f�[�^�Ȃǂ�body���ƂȂ��Ă���.
	 *   ���̏ꍇ��body���́AQUERY_STRING�Ɠ����`����&�ŘA�����ꂽ�`�ƂȂ��Ă���.
	 */
	{
		size_t remain_size = 0;
		if( ZnkBfr_size( stream ) < hdr_size + content_length ){
			remain_size = hdr_size + content_length - ZnkBfr_size( stream );
		} else {
			remain_size = 0;
		}
		if( remain_size ){
			size_t result_size = 0;
			MoaiIO_recvBySize( stream, sock, mfds, remain_size, &result_size );
		}
	}
	ZnkBfr_append_dfr( in_bfr, ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size );

	result = RanoCGIUtil_getPostVars_fromInBfr( post_vars, mod, htp_hdrs,
			in_bfr, content_length,
			content_type, ZnkStr_cstr(src_boundary), ZnkStr_cstr(dst_boundary),
			pst_str, varname_of_urlenc_body, is_unescape_val, NULL );

	/***
	 * Header and Cookie filtering
	 * PostVar filtering�ɂ�����on_post�֐��̌Ăяo���ŁAheader��cookie_vars�̒l��
	 * �Q��/���H����悤�ȏ����ɂ��Ή����邽�߁A���� filtering�������ŌĂяo��.
	 */
	if( mod ){
		const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
		ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
		RanoModule_filtHtpHeader(  mod, htp_hdrs->vars_ );
		RanoModule_filtCookieVars( mod, htp_hdrs->vars_, is_all_replace, extra_vars );
	}

	/***
	 * confirm�p
	 * ���b�Z�[�W���e�𐳊m�Ȃ��̂Ƃ��邽�߁Afiltering�����Ȃǂ��S�ďI�������
	 * �S�����\�z���Ȃ���΂Ȃ�Ȃ�.
	 */
	{
		ZnkStr_add( str, "<p><b>Moai : HTTP Header Confirmation.</b></p>" );
		addHdrFirstLine( str, htp_hdrs->hdr1st_ );
		ZnkStr_add( str, "\n" );
		RanoCGIUtil_addHdrVarsStr( str, htp_hdrs->vars_ );
		ZnkStr_add( str, "\n" );

		ZnkStr_add( str, "<p><b>Moai : POST Variable Confirmation.</b></p>" );
		ZnkStr_addf( str, "  This is %s\n", content_type );
		ZnkStr_add( str, ZnkStr_cstr(pst_str) );
	}

	ZnkStr_delete( src_boundary );
	ZnkStr_delete( dst_boundary );
	ZnkStr_delete( pst_str );
	ZnkBfr_destroy( in_bfr );

	return result;
}

bool
MoaiPost_sendRealy( MoaiInfoID info_id,
		bool is_realy_send, ZnkSocket O_sock, MoaiFdSet mfds )
{
	bool result = false;
	ZnkBfr stream_fnl = ZnkBfr_create_null();
	MoaiInfo* info = MoaiInfo_find( info_id );

	{
		const char* content_type = "";
		ZnkStr boundary = ZnkStr_new( "--" );
		ZnkHtpHdrs_extendToStream( info->hdrs_.hdr1st_, info->hdrs_.vars_, stream_fnl, true );
		content_type = ZnkHtpHdrs_scanContentType( info->hdrs_.vars_, boundary );
		if( content_type == NULL ){
			/***
			 * Content-Type �����݂��Ȃ�.
			 * ���̂悤��POST�͂ǂ������ׂ���?
			 * �����̏ꍇ�A�{���G���[�Ƃ��ׂ��Ǝv���邪�A
			 * �����ł͂Ƃ肠���� application/x-www-form-urlencoded �Ƃ��Ď�舵��.
			 */
			content_type = "application/x-www-form-urlencoded";
		}
		if( ZnkS_isBegin_literal( content_type, "multipart/form-data" ) ){
			RanoPost_extendPostVarsToStream_forMPFD( info->vars_, stream_fnl, ZnkStr_cstr(boundary) );
		} else if( ZnkS_isBegin_literal( content_type, "application/x-www-form-urlencoded" )
//	        	|| ZnkS_isBegin_literal( content_type, "application/ocsp-request" )
		        || ZnkS_isBegin_literal( content_type, "text/xml" ) ){
			ZnkVarp varp = ZnkVarpAry_find_byName( info->vars_, "Moai_HtpPostTextBody", Znk_NPOS, false );
			if( varp ){
				const char* val = ZnkStr_cstr( varp->prim_.u_.str_ );
				appendBfr_byCStr( stream_fnl, val );
			}
		}
		ZnkStr_delete( boundary );
	}
	if( is_realy_send ){
		ZnkVarp varp = ZnkHtpHdrs_find_literal( info->hdrs_.vars_, "Host" );
		if( varp ){
			ZnkFdSet fdst_observe = MoaiFdSet_fdst_observe_r( mfds );
			ZnkBfr_swap( info->stream_, stream_fnl );
			ZnkSocket_send( O_sock, ZnkBfr_data(info->stream_), ZnkBfr_size(info->stream_) );
			if( !ZnkFdSet_set( fdst_observe, O_sock ) ){
				MoaiFdSet_reserveConnectSock( mfds, O_sock );
			}
		}
	}
	result = true;
	ZnkBfr_destroy( stream_fnl );
	return result;
}

