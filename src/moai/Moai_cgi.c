/***
 * MoaiCGI Engine (introduced by Ver2.0)
 */
#include <Moai_cgi.h>
#include <Moai_io_base.h>
#include <Moai_server_info.h>
#include <Moai_connection.h>

#include <Rano_log.h>

#include <Znk_envvar.h>
#include <Znk_s_base.h>
#include <Znk_missing_libc.h>
#include <Znk_str.h>
#include <Znk_str_ex.h>
#include <Znk_htp_hdrs.h>
#include <Znk_mem_find.h>
#include <Znk_net_ip.h>
#include <Znk_dir.h>
#include <Znk_thread.h>
#include <time.h>

static size_t st_cgi_count = 0;
static size_t st_cgi_count_max = 16;

static void
formatTimeString( char* buf, size_t buf_size, time_t clck )
{
	struct 	tm *timePtr;
	if( clck == 0 ) clck = time(NULL);
	timePtr = gmtime( &clck );
	//strftime( buf, buf_size, "%a, %d %b %Y %T GMT", timePtr );
	strftime( buf, buf_size, "%a, %d %b %Y %H:%M:%S GMT", timePtr );
}
static int
sendHtpHdrOfCGI_withContentLength( ZnkStr hdr_str, ZnkSocket sock, size_t content_length )
{
	int ret = 0;
	char timeBuf[ 128 ];
	ZnkStr ans = ZnkStr_new( "" );

	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	if( ZnkStr_empty( hdr_str ) ){
		/* HTTP�w�b�_�̎w�肪�Ȃ���Ă��Ȃ�CGI�X�N���v�g�ւ̓��ʑ[�u */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Content-Type: text/html\r\n",
				timeBuf );
		ZnkStr_addf( ans,
				"Content-Length: %d\r\n",
				content_length );
		ZnkStr_add( ans, "\r\n" );
	} else {
		/* Generally type. */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n",
				timeBuf );
		ZnkStr_addf( ans,
				"Content-Length: %d\r\n",
				content_length );
		ZnkStr_add( ans,
				ZnkStr_cstr( hdr_str ) );
	}

	ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
	ZnkStr_delete( ans );
	return ret;
}
static int
sendHtpHdrOfCGI_forChunked( ZnkStr hdr_str, ZnkSocket sock )
{
	int ret = 0;
	char timeBuf[ 128 ];
	ZnkStr ans = ZnkStr_new( "" );

	formatTimeString( timeBuf, sizeof(timeBuf), 0 );

	/***
	 * ����: Transfer-Encoding chunked �� HTTP 1.1����T�|�[�g�����@�\�ł���.
	 * �]���Ĉȉ���HTTP/1.1 ��ւ���HTTP/1.0�Ə������Ƃ͂ł��Ȃ�.
	 * (����HTTP/1.0�Ə������ꍇ�A�u���E�U�͐�����chunked�f�[�^���������Ȃ�)
	 */
	if( ZnkStr_empty( hdr_str ) ){
		/* HTTP�w�b�_�̎w�肪�Ȃ���Ă��Ȃ�CGI�X�N���v�g�ւ̓��ʑ[�u */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Server: Moai/2.0\r\n"
    			"Keep-Alive: timeout=5, max=100\r\n"
				"Connection: Keep-Alive\r\n"
				"Content-Type: text/html\r\n",
				timeBuf );
		ZnkStr_add( ans,
				"Transfer-Encoding: chunked\r\n" );
		ZnkStr_add( ans, "\r\n" );
	} else {
		/* Generally type. */
		ZnkStr_addf( ans,
				"HTTP/1.1 200 OK\r\n"
				"Date: %s\r\n"
				"Server: Moai/2.0\r\n"
    			"Keep-Alive: timeout=5, max=100\r\n"
				"Connection: Keep-Alive\r\n",
				timeBuf );
		ZnkStr_add( ans,
				"Transfer-Encoding: chunked\r\n" );
		ZnkStr_add( ans,
				ZnkStr_cstr( hdr_str ) );
	}

	ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
	ZnkStr_delete( ans );
	return ret;
}

static int
sendResponseOfCGI( ZnkBfr bfr, ZnkSocket sock )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl[256] = {0};
	int ret = 0;
	if( ZnkBfr_size(bfr) ){
		char timeBuf[ 128 ];
		size_t pos;
		ZnkStr ans = ZnkStr_new( "" );
	
		if( !st_initialized ){
			ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"\r\n\r\n", 4 );
			st_initialized = true;
		}
	
		formatTimeString( timeBuf, sizeof(timeBuf), 0 );
	
		pos = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr)-1,
				(uint8_t*)"\r\n\r\n", 4, 1, st_occ_tbl );
		if( pos == Znk_NPOS ){
			/***
			 * Content-Type is not specified.
			 * We take this as text/html.
			 */
			ZnkStr_addf( ans,
					"HTTP/1.1 200 OK\r\n"
					"Date: %s\r\n"
					"Content-Type: text/html\r\n",
					timeBuf );
			ZnkStr_addf( ans,
					"Content-Length: %d\r\n",
					ZnkBfr_size(bfr)-1 );
			ZnkStr_add( ans, "\r\n" );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr), ZnkBfr_size(bfr)-1 );

			ret = ZnkSocket_send_cstr( sock, ZnkStr_cstr(ans) );
		} else {
			/***
			 * Generally type.
			 */
			ZnkStr_addf( ans,
					"HTTP/1.1 200 OK\r\n"
					"Date: %s\r\n",
					timeBuf );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr), pos+2 );
			ZnkStr_addf( ans,
					"Content-Length: %d\r\n",
					ZnkBfr_size(bfr)-1-pos-4 );
			ZnkStr_add( ans, "\r\n" );
			ZnkStr_append( ans, (char*)ZnkBfr_data(bfr)+pos+4, ZnkBfr_size(bfr)-1-pos-4 );

			ret = ZnkSocket_send( sock, (uint8_t*)ZnkStr_cstr(ans), ZnkStr_leng(ans) );
		}
		ZnkStr_delete( ans );
	}
	return ret;
}

#if 0
/***
 * end�ɂ͕K���w�b�_���E�̊J�n�ʒu���w��.
 */
static bool
checkValidHeader( ZnkStr str, size_t end )
{
	size_t idx = 0;
	char ch = 0;
	int state = 0;
	for( idx=0; idx<end; ++idx ){
		ch = ZnkStr_at( str, idx );
		switch( ch ){
		case '\n':
			if( state != 1 ){
				/* �ُ�: :���܂��o�ꂵ�Ă��Ȃ��ɂ��ւ�炸���s������ */
				return false;
			}
			state = 0;
			continue;
		case '\r':
		case '\t':
			continue;
		case ':':
			state = 1;
			continue;
		default:
			break;
		}
		if( ch < 0x20 && ch >= 0x80 ){
			/* ���͈̔͂ɂ���R�[�h�������ꍇ�ُ͈�Ƃ��� */
			return false;
		}
	}
	/***
	 * ���Ȃ��Ƃ� : ���o�ꂵ����ɏI���ƂȂ�Ȃ���΂Ȃ�Ȃ�
	 */
	return (bool)( state == 1 );
}
#endif

