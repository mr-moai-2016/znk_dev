#include "Moai_context.h"
#include "Moai_io_base.h"
#include "Moai_connection.h"
#include "Moai_info.h"
#include "Moai_fdset.h"
#include "Moai_post.h"
#include "Moai_server_info.h"
#include "Moai_cgi.h"

#include <Rano_module_ary.h>
#include <Rano_log.h>
#include <Rano_parent_proxy.h>
#include <Rano_myf.h>
#include <Rano_file_info.h>
#include <Rano_cgi_util.h>

#include <Znk_str.h>
#include <Znk_str_ptn.h>
#include <Znk_str_ex.h>
#include <Znk_str_path.h>
#include <Znk_stdc.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_bird.h>
#include <Znk_dir.h>
#include <Znk_envvar.h>
#include <Znk_htp_util.h>
#include <Znk_net_ip.h>
#include <string.h>

#define SJIS_HYO "\x95\x5c" /* 表 */
#define SJIS_NOU "\x94\x5c" /* 能 */

static const char* st_version_str = "2.0";
static size_t      st_input_ui_idx = 0;

static void
printInputUI_Text( ZnkStr html,
		const char* varname, const char* current_val, const char* new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";
	ZnkStr_addf( html,
			"<tr class=\"%s\"><td><b>%s</b></td><td><input class=MstyInputField type=text name=%s value=\"%s\" size=30 %s></td><td><font size=-1>%s</font></td></tr>\n",
			class_name,
			varname, varname, new_val, readonly_str, destination );
	++st_input_ui_idx;
}
static void
printInputUI_CheckBox( ZnkStr html,
		const char* varname, bool current_val, bool new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";

	ZnkStr_addf( html,
			"<tr class=\"%s\"><td><b>%s</b></td><td><input type=checkbox name=%s value=\"on\" %s %s></td><td><font size=-1>%s</font></td></tr>\n",
			class_name,
			varname, varname, new_val ? "checked" : "", readonly_str, destination );
	++st_input_ui_idx;
}
static void
printInputUI_Int( ZnkStr html,
		const char* varname, int current_val, int new_val, bool is_enable,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "readonly disabled=true";
	ZnkStr_addf( html,
			"<tr class=\"%s\"><td><b>%s</b></td><td><input class=MstyInputField type=text name=%s value=\"%d\" %s></td><td><font size=-1>%s</font></td></tr>\n",
			class_name,
			varname, varname, new_val, readonly_str, destination );
	++st_input_ui_idx;
}


static void
printInputUI_SelectBox( ZnkStr html,
		const char* varname, const char* current_val, const char* new_val, bool is_enable,
		ZnkStrAry str_list,
		const char* destination )
{
	const char* class_name = ( st_input_ui_idx % 2 ) ? "MstyItemOdd" : "MstyItemEvn";
	const char* readonly_str = is_enable ? "" : "disabled=true";
	const size_t size = ZnkStrAry_size( str_list );
	size_t idx;
	bool is_selected = false;
	const char* val = "";

	ZnkStr_addf( html,
			"<tr class=\"%s\"><td><b>%s</b></td><td><select class=MstyInputField name=%s %s>\n",
			class_name,
			varname, varname, readonly_str );

	for( idx=0; idx<size; ++idx ){
		val = ZnkStrAry_at_cstr( str_list, idx );
		is_selected = ZnkS_eq( val, current_val );
		ZnkStr_addf( html, "<option%s>%s</option>", is_selected ? " selected" : "", val );
	}
	ZnkStr_addf( html,
			"</td><td><font size=-1>%s</font></td></tr>\n", destination );
	++st_input_ui_idx;
}

