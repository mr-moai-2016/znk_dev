#include <Moai_plugin_dev.h>
#include <Znk_stdc.h>
#include <Znk_def_util.h>
#include <Znk_cookie.h>
#include <Znk_htp_hdrs.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_mem_find.h>
#include <string.h>

Znk_INLINE ZnkVarp
refPostVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "post_vars", var_name );
}
Znk_INLINE ZnkVarp
refCookieVar( ZnkMyf myf, const char* var_name )
{
	return ZnkMyf_refVar( myf, "cookie_vars", var_name );
}

MoaiPluginState
on_post( ZnkMyf ftr_send, ZnkVarpAry hdr_vars, ZnkVarpAry post_vars )
{
	bool referer_with_bbs_key = false;
	ZnkVarp yuki = refCookieVar( ftr_send, "yuki" );
	if( yuki ){
		Znk_printf_e( "on_post yuki=[%s]\n", ZnkVar_cstr(yuki) );
		if( ZnkVar_str_leng(yuki) == 0 ){
			/***
			 * ���񓊍e�ƍl������.
			 * ���̂Ƃ����e�̊m�F��ʂ��\������邪�A���̕����ɂ�����POST�ϐ�MESSAGE�̓��e��
			 * " ������ &quot; �փT�j�^�C�Y����Ȃ����߁A������T�j�^�C�Y���Ă����K�v������.
			 * ���A���񓊍e�ȊO�̒ʏ�̓��e�ł� " �� &quot;�ɃT�j�^�C�Y����邽�߁A���̕K�v�͂Ȃ��A
			 * �܂������� &quot; �� &amp;quot; �ɃT�j�^�C�Y����Ă��܂����߁A���̒u�����������Ă͂Ȃ�Ȃ�.
			 */
			ZnkVarp message = ZnkVarpAry_find_byName( post_vars, "MESSAGE", Znk_strlen_literal("MESSAGE"), false );
			if( message ){
				ZnkStr message_str = ZnkVar_str( message );
				Znk_printf_e( "message before=[%s]\n", ZnkStr_cstr(message_str) );
				ZnkStrEx_replace_BF( message_str, 0,
						"\"", 1,
						"&quot;", Znk_strlen_literal("&quot;"), 
						Znk_NPOS, Znk_NPOS );
				Znk_printf_e( "message after=[%s]\n", ZnkStr_cstr(message_str) );
			}
		}
	}

	/***
	 * Referer�C��.
	 *
	 * ���݁A5ch�ł�POST����Referer�͈ȉ��̂悤�łȂ���΂Ȃ�Ȃ�.
	 * http://<hostname>/<cgi_script>
	 * ��. http://lavender.5ch.net/test/read.cgi
	 * http://���K�{�ŁA���ꂪ�Ȃ��ƁuReferer���ςł��v�G���[�ƂȂ�.
	 * 
	 * <cgi_script>�ȍ~�͋�ł��悢���̏��܂Ŋ܂܂�Ă��Ă��悢.
	 * ���Ƃ��΁Ahttp://<hostname>/<cgi_script>/<bbs>/<key>/l50
	 * �ȂǂƂȂ��Ă��Ă��悢.
	 *
	 * Moai�����[�J���v���L�V���[�h�Ŏg�����ꍇ�AReferer�l�̓u���E�U�̐ݒ肵���l�����̂܂܈����p�����߁A
	 * <bbs>/<key>/l50 �Ȃǂ܂Ŋ܂ޒl�ƂȂ��Ă���.
	 * ����AEaster�����Post�̏ꍇ�A����ł�Referer�l�� http://<hostname>/<cgi_script>
	 * ���ݒ肳���悤�ɂȂ��Ă���.
	 *
	 * ����ł͂��̂܂܂ł��ǂ���ł����Ȃ����A����5ch�̎d�l���ς����<bbs>/<key>�̕����܂�
	 * �K�{�ƂȂ����ꍇ�́A�ȉ���add_bbs_key��L���ɂ����Easter�ł�����ɑΉ��ł���.
	 */
	if( referer_with_bbs_key ){
		ZnkVarp bbs  = ZnkVarpAry_find_byName_literal( post_vars, "bbs", false );
		ZnkVarp key  = ZnkVarpAry_find_byName_literal( post_vars, "key", false );
		if( bbs && key ){
			ZnkVarp varp = ZnkHtpHdrs_find_literal( hdr_vars, "Referer" );
			if( varp ){
				ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
				/* ����<bbs>/<key>���t������Ă���悤�ȏꍇ��e�����߁A������.cgi�ł��邩���`�F�b�N����. */
				if( ZnkStr_isEnd( val, ".cgi" ) ){
					ZnkStr_addf( val, "/%s/%s/l50", ZnkVar_cstr(bbs), ZnkVar_cstr(key) );
				}
			}
		}
	}
	return true;
}

static int
landmarkMoai( ZnkStr str, void* arg )
{
	ZnkStr_add( str, " <font size=\"-1\" color=\"#800080\">via Moai</font>" );
	return 1;
}

MoaiPluginState
on_response( ZnkMyf ftr_send,
		ZnkVarpAry hdr_vars, ZnkStr text, const char* req_urp )
{
	if( text ){
		ZnkSRef old_ptn = { 0 };
		ZnkSRef new_ptn = { 0 };
		size_t  old_occ[ 256 ] = { 0 };

		ZnkSRef_set_literal( &old_ptn, "</div><div class=\"post\" id=" );
		ZnkSRef_set_literal( &new_ptn, "</div>\n<div class=\"post\" id=" );
		ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_, old_ptn.leng_ );
		ZnkStrEx_replace_BMS( text, 0,
				old_ptn.cstr_, old_ptn.leng_, old_occ,
				new_ptn.cstr_, new_ptn.leng_,
				Znk_NPOS, Znk_NPOS );

		ZnkSRef_set_literal( &old_ptn, "</div><form method=\"POST\"" );
		ZnkSRef_set_literal( &new_ptn, "</div>\n<form method=\"POST\"" );
		ZnkMem_getLOccTable_forBMS( old_occ, (uint8_t*)old_ptn.cstr_, old_ptn.leng_ );
		ZnkStrEx_replace_BMS( text, 0,
				old_ptn.cstr_, old_ptn.leng_, old_occ,
				new_ptn.cstr_, new_ptn.leng_,
				Znk_NPOS, Znk_NPOS );

		/* 5ch : via Moai landmarking */
		ZnkStrPtn_invokeInQuote( text,
				"<h1 class=\"title\">", "\n",
				NULL, NULL,
				landmarkMoai, NULL, false );

		/* 5ch smart phone : via Moai landmarking */
		ZnkSRef_set_literal( &old_ptn, "<div id=\"title\" class=\"threadview_response_title\"></div>" );
		ZnkSRef_set_literal( &new_ptn, "<span id=\"title\" class=\"threadview_response_title\"></span> <font size=\"-1\" color=\"#800080\">via Moai</font>" );
		ZnkStrEx_replace_BF( text, 0, old_ptn.cstr_, old_ptn.leng_, new_ptn.cstr_, new_ptn.leng_, Znk_NPOS, Znk_NPOS ); 

	}
	return true;
}