/***
 * \r\r\n => \r\n �ϊ������Ȃ���΂Ȃ�Ȃ�.
 * �������S���𖳏����ɒu��������@�ł̓_���ł���.
 * (�摜�Ȃǂ̃o�C�i���ɂ��܂��܂��̂悤�ȃp�^�[�����܂܂�Ă���ꍇ
 * ���f�[�^���j�󂳂�Ă��܂�)
 * �����ł����ł͂܂�������HTTP�w�b�_�����߂��A�w�b�_�ɂ�������s��
 * chunk�ɂ����s�݂̂��̕ϊ����s���悤�ɂ��Ȃ���΂Ȃ�Ȃ�.
 *
 * CGI�X�N���v�g��Text���[�h�ŕW���o�͂��Ă���ꍇ�A\n�̎����ϊ��ɂ����
 * \r\n��\r\r\n�ȂǂƂȂ�.
 * ������~�ς��邽�߁A���s�R�[�h�Ɋւ��邱�̒������K�v�ł���.
 */

/***
 * end�ɂ͕K���w�b�_���E�̊J�n�ʒu���w��.
 */
static bool
checkValidHeader2( ZnkBfr bfr, size_t end )
{
	const uint8_t* data = ZnkBfr_data( bfr );
	size_t idx   = 0;
	int    u8    = 0;
	int    state = 0;
	for( idx=0; idx<end; ++idx ){
		u8 = data[ idx ];
		switch( u8 ){
		case '\n':
			if( state != 1 ){
				/* �ُ�: :���܂��o�ꂵ�Ă��Ȃ��ɂ��ւ�炸���s������ */
				return false;
			}
			state = 0;
			continue;
		case '\r':
		case '\t':
			continue;
		case ':':
			state = 1;
			continue;
		default:
			break;
		}
		if( u8 < 0x20 && u8 >= 0x80 ){
			/* ���͈̔͂ɂ��镶���R�[�h�������ꍇ�ُ͈�Ƃ��� */
			return false;
		}
	}
	/***
	 * ���Ȃ��Ƃ� : ���o�ꂵ����ɏI���ƂȂ�Ȃ���΂Ȃ�Ȃ�
	 */
	return (bool)( state == 1 );
}
static size_t
seekHdrTerminate( ZnkBfr bfr )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl_rrnrrn[256] = {0};
	static size_t st_occ_tbl_rnrn[256]   = {0};
	static const uint8_t* rrnrrn = (uint8_t*)"\r\r\n\r\r\n";
	static const uint8_t* rnrn   = (uint8_t*)"\r\n\r\n";
	size_t pos1 = Znk_NPOS;
	size_t pos2 = Znk_NPOS;

	if( !st_initialized ){
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rrnrrn, rrnrrn, 6 );
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rnrn,   rnrn,   4 );
		st_initialized = true;
	}
	pos1 = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr),
			rrnrrn, 6, 1, st_occ_tbl_rrnrrn );
	pos2 = ZnkMem_lfind_data_BMS( ZnkBfr_data(bfr), ZnkBfr_size(bfr),
			rnrn, 4, 1, st_occ_tbl_rnrn );

	if( pos1 == Znk_NPOS && pos2 == Znk_NPOS ){
		/***
		 * �w�b�_���͑��݂��Ȃ�.
		 */
		return Znk_NPOS;
	} else if( pos1 < pos2 ){
		/***
		 * pos1 �� �w�b�_���I�[�Ɖ��肵checkValidHeader�ōŏI�e�X�g.
		 */
		if( checkValidHeader2( bfr, pos1 ) ){
			return pos1 + 6;
		}
	} else {
		/***
		 * pos2 �� �w�b�_���I�[�Ɖ��肵checkValidHeader�ōŏI�e�X�g.
		 */
		if( checkValidHeader2( bfr, pos2 ) ){
			return pos2 + 4;
		}
	}
	/***
	 * rrnrrn �܂��� rnrn �͌����������̂� checkValidHeader �̃e�X�g�ɍ��i���Ȃ�����.
	 * ���܂��܂��̂悤�ȃp�^�[�����܂ރo�C�i���ł���Ƃ݂Ȃ�.
	 */
	return Znk_NPOS;
}
static void
getHdrStrSanitizedRRN( ZnkStr hdr_str, ZnkBfr bfr, size_t hdr_end )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl_rrn[256] = {0};
	static const uint8_t* rrn = (uint8_t*)"\r\r\n";

	if( !st_initialized ){
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rrn, rrn, 3 );
		st_initialized = true;
	}

	/***
	 * ���Ȃ��Ƃ� bfr���ɂ����� hdr_end�܂ł͈̔͂͒ʏ�̕����񂪊i�[����Ă���ƍl���Ă悢.
	 */
	ZnkStr_assign( hdr_str, 0, (char*)ZnkBfr_data( bfr ), hdr_end );

	ZnkStrEx_replace_BMS( hdr_str, 0,
			(const char*)rrn, 3, st_occ_tbl_rrn,
			"\r\n", 2,
			Znk_NPOS, Znk_NPOS );

}


#if 0
static void
sanitizeRRN( ZnkStr str )
{
	static bool   st_initialized = false;
	static size_t st_occ_tbl_rrn[256]    = {0};
	static const uint8_t* rrn    = (uint8_t*)"\r\r\n";
	size_t pos = Znk_NPOS;
	ZnkStr hdr_str = ZnkStr_new( "" );

	if( !st_initialized ){
		ZnkMem_getLOccTable_forBMS( st_occ_tbl_rrn,    rrn,    3 );
		st_initialized = true;
	}

	pos = seekHdrTerminate( str );

	if( pos == Znk_NPOS ){
		/***
		 * �w�b�_���E�͑��݂��Ȃ��ƍl������.
		 * ���̏ꍇ�w�b�_�����݂��Ȃ����߉����ϊ��͂��Ȃ�.
		 */
	} else {
		/***
		 * �w�b�_���E�����݂���.
		 */
		if( checkValidHeader( str, pos ) ){
			ZnkStr_assign( hdr_str, 0, ZnkStr_cstr( str ), pos );
			ZnkStrEx_replace_BMS( hdr_str, 0,
					(const char*)rrn, 3, st_occ_tbl_rrn,
					"\r\n", 2,
					Znk_NPOS, Znk_NPOS );
			ZnkBfr_replace( str, 0, pos, (uint8_t*)ZnkStr_cstr( hdr_str ), ZnkStr_leng( hdr_str ) );
		} else {
			/***
			 * ���̃w�b�_���E�̓f�[�^���Ɋ܂܂�Ă���p�^�[���ł���Ƃ݂Ȃ��A
			 * �����ϊ����Ȃ�.
			 */
		}

	}

	ZnkStr_delete( hdr_str );
}
#endif

