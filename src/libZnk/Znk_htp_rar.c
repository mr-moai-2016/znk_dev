#include <Znk_htp_rar.h>
#include <Znk_net_base.h>
#include <Znk_socket.h>
#include <Znk_cookie.h>
#include <Znk_s_base.h>
#include <Znk_str_ary.h>
#include <Znk_stdc.h>
#include <Znk_mem_find.h>
#include <Znk_sys_errno.h>
#include <Znk_str_ex.h>
#include <Znk_stock_bio.h>
#include <Znk_zlib.h>
#include <Znk_def_util.h>

#include <stdio.h>
#include <string.h>

static void
printLastRecvError( ZnkStr ermsg, int line_idx )
{
	if( ermsg ){
		ZnkStr_addf( ermsg,
				"ZnkHtpRAR : Error : line=%d : ZnkSocket_recv error : [%s]\n",
				line_idx, ZnkNetBase_getErrKey(ZnkNetBase_getLastErrCode()) );
	}
}

typedef struct HeaderInfo_tag {
	bool       is_chunked_;
	size_t     content_length_;
	bool       is_gzip_;
	ZnkStrAry set_cookie_; 
} HeaderInfo;

static HeaderInfo*
createHeaderInfo( void )
{
	HeaderInfo* hdr_info = Znk_malloc( sizeof( struct HeaderInfo_tag ) );
	hdr_info->is_chunked_     = false;
	hdr_info->content_length_ = 0;
	hdr_info->is_gzip_        = false;
	hdr_info->set_cookie_     = ZnkStrAry_create( true );
	return hdr_info;
}
static void
clearHeaderInfo( HeaderInfo* hdr_info )
{
	hdr_info->is_chunked_     = false;
	hdr_info->content_length_ = 0;
	hdr_info->is_gzip_        = false;
	ZnkStrAry_clear( hdr_info->set_cookie_ );
}
static void
destroyHeaderInfo( HeaderInfo* hdr_info )
{
	if( hdr_info ){
		ZnkStrAry_destroy( hdr_info->set_cookie_ );
		Znk_free( hdr_info );
	}
}

static bool
appendRecvedData( ZnkStockBIO sbio, ZnkBfr dst, size_t data_size, ZnkStr ermsg )
{
	uint8_t buf[ 4096 ];
	size_t readed_size;
	size_t begin = ZnkBfr_size(dst);
	while( data_size ){
		if( data_size > sizeof(buf) ){
			readed_size = ZnkStockBIO_read( sbio, buf, sizeof(buf) );
		} else {
			readed_size = ZnkStockBIO_read( sbio, buf, data_size );
		}
		if( readed_size == 0 ){
			/* end of recv */
			ZnkStr_addf( ermsg, "ZnkHtpRAR : appendRecvedData end of recv(readed_size=0)\n" );
			return false;
		} else if( readed_size < 0 ){
			/* error of recv */
			printLastRecvError( ermsg, __LINE__ );
			return false;
		}
		data_size -= readed_size;
		ZnkBfr_append_dfr( dst, buf, readed_size );
	}
	if( ermsg ){
		ZnkStr_addf( ermsg, "ZnkHtpRAR : appendRecvedData : size=%zu : [", data_size );
		ZnkStr_append( ermsg, (char*)ZnkBfr_data(dst)+begin, data_size );
		ZnkStr_add( ermsg, "]\n" );
	}
	return true;
}

/****
 * chunked���[�h�̏ꍇ�A"hello_world" �Ƃ���body�͈ȉ��̂悤�Ȍ`�̃f�[�^�ƂȂ��Ă���.
 *
 * HTTP/1.1 200 OK
 * Content-Type: text/html
 * Transfer-Encoding: chunked
 * 
 * 6
 * hello_
 * 5
 * world
 * 0
 *
 * chunk_size�̌��\r\n������̂͂������ł��邪�A���̌�chunk_size byte�i�񂾂Ƃ���ɂ�\r\n�����邱�Ƃɒ���.
 * ���̌�Ɏ���chunk_size������.
 */
