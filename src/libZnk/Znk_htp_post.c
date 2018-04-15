#include "Znk_htp_post.h"
#include "Znk_str.h"
#include "Znk_missing_libc.h"

ZnkVarp
ZnkHtpPostVars_regist( ZnkVarpAry vars, const char* name, const char* filename,
		ZnkHtpPostVarType type, const uint8_t* data, size_t data_size )
{
	ZnkVarp varp = ZnkVarp_create( name, filename, type, ZnkPrim_e_None, NULL );
	switch( type ){
	case ZnkHtpPostVar_e_BinaryData:
		ZnkVar_set_val_Bfr( varp, data, data_size );
		break;
	case ZnkHtpPostVar_e_None:
	default:
		ZnkVar_set_val_Str( varp, (char*)data, data_size );
		break;
	}
	ZnkVarpAry_push_bk( vars, varp );
	return varp;
}

Znk_INLINE bool
isASCII( uint32_t c ) { return (bool)( c < 0x80 ); }
Znk_INLINE bool
SJIS_isHankakuKatakana( uint8_t b ){ return ( b >= 0xa1 && b <= 0xdf ); }
Znk_INLINE bool
SJIS_isSecondByte( const char* base, const char* p )
{
	int lbc = 0;
	while( p > base ){
		--p;
		if( isASCII(*p) ){
			/* second or ascii */
			break;
		} else if( SJIS_isHankakuKatakana(*p) ){
			/* second or hankaku_katakana */
			break;
		} else {
			/* second or first */
		}
		++lbc;
	}
	return (bool)(lbc & 1);
}
static const char*
getTail_byDOS( ZnkStr filename )
{
	const char* begin = ZnkStr_cstr( filename );
	size_t      size  = Znk_strlen( begin );
	char*       p     = Znk_memrchr( begin, '\\', size );
	while( p ){
		if( SJIS_isSecondByte( begin, p ) ){
			size = p - begin;
			p = Znk_memrchr( begin, '\\', size );
		} else {
			/* p is really separator. */
			return p+1;
		}
	}
	/* separator is not found */
	return begin;
}
static const char*
getTail_byUNIX( ZnkStr filename )
{
	const char* begin = ZnkStr_cstr( filename );
	size_t      size  = Znk_strlen( begin );
	char*       p     = Znk_memrchr( begin, '/', size );
	if( p ){
		/* p is really separator. */
		return p+1;
	}
	/* separator is not found */
	return begin;
}
static void
eraseHeadOfPath( ZnkStr filename )
{
	const char* begin = ZnkStr_cstr( filename );
	const char* p     = getTail_byDOS( filename );
	const char* q     = getTail_byUNIX( filename );
	if( p < q ){
		ZnkStr_erase( filename, 0, q-begin );
	} else if( p > q ){
		ZnkStr_erase( filename, 0, p-begin );
	} else {
		if( p != begin ){
			ZnkStr_erase( filename, 0, p-begin );
		}
	}
}
bool
ZnkHtpPostVars_regist_byMPFDBody( ZnkVarpAry vars,
		const char* boundary, size_t boundary_leng,
		const uint8_t* body, size_t body_size,
		ZnkStr emsg )
{
	const uint8_t* p = body;
	const uint8_t* q = NULL;
	const uint8_t* r = NULL;
	const uint8_t* end = body + body_size;
	bool result = false;
	bool is_upfile = false;

	p = Znk_memmem( p, end-p, boundary, boundary_leng );
	if( p == NULL ){
		/* not multitype/form-data or invalid boundary */
		return false;
	} else {

		ZnkVarp varp = NULL;
		ZnkStr  cntype = ZnkStr_new( "" );

		p += boundary_leng;
		p += 2; /* skip \r\n or -- */

		while( end-p ){
			if( !ZnkS_isCaseBegin_literal( (char*)p, "Content-Disposition: form-data;" ) ){
				if( emsg ){
					char dbg_buf[ 16 ];
					ZnkS_copy( dbg_buf, sizeof(dbg_buf), (char*)p, end-p );
					ZnkStr_addf( emsg, "ZnkHtpPostVars_regist_byMPFDBody : Not Content-Disposition p=[%p][%s]\n",
							p, dbg_buf );
				}
				goto FUNC_END;
			}
			p += 31; /* skip Content-Disposition; form-data; */

			/* ���s�܂ł̈ʒu��\�ߓ��Ă��� */
			r = Znk_memstr_literal( p, end-p, "\r\n" );
	
			varp = ZnkVarp_create( "", "", ZnkHtpPostVar_e_None, ZnkPrim_e_None, NULL );

			/* ����͕K�{�ł��� */
			p = Znk_memstr_literal( p, r-p, "name=" );
			if( p ){
				p += Znk_strlen_literal( "name=" ); /* skip keyword */
				if( *p == '"' ){ ++p; } else { assert(0); } /* skip quote */
				q = Znk_memstr_literal( p, r-p, "\"" );
				ZnkStr_assign( varp->name_, 0, (char*)p, q-p );
				p += q-p+1; /* after end quote */
			} else {
				assert(0);
			}
	
			/* ����̓I�v�V�����ł��� */
			is_upfile = false;
			q = Znk_memstr_literal( p, r-p, "filename=" );
			if( q ){
				p += q-p+Znk_strlen_literal( "filename=" ); /* skip keyword */
				if( *p == '"' ){ ++p; } else { assert(0); } /* skip quote */
				q = Znk_memstr_literal( p, r-p, "\"" );
				/* misc��filename���i�[ */
				ZnkStr_assign( ZnkVar_misc(varp), 0, (char*)p, q-p );
				eraseHeadOfPath( ZnkVar_misc(varp) ); /* remove fullpath on IE */
				p += q-p+1; /* after end quote */
				is_upfile = true;
			}
	
			/* next line */
			p += r-p+2;
			/* ���s�܂ł̈ʒu��\�ߓ��Ă��� */
			r = Znk_memstr_literal( p, end-p, "\r\n" );

			if( ZnkS_isCaseBegin_literal( (char*)p, "Content-Type: " ) ){
				p += Znk_strlen_literal( "Content-Type: " ); /* skip Content-Type:  */
				ZnkStr_assign( cntype, 0, (char*)p, r-p );
				/***
				 * ����cntype�̒l��image/png, image/jpeg�ȂǗl�X�Ȓl���Ƃ蓾��.
				 * filename�������w�肳��Ă��Ȃ��ꍇ�Ȃǂ�application/octet-stream�ƂȂ��Ă����������.
				 * �����������������g���Ȃ�ł���(���Ƃ�text/plain�ł���),
				 * file��upload�̎��_�ł��̒l��BinaryData�Ƃ݂Ȃ��ׂ��ł���.
				 */
				if( is_upfile ){
					varp->type_ = ZnkHtpPostVar_e_BinaryData;
				}
				/* next line */
				p += r-p+2;
				/* ���s�܂ł̈ʒu��\�ߓ��Ă��� */
				r = Znk_memstr_literal( p, end-p, "\r\n" );
			}

			/* �����Ɉ��s�����݂���. */
			/* next line */
			p += r-p+2;
			/* ���s�܂ł̈ʒu��\�ߓ��Ă��� */
			r = Znk_memstr_literal( p, end-p, "\r\n" );
	
			/***
			 * ����boundary�������܂ł����ۂ̒l�ł���
			 * (���m�ɂ͌���\r\n���t������Ă�����̂��擾�����̂ł�����������邽�߁A-2���s��.)
			 *
			 * ���A���̉ӏ��̂݁Ap��T������͈͂�body�̍Ō�܂łɂ��Ȃ���΂Ȃ�Ȃ����Ƃɂ����ӂ���.
			 */
			q = Znk_memmem( p, end-p, boundary, boundary_leng );
			if( q ){
				if( varp->type_ == ZnkHtpPostVar_e_BinaryData ){
					ZnkVar_set_val_Bfr( varp, p, q-p-2 );
				} else {
					ZnkVar_set_val_Str( varp, (char*)p, q-p-2 );
				}
				p += q-p+boundary_leng; /* after boundary */
				if( end-p && ZnkS_isBegin( (char*)p, "--" ) ){
					p += 2; /* skip -- */
				}
				if( end-p && ZnkS_isBegin( (char*)p, "\r\n" ) ){
					p += 2; /* skip \r\n */
				}
			} else {
				assert( 0 );
			}

			/* ���� varp �̎����� info->vars_�̊Ǘ����ɒu����� */
			ZnkVarpAry_push_bk( vars, varp );
		}

		result = true;
FUNC_END:
		ZnkStr_delete( cntype );
	}
	return result;
}