/***
 * CGI environment variable list :
 * �����ɂ͂��ꂪ���ׂĂł͂Ȃ�.
 *
 * REQUEST_METHOD :
 *   GET, POST, HEAD �Ȃǂ� HTTP Request������.
 * QUERY_STRING :
 *   URL�ɂ�����u?�v�����ȍ~�̕���.
 * SCRIPT_NAME :
 *   CGI�X�N���v�g�̖��O.
 *
 * GATEWAY_INTERFACE :
 *   �Q�[�g�E�F�C�v���g�R������(�Ⴆ��CGI/1.1)
 * AUTH_TYPE :
 *   �F�ؕ���(�Ⴆ�� MD5, Basic�Ȃ�).
 *
 * REMOTE_IDENT :
 *   �N���C�A���g���̃��[�U�[ID
 * REMOTE_USER :
 *   �N���C�A���g���̃��[�U�[��
 *
 * HTTP_FORWARDED :
 *   ���̗v�����t�H���[�h�����v���L�V�T�[�o�[�̏��.
 * HTTP_REFERER :
 *   ������Referer�ł���.
 * HTTP_X_FORWARDED_FOR :
 *   ���̗v�����t�H���[�h�����v���L�V�T�[�o�[��IP�A�h���X.
 */
static void
setupEnv_forGET( ZnkVarpAry hdr_vars, ZnkStr query_str, RanoModule mod )
{
	/* for PHP */
	{
		ZnkEnvVar_set( "REDIRECT_STATUS", "1" );
	}

	/* Phase-1 : Base Info */
	{
		static const char* content_type = "application/x-www-form-urlencoded";
	
		ZnkEnvVar_set( "REQUEST_METHOD", "GET" );
		ZnkEnvVar_set( "CONTENT_TYPE", content_type );
		{
			char content_length_buf[ 256 ] = "";
			Znk_snprintf( content_length_buf, sizeof(content_length_buf), "%zu", ZnkStr_leng(query_str) );
			ZnkEnvVar_set( "CONTENT_LENGTH", content_length_buf );
		}
		ZnkEnvVar_set( "QUERY_STRING", ZnkStr_cstr(query_str) );
#if 0
		/* TODO */
		ZnkEnvVar_set( "SCRIPT_NAME", "cgi_test.cgi" );
#endif
	}

	/* Phase-2 : Seaver Info */
	{
		if( ZnkS_eq( MoaiServerInfo_acceptable_host(), "ANY" ) ){ 
			const uint32_t private_ip = MoaiServerInfo_private_ip();
			char private_ipstr[ 64 ] = "";
			ZnkNetIP_getIPStr_fromU32( private_ip, private_ipstr, sizeof(private_ipstr) );
			ZnkEnvVar_set( "SERVER_NAME", private_ipstr );
		} else {
			/* Anyway, we set as LOOPBACK */
			ZnkEnvVar_set( "SERVER_NAME", "127.0.0.1" );
		}
		{
			const uint16_t port = MoaiServerInfo_port();
			char port_buf[ 64 ] = "";
			Znk_snprintf( port_buf, sizeof(port_buf), "%u", port );
			ZnkEnvVar_set( "SERVER_PORT", port_buf );
		}
		ZnkEnvVar_set( "SERVER_PROTOCOL", "HTTP/1.1" );
		ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai" );
		ZnkEnvVar_set( "REMOTE_ADDR", "127.0.0.1" );
		ZnkEnvVar_set( "REMOTE_HOST", "127.0.0.1" );
	}

	/* Phase-3 : User-Agent Info */
	{
		ZnkVarp varp = NULL;
		if( mod ){
			const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
			ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
			RanoModule_filtHtpHeader(  mod, hdr_vars );
			RanoModule_filtCookieVars( mod, hdr_vars, is_all_replace, extra_vars );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_COOKIE", ZnkStr_cstr(val) );
			{
				size_t idx;
				size_t size = ZnkHtpHdrs_val_size( varp );
				RanoLog_printf( "HTTP_COOKIE debug\n" );
				for( idx=0; idx<size; ++idx ){
					val = ZnkHtpHdrs_val( varp, 0 );
					RanoLog_printf( "cookie_val[%zu]=[%s]\n", idx, ZnkStr_cstr(val) );
				}
			}
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "User-Agent" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_USER_AGENT", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Accept" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_ACCEPT", ZnkStr_cstr(val) );
		}
	}
}

static void
setupEnv_forPOST( ZnkVarpAry hdr_vars, size_t content_length, ZnkStr query_str, RanoModule mod )
{
	/* for PHP */
	{
		ZnkEnvVar_set( "REDIRECT_STATUS", "1" );
	}

	/* Phase-1 : Base Info */
	{
		const char* content_type = NULL;

		ZnkEnvVar_set( "REQUEST_METHOD", "POST" );
		content_type = ZnkHtpHdrs_scanContentType( hdr_vars, NULL );
		if( content_type == NULL ){
			/***
			 * Content-Type �����݂��Ȃ�.
			 * ���̂悤��POST�͂ǂ������ׂ���?
			 * �����̏ꍇ�A�{���G���[�Ƃ��ׂ��Ǝv���邪�A
			 * �����ł͂Ƃ肠���� application/x-www-form-urlencoded �Ƃ��Ď�舵��.
			 */
			content_type = "application/x-www-form-urlencoded";
		}
	
		ZnkEnvVar_set( "CONTENT_TYPE", content_type );
		{
			char content_length_buf[ 256 ];
			Znk_snprintf( content_length_buf, sizeof(content_length_buf), "%zu", content_length );
			ZnkEnvVar_set( "CONTENT_LENGTH", content_length_buf );
		}
		ZnkEnvVar_set( "QUERY_STRING", ZnkStr_cstr(query_str) );

#if 0
		/* TODO */
		ZnkEnvVar_set( "SCRIPT_NAME", "cgi_test.cgi" );
#endif
	}

	/* Phase-2 : Seaver Info */
	{
		{
			uint32_t private_ip = MoaiServerInfo_private_ip();
			char private_ipstr[ 64 ] = "";
			ZnkNetIP_getIPStr_fromU32( private_ip, private_ipstr, sizeof(private_ipstr) );
			ZnkEnvVar_set( "SERVER_NAME", private_ipstr );
		}
		{
			const uint16_t port = MoaiServerInfo_port();
			char port_buf[ 64 ] = "";
			Znk_snprintf( port_buf, sizeof(port_buf), "%u", port );
			ZnkEnvVar_set( "SERVER_PORT", port_buf );
		}
		ZnkEnvVar_set( "SERVER_PROTOCOL", "HTTP/1.1" );
		ZnkEnvVar_set( "SERVER_SOFTWARE", "Moai" );

		ZnkEnvVar_set( "REMOTE_ADDR", "127.0.0.1" );
		ZnkEnvVar_set( "REMOTE_HOST", "127.0.0.1" );
		// ZnkEnvVar_set( "REMOTE_PORT", "0" );

	}

	/* Phase-3 : User-Agent Info */
	{
		ZnkVarp varp = NULL;
		if( mod ){
			const bool is_all_replace = MoaiServerInfo_isAllReplaceCookie();
			ZnkVarpAry extra_vars     = MoaiServerInfo_refCookieExtraVars();
			RanoModule_filtHtpHeader(  mod, hdr_vars );
			RanoModule_filtCookieVars( mod, hdr_vars, is_all_replace, extra_vars );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Cookie" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_COOKIE", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "User-Agent" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_USER_AGENT", ZnkStr_cstr(val) );
		}
		varp = ZnkHtpHdrs_find_literal( hdr_vars, "Accept" );
		if( varp ){
			ZnkStr val = ZnkHtpHdrs_val( varp, 0 );
			ZnkEnvVar_set( "HTTP_ACCEPT", ZnkStr_cstr(val) );
		}
	}
}