static bool
appendBody( ZnkStockBIO sbio, ZnkBfr body, const HeaderInfo* hdr_info, ZnkStr ermsg )
{
	/***
	 * ��:
	 * recv����̖߂�l�� 0 �̏ꍇ�A0 ���傫���ꍇ�ɔ�ׂĂ��Ȃ蒷���҂����Ԃ���������\��������.
	 * �����������邽�߁A�Ȃ�ׂ��I���̃^�C�~���O��recv�ɗ���Ȃ��悤�ɂ���.
	 */
	if( hdr_info->is_chunked_ ){
		size_t chunk_size = 0;
		char chunk_size_line[ 1024 ] = "";
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : [chunked mode]\n" );
		}

		while( true ){
			/* chunk_size_line��ǂݍ��� */
			ZnkStockBIO_readLineCRLF( sbio, chunk_size_line, sizeof(chunk_size_line) );
			/* chunk_size �� 16�i���ŕ\�L����Ă��� */
			if( !ZnkS_getSzX( &chunk_size, chunk_size_line ) ){
				/* error */
				ZnkStr_addf( ermsg, "ZnkHtpRAR : Error : Invalid chunk_size_line=[%s]\n", chunk_size_line );
				return false;
			}
			/***
			 * chunked���[�h�ɂ����ẮA���т�chunk_size�͕K��0�ɂȂ��Ă���.
			 * �܂���������ɓ��B�����Ȃ�result�͐����ł���.
			 */
			if( chunk_size == 0 ){
				/* success */
				break;
			}

			if( !appendRecvedData( sbio, body, chunk_size, ermsg ) ){
				/* error */
				return false;
			}

			/* chunk data�̈�ԍŌ�ɂ�CRLF������. �����ǂݔ�΂� */
			ZnkStockBIO_readLineCRLF( sbio, chunk_size_line, sizeof(chunk_size_line) );
		}

	} else if( hdr_info->content_length_ > 0 ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : [content_length(%zu) mode]\n", hdr_info->content_length_ );
		}
		if( !appendRecvedData( sbio, body, hdr_info->content_length_, ermsg ) ){
			/* error */
			return false;
		}
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : appendRecvedData result of body size=[%zu]\n", ZnkBfr_size(body) );
		}

	} else {
		uint8_t buf[ 4096 ];
		size_t readed_size;
		if( ermsg ){
			ZnkStr_add( ermsg, "ZnkHtpRAR : [unknown mode]\n" );
		}
		/***
		 * ���̏ꍇ�A�I���̃^�C�~���O��recv�����0�߂�l�ɗ���ق��Ȃ�.
		 */
		while( true ){
			readed_size = ZnkStockBIO_read( sbio, buf, sizeof(buf) );
			if( readed_size < 0 ){
				/* error */
				break;
			}
			if( readed_size == 0 ){
				/* ����ňꉞ�����Ƃ݂Ȃ�... */
				break;
			}
			ZnkBfr_append_dfr( body, buf, readed_size );
		}
	}
	return true;
}

/***
 * �܂��SHeader�𓾂�܂�Sock�ʐM�ɂ��recv���J��Ԃ�.
 * ���̍ہAsizeof(buf) byte����M���邪�A������first_recved_buf�Ƃ����`��
 * �~�ς��Ď擾���AHeader�S�̂�ԗ��������_��recv�͒��f����.
 * ���̊֐���first_recved_buf��Ԃ����A����̎c��ɂ͒ʏ�body�̊J�n�������܂܂��
 * ����͂��Ȃ̂ŁA��ɉ����ߒ��̊֐��ł��̕������Q�Ƃ���.
 * �܂�������Header�̑S�������Header�̏I���o�C�g�ʒu���擾����.
 */