static void
makeHeader( ZnkStr html, const char* title )
{
	ZnkStr_add( html, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	ZnkStr_add( html, "<html><head>\n" );
	ZnkStr_add( html, "\t<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	ZnkStr_add( html, "\t<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	ZnkStr_add( html, "\t<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	ZnkStr_add( html, "\t<link href=\"/msty.css\" rel=\"stylesheet\" type=\"text/css\" />\n" );
	ZnkStr_addf( html, "\t<title>%s</title>\n", title );
	ZnkStr_add( html, "</head>\n" );
}

static int
printConfig( ZnkSocket sock, ZnkStrAry result_msgs, uint32_t peer_ipaddr )
{
	int         ret                   = 0;
	char        proxy[ 1024 ]         = "";
	const char* parent_proxy_hostname = NULL;
	ZnkStr      html                  = ZnkStr_new( "" );
	ZnkStrAry   str_list              = ZnkStrAry_create( true );
	ZnkStr      uxs_tmp               = ZnkStr_new( "" );
	const bool  is_enable_log_file    = MoaiServerInfo_is_enable_log_file();
	const bool  is_enable_log_verbose = MoaiServerInfo_is_enable_log_verbose();
	const char* filters_dir           = MoaiServerInfo_filters_dir();
	const char* profile_dir           = MoaiServerInfo_profile_dir();
	const char* moai_authentic_key    = MoaiServerInfo_authenticKey();

	makeHeader( html, "Moai Configuration" );
	ZnkStr_add( html, "<body>\n" );
	ZnkStr_add( html, "<b><img src=\"moai.png\"> Moaiエンジン設定</b><br>\n" );
	ZnkStr_add( html, "<a class=MstyNowSelectedLink href=\"/config\"    >Moai基本設定</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=\"/sysconfig\" >Moaiセキュリティ設定</a> &nbsp;\n" );
	ZnkStr_add( html, "<br> <br>\n" );

	/* ==== Instantly Updatable variables ==== */
	ZnkStr_add( html, "<b>Moai基本設定</b><br>\n" );
	st_input_ui_idx = 0;
	ZnkStr_add( html, "<form action=\"/config\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
	parent_proxy_hostname = RanoParentProxy_getHostname();
	if( ZnkS_empty(parent_proxy_hostname) ){
		ZnkS_copy( proxy, sizeof(proxy), "NONE", Znk_NPOS );
	} else {
		Znk_snprintf( proxy, sizeof(proxy), "%s:%u",
				parent_proxy_hostname, RanoParentProxy_getPort() );
	}

	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<thead>\n" );
	ZnkStr_add( html, "<tr><th>変数名</th><th>現在の値</th><th>説明</th></tr>\n" );
	ZnkStr_add( html, "</thead>\n" );
	ZnkStr_add( html, "<tbody>\n" );

	{
		/* Parent Proxy */
		ZnkStrAry parent_proxys;
		RanoParentProxy_loadCandidate( "parent_proxy.txt" );
		parent_proxys = RanoParentProxy_getParentProxys();
		printInputUI_SelectBox( html,
				"parent_proxy", proxy, proxy, true,
				parent_proxys,
				"外部プロキシを使用する場合選択します.<br>(NONEの場合使用しません)." );
	}

	printInputUI_Text( html, "filters_dir", filters_dir, filters_dir, true,
			"filtersフォルダのパスを指定します.<br>(絶対パスまたはmoaiディレクトリからの相対パスです)." );
	printInputUI_Text( html, "profile_dir", profile_dir, profile_dir, true,
			"profileフォルダのパスを指定します.<br>(絶対パスまたはmoaiディレクトリからの相対パスです)." );

	printInputUI_CheckBox( html,
			"post_confirm", MoaiPost_isPostConfirm(), MoaiPost_isPostConfirm(), true,
			"Moaiをローカルプロキシとして使用した場合、<br>Post時に変数の内容を" SJIS_HYO "示した確認画面を出します." );

	printInputUI_CheckBox( html,
			"enable_log_file", is_enable_log_file, is_enable_log_file, true,
			"Moaiのログファイル書き出しモードのOn/Offです" );

	printInputUI_CheckBox( html,
			"enable_log_verbose", is_enable_log_verbose, is_enable_log_verbose, true,
			"Moaiのログファイルをさらに詳細に書き出すか否かです" );

#if 0
	/* もはやユーザが設定するまでもなく常時off(非ブロッキング)でよかろう */
	printInputUI_CheckBox( html,
			"blocking_mode", MoaiConnection_isBlockingMode(), MoaiConnection_isBlockingMode(), true,
			"Connection Blocking Mode(on/off)." );
#endif

	ZnkStr_add(  html, "<tr colspan=4><td><br>\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_Update       value=\"update\">\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"更新\">\n" );
	ZnkStr_add(  html, "</td></tr>\n" );

	ZnkStr_add( html, "</tbody>\n" );
	ZnkStr_add( html, "</table>\n" );

	ZnkStr_add(  html, "</form>\n" );

	ZnkStr_add(  html, "&nbsp;\n" );

	if( result_msgs && ZnkStrAry_size(result_msgs) ){
		size_t idx;
		const size_t size = ZnkStrAry_size( result_msgs );
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>\n" );
		for( idx=0; idx<size; ++idx ){
			ZnkStr_set( uxs_tmp, ZnkStrAry_at_cstr( result_msgs, idx ) );
			ZnkHtpURL_negateHtmlTagEffection( uxs_tmp );
			ZnkStr_addf( html, "%s<br>\n", ZnkStr_cstr( uxs_tmp ) ); /* XSS-safe */
		}
		ZnkStr_add( html, "</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}

	/* ==== Restart ==== */
	if( MoaiServerInfo_you_need_to_restart_moai() ){
		//ZnkStr_add( html, "<font color=#ff0000><b>You need to restart Moai for adapting to system configuration.</b><br>\n" );
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>　この設定の変更を反映させるにはMoaiを再起動させる必要があります.</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}
	ZnkStr_add(  html, "<form action=\"/config\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_RestartServer value=\"reboot\">\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey  value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"Moaiを再起動\">\n" );
	ZnkStr_add(  html, "</form>\n" );

	//ZnkStr_add( html, "<hr>\n" );
	ZnkStr_add( html, "<br>\n" );

	ZnkStr_add( html, "<a class=MstyElemLink href=\"/\">Moaiトップページ</a>\n" );
	ZnkStr_add( html, "</body></html>\n" );

	ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(html) );
	ZnkStr_delete( html );
	ZnkStr_delete( uxs_tmp );
	ZnkStrAry_destroy( str_list );
	return ret;
}

static int
printSysConfig( ZnkSocket sock, ZnkStrAry result_msgs, uint32_t peer_ipaddr )
{
	int         ret                = 0;
	bool        sys_config_enable  = true;
	ZnkStr      html               = ZnkStr_new( "" );
	ZnkStrAry   str_list           = ZnkStrAry_create( true );
	const char* server_name        = MoaiServerInfo_server_name();
	uint16_t    moai_port          = MoaiServerInfo_port();
	uint16_t    xhr_dmz_port       = MoaiServerInfo_XhrDMZPort();
	const char* acceptable_host    = MoaiServerInfo_acceptable_host();
	const char* moai_authentic_key = MoaiServerInfo_authenticKey();

	makeHeader( html, "Moai System Configuration" );
	ZnkStr_add( html, "<body>\n" );

	ZnkStr_add( html, "<b><img src=\"moai.png\"> Moaiエンジン設定</b><br>\n" );
	ZnkStr_add( html, "<a class=MstyElemLink        href=\"/config\"    >Moai基本設定</a> &nbsp;\n" );
	ZnkStr_add( html, "<a class=MstyNowSelectedLink href=\"/sysconfig\" >Moaiセキュリティ設定</a> &nbsp;\n" );
	ZnkStr_add( html, "<br> <br>\n" );

	/* ==== Security Configuration( 外部マシンからの更新は不可とする ) ==== */
	if( peer_ipaddr == 0x0100007f || peer_ipaddr == MoaiServerInfo_private_ip() ){
		/* Loopback接続 */
		sys_config_enable = true;
	} else {
		/* 他のマシンからの接続 */
		sys_config_enable = false;
	}
	ZnkStr_add( html, "<b>Moaiセキュリティ設定</b><br>\n" );
	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<tr><td colspan=2>\n" );
	ZnkStr_add( html, "<font size=-1 color=red>　注: このパネルはエキスパート向けのもので通常は利用する必要はありません。<br>" );
	ZnkStr_add( html, "　またここでの設定は、セキュリティー上の理由でlocalhostからのみ可" SJIS_NOU "となります.</font>\n" );
	ZnkStr_add( html, "</td></tr>\n" );
	ZnkStr_add( html, "</table>\n" );

	st_input_ui_idx = 0;
	ZnkStr_add( html, "<form action=\"/sysconfig\" method=\"POST\" enctype=\"multipart/form-data\">\n" );

	ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
	ZnkStr_add( html, "<thead>\n" );
	ZnkStr_add( html, "<tr><th>変数名</th><th>現在の値</th><th>説明</th></tr>\n" );
	ZnkStr_add( html, "</thead>\n" );
	ZnkStr_add( html, "<tbody>\n" );

	{
		ZnkStrAry_clear( str_list );
		ZnkStrAry_push_bk_cstr( str_list, "LOOPBACK", Znk_NPOS );
		ZnkStrAry_push_bk_cstr( str_list, "ANY", Znk_NPOS );
		printInputUI_SelectBox( html,
				"acceptable_host", acceptable_host, acceptable_host, sys_config_enable,
				str_list,
				"LOOPBACKの場合、ローカルマシン以外からの接続を完全に門前払いします.<br>"
				"ANYの場合、ローカルマシン以外からの接続もとりあえず受け付け、<br>"
				"そこから先の更なる細かい受理の判定をhosts.myfの内容に委ねます." );
	}

	printInputUI_Text( html, "server_name", server_name, server_name, sys_config_enable,
			"サーバ名またはLANから見た場合のIPアドレスです.<br>XhrDMZのhostnameとしても使われます." );

	printInputUI_Int( html, "moai_port", moai_port, moai_port, sys_config_enable,
			"Moai本体が使用するTCP portです." );

	printInputUI_Int( html, "xhr_dmz_port", xhr_dmz_port, xhr_dmz_port, sys_config_enable,
			"XhrDMZが使用するTCP portです." );

	if( sys_config_enable ){
		ZnkStr_add(  html, "<tr colspan=4><td><br>\n" );
		ZnkStr_add(  html, "<input type=hidden name=Moai_UpdateSys    value=\"update_sys\">\n" );
		ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey value=\"%s\">\n", moai_authentic_key );
		ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"更新\">\n" );
		ZnkStr_add(  html, "</td></tr>\n" );
	}

	ZnkStr_add( html, "</tbody>\n" );
	ZnkStr_add( html, "</table>\n" );

	ZnkStr_add( html, "</form>\n" );

	ZnkStr_add(  html, "&nbsp;\n" );

	/* ==== Restart ==== */
	if( MoaiServerInfo_you_need_to_restart_moai() ){
		//ZnkStr_add( html, "<font color=#ff0000><b>You need to restart Moai for adapting to system configuration.</b></font><br>\n" );
		ZnkStr_add( html, "<table class=MstyManagerTable>\n" );
		ZnkStr_add( html, "<tr><td colspan=2>\n" );
		ZnkStr_add( html, "<font size=-1 color=red>　この設定の変更を反映させるにはMoaiを再起動させる必要があります.</font>\n" );
		ZnkStr_add( html, "</td></tr>\n" );
		ZnkStr_add( html, "</table>\n" );
	}
	ZnkStr_add(  html, "<form action=\"/sysconfig\" method=\"POST\" enctype=\"multipart/form-data\">\n" );
	ZnkStr_add(  html, "<input type=hidden name=Moai_RestartServer value=\"reboot\">\n" );
	ZnkStr_addf( html, "<input type=hidden name=Moai_AuthenticKey  value=\"%s\">\n", moai_authentic_key );
	ZnkStr_add(  html, "<input class=MstyWideButton type=submit value=\"Moaiを再起動\">\n" );
	ZnkStr_add(  html, "</form>\n" );

	//ZnkStr_add( html, "<hr>\n" );
	ZnkStr_add( html, "<br>\n" );

	ZnkStr_add( html, "<a class=MstyElemLink href=\"/\">Moaiトップページ</a>\n" );
	ZnkStr_add(  html, "</body></html>\n" );

	ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(html) );
	ZnkStr_delete( html );
	ZnkStrAry_destroy( str_list );
	return ret;
}


static void
doConfigUpdate( const MoaiInfo* info, bool* is_proxy_updated, ZnkStrAry result_msgs )
{
	ZnkVarp var;

	var = ZnkVarpAry_find_byName_literal( info->vars_, "parent_proxy", false );
	if( var ){
		if( RanoParentProxy_set_byAuthority( ZnkVar_cstr(var) ) ){
			*is_proxy_updated = true;
		}
	}
	var = ZnkVarpAry_find_byName_literal( info->vars_, "post_confirm", false );
	MoaiPost_setPostConfirm( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "enable_log_file", false );
	MoaiServerInfo_set_enable_log_file( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "enable_log_verbose", false );
	MoaiServerInfo_set_enable_log_verbose( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "blocking_mode", false );
	MoaiConnection_setBlockingMode( var ? true : false );

	var = ZnkVarpAry_find_byName_literal( info->vars_, "filters_dir", false );
	if( var ){
		MoaiServerInfo_set_filters_dir( ZnkVar_cstr(var) );
	}

	{
		char* envvar_val = ZnkEnvVar_get( "MOAI_PROFILE_DIR" );
		if( envvar_val ){
			/* defined in OS */
			MoaiServerInfo_set_profile_dir( envvar_val );

			var = ZnkVarpAry_find_byName_literal( info->vars_, "profile_dir", false );
			if( var ){
				if( !ZnkS_eq( ZnkVar_cstr(var), envvar_val ) ){
					ZnkStrAry_push_bk_cstr( result_msgs,
							"環境変数MOAI_PROFILE_DIRが設定されているため、profile_dirについてはその値が優先されました.",
							Znk_NPOS );
				}
			}
		} else {
			var = ZnkVarpAry_find_byName_literal( info->vars_, "profile_dir", false );
			if( var ){
				MoaiServerInfo_set_profile_dir( ZnkVar_cstr(var) );
			}
		}
		ZnkEnvVar_free( envvar_val );
	}

	{
		ZnkMyf config = RanoMyf_theConfig();
		ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVarp var = NULL;

			var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
			if( var ){
				const char*    hostname = RanoParentProxy_getHostname();
				const uint16_t port     = RanoParentProxy_getPort();
				char data[ 512 ];
				Znk_snprintf( data, sizeof(data), "%s:%u", hostname, port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "post_confirm", false );
			if( var ){
				const char* data = MoaiPost_isPostConfirm() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_file", false );
			if( var ){
				const char* data = MoaiServerInfo_is_enable_log_file() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "enable_log_verbose", false );
			if( var ){
				const char* data = MoaiServerInfo_is_enable_log_verbose() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "blocking_mode", false );
			if( var ){
				const char* data = MoaiConnection_isBlockingMode() ? "on" : "off";
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "filters_dir", false );
			if( var ){
				const char* data = MoaiServerInfo_filters_dir();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "profile_dir", false );
			if( var ){
				const char* data = MoaiServerInfo_profile_dir();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			ZnkMyf_save( config, "config.myf" );
		}
	}
}
static void
doConfigUpdateSys( const MoaiInfo* info )
{
	ZnkVarp var;
	uint16_t moai_port    = MoaiServerInfo_port();
	uint16_t xhr_dmz_port = MoaiServerInfo_XhrDMZPort();

	var = ZnkVarpAry_find_byName_literal( info->vars_, "acceptable_host", false );
	if( var ){
		MoaiServerInfo_set_acceptable_host( ZnkVar_cstr(var) );
	}

	var = ZnkVarpAry_find_byName_literal( info->vars_, "server_name", false );
	if( var ){
		MoaiServerInfo_set_server_name( ZnkVar_cstr(var) );
	}

	MoaiServerInfo_autosetServerName();

	var = ZnkVarpAry_find_byName_literal( info->vars_, "moai_port", false );
	if( var ){
		ZnkS_getU16U( &moai_port, ZnkVar_cstr(var) );
		MoaiServerInfo_set_port( moai_port );
	}
	var = ZnkVarpAry_find_byName_literal( info->vars_, "xhr_dmz_port", false );
	if( var ){
		ZnkS_getU16U( &xhr_dmz_port, ZnkVar_cstr(var) );
		MoaiServerInfo_setXhrDMZPort( xhr_dmz_port );
	}

	{
		ZnkMyf config = RanoMyf_theConfig();
		ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
		if( vars ){
			ZnkVarp var = NULL;

			var = ZnkVarpAry_find_byName_literal( vars, "acceptable_host", false );
			if( var ){
				const char* data = MoaiServerInfo_acceptable_host();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "moai_port", false );
			if( var ){
				char data[ 256 ] = "0";
				Znk_snprintf( data, sizeof(data), "%u", moai_port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "xhr_dmz_port", false );
			if( var ){
				char data[ 256 ] = "0";
				Znk_snprintf( data, sizeof(data), "%u", xhr_dmz_port );
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			var = ZnkVarpAry_find_byName_literal( vars, "server_name", false );
			if( var ){
				const char* data = MoaiServerInfo_server_name();
				ZnkVar_set_val_Str( var, data, Znk_strlen(data) );
			}

			ZnkMyf_save( config, "config.myf" );
		}
	}
}

static ZnkBird st_bird = NULL;

static void
replaceVersion( ZnkBfr file_img, const char* content_type )
{
	if( ZnkS_eq( content_type, "text/html" ) ){
		const size_t img_size = ZnkBfr_size(file_img);
		if( img_size ){
			const uint8_t* data = ZnkBfr_data( file_img );
			const bool is_terminate_null = (bool)( data[img_size-1] == '\0' );
			const size_t expected_size = img_size;
			ZnkStr_terminate_null( file_img, true );
			ZnkBird_extend_self( st_bird, file_img, expected_size );
			if( !is_terminate_null ){
				ZnkBfr_resize( file_img, ZnkBfr_size(file_img)-1 );
			}
		}
	}
}


static int
procCGI( const ZnkStr req_urp, ZnkSocket sock, RanoModule mod,
		ZnkHtpReqMethod req_method, MoaiInfo* info, size_t content_length )
{
	int    ret = 0;
	ZnkStr filename  = ZnkStr_new( "" );
	ZnkStr dir       = ZnkStr_new( "" );
	ZnkStr query_str = ZnkStr_new( "" );
	ZnkStr path_info = ZnkStr_new( "" );
	ZnkStr curdir_save = ZnkStr_new( "" );
	const char* urp = ZnkStr_cstr(req_urp);
	const char* intp_path = NULL;
	char curdir_new[ 256 ] = "";
	const bool is_cgi_exec_deny = MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "cgi_exec_deny_urp" );

	ZnkVarp varp = MoaiCGIManager_parseURPTail( urp, dir, filename, path_info, query_str, is_cgi_exec_deny );

	//RanoLog_printf( "dir=[%s] fname=[%s], path_info=[%s] query_str=[%s]\n",
	//		ZnkStr_cstr(dir), ZnkStr_cstr(filename), ZnkStr_cstr(path_info), ZnkStr_cstr(query_str) );

	intp_path = varp ? ZnkVar_cstr(varp) : NULL;

	if( intp_path ){
		char cmd[ 256 ] = "";
		bool moai_base_curdir = false;

		ZnkDir_getCurrentDir( curdir_save );

		if( ZnkS_empty(intp_path) ){
			if( moai_base_curdir ){
				Znk_snprintf( curdir_new, sizeof(curdir_new), "%s", ZnkStr_cstr(curdir_save) );
				Znk_snprintf( cmd, sizeof(cmd), "%s/%s", ZnkStr_cstr(dir), ZnkStr_cstr(filename) );
			} else {
				Znk_snprintf( curdir_new, sizeof(curdir_new), "%s/%s", ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir) );
				Znk_snprintf( cmd, sizeof(cmd), "./%s", ZnkStr_cstr(filename) );
			}
		} else {
			Znk_snprintf( cmd, sizeof(cmd), "%s %s", intp_path, ZnkStr_cstr(filename) );
		}

		/**
		 * MoaiディレクトリにあるDLLをも検索できるようにPATHあるいはLD_LIBRARY_PATHにcurdir_saveを追加する.
		 */
#if defined(__CYGWIN__)
		{
			char* envval = ZnkEnvVar_get( "PATH" );
			ZnkStr path_envvar = ZnkStr_new( envval );
			ZnkStrPath_addPathEnvVar( path_envvar, ':', ZnkStr_cstr(curdir_save) );
			ZnkEnvVar_set( "PATH", ZnkStr_cstr(path_envvar) );
			ZnkStr_delete( path_envvar );
			ZnkEnvVar_free( envval );
		}
#elif defined(Znk_TARGET_WINDOWS)
		{
			char* envval = ZnkEnvVar_get( "PATH" );
			ZnkStr path_envvar = ZnkStr_new( envval );
			ZnkStrPath_addPathEnvVar( path_envvar, ';', ZnkStr_cstr(curdir_save) );
			ZnkEnvVar_set( "PATH", ZnkStr_cstr(path_envvar) );
			ZnkStr_delete( path_envvar );
			ZnkEnvVar_free( envval );
		}
#else
		{
			char* envval = ZnkEnvVar_get( "LD_LIBRARY_PATH" );
			ZnkStr path_envvar = ZnkStr_new( envval );
			ZnkStrPath_addPathEnvVar( path_envvar, ':', ZnkStr_cstr(curdir_save) );
			ZnkEnvVar_set( "LD_LIBRARY_PATH", ZnkStr_cstr(path_envvar) );
			ZnkStr_delete( path_envvar );
			ZnkEnvVar_free( envval );
		}
#endif

		/**
		 * PHPではこれを参照するため、設定が必須.
		 */
		{
			ZnkStr path = ZnkStr_new( "" );
			ZnkStr_setf( path, "%s/%s/%s",
					ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir), ZnkStr_cstr(filename) );
#if defined(__CYGWIN__)
			/***
			 * Cygwinの場合はCygwin型ドライブをDOSドライブに変更しておく.
			 * Win32用のphpを指定している場合、これがDOSドライブ型ではないと認識できないためである.
			 * (しかし、Cygwin内でインストールされたphpをインタプリタとして用いるなら
			 * この変換は逆に問題かもしれない)
			 */
			ZnkStrPath_convertCygDrive_toDosDrive( path );
#endif
			ZnkEnvVar_set( "PATH_TRANSLATED", ZnkStr_cstr(path) );
			ZnkStr_delete( path );
			//RanoLog_printf( "  PATH_TRANSLATED=[%s]\n", path );
		}

		if( req_method == ZnkHtpReqMethod_e_POST ){
			ret = MoaiCGI_runPost( cmd, curdir_new, sock, mod,
					info->hdr_size_, info->hdrs_.hdr1st_, info->hdrs_.vars_,
					content_length, info->stream_, query_str );
		} else {
			ret = MoaiCGI_runGet( cmd, curdir_new, sock, mod,
					info->hdr_size_, info->hdrs_.hdr1st_, info->hdrs_.vars_,
					content_length, info->stream_, query_str );
		}
	} else {
		/* Simply FileGetting */

		ZnkDir_getCurrentDir( curdir_save );
		Znk_snprintf( curdir_new, sizeof(curdir_new), "%s/%s", ZnkStr_cstr(curdir_save), ZnkStr_cstr(dir) );
		ZnkDir_changeCurrentDir( curdir_new );

		if( ZnkDir_getType( ZnkStr_cstr(filename) ) != ZnkDirType_e_File ){
			MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
					urp );
		} else {
			if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(filename), replaceVersion ) ){
				MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/moai.png\"> Moai WebServer : 503 Service Unavaliable [%s].</b></p>\n",
						urp );
			}
		}

		ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
	}

	ZnkStr_delete( dir );
	ZnkStr_delete( filename );
	ZnkStr_delete( query_str );
	ZnkStr_delete( path_info );
	ZnkStr_delete( curdir_save );

	/***
	 * 更新時間が異なる場合のみconfig内を再読み込み.
	 * 一部の項目のみを更新.
	 */
	{
		ZnkDate conf_date  = { 0 };
		ZnkDate proxy_date = RanoParentProxy_getDate();

		RanoFileInfo_getLastUpdatedTime( "config.myf", &conf_date );
		if( ZnkDate_compareDay( &conf_date, &proxy_date ) || ZnkDate_diffSecond( &conf_date, &proxy_date, 0 ) ){
			if( RanoMyf_loadConfig() ){
				ZnkMyf config = RanoMyf_theConfig();
				ZnkVarpAry vars = ZnkMyf_find_vars( config, "config" );
				if( vars ){
					ZnkVarp var = ZnkVarpAry_find_byName_literal( vars, "parent_proxy", false );
					RanoParentProxy_set_byAuthority( ZnkVar_cstr(var) );
				}
			}
		}
	}
	RanoLog_printf( "Moai : procCGI end.\n" );
	return ret;
}