static void
sendChunk( ZnkSocket sock, ZnkBfr bfr )
{
	//ZnkBfr chnk = ZnkBfr_create_null();
	char chunk_size_line[ 256 ] = "";
	Znk_snprintf( chunk_size_line, sizeof(chunk_size_line), "%zx\r\n", ZnkBfr_size(bfr) );

#if 1
	ZnkSocket_send_cstr( sock, chunk_size_line );
	ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
	ZnkSocket_send_cstr( sock, "\r\n" );
#else
	ZnkBfr_append_dfr( chnk, (uint8_t*)chunk_size_line, Znk_strlen(chunk_size_line) );
	ZnkBfr_append_dfr( chnk, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
	ZnkBfr_append_dfr( chnk, (uint8_t*)"\r\n", 2 );
	ZnkSocket_send( sock, ZnkBfr_data(chnk), ZnkBfr_size(chnk) );
#endif
	ZnkBfr_clear( bfr );
	//ZnkBfr_destroy( chnk );
}


static void
appointCGISockAsExile( ZnkSocket sock )
{
	MoaiConnection mcn = MoaiConnection_find_byISock( sock );
	if( mcn ){
		ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
		if( O_sock == sock ){
			/***
			 * sock �� Exile �ɖ߂�.
			 * �������E�̑ΏۂƂ���.
			 */
			mcn->O_sock_ = ZnkSocket_INVALID;
		}
	}
}


#if defined(Znk_TARGET_WINDOWS)
#  include<windows.h>

typedef struct {
	HANDLE    hChildProcess_;
	HANDLE    hOutputRead_;
	HANDLE    hInputWrite_;
	ZnkSocket sock_;
} ChildProcessInfo;

/**
 * Displays the error number and corresponding message.
 */
static void
printError( char* pszAPI )
{
	const int error_code = GetLastError();
	if( error_code != 0 ){
		LPVOID lpvMessageBuffer;
		
		FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, error_code,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpvMessageBuffer, 0, NULL);
		
		RanoLog_printf( "MoaiCGI : Error: %s : error_code=[%d] message=[%s].\n",
				pszAPI, error_code, (char *)lpvMessageBuffer );
		
		LocalFree(lpvMessageBuffer);
	}
}

static BOOL
closeHandle( HANDLE handle ){

	if( handle && handle != INVALID_HANDLE_VALUE ){
		return CloseHandle( handle );
	}
	return FALSE;
}

static void
ReadAndHandleOutput2( HANDLE hPipeRead, ZnkSocket sock, bool use_chunked_mode )
{
	uint8_t ubuf[ 4096 ] = { 0 };
	DWORD   nBytesRead = 0;
	ZnkBfr  bfr = ZnkBfr_create_null();
	BOOL    result;
	size_t  hdr_end = Znk_NPOS;

	/***
	 * use_chunked_mode �ł�ReadFile������������
	 * HTTP�Ƃ��Ă�chunked�`���Ƃ��đ��\�P�b�g�֑���.
	 * �������ŏ��̃w�b�_�������͂܂����S�ɓǂݍ���.
	 */
	RanoLog_printf( "ReadAndHandleOutput2 : begin.\n" );
	while( true ){
		result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
		if( !result || nBytesRead == 0 ){
			if( GetLastError() == ERROR_BROKEN_PIPE ){
				/***
				 * pipe done - normal exit path.
				 */
				break;
			} else {
				printError("ReadFile"); // Something bad happened.
				break;
			}
		}
		if( ubuf[ nBytesRead-1 ] == '\0' ){
			RanoLog_printf( "ReadAndHandleOutput2 : null-terminate.\n" );
		}
		ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );

		hdr_end = seekHdrTerminate( bfr );
		if( hdr_end != Znk_NPOS ){
			break;
		}
	}

	if( hdr_end == Znk_NPOS ){
		/***
		 * �ŏ��̃w�b�_�������݂��Ȃ��p�^�[��.
		 * broken pipe �ɂ���̃��[�v���I�����Ă���͂��ł���A
		 * ����body�����܂߂��ׂēǂݏI����Ă���͂��ł���.
		 */
		sendResponseOfCGI( bfr, sock );

	} else {
		ZnkStr hdr_str = ZnkStr_new( "" );

		/***
		 * �ŏ��̃w�b�_������ RRN �Ƃ����p�^�[����RN�ɒu�����������̂��擾���Ă���.
		 */
		getHdrStrSanitizedRRN( hdr_str, bfr, hdr_end );

		/***
		 * �w�b�_���͂��͂�s�v�Ȃ̂ŏ������Ă���.
		 */
		ZnkBfr_erase( bfr, 0, hdr_end );

		if( use_chunked_mode ){

			/***
			 * hdr_str�𑗐M.
			 */
			sendHtpHdrOfCGI_forChunked( hdr_str, sock );
			/***
			 * �ŏ���chunk�𑗐M.
			 */
			if( ZnkBfr_size( bfr ) ){
				sendChunk( sock, bfr );
			}

			while( true ){
				result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
				if( !result || nBytesRead == 0 ){
					if( GetLastError() == ERROR_BROKEN_PIPE ){
						/***
						 * pipe done - normal exit path.
						 */
						break;
					} else {
						printError("ReadFile"); // Something bad happened.
						break;
					}
				}
				/* chunk�̑��M. */
				ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );
				if( ZnkBfr_size( bfr ) ){
					sendChunk( sock, bfr );
				}
			}
			/***
			 * �I�[chunk
			 * 0�݂̂���Ȃ�s�Ƌ�s�𑗂�.
			 * �]���āA0 �̌��ɂ� \r\n ���u���Ȃ���΂Ȃ�Ȃ����Ƃɒ��ӂ���.
			 * (��₱�������������邪�A���ꂪ 0 byte��chunk�𑗂�Ƃ������Ƃł���).
			 */
			ZnkSocket_send( sock, (uint8_t*)"0\r\n\r\n", 5 );
		} else {
			size_t content_length = 0;

			/***
			 * �c������S�ɓǂ�ł��瑗�M����.
			 */
			while( true ){
				result = ReadFile( hPipeRead, ubuf, sizeof(ubuf), &nBytesRead, NULL );
				if( !result || nBytesRead == 0 ){
					if( GetLastError() == ERROR_BROKEN_PIPE ){
						/***
						 * pipe done - normal exit path.
						 */
						break;
					} else {
						printError("ReadFile"); // Something bad happened.
						break;
					}
				}
				ZnkBfr_append_dfr( bfr, ubuf, nBytesRead );
			}

			/***
			 * body�̑S�T�C�Y���m��.
			 */
			content_length = ZnkBfr_size( bfr );
			/***
			 * hdr_str�𑗐M.
			 */
			sendHtpHdrOfCGI_withContentLength( hdr_str, sock, content_length );
			/***
			 * body��(��ʂɃo�C�i���̉\��������)�𑗐M.
			 */
			ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		}
		
		ZnkStr_delete( hdr_str );
	}

	ZnkBfr_destroy( bfr );
	RanoLog_printf( "ReadAndHandleOutput2 : end.\n" );
}