static int
recvHeader( ZnkStockBIO sbio, ZnkHtpHdrs recv_hdrs, HeaderInfo* hdr_info, ZnkStr ermsg )
{
	char buf[ 8192 ] = { 0 };
	size_t readed_size;

	if( !ZnkStockBIO_readLineCRLF( sbio, buf, sizeof(buf) ) ){
		/***
		 * �ʐM�Ɋւ���G���[������.
		 * ���ꂪ�������邱�Ƃ͂��܂�Ȃ����A���������ꍇ�� WSAECONNRESET( �s�A���ł̐ڑ������Z�b�g����܂��� ) 
		 * ���ł��T�^�I�Ȃ��̂ł���. ����͒ʐM���Ă��鑊�肪�����ɐڑ���f�������Ƃ��Ӗ�����.
		 */
		printLastRecvError( ermsg, __LINE__ );
		return -1;
	}

	ZnkHtpHdrs_registHdr1st( recv_hdrs->hdr1st_, buf, strlen(buf) );

	/* �ŏ��� "HTTP/" �ł͂��܂邩�ǂ������m�F */
	if( !ZnkS_isBegin( buf, "HTTP/" ) ){
		/* �v���g�R����HTTP�Ƃ͈قȂ� */
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : Error : recved status line : [%s]\n", (char*)buf );
			ZnkStr_add(  ermsg, "          : This is not begun by \"HTTP/\"\n" );
		}
		return -1;
	}

	/***
	 * header Key:Val���ǂݍ���
	 */
	clearHeaderInfo( hdr_info );

	{
		/* HTTP version���m�F */
		char* ptr = (char*)( buf + 5 );
		if( strncmp("1.0 ", ptr, 4) == 0 ){
			ptr += 4;
		} else if( strncmp("1.1 ", ptr, 4) == 0 ){
			ptr += 4;
		} else {
			if( ermsg ){
				ZnkStr_add( ermsg, "ZnkHtpRAR : unknown HTTP version\n" );
			}
			return -1;
		}
	
		/* ���̍���(�g�[�N��)�܂ŃX�y�[�X���X�L�b�v */
		while( *ptr == ' ' ){ ++ptr; }
	
		/***
		 * HTTP�̌��ʂ����
		 */
		if( ermsg ){
			int status_code = 0;
			sscanf( ptr, "%d", &status_code );
			switch( status_code ){
			case 200:
				break;
			case 302:
				ZnkStr_add( ermsg, "ZnkHtpRAR : This is 302 moved.\n" );
				break;
			case 404:
				ZnkStr_add( ermsg, "ZnkHtpRAR : This is 404 not found.\n" );
				break;
			default:
				ZnkStr_addf( ermsg, "ZnkHtpRAR : This is status_code[%d].\n", status_code );
				break;
			}
		}
	}

	while( ZnkStockBIO_readLineCRLF( sbio, buf, sizeof(buf) ) ){
		if( ZnkS_empty( buf ) ){
			/* �w�b�_���I����CRLF�ł���Ǝv���� */
			break;
		}
		ZnkHtpHdrs_regist_byLine( recv_hdrs->vars_, buf, Znk_NPOS );
	}

	readed_size = ZnkStockBIO_getReadedSize( sbio );
	if( readed_size < 0 ){
		printLastRecvError( ermsg, __LINE__ );
		return -1;
	}

	{
		ZnkVarp     var = NULL;
		const char* val = NULL;

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Transfer-Encoding" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( ZnkS_eq( val, "chunked" ) ){
				hdr_info->is_chunked_ = true;
			}
		}
		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Content-Length" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( !ZnkS_empty( val ) ){
				ZnkS_getSzU( &hdr_info->content_length_, val );
			}
		}

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Content-Encoding" );
		if( var ){
			val = ZnkHtpHdrs_val_cstr( var, 0 );
			if( ZnkS_eq( val, "gzip" ) ){
				hdr_info->is_gzip_ = true;
			}
		}

		var = ZnkHtpHdrs_find_literal( recv_hdrs->vars_, "Set-Cookie" );
		if( var ){
			const size_t val_size = ZnkHtpHdrs_val_size( var );
			size_t val_leng = 0;
			size_t idx;
			size_t pos;
			for( idx=0; idx<val_size; ++idx ){
				val = ZnkHtpHdrs_val_cstr( var, idx );
				val_leng = Znk_strlen( val );
				pos = ZnkMem_lfind_8( (uint8_t*)val, val_leng, (uint8_t)';', 1 );
				ZnkStrAry_push_bk_cstr( hdr_info->set_cookie_, val, ( pos == Znk_NPOS ) ? val_leng : pos );
			}
		}
	}
	return 0;
}