static bool
extendURPAlias( ZnkStr req_urp, const ZnkVarpAry urp_alias_list )
{
	const size_t size  = ZnkVarpAry_size( urp_alias_list );
	ZnkVarp      alias = NULL;
	ZnkStrAry    tkns  = NULL;
	const char*  src   = NULL;
	const char*  dst   = NULL;
	size_t idx;
	for( idx=0; idx<size; ++idx ){
		alias = ZnkVarpAry_at( urp_alias_list, idx );
		tkns  = ZnkVar_str_ary( alias );
		src   = ZnkStrAry_at_cstr( tkns, 0 );
		dst   = ZnkStrAry_at_cstr( tkns, 1 );
		if( ZnkStr_isBegin( req_urp, src ) ){
			char next_ch = ZnkStr_at( req_urp, Znk_strlen(src) );
			switch( next_ch ){
			case '/': case '?': case '#': case '\0':
				ZnkStr_replace( req_urp, 0, Znk_strlen(src), dst, Znk_NPOS );
				return true;
			default:
				break;
			}
		}
	}
	return false;
}


/* 日本語ファイル名対応 */
static void
escapeEvilCharOnFSys( ZnkStr ans, const uint8_t* data, size_t data_size )
{
	const uint8_t* begin = data;
	const uint8_t* end   = data + data_size;
	const uint8_t* p = begin;
	while( p != end ){
		switch( *p ){
		case '*':
		case '?':
		case ':':
		case ';':
		//case '~':
			ZnkStr_addf2( ans, "%%%02X", *p );
			break;
		default:
			ZnkStr_add_c( ans, *p );
			break;
		}
		++p;
	}
}
static void
unescape_forMbcFSysPath( ZnkStr fsys_path )
{
	ZnkStr tmp = ZnkStr_new( "" );
	ZnkHtpURL_unescape_toStr( tmp, ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_clear( fsys_path );
	ZnkHtpURL_unescape_toStr( fsys_path, ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );
	ZnkStr_clear( tmp );
	escapeEvilCharOnFSys( tmp, (uint8_t*)ZnkStr_cstr(fsys_path), ZnkStr_leng(fsys_path) );
	ZnkStr_swap( fsys_path, tmp );
	ZnkStr_delete( tmp );
}

static MoaiRASResult
do_get( ZnkSocket sock, ZnkStr req_urp,
		bool is_xhr_dmz, const char* xhr_authhost,
		MoaiInfo* info, const MoaiContext ctx )
{
	ZnkVarpAry          urp_alias_list = MoaiCGIManager_getURPAliasList();
	MoaiRASResult       ras_result     = MoaiRASResult_e_OK;
	int                 ret            = 0;
	bool                is_authenticated = false;
	uint32_t            peer_ipaddr    = ctx->peer_ipaddr_;
	const RanoModule    mod            = NULL;
	const char*         profile_dir    = MoaiServerInfo_profile_dir();

	ZnkStr    msg_str       = ZnkStr_new( "" );
	ZnkStr    fsys_path     = ZnkStr_new( "./doc_root" );
	ZnkStr    req_urp_dir   = ZnkStr_new( "" );
	ZnkStr    req_urp_tail  = ZnkStr_new( "" );

	Znk_UNUSED( ret );
	RanoLog_printf( "  As WebServer GET\n" );

	/***
	 * Unaliasing.
	 */
	if( urp_alias_list ){
		extendURPAlias( req_urp, urp_alias_list );
	}

	/***
	 * Get req_urp_dir and req_urp_tail.
	 * (MUST)
	 */
	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, ZnkStr_cstr(req_urp) );
	/***
	 * Unescape req_urp_dir for MBC fsys path.
	 * (MUST)
	 */
	unescape_forMbcFSysPath( req_urp_dir );
	unescape_forMbcFSysPath( req_urp_tail );
	/***
	 * Sanitize : /../ in req_urp_dir.
	 * (MUST)
	 */
	ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, false );
	/***
	 * Essential protection against XSS.
	 * (MUST)
	 */
	ZnkHtpURL_negateHtmlTagEffection( req_urp_dir );

	/***
	 * Rebuild fsys_path by Sanitized result.
	 */
	ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_dir) );
	ZnkStr_add_c( fsys_path, '/' );
	if( ZnkStr_empty(req_urp_tail) ){
		ZnkStr_add( fsys_path, "index.html" );
		RanoLog_printf( "  fsys_path=[%s]\n", ZnkStr_cstr(fsys_path) );
	} else {
		ZnkStr_add( fsys_path, ZnkStr_cstr(req_urp_tail) );
	}

	/***
	 * Mount special fsys_path.
	 */
	MoaiCGIManager_mapFSysDir( fsys_path, profile_dir );

	/***
	 * is_authenticated or not.
	 */
	{
		const char* q = strchr( ZnkStr_cstr(req_urp), '?' );
		if( q ){
			static const bool is_unescape_val = true;
			const char* query_str = q+1;
			ZnkVarpAry post_vars = ZnkVarpAry_create( true );
			RanoCGIUtil_splitQueryString( post_vars, query_str, Znk_NPOS, is_unescape_val );
			{
				ZnkVarp var = NULL;
				const char* moai_authentic_key = MoaiServerInfo_authenticKey();
				var = ZnkVarpAry_find_byName_literal( post_vars, "Moai_AuthenticKey", false );
				if( var && ZnkS_eq( moai_authentic_key, ZnkVar_cstr(var) ) ){
					is_authenticated = true;
				}
			}
			ZnkVarpAry_destroy( post_vars );
		}
	}

	if( ZnkStr_isBegin( req_urp, "/test_req_urp/" ) ){
		ZnkStr uxs_fsys_path = ZnkStr_new( ZnkStr_cstr(fsys_path) );
		/***
		 * Essential protection against XSS.
		 * (MUST)
		 */
		ZnkHtpURL_negateHtmlTagEffection( uxs_fsys_path );
		/***
		 * Check uxs_fsys_path.
		 */
		ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b>Moai WebServer : TestReqURP :</b></p>\n",
				"uxs_fsys_path=[%s]\n",
				ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */

		ZnkStr_delete( uxs_fsys_path );
		goto FUNC_END;
	}

	if( is_xhr_dmz ){
		/***
		 * from XhrDMZ
		 */
		ZnkStr uxs_fsys_path = ZnkStr_new( ZnkStr_cstr(fsys_path) );

		/***
		 * Essential protection against XSS.
		 * (MUST)
		 */
		ZnkHtpURL_negateHtmlTagEffection( uxs_fsys_path );

		if( MoaiCGIManager_isValidReqUrp_forPathBegin( ZnkStr_cstr(req_urp), "xhrdmz_cgi_urp" ) ){
			/* Moai-CGI */
			ret = procCGI( req_urp, sock, mod,
					ctx->req_method_, info, ctx->body_info_.content_length_ );
		} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "xhrdmz_only_urp" ) ){
			/* Moai-XhrDMZOnly */
			/* この範囲は authentic key は不要.
			 * xhr_dmz からのみアクセスが可能.
			 * xhr_authhost からは逆にURLによるアクセスが許可されない( authentic key を指定しても不可 ). */
			/* Moai File Responsing */
			if( ZnkStr_isBegin( uxs_fsys_path, "./doc_root/cgis/" ) ){
				ZnkStr_replace( uxs_fsys_path, 0, Znk_strlen_literal("./doc_root/cgis/"), "./cgis/", Znk_NPOS );
			}
			if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
				ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/public/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
						ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
			}

		} else if( MoaiCGIManager_isValidReqUrp_forBox( ZnkStr_cstr(req_urp), "publicbox_list" ) ){
			/* Moai-Public */
			/* この範囲は authentic key は不要.
			 * またすべての hostname からの直接のアクセスも可能. */
			/* Moai File Responsing */
			if( ZnkStr_isBegin( uxs_fsys_path, "./doc_root/cgis/" ) ){
				ZnkStr_replace( uxs_fsys_path, 0, Znk_strlen_literal("./doc_root/cgis/"), "./cgis/", Znk_NPOS );
			}
			if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
				ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/public/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
						ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
			}
		} else {
			/***
			 * 上記以外はすべてアクセス不可.
			 */
			ZnkStr_addf( msg_str, "<p><b>Moai WebServer : 403 Forbidden.</b></p>\n" );
			ZnkStr_addf( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;This access is from Moai XhrDMZ. <br>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;Please restart from <a href=\"http://%s/\">Moai top page.</a><br>\n",
					xhr_authhost );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
		}
		ZnkStr_delete( uxs_fsys_path );

	} else {
		/***
		 * from AuthenticHost
		 */

		ZnkStr uxs_fsys_path = ZnkStr_new( ZnkStr_cstr(fsys_path) );
		/***
		 * Essential protection against XSS.
		 * (MUST)
		 */
		ZnkHtpURL_negateHtmlTagEffection( uxs_fsys_path );

		if( ZnkStr_eq( req_urp, "/config" ) ){
			/* Moai-Config */
			/* 発火自体はauthentic keyは不要. */
			ret = printConfig( sock, NULL, peer_ipaddr );
		} else if( ZnkStr_eq( req_urp, "/sysconfig" ) ){
			/* Moai-SysConfig */
			/* 発火自体はauthentic keyは不要. */
			ret = printSysConfig( sock, NULL, peer_ipaddr );
		} else if( ZnkStr_eq( req_urp, "/init_mod" ) ){
			/* Moai-InitCommand */
			if( is_authenticated ){
				MoaiServerInfo_initiate_mod_ary();
				ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary done." );
			} else {
				ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary is not permitted." );
			}

		} else if( MoaiCGIManager_isMatchedReqUrp( ZnkStr_cstr(req_urp), "xhrdmz_only_urp" ) ){
			/* Moai-XhrDMZOnly */
			/* この範囲は xhr_authhost からは逆にURLによるアクセスが許可されない( authentic key を指定しても不可 ). */
			const char* xhr_dmz = MoaiServerInfo_XhrDMZ();
			//const char* retry_req_urp =  ZnkStr_cstr(uxs_fsys_path);
			const char* retry_req_urp =  ZnkStr_cstr(req_urp);
			if( ZnkS_isBegin_literal( retry_req_urp, "./doc_root" ) ){
				retry_req_urp += Znk_strlen_literal( "./doc_root" );
			}
			ZnkStr_addf( msg_str, "<p><b>Moai WebServer : 403 Forbidden.</b></p>\n" );
			ZnkStr_addf( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;This access is from Moai Authentic Host. <br>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;Please retry <a href=\"http://%s%s\">it.</a><br>\n",
					xhr_dmz, retry_req_urp );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */

		} else if( ZnkStr_isBegin( req_urp, "/cgis/" ) ){

			if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
				/* Moai-CGI */
				/* 実行自体は許可(発火自体にauthentic keyは不要) */
				ret = procCGI( req_urp, sock, mod,
						ctx->req_method_, info, ctx->body_info_.content_length_ );

			} else if( MoaiCGIManager_isValidReqUrp_forBox( ZnkStr_cstr(req_urp), "publicbox_list" ) ){
				/* Moai-Public */
				/* この範囲は authentic key は不要.
				 * またすべての hostname からの直接のアクセスも可能. */
				/* Moai File Responsing */
				if( ZnkStr_isBegin( uxs_fsys_path, "./doc_root/cgis/" ) ){
					ZnkStr_replace( uxs_fsys_path, 0, Znk_strlen_literal("./doc_root/cgis/"), "./cgis/", Znk_NPOS );
				}
				if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
					ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/public/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
							ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
				}

			} else {
				/* Moai-Private */
				/***
				 * Moai File Responsing.
				 * この範囲では authentic key を必要とする.
				 */
				if( is_authenticated ){
					if( ZnkStr_isBegin( uxs_fsys_path, "./doc_root/cgis/" ) ){
						ZnkStr_replace( uxs_fsys_path, 0, Znk_strlen_literal("./doc_root/cgis/"), "./cgis/", Znk_NPOS );
					}
					if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
						ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
								ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
					}
				} else {
					const char* moai_authentic_key = MoaiServerInfo_authenticKey();
					const char* retry_req_urp =  ZnkStr_cstr(uxs_fsys_path);
					if( ZnkS_isBegin_literal( retry_req_urp, "./doc_root" ) ){
						retry_req_urp += Znk_strlen_literal( "./doc_root" );
					}
					/***
					 * Essential protection against XSS.
					 * (MUST)
					 */
					ZnkStr_addf( msg_str, "<p><b>Moai WebServer : 401 Unauthorized.</b></p>\n" );
					ZnkStr_addf( msg_str, "<p><b><u>What this?</u></b></p>\n" );
					ZnkStr_addf( msg_str, "&nbsp;&nbsp;We must affix authentication key to our URL to get this resource. <br>\n" );
					ZnkStr_addf( msg_str, "&nbsp;&nbsp;Please retry <a href=\"%s?Moai_AuthenticKey=%s\">it.</a><br>\n",
							retry_req_urp, moai_authentic_key );
					ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
				}
			}
		} else {
			/* Moai File Responsing */

			if( ZnkStr_isBegin( req_urp, "/private/" ) ){
				/* Moai-Private */
				/***
				 * Moai File Responsing.
				 * この範囲では authentic key を必要とする.
				 */
				if( is_authenticated ){
					if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
						ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
								ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
					}
				} else {
					const char* moai_authentic_key = MoaiServerInfo_authenticKey();
					const char* retry_req_urp = ZnkStr_cstr(uxs_fsys_path);
					if( ZnkS_isBegin_literal( retry_req_urp, "./doc_root" ) ){
						retry_req_urp += Znk_strlen_literal( "./doc_root" );
					}
					ZnkStr_addf( msg_str, "<p><b>Moai WebServer : 401 Unauthorized.</b></p>\n" );
					ZnkStr_addf( msg_str, "<p><b><u>What this?</u></b></p>\n" );
					ZnkStr_addf( msg_str, "&nbsp;&nbsp;We must affix authentication key to our URL to get this resource. <br>\n" );
					ZnkStr_addf( msg_str, "&nbsp;&nbsp;Please retry <a href=\"%s?Moai_AuthenticKey=%s\">it.</a><br>\n",
							retry_req_urp, moai_authentic_key );
					ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr(msg_str) ); /* XSS-safe */
				}
			} else {
				/* Moai-Public */
				/***
				 * Moai File Responsing.
				 * この範囲ではすべての hostname より authentic key なしでアクセス可能である.
				 */
				if( !MoaiIO_sendResponseFile( sock, ZnkStr_cstr(uxs_fsys_path), replaceVersion ) ){
					ret = MoaiIO_sendTxtf( sock, "text/html", "<p><b><img src=\"/moai.png\"> Moai WebServer : 404 Not found [%s].</b></p>\n",
							ZnkStr_cstr(uxs_fsys_path) ); /* XSS-safe */
				}
			}
		}
		ZnkStr_delete( uxs_fsys_path );

	}