static int
waitForCGIEnd( HANDLE hChildProcess, HANDLE hOutputRead, HANDLE hInputWrite, ZnkSocket sock )
{
	int result = 0;
	DWORD r = 0;
	/**
	 * �W���o�͂̕ߑ��̓��C���X���b�h�ł̖������[�v�ōs��.
	 */
	ReadAndHandleOutput2( hOutputRead, sock, true );
	if( !closeHandle(hOutputRead) ){
		//printError("closeHandle:hOutputRead");
	}

	if( !closeHandle(hInputWrite) ){
		printError("closeHandle:hInputWrite");
	}

	r = WaitForSingleObject( hChildProcess, INFINITE );
	switch( r ){
	case WAIT_FAILED:
		printError("WaitForSingleObject");
		result = -1;
		break;
	case WAIT_ABANDONED:
		RanoLog_printf("MoaiCGI : wait result=WAIT_ABANDONED\n");
		result = -1;
		break;
	case WAIT_OBJECT_0: //����I��
		RanoLog_printf("MoaiCGI : wait result=WAIT_OBJECT_0\n");
		result = 0;
		break;
	case WAIT_TIMEOUT:
		RanoLog_printf("MoaiCGI : wait result=WAIT_TIMEOUT\n");
		result = -1;
		break;
	default:
		RanoLog_printf("MoaiCGI : wait result=[%d]\n", r);
		result = -1;
		break;
	}

	if( !closeHandle(hChildProcess) ){
		//printError("closeHandle:hChildProcess");
	}

	--st_cgi_count;
	return result;
}

static void*
waitForCGIEndThread( void* arg )
{
	int result = 0;
	ChildProcessInfo* cpinfo = Znk_force_ptr_cast( ChildProcessInfo*, arg );
	HANDLE hChildProcess = cpinfo->hChildProcess_;
	HANDLE hOutputRead   = cpinfo->hOutputRead_;
	HANDLE hInputWrite   = cpinfo->hInputWrite_;
	ZnkSocket sock       = cpinfo->sock_;

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread begin.\n" );

	Znk_free( cpinfo );

	result = waitForCGIEnd( hChildProcess, hOutputRead, hInputWrite, sock );

	appointCGISockAsExile( sock );

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread end.\n" );
	return Znk_force_ptr_cast( void*, result );
}

/**
 * Sets up STARTUPINFO structure, and launches redirected child.
 */
static HANDLE
PrepAndLaunchRedirectedChild( const char* cmd, const char* curdir_new, HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr )
{
	HANDLE hChildProcess = NULL;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL result;
	ZnkStr curdir_save = ZnkStr_new( "" );
	
	/***
	 * Set up the start up info struct.
	 */
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;

	/***
	 * Use this if you want to hide the child:
	 */
	// si.wShowWindow = SW_HIDE;
	/*
	 * Note that dwFlags must include STARTF_USESHOWWINDOW if you want to use the wShowWindow flags.
	 ***/

	/***
	 * CreateProcess�̑�5������TRUE���w�肷��΁A�n���h���͍쐬�����v���Z�X�Ɍp�������.
	 * ��6������CREATE_NO_WINDOW���w�肷��ƁA�쐬�����v���Z�X�ŃR���\�[�����\������Ȃ��Ȃ�.
	 *
	 * Launch the process that you want to redirect (in this case, Child.exe).
	 * Make sure Child.exe is in the same directory as redirect.c launch redirect
	 * from a command line to prevent location confusion.
	 */
	ZnkDir_getCurrentDir( curdir_save );
	ZnkDir_changeCurrentDir( curdir_new );
	result = CreateProcess( NULL, (char*)cmd,
			//NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi );
			NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi );
			//NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, curdir_new, &si, &pi );
	ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir_save) );
	RanoLog_printf( "MoaiCGI : Report : curdir_new=[%s]\n", curdir_new ? curdir_new : "NULL" );
	if( !result ){
	   printError( "CreateProcess" );
	}

	/***
	 * Set global child process handle to cause threads to exit.
	 */
	hChildProcess = pi.hProcess;

	/***
	 * Close any unnecessary handles.
	 */
	if( !closeHandle(pi.hThread) ){
		printError( "closeHandle:pi.hThread" );
	}
	ZnkStr_delete( curdir_save );

	return hChildProcess;
}

/**
 * Monitors handle for input. Exits when child exits or pipe breaks.
 */
#if 0
static void
ReadAndHandleOutput( HANDLE hPipeRead, ZnkSocket sock )
{
	char   lpBuffer[512];
	DWORD  nBytesRead = 0;
	ZnkStr str = ZnkStr_new( "" );
	BOOL   result;

	/***
	 * ���̏����ł͂܂����ׂĂ�ǂݍ��ނ܂ő҂�.
	 */
	while( true ){
		lpBuffer[0] = '\0';
		result = ReadFile( hPipeRead, lpBuffer, sizeof(lpBuffer), &nBytesRead, NULL );
		if( !result || nBytesRead == 0 ){
			if( GetLastError() == ERROR_BROKEN_PIPE ){
				/***
				 * pipe done - normal exit path.
				 */
				break;
			} else {
				printError("ReadFile"); /* Something bad happened. */
				break;
			}
		}
		ZnkStr_append( str, lpBuffer, nBytesRead );
	}

	sanitizeRRN( str );
	sendResponseOfCGI( str, sock );
	ZnkStr_delete( str );
}
#endif

static int
runCGIProcess_forWin32( const char* cmd, const char* curdir_new, ZnkSocket sock,
		uint8_t* data, size_t data_size,
		bool enable_std_input )
{
	int result = 0;
	HANDLE hOutputRead    = NULL;
	HANDLE hOutputReadTmp = NULL;
	HANDLE hOutputWrite   = NULL;
	HANDLE hInputRead     = NULL;
	HANDLE hInputWrite    = NULL;
	HANDLE hInputWriteTmp = NULL;
	HANDLE hErrorWrite    = NULL;
	HANDLE hChildProcess  = NULL;
	SECURITY_ATTRIBUTES sa = { 0 };

	/***
	 * Set up the security attributes struct.
	 */
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	/***
	 * Create the child output pipe.
	 */
	if( !CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0) ){
		printError("CreatePipe:StdOut");
	}


	/***
	 * �Ƃ肠����stderr�ւ̏o�͂̏ꍇ��CGI�ւ͔��f�����Ȃ��悤�ɂ���.
	 * �������P�ɂ����𖳌��ɂ��������ł́AMoai���̃R���\�[���ɕ\������
	 * �ȂǂƂ��������Ƃ͏o���Ȃ�.
	 */
#if 0
	/***
	 * Create a duplicate of the output write handle for the std error write handle.
	 * This is necessary in case the child application closes one of its std output handles.
	 */
	if( !DuplicateHandle(
				GetCurrentProcess(), hOutputWrite,
				GetCurrentProcess(), &hErrorWrite,
				0, TRUE,
				DUPLICATE_SAME_ACCESS ) )
	{
		printError("DuplicateHandle:hOutputWrite");
	}
#endif