typedef struct {
	const uint8_t* src_;
	size_t         src_size_;
	ZnkBfr         ans_;
} GZipInfo;
static unsigned int
supplyDstBfr( uint8_t* dst_buf, size_t dst_size, void* arg )
{
	GZipInfo* info = Znk_force_ptr_cast( GZipInfo*, arg );
	ZnkBfr_append_dfr( info->ans_, dst_buf, dst_size );
	return dst_size;
}
static unsigned int
demandSrc( uint8_t* src_buf, size_t src_size, void* arg )
{
	GZipInfo* info = Znk_force_ptr_cast( GZipInfo*, arg );
	const size_t cpy_size = Znk_MIN( info->src_size_, src_size );
	memmove( src_buf, info->src_, cpy_size );
	info->src_      += cpy_size;
	info->src_size_ -= cpy_size;
	return (unsigned int)cpy_size;
}
static bool
inflateGZipBody_toBfr( ZnkBfr ans, const uint8_t* body_data, const size_t body_size, ZnkStr emsg )
{
	bool result = false;
	uint8_t dst_buf[ 4096 ];
	ZnkZStream zst = ZnkZStream_create();

	if( !ZnkZStream_inflateInit( zst, emsg ) ){
		goto FUNC_END;
	} else {
		uint8_t src_buf[ 4096 ];
		GZipInfo gzip_info = { 0 };

		gzip_info.src_      = body_data;
		gzip_info.src_size_ = body_size;
		gzip_info.ans_      = ans;

		while( gzip_info.src_size_ ){
			if( !ZnkZStream_inflate2( zst,
						dst_buf, sizeof(dst_buf), supplyDstBfr, &gzip_info,
						src_buf, sizeof(src_buf), demandSrc,    &gzip_info,
						emsg ) )
			{
				goto FUNC_END;
			}
		}
	}
	if( !ZnkZStream_inflateEnd( zst, emsg ) ){
		goto FUNC_END;
	}

	result = true;
FUNC_END:

	ZnkZStream_destroy( zst );
	return result;
}


#if 0
static bool
uncompressBfr( ZnkZStream zst, ZnkBfr dst_bfr, const ZnkBfr src_bfr, ZnkStr ermsg )
{
	uint8_t dst_buf[ 4096 ];
	size_t expanded_dst_size = 0;
	size_t expanded_src_size = 0;
	const uint8_t* src;
	size_t src_size;
	src      = ZnkBfr_data( src_bfr );
	src_size = ZnkBfr_size( src_bfr );
	while( src_size ){
		if( !ZnkZStream_inflate( zst, dst_buf, sizeof(dst_buf), src, src_size,
				&expanded_dst_size, &expanded_src_size, ermsg ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkHtpRAR : inflateGZip : Error : expanded_src_size=[%zu]\n", expanded_src_size );
			}
			return false;
		}
		if( expanded_src_size == 0 ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkHtpRAR : inflateGZip : Error : expanded_src_size=0\n" );
			}
			return false;
		}
		ZnkBfr_append_dfr( dst_bfr, dst_buf, expanded_dst_size );
		src_size -= expanded_src_size;
		src += expanded_src_size;
	}
	return true;
}
#endif

static bool
recvHeaderAndBody( ZnkStockBIO sbio, ZnkHtpReqMethod req_method, HeaderInfo* hdr_info, ZnkHtpHdrs recv_hdrs, ZnkBfr body, ZnkStr ermsg )
{
	int result;
	bool uncomp_result = true;

	result = recvHeader( sbio, recv_hdrs, hdr_info, ermsg );
	if( result < 0 ){
		return false;
	}

	if( req_method != ZnkHtpReqMethod_e_HEAD ){
		/***
		 * body���擾.
		 * HTML�Ƃ����ǂ����k����binary��Ԃœ����邱�Ƃ�����.
		 * �]���Ă����ł�ZnkStr�ł͂Ȃ�ZnkBfr���g��.
		 */
		if( !appendBody( sbio, body, hdr_info, ermsg ) ){
			return false;
		}
	
		/***
		 * HTML�Ȃǂ�gzip���k����Ă���P�[�X��������.
		 * ����������͕ʂ�HTML����Ƃ����킯�łȂ��A��ʂɂǂ�ȃo�C�i���ł���
		 * ���̌`���ň��k����Ă��Ă��\��Ȃ�.
		 * ������̏ꍇ�ł��A�w�肳�ꂽ���k�`���̎w���ɏ]���A�����Ŗ{���̃o�C�i���C���[�W��
		 * �W�J���邾���ł���.
		 *
		 * ����̉ۑ�Ƃ��ẮA����e�ʃt�@�C�����_�E�����[�h����ꍇ�ɁA�������t�@�C����
		 * ���Ƃ��悤�Ȍ`�ŕۑ����鏈�����T�|�[�g���邱�Ƃ����A�Ƃ肠��������͌��.
		 */
		if( hdr_info->is_gzip_ ){
			ZnkBfr dst_bfr = ZnkBfr_create_null();
			ZnkStr gz_emsg = ZnkStr_new( "" );
			const uint8_t* body_data = ZnkBfr_data( body );
			const size_t   body_size = ZnkBfr_size( body );
			if( !inflateGZipBody_toBfr( dst_bfr, body_data, body_size, gz_emsg ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "ZnkHtpRAR : recvHeaderAndBody : [%s]\n", ZnkStr_cstr(gz_emsg) );
				}
			} else {
				ZnkBfr_swap( dst_bfr, body );
			}
			ZnkStr_delete( gz_emsg );
			ZnkBfr_destroy( dst_bfr );

#if 0
			ZnkZStream zst = ZnkZStream_create();
			ZnkBfr dst_bfr = ZnkBfr_create_null();
	
			ZnkZStream_inflateInit( zst, ermsg );
	
			uncomp_result = uncompressBfr( zst, dst_bfr, body, ermsg );
	
			ZnkZStream_inflateEnd( zst );
			ZnkZStream_destroy( zst );
	
			if( uncomp_result ){
				ZnkBfr_swap( dst_bfr, body );
			}
			ZnkBfr_destroy( dst_bfr );
#endif
		}
	}
	return uncomp_result;
}