FUNC_END:
	ZnkStr_delete( msg_str );
	ZnkStr_delete( fsys_path );
	ZnkStr_delete( req_urp_dir );
	ZnkStr_delete( req_urp_tail );

	return ras_result;
}


static MoaiRASResult
do_post( ZnkSocket sock, ZnkStr req_urp,
		bool is_xhr_dmz, const char* xhr_authhost,
		MoaiInfo* info, const MoaiContext ctx, MoaiFdSet mfds, bool* is_proxy_updated )
{
	ZnkVarpAry          urp_alias_list = MoaiCGIManager_getURPAliasList();
	MoaiRASResult       ras_result     = MoaiRASResult_e_OK;
	int                 ret            = 0;
	bool                is_authenticated = false;
	uint32_t            peer_ipaddr    = ctx->peer_ipaddr_;
	const RanoModule    mod            = NULL;
	const RanoModuleAry mod_ary        = MoaiServerInfo_mod_ary();

	ZnkStr    msg_str      = ZnkStr_new( "" );
	ZnkStr    req_urp_dir  = ZnkStr_new( "" );
	ZnkStr    req_urp_tail = ZnkStr_new( "" );
	ZnkStrAry result_msgs  = ZnkStrAry_create( true );

	bool      show_forbidden = false;

	Znk_UNUSED( ret );
	*is_proxy_updated = false;

	/***
	 * Unaliasing.
	 */
	if( urp_alias_list ){
		extendURPAlias( req_urp, urp_alias_list );
	}

	/***
	 * Get req_urp_dir and Sanitize : /../ in req_urp_dir.
	 * (MUST)
	 */
	ZnkHtpURL_getReqUrpDir( req_urp_dir, req_urp_tail, ZnkStr_cstr(req_urp) );
	ZnkHtpURL_sanitizeReqUrpDir( req_urp_dir, true );
	/***
	 * Essential protection against XSS.
	 * (MUST)
	 */
	ZnkHtpURL_negateHtmlTagEffection( req_urp_dir );

	{
		ZnkStr uxs_req_urp = ZnkStr_new( ZnkStr_cstr(req_urp) );
		ZnkHtpURL_negateHtmlTagEffection( uxs_req_urp );

		/* XSS-safe */
		ZnkStr_addf( msg_str, "<p><b><img src=\"moai.png\"> Moai WebServer : POST [%s].</b></p>\n",
				ZnkStr_cstr(uxs_req_urp) );
		ZnkStr_add( msg_str, "<pre>\n" );

		ZnkStr_delete( uxs_req_urp );
	}


	MoaiPost_parsePostAndCookieVars( sock, mfds,
			msg_str,
			info->hdr_size_, &info->hdrs_,
			ctx->body_info_.content_length_, info->stream_,
			info->vars_,
			mod, is_authenticated );

	/***
	 * is_authenticated or not.
	 */
	if( info->vars_ ){
		ZnkVarp var = NULL;
		const char* moai_authentic_key = MoaiServerInfo_authenticKey();
		var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_AuthenticKey", false );
		if( var && ZnkS_eq( moai_authentic_key, ZnkVar_cstr(var) ) ){
			is_authenticated = true;
		}
	}

	ZnkStr_add( msg_str, "</pre>\n" );

	if( is_xhr_dmz ){
		/***
		 * 基本的にXhrDMZ上では、POST Requestによるファイルアクセスを認めるべきではない.
		 * 一方POST RequestによるCGI実行はあり得る.
		 * 例えばBBS上に用意された投稿フォームなどはこの部分において処理する形になるだろう.
		 */
		//if( ZnkStr_isBegin( req_urp, "/cgis/" ) ){
		if( MoaiCGIManager_isValidReqUrp_forPathBegin( ZnkStr_cstr(req_urp), "xhrdmz_cgi_urp" ) ){
			/* Moai-CGI */
			if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
				/* 実行自体は許可(発火自体にauthentic keyは不要) */
				ret = procCGI( req_urp, sock, mod,
						ctx->req_method_, info, ctx->body_info_.content_length_ );
			}
		} else {
			/* Moai Msg Responsing */
			ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
		}
	} else {
		if( info->vars_ ){
			ZnkVarp var = NULL;
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_Update", false );
			if( var ){
				if( is_authenticated ){
					doConfigUpdate( info, is_proxy_updated, result_msgs );
					ras_result = MoaiRASResult_e_OK;
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_Initiate", false );
			if( var ){
				if( is_authenticated ){
					RanoModuleAry_initiateFilters( mod_ary, result_msgs );
					ras_result = MoaiRASResult_e_OK;
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_RestartServer", false );
			if( var ){
				if( is_authenticated ){
					MoaiServerInfo_set_you_need_to_restart_moai( false );
					ras_result = MoaiRASResult_e_RestartServer;
				} else {
					show_forbidden = true;
				}
			}
	
			var = ZnkVarpAry_find_byName_literal( info->vars_, "Moai_UpdateSys", false );
			if( var ){
				if( is_authenticated ){
					doConfigUpdateSys( info );
					ras_result = MoaiRASResult_e_OK;
					MoaiServerInfo_set_you_need_to_restart_moai( true );
				} else {
					show_forbidden = true;
				}
			}
	
		}
		if( show_forbidden ){
			ZnkStr_addf( msg_str, "<p><b><img src=\"/moai.png\"> Moai WebServer : 403 Forbidden.</b></p>\n" );
			ZnkStr_addf( msg_str, "<p><b><u>Why?</u></b></p>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;Maybe, your moai server is restarted in midstream. <br>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;Or this operation doesn't follow a regular procedure.<br>\n" );
			ZnkStr_addf( msg_str, "&nbsp;&nbsp;Please retry it from <a href=\"/\">Moai top page.</a><br>\n" );
		}
	
		if( !MoaiServerInfo_is_enable_log_file() ){
			RanoLog_printf( "Moai : config : enable_log_file is off.\n" );
			RanoLog_printf( "Moai : Stop writing to log file and close it here.\n" );
			RanoLog_close();
		}
	
		if( ras_result == MoaiRASResult_e_RestartServer ){
			const char* back_to_dst = "config";
	
			if( ZnkStr_eq( req_urp, "/config" ) ){
				/* Moai-Config */
				back_to_dst = "config";
			} else if( ZnkStr_eq( req_urp, "/sysconfig" ) ){
				/* Moai-SysConfig */
				back_to_dst = "sysconfig";
			} else if( ZnkStr_eq( req_urp, "/init_mod" ) ){
				/* Moai-InitCommand */
				back_to_dst = "init_mod";
			}
	
			ZnkStr_clear( msg_str );
			ZnkStr_add(  msg_str, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">" );
			ZnkStr_addf( msg_str, "<html><head><META HTTP-EQUIV=\"refresh\" content=\"1;URL=%s\"></head>", back_to_dst );
			ZnkStr_add(  msg_str, "<body>Moai Server is Restarted.<br>\n" );
			ZnkStr_addf( msg_str, "<a class=MstyElemLink href=\"./%s\">Back to %s</a></body></html>\n", back_to_dst, back_to_dst );
			ret = MoaiIO_sendTxtf( sock, "text/html", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
	
		} else {
			/***
			 * Unaliasing.
			 */
	
			if( is_authenticated && ZnkStr_eq( req_urp, "/config" ) ){
				/* Moai-Config */
				ret = printConfig( sock, result_msgs, peer_ipaddr );
			} else if( is_authenticated && ZnkStr_eq( req_urp, "/sysconfig" ) ){
				/* Moai-SysConfig */
				ret = printSysConfig( sock, result_msgs, peer_ipaddr );
			} else if( is_authenticated && ZnkStr_eq( req_urp, "/init_mod" ) ){
				/* Moai-InitCommand */
				MoaiServerInfo_initiate_mod_ary();
				ret = MoaiIO_sendTxtf( sock, "text/html", "initiate_mod_ary done." ); /* XSS-safe */
			} else if( ZnkStr_isBegin( req_urp, "/cgis/" ) ){
				/* Moai-CGI */
				if( MoaiCGIManager_isValidReqUrp_forRun( ZnkStr_cstr(req_urp) ) ){
					/* 実行自体は許可(発火自体にauthentic keyは不要) */
					ret = procCGI( req_urp, sock, mod,
							ctx->req_method_, info, ctx->body_info_.content_length_ );
				}
			} else {
				/* Moai Msg Responsing */
				ret = MoaiIO_sendTxtf( sock, "text/html", "%s", ZnkStr_cstr( msg_str ) ); /* XSS-safe */
			}
		}
	}

	ZnkStr_delete( msg_str );
	ZnkStr_delete( req_urp_dir );
	ZnkStr_delete( req_urp_tail );
	ZnkStrAry_destroy( result_msgs );

	return ras_result;
}

/***
 * Moai WebServer Engine Ver2.0
 */
MoaiRASResult
MoaiWebServer_do( const MoaiContext ctx, ZnkSocket sock, MoaiConnection mcn, MoaiFdSet mfds )
{
	ZnkFdSet            fdst_observe_r = MoaiFdSet_fdst_observe_r( mfds );
	MoaiRASResult       ras_result     = MoaiRASResult_e_OK;
	int                 ret            = 0;

	MoaiInfo*           info           = ctx->draft_info_;
	uint32_t            peer_ipaddr    = ctx->peer_ipaddr_;
	ZnkStr              req_urp        = info->req_urp_;
	bool                is_xhr_dmz     = false;
	const char*         xhr_dmz      = MoaiServerInfo_XhrDMZ();
	const char*         xhr_authhost = MoaiServerInfo_XhrAuthHost();
	bool used_by_dmz_port = true;

	/***
	 * Decide xhr_dmz or not.
	 */
	if( used_by_dmz_port ){
		/**
		 * portのみで判定.
		 * こちらへ移行予定.
		 */
		is_xhr_dmz = (bool)( MoaiConnection_port( mcn ) == MoaiServerInfo_XhrDMZPort() );
	} else {
		/**
		 * このアルゴリズムはループバック接続の場合のみ有効である.
		 *
		 * 他サーバからの接続の場合、peer_ipaddrは固定になってしまうわけで
		 * ホスト名だけでxhr_dmzと一致させるのは本質的にムリがある.
		 * そこでとりあえずeasterにおいてもループバック以外の接続においては使用できないように
		 * なんらかの工夫をする必要がある.
		 */
		char     peer_ipstr[ 64 ] = "";
		char     peer_host[ 256 ] = "";
		uint16_t moai_port = MoaiServerInfo_port();
		ZnkNetIP_getIPStr_fromU32( peer_ipaddr, peer_ipstr, sizeof(peer_ipstr) );
		Znk_snprintf( peer_host, sizeof(peer_host), "%s:%u", peer_ipstr, moai_port );
		if( ZnkS_eq( peer_host, xhr_dmz ) ){
			is_xhr_dmz = true;
		}
	}

	/***
	 * Bird version replacer.
	 */
	if( st_bird == NULL ){
		st_bird = ZnkBird_create( "$[", "]$" );
		ZnkBird_regist( st_bird, "MoaiVersion", st_version_str );
	}

	switch( ctx->req_method_ ){
	case ZnkHtpReqMethod_e_GET:
	{
		/***
		 * Ver2.0: GET Security-enhancement:
		 */
		ras_result = do_get( sock, req_urp,
				is_xhr_dmz, xhr_authhost,
				info, ctx );

		RanoLog_printf( "  Requested is done.\n" );
		if( ret < 0 || !mcn->I_is_keep_alive_ ){
			MoaiIO_close_ISock( "  WebServerGET", sock, mfds );
		}
		break;
	}
	case ZnkHtpReqMethod_e_POST:
	{
		bool is_proxy_updated = false;
		/***
		 * Ver2.0: POST Security-enhancement:
		 */
		ras_result = do_post( sock, req_urp,
				is_xhr_dmz, xhr_authhost,
				info, ctx, mfds, &is_proxy_updated );

		/***
		 * MoaiSeverをRestartする場合も、一旦コネクトをクロースした方がよい.
		 * さもないとブラウザ側で「接続がリセットされました」が表示されることがあり不恰好である.
		 */
		if( ret < 0 || !mcn->I_is_keep_alive_ || ras_result == MoaiRASResult_e_RestartServer ){
			MoaiIO_close_ISock( "  WebServerPOST", sock, mfds );
		}
		/***
		 * 現在ObserveされているSocketをすべて強制的にcloseする.
		 */
		if( is_proxy_updated ){
			MoaiIO_closeSocketAll( "  ProxyUpdate", fdst_observe_r, mfds );
		}
		break;
	}
	case ZnkHtpReqMethod_e_HEAD:
	{
#if 1
		ZnkStr ans = ZnkStr_new( "" );
		ZnkStr_add( ans, "HTTP/1.1 200 OK\r\n" );
		//ZnkStr_add( ans, "Date: Fri, 22 Sep 2017 21:18:00 GMT\r\n" );
		//ZnkStr_add( ans, "Content-Type: text/html\r\n" );
		//ZnkStr_add( ans, "Content-Length: 363857\r\n" );
		//ZnkStr_add( ans, "Connection: keep-alive\r\n" );
		//ZnkStr_add( ans, "Last-Modified: Fri, 22 Sep 2017 21:18:00 GMT\r\n" );
		////ZnkStr_add( ans, "CF-Cache-Status: HIT\r\n" );
		//ZnkStr_add( ans, "Vary: Accept-Encoding\r\n" );
		//ZnkStr_add( ans, "Expires: Fri, 23 Sep 2017 15:52:29 GMT\r\n" );
		////ZnkStr_add( ans, "Cache-Control: public, max-age=14400\r\n" );
		//ZnkStr_add( ans, "Accept-Ranges: bytes\r\n" );
		//ZnkStr_add( ans, "Server: cloudflare-nginx\r\n" );
		ZnkStr_add( ans, "\r\n" );
		/* test. */
		ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
#endif

		RanoLog_printf( "  Requested is done.\n" );
		if( ret < 0 || !mcn->I_is_keep_alive_ ){
			MoaiIO_close_ISock( "  WebServerGET", sock, mfds );
		}

		break;
	}
	default:
		break;
	}
	return ras_result;
}