#if 0
	/***
	 * ����͎v�f�ʂ�ɂ����Ȃ�.
	 */
	{
		HANDLE hStdError = GetStdHandle( STD_ERROR_HANDLE );
		if( hStdError != INVALID_HANDLE_VALUE ){
			if( !DuplicateHandle(
						GetCurrentProcess(), hStdError,
						GetCurrentProcess(), &hErrorWrite,
						0, TRUE,
						DUPLICATE_SAME_ACCESS ) )
			{
				printError("DuplicateHandle:hErrorWrite");
			}
		}
	}
#endif


	/***
	 * Create the child input pipe.
	 */
	if( enable_std_input ){
		if( !CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0) ){
			printError("CreatePipe:StdIn");
		}
	}

	/***
	 * Create new output read handle and the input write handles.
	 * Set the Properties to FALSE. Otherwise, the child inherits the properties and,
	 * as a result, non-closeable handles to the pipes are created.
	 */
	if( !DuplicateHandle(
				GetCurrentProcess(), hOutputReadTmp,
				GetCurrentProcess(), &hOutputRead, // Address of new handle.
				0, FALSE,                          // Make it uninheritable.
				DUPLICATE_SAME_ACCESS ) )
	{
		printError("DupliateHandle:hOutputReadTmp");
	}

	if( hInputWriteTmp ){
		if (!DuplicateHandle(
					GetCurrentProcess(), hInputWriteTmp,
					GetCurrentProcess(), &hInputWrite, // Address of new handle.
					0, FALSE,                          // Make it uninheritable.
					DUPLICATE_SAME_ACCESS))
		{
			printError("DupliateHandle");
		}
	}

	/***
	 * Close inheritable copies of the handles you do not want to be inherited.
	 */
	if (!closeHandle(hOutputReadTmp)){
		//printError("closeHandle:hOutputReadTmp");
	}
	if (!closeHandle(hInputWriteTmp)){
		//printError("closeHandle:hInputWriteTmp");
	}

	hChildProcess = PrepAndLaunchRedirectedChild( cmd, curdir_new, hOutputWrite, hInputRead, hErrorWrite );

	/***
	 * Close pipe handles (do not continue to modify the parent).
	 * You need to make sure that no handles to the write end of the output pipe are maintained
	 * in this process or else the pipe will not close when the child process exits
	 * and the ReadFile will hang.
	 */
	if( !closeHandle(hOutputWrite) ){
		//printError("closeHandle:hOutputWrite");
	}
	if( !closeHandle(hErrorWrite) ){
		//printError("closeHandle:hErrorWrite");
	}
	if( !closeHandle(hInputRead) ){
		//printError("closeHandle:hInputRead");
	}
	

	if( hInputWrite ){
		DWORD nBytesWrote = 0;
		if( !WriteFile( hInputWrite, data, data_size, &nBytesWrote, NULL) ){
			if( GetLastError() == ERROR_NO_DATA ){
				/* Pipe was closed (normal exit path). */
			} else {
				printError("WriteFile");
			}
		}
		if( !WriteFile( hInputWrite, "\0", 1, &nBytesWrote, NULL) ){
			if( GetLastError() == ERROR_NO_DATA ){
				/* Pipe was closed (normal exit path). */
			} else {
				printError("WriteFile");
			}
		}
	}

	++st_cgi_count;
	if( st_cgi_count < st_cgi_count_max ){
		MoaiConnection mcn = NULL;
		ChildProcessInfo* cpinfo = Znk_malloc( sizeof(ChildProcessInfo) );
		cpinfo->sock_          = sock;
		cpinfo->hOutputRead_   = hOutputRead;
		cpinfo->hInputWrite_   = hInputWrite;
		cpinfo->hChildProcess_ = hChildProcess; 

		mcn = MoaiConnection_find_byISock( sock );
		if( mcn ){
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			if( O_sock == ZnkSocket_INVALID ){
				/***
				 * Exile sock�Ƃ��Ď������E����Ă��܂��̂�h�~���邽�߁A
				 * O_sock �� ZnkSocket_INVALID �ȊO�̒l�������I�ɑ�����Ă���.
				 * sock �Ɠ����l�ł悢���낤.
				 */
				mcn->O_sock_ = sock;
			}
		}

		/* �q�v���Z�X�̏I���҂��X���b�h��������グ��.
		 * cpinfo �̉���� waitForCGIEndThread ���ōs��. */
		ZnkThread_create( waitForCGIEndThread, cpinfo );
	} else {
		/***
		 * CGI�����N������MAX�ƂȂ����ꍇ��Main�X���b�h�ŏI����҂�.
		 *
		 * �҂��s��ɓ���Ă������悢�Ƃ����l���������邪�A���������ŏ��ɓo�^���ꂽ�A����
		 * �����܂Ŏ��Ԃ̊|���鏈���ł���Ƃ����Ȃ�A�����̏I����҂��ŐV�̗v����
		 * �������ƍق��ׂ��Ƃ����l�������ł���.
		 * ��Ԃ悢�͓̂o�^���ꂽ���ׂẴX���b�h��������Ƃ������I���������̂�ߑ����邱�Ƃ�
		 * Windows �Ȃ�WaitForMultipleObjectsEx���g���Ύ����ł����������Ƃ肠�������i�K�ł̓��C���X���b�h�ő҂d�l�ɂ��Ă���.
		 */
		result = waitForCGIEnd( hChildProcess, hOutputRead, hInputWrite, sock );
	}

	return result;
}

#elif defined(Znk_TARGET_UNIX)
#  include <stdio.h>
#  include <unistd.h>
#  include <signal.h>
#  include <stdlib.h>
#  include <sys/types.h> /* for waitpid */
#  include <sys/wait.h>  /* for waitpid */

#define R (0)
#define W (1)

typedef struct {
	pid_t     child_process_pid_;
	int       fd_OutputRead_;
	int       fd_InputWrite_;
	ZnkSocket sock_;
} ChildProcessInfo;