static int
recvSocket( void* arg, uint8_t* buf, size_t buf_size )
{
	ZnkSocket sock = (ZnkSocket)arg;
	return ZnkSocket_recv( sock, buf, buf_size );
}

static bool
sendAndRecv_bySocket( ZnkSocket sock,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	ZnkErr_D( err );
	bool result = false;
	bool internal_wk_bfr = false;
	ZnkStockBIO sbio = ZnkStockBIO_create( 256, recvSocket, (void*)sock );
	ZnkHtpReqMethod req_method = ZnkHtpReqMethod_getType_fromCStr(
			ZnkStrAry_at_cstr( send_hdrs->hdr1st_, 0 ) );

	if( wk_bfr == NULL ){
		internal_wk_bfr = true;
		wk_bfr = ZnkBfr_create_null();
	}

	ZnkBfr_clear( wk_bfr );
	ZnkHtpHdrs_extendToStream( send_hdrs->hdr1st_, send_hdrs->vars_, wk_bfr, true );

	/* BodyImage��ǉ� */
	if( send_body ){
		ZnkBfr_append_dfr( wk_bfr, ZnkBfr_data(send_body), ZnkBfr_size(send_body) );
	}

	if( ZnkSocket_send( sock, ZnkBfr_data( wk_bfr ), ZnkBfr_size( wk_bfr ) ) == -1 ){
		ZnkSysErrnoInfo* err_info = ZnkSysErrno_getInfo( ZnkSysErrno_errno() );
		ZnkErr_internf( &err,
				"ZnkSocket_send : Failure : SysErr=[%s:%s]",
				err_info->sys_errno_key_, err_info->sys_errno_msg_ );
		result = false;
		goto FUNC_END;
	}

	{
		HeaderInfo* hdr_info = createHeaderInfo();

		ZnkBfr_clear( wk_bfr );

		if( !recvHeaderAndBody( sbio, req_method, hdr_info, recv_hdrs, wk_bfr, ermsg ) ){
			if( ermsg ){
				ZnkStr_add( ermsg, "ZnkHtpRAR : recvHeaderAndBody : [Failure].\n" );
			}
			result = false;
		} else {
			if( cookie ){
				size_t i;
				size_t n = ZnkStrAry_size( hdr_info->set_cookie_ );
				for( i=0; i<n; ++i ){
					const char* line = ZnkStrAry_at_cstr( hdr_info->set_cookie_, i );
					ZnkCookie_regist_byAssignmentStatement( cookie, line, Znk_NPOS );
				}
			}
		
			if( recv_fnca.func_ ){
				(*recv_fnca.func_)( recv_fnca.arg_,
						ZnkBfr_data(wk_bfr), ZnkBfr_size(wk_bfr) );
			}
			result = true;
		}

		destroyHeaderInfo( hdr_info );
	}
	
FUNC_END:
	if( internal_wk_bfr ){
		ZnkBfr_destroy( wk_bfr );
	}
	ZnkStockBIO_destroy( sbio );
	return result;
}