static void
ReadAndHandleOutput2( int fd_OutputRead, ZnkSocket sock, bool use_chunked_mode )
{
	uint8_t ubuf[ 4096 ] = { 0 };
	long    readed_size = 0;
	ZnkBfr  bfr = ZnkBfr_create_null();
	//bool    result;
	size_t  hdr_end = Znk_NPOS;

	/***
	 * use_chunked_mode �ł�ReadFile������������
	 * HTTP�Ƃ��Ă�chunked�`���Ƃ��đ��\�P�b�g�֑���.
	 * �������ŏ��̃w�b�_�������͂܂����S�ɓǂݍ���.
	 */
	RanoLog_printf( "ReadAndHandleOutput2 : begin.\n" );
	while( true ){
		ubuf[ 0 ] = '\0';
		readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
		if( readed_size <= 0 ){
			/***
			 * pipe done - normal exit path.
			 */
			break;
		}
		if( ubuf[ readed_size-1 ] == '\0' ){
			RanoLog_printf( "ReadAndHandleOutput2 : null-terminate.\n" );
		}
		ZnkBfr_append_dfr( bfr, ubuf, readed_size );

		hdr_end = seekHdrTerminate( bfr );
		if( hdr_end != Znk_NPOS ){
			break;
		}
	}

	if( hdr_end == Znk_NPOS ){
		/***
		 * �ŏ��̃w�b�_�������݂��Ȃ��p�^�[��.
		 * broken pipe �ɂ���̃��[�v���I�����Ă���͂��ł���A
		 * ����body�����܂߂��ׂēǂݏI����Ă���͂��ł���.
		 */
		sendResponseOfCGI( bfr, sock );

	} else {
		ZnkStr hdr_str = ZnkStr_new( "" );

		/***
		 * �ŏ��̃w�b�_������ RRN �Ƃ����p�^�[����RN�ɒu�����������̂��擾���Ă���.
		 */
		getHdrStrSanitizedRRN( hdr_str, bfr, hdr_end );

		/***
		 * �w�b�_���͂��͂�s�v�Ȃ̂ŏ������Ă���.
		 */
		ZnkBfr_erase( bfr, 0, hdr_end );

		if( use_chunked_mode ){

			/***
			 * hdr_str�𑗐M.
			 */
			sendHtpHdrOfCGI_forChunked( hdr_str, sock );
			/***
			 * �ŏ���chunk�𑗐M.
			 */
			if( ZnkBfr_size( bfr ) ){
				sendChunk( sock, bfr );
			}

			while( true ){
				ubuf[ 0 ] = '\0';
				readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
				if( readed_size <= 0 ){
					/***
					 * pipe done - normal exit path.
					 */
					break;
				}
				/* chunk�̑��M. */
				ZnkBfr_append_dfr( bfr, ubuf, readed_size );
				if( ZnkBfr_size( bfr ) ){
					sendChunk( sock, bfr );
				}
			}
			/***
			 * �I�[chunk
			 * 0�݂̂���Ȃ�s�Ƌ�s�𑗂�.
			 * �]���āA0 �̌��ɂ� \r\n ���u���Ȃ���΂Ȃ�Ȃ����Ƃɒ��ӂ���.
			 * (��₱�������������邪�A���ꂪ 0 byte��chunk�𑗂�Ƃ������Ƃł���).
			 */
			ZnkSocket_send( sock, (uint8_t*)"0\r\n\r\n", 5 );
		} else {
			size_t content_length = 0;

			/***
			 * �c������S�ɓǂ�ł��瑗�M����.
			 */
			while( true ){
				ubuf[ 0 ] = '\0';
				readed_size = read( fd_OutputRead, ubuf, sizeof(ubuf)-1 );
				if( readed_size <= 0 ){
					/***
					 * pipe done - normal exit path.
					 */
					break;
				}
				ZnkBfr_append_dfr( bfr, ubuf, readed_size );
			}

			/***
			 * body�̑S�T�C�Y���m��.
			 */
			content_length = ZnkBfr_size( bfr );
			/***
			 * hdr_str�𑗐M.
			 */
			sendHtpHdrOfCGI_withContentLength( hdr_str, sock, content_length );
			/***
			 * body��(��ʂɃo�C�i���̉\��������)�𑗐M.
			 */
			ZnkSocket_send( sock, ZnkBfr_data(bfr), ZnkBfr_size(bfr) );
		}
		
		ZnkStr_delete( hdr_str );
	}

	ZnkBfr_destroy( bfr );
	RanoLog_printf( "ReadAndHandleOutput2 : end.\n" );
}

static int
waitForCGIEnd( pid_t child_process_pid, int fd_OutputRead, int fd_InputWrite, ZnkSocket sock )
{
	int result = 0;
	int state   = 0;
	int options = 0;
	//DWORD r = 0;
	/**
	 * �W���o�͂̕ߑ��̓��C���X���b�h�ł̖������[�v�ōs��.
	 */
	ReadAndHandleOutput2( fd_OutputRead, sock, true );

	close( fd_OutputRead );
	close( fd_InputWrite );

	result = waitpid( child_process_pid, &state, options );
#if 0
	r = WaitForSingleObject( hChildProcess, INFINITE );
	switch( r ){
	case WAIT_FAILED:
		printError("WaitForSingleObject");
		result = -1;
		break;
	case WAIT_ABANDONED:
		RanoLog_printf("MoaiCGI : wait result=WAIT_ABANDONED\n");
		result = -1;
		break;
	case WAIT_OBJECT_0: //����I��
		RanoLog_printf("MoaiCGI : wait result=WAIT_OBJECT_0\n");
		result = 0;
		break;
	case WAIT_TIMEOUT:
		RanoLog_printf("MoaiCGI : wait result=WAIT_TIMEOUT\n");
		result = -1;
		break;
	default:
		RanoLog_printf("MoaiCGI : wait result=[%d]\n", r);
		result = -1;
		break;
	}

	if( !closeHandle(hChildProcess) ){
		//printError("closeHandle:hChildProcess");
	}
#endif

	--st_cgi_count;
	return result;
}

static void*
waitForCGIEndThread( void* arg )
{
	int result = 0;
	ChildProcessInfo* cpinfo = Znk_force_ptr_cast( ChildProcessInfo*, arg );
	pid_t child_process_pid = cpinfo->child_process_pid_;
	int   fd_OutputRead     = cpinfo->fd_OutputRead_;
	int   fd_InputWrite     = cpinfo->fd_InputWrite_;
	ZnkSocket sock          = cpinfo->sock_;

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread begin.\n" );

	Znk_free( cpinfo );

	result = waitForCGIEnd( child_process_pid, fd_OutputRead, fd_InputWrite, sock );

	appointCGISockAsExile( sock );

	RanoLog_printf( "MoaiCGI : waitForCGIEndThread end.\n" );
	return Znk_force_ptr_cast( void*, result );
}


static ZnkStrAry
makeCmdList( const char* cmd, const char*** cmd_argv_and )
{
	ZnkStrAry    cmd_list = ZnkStrAry_create( true );
	const char** cmd_argv = NULL;
	size_t       cmd_size = 0;
	size_t       idx;

	ZnkStrEx_addSplitCSet( cmd_list,
			cmd, Znk_strlen(cmd),
			" \t", 2,
			4 );
	cmd_size = ZnkStrAry_size(cmd_list);
	cmd_argv = Znk_malloc( sizeof(char*) * ( cmd_size + 1 ) );

	for( idx=0; idx<cmd_size; ++idx ){
		cmd_argv[ idx ] = ZnkStrAry_at_cstr(cmd_list, idx);
	}
	cmd_argv[ cmd_size ] = NULL;

	*cmd_argv_and = cmd_argv;
	return cmd_list;
}

int
popen2( const char* cmd_file, const char** cmd_argv, const char* curdir_new, int* fd_r, int* fd_w )
{
	int pipe_c2p[2], pipe_p2c[2];
	int pid;

	/* Create two of pipes. */
	if(pipe(pipe_c2p)<0){
		perror("popen2");
		return(-1);
	}
	if(pipe(pipe_p2c)<0){
		perror("popen2");
		close(pipe_c2p[R]);
		close(pipe_c2p[W]);
		return(-1);
	}

	/* Invoke processs */
	if((pid=fork())<0){
		perror("popen2");
		close(pipe_c2p[R]);
		close(pipe_c2p[W]);
		close(pipe_p2c[R]);
		close(pipe_p2c[W]);
		return(-1);
	}
	if(pid==0){
		/* I'm Child */
		close(pipe_p2c[W]);
		close(pipe_c2p[R]);
		dup2(pipe_p2c[R],0); /* Parent2Child�Ŏq������͓ǂ݂ƂȂ�A�܂肱�̃p�C�v�̏o��. �����W�����͂֏㏑���R�s�[. */
		dup2(pipe_c2p[W],1); /* Child2Parent�Ŏq������͏����ƂȂ�A�܂肱�̃p�C�v�̓���. �����W���o�͂֏㏑���R�s�[. */
		close(pipe_p2c[R]);
		close(pipe_c2p[W]);
		ZnkDir_changeCurrentDir( curdir_new );
		//if(execlp("sh","sh","-c",command,NULL)<0){
		//if( execlp( command, command, NULL ) < 0 ){
		if( execvp( cmd_file, Znk_force_ptr_cast(char* const*, cmd_argv) ) < 0 ){
			perror("popen2:execvp");
			close(pipe_p2c[R]);
			close(pipe_c2p[W]);
			exit(1);
		}
	}
	/* I'm Parent */
	close(pipe_p2c[R]);
	close(pipe_c2p[W]);
	*fd_w = pipe_p2c[W]; /* Parent2Child�Őe������͏����ƂȂ�A�܂肱�̃p�C�v�̓��� */
	*fd_r = pipe_c2p[R]; /* Child2Parent�Őe������͓ǂ݂ƂȂ�A�܂肱�̃p�C�v�̏o�� */
	
	return pid;
}


#if 0
static void
readFD_andSend( int fd_r, ZnkSocket sock )
{
	char buf[ 512+1 ];
	ZnkStr str = ZnkStr_new( "" );
	int size;
	while( true ){
		buf[ 0 ] = '\0';
		size = read( fd_r, buf, sizeof(buf)-1 );
		if( size >= 0 ){
			buf[ size ] = '\0';
		}
		if( size <= 0 ){
			break;
		}
		ZnkStr_append( str, buf, size );
	}

	sanitizeRRN( str );
	sendResponseOfCGI( str, sock );
	ZnkStr_delete( str );
}
#endif

static int
runCGIProcess_forUNIX( const char* cmd, const char* curdir_new, ZnkSocket sock,
		uint8_t* data, size_t data_size,
		bool is_write_terminate_nul )
{
	int fd_r = -1;
	int fd_w = -1;
	pid_t child_process_pid = -1;
	int result = 0;

	const char** cmd_argv = NULL;
	ZnkStrAry    cmd_list = makeCmdList( cmd, &cmd_argv );

	child_process_pid = popen2( cmd_argv[0], cmd_argv, curdir_new, &fd_r, &fd_w );

	/***
	 * �Ō��terminate nul�͕K�v.
	 * CGI����stdin�̓��͑҂������݂���ꍇ��
	 * ���������Ŋm���ɏI�点�Ȃ���΂Ȃ�Ȃ�.
	 */
	write( fd_w, data, data_size );
	if( is_write_terminate_nul ){
		write( fd_w, "\0", 1 );
	}

	//readFD_andSend( fd_r, sock );
	++st_cgi_count;
	if( st_cgi_count < st_cgi_count_max ){
		MoaiConnection mcn = NULL;
		ChildProcessInfo* cpinfo = Znk_malloc( sizeof(ChildProcessInfo) );
		cpinfo->sock_          = sock;
		cpinfo->fd_OutputRead_ = fd_r;
		cpinfo->fd_InputWrite_ = fd_w;
		cpinfo->child_process_pid_ = child_process_pid;

		mcn = MoaiConnection_find_byISock( sock );
		if( mcn ){
			ZnkSocket O_sock = MoaiConnection_O_sock( mcn );
			if( O_sock == ZnkSocket_INVALID ){
				/***
				 * Exile sock�Ƃ��Ď������E����Ă��܂��̂�h�~���邽�߁A
				 * O_sock �� ZnkSocket_INVALID �ȊO�̒l�������I�ɑ�����Ă���.
				 * sock �Ɠ����l�ł悢���낤.
				 */
				mcn->O_sock_ = sock;
			}
		}

		/* �q�v���Z�X�̏I���҂��X���b�h��������グ��.
		 * cpinfo �̉���� waitForCGIEndThread ���ōs��. */
		ZnkThread_create( waitForCGIEndThread, cpinfo );
	} else {
		/***
		 * CGI�����N������MAX�ƂȂ����ꍇ��Main�X���b�h�ŏI����҂�.
		 *
		 * �҂��s��ɓ���Ă������悢�Ƃ����l���������邪�A���������ŏ��ɓo�^���ꂽ�A����
		 * �����܂Ŏ��Ԃ̊|���鏈���ł���Ƃ����Ȃ�A�����̏I����҂��ŐV�̗v����
		 * �������ƍق��ׂ��Ƃ����l�������ł���.
		 * ��Ԃ悢�͓̂o�^���ꂽ���ׂẴX���b�h��������Ƃ������I���������̂�ߑ����邱�Ƃ�
		 * Windows �Ȃ�WaitForMultipleObjectsEx���g���Ύ����ł����������Ƃ肠�������i�K�ł̓��C���X���b�h�ő҂d�l�ɂ��Ă���.
		 */
		result = waitForCGIEnd( child_process_pid, fd_r, fd_w, sock );
	}


	Znk_free( cmd_argv );
	ZnkStrAry_destroy( cmd_list );
	return result;
}

#else

#  error "Not Supported"

#endif



int
MoaiCGI_runGet( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str )
{

#if defined(Znk_TARGET_WINDOWS)
	static const bool enable_std_input = false;
	setupEnv_forGET( hdr_vars, query_str, mod );
	return runCGIProcess_forWin32( cmd, curdir_new, sock,
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			enable_std_input );

#elif defined(Znk_TARGET_UNIX)
	setupEnv_forGET( hdr_vars, query_str, mod );
	/***
	 * QUERY_STRING�����݂���ꍇ
	 * PHP�ł͂����炭QUERY_STRING�S�̂������ɂ��邩�ǂ������`�F�b�N���Ă���悤��
	 * ����S�̂������ő���Ȃ���Ώ����҂����������A���䂪�߂�Ȃ��Ȃ��Ă��܂�.
	 */
	return runCGIProcess_forUNIX( cmd, curdir_new, sock, 
			(uint8_t*)ZnkStr_cstr( query_str ), ZnkStr_leng( query_str )+1,
			false );
#endif
}

int
MoaiCGI_runPost( const char* cmd, const char* curdir_new, ZnkSocket sock, RanoModule mod,
		const size_t hdr_size, ZnkStrAry hdr1st, ZnkVarpAry hdr_vars,
		size_t content_length, ZnkBfr stream, ZnkStr query_str )
{

#if defined(Znk_TARGET_WINDOWS)
	static const bool enable_std_input = true;
	setupEnv_forPOST( hdr_vars, content_length, query_str, mod );
	return runCGIProcess_forWin32( cmd, curdir_new, sock,
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			enable_std_input );

#elif defined(Znk_TARGET_UNIX)
	setupEnv_forPOST( hdr_vars, content_length, query_str, mod );
	return runCGIProcess_forUNIX( cmd, curdir_new, sock, 
			ZnkBfr_data(stream)+hdr_size, ZnkBfr_size(stream)-hdr_size,
			true );
#endif
}