const char*
ZnkHtpRAR_getHostnameAndUnderpath_fromURL( const char* url, char* hostname_buf, size_t hostname_buf_size )
{
	size_t slash_pos;
	const char* underpath = "";
	if( ZnkS_isBegin( url, "http://" ) ){
		url = url + 7;
	}
	slash_pos = ZnkS_lfind_one_of( url, 0, Znk_NPOS, "/", 1 );
	ZnkS_copy( hostname_buf, hostname_buf_size, url, slash_pos );
	if( slash_pos != Znk_NPOS ){
		underpath = url + slash_pos + 1;
	}
	return underpath;
}

void
ZnkHtpRAR_getHostnameAndPort( const char* url,
		char* hostname_buf, size_t hostname_buf_size, uint16_t* port )
{
	size_t colon_pos;
	size_t slash_pos;
	char host_and_port[ 512 ];

	if( ZnkS_isBegin( url, "http://" ) ){
		url = url + 7;
	} else if( ZnkS_isBegin( url, "https://" ) ){
		url = url + 8;
	}

	slash_pos = ZnkS_lfind_one_of( url, 0, Znk_NPOS, "/", 1 );
	ZnkS_copy( host_and_port, sizeof(host_and_port), url, slash_pos );

	colon_pos = ZnkS_lfind_one_of( host_and_port, 0, Znk_NPOS, ":", 1 );
	ZnkS_copy( hostname_buf, hostname_buf_size, host_and_port, colon_pos );

	if( colon_pos == Znk_NPOS ){
		if( port ){ *port = 80; }
	} else {
		if( port ){
			const char* port_str = url + colon_pos + 1;
			ZnkS_getU16U( port, port_str );
		}
	}
}

bool
ZnkHtpRAR_sendAndRecv( const char* cnct_hostname, uint16_t cnct_port,
		ZnkHtpHdrs send_hdrs, ZnkBfr send_body,
		ZnkHtpHdrs recv_hdrs, ZnkHtpOnRecvFuncArg recv_fnca,
		ZnkVarpAry cookie,
		size_t try_connect_num, bool is_proxy, ZnkBfr wk_bfr, ZnkStr ermsg )
{
	bool        result = false;
	const char* hostname = cnct_hostname;
	bool        is_need_modify_req_uri = false;
	ZnkVarp     varp   = ZnkHtpHdrs_find_literal( send_hdrs->vars_, "Host" );
	ZnkStrAry  hdr1st = send_hdrs->hdr1st_;

	if( varp ){
		hostname = ZnkVar_name_cstr( varp );
		if( is_proxy ){
			ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
			/* connect via proxy. */
			switch( ZnkStr_first(req_uri) ){
			case '/':
				/* req_uri begin from under-path */
				is_need_modify_req_uri = true;
				break;
			case '*':
			default:
				/* req_uri begin from hostname or * */
				break;
			}
		}
	}

	if( is_need_modify_req_uri ){
		ZnkStr tmp = ZnkStr_new( "" );
		ZnkStr req_uri = ZnkStrAry_at( hdr1st, 1 );
		ZnkStr_addf( tmp, "%s%s", hostname, ZnkStr_cstr( req_uri ) );
		ZnkStr_swap( tmp, req_uri );
		ZnkStr_delete( tmp );
	}

	{
		ZnkErr_D( err );
		ZnkSocket sock = ZnkSocket_open();
		bool is_inprogress = false;
	
		while( try_connect_num ){
			if( !ZnkSocket_connectToServer( sock, cnct_hostname, cnct_port, &err, &is_inprogress ) ){
				if( ermsg ){
					ZnkStr_addf( ermsg, "%s (try=%zu).\n", ZnkErr_cstr(err),try_connect_num );
				}
				--try_connect_num;
				if( try_connect_num == 0 ){
					result = false;
					goto FUNC_END;
				}
			} else {
				break;
			}
		}
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkHtpRAR : connectTo[%s:%hu] : [Success].\n", cnct_hostname, cnct_port );
		}
	
		result = sendAndRecv_bySocket( sock,
				send_hdrs, send_body,
				recv_hdrs, recv_fnca,
				cookie, wk_bfr, ermsg );

		ZnkSocket_close( sock );
	}

FUNC_END:
	return result;
}
