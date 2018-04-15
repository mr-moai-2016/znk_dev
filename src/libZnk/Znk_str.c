#include "Znk_str.h"
#include "Znk_stdc.h"
#include "Znk_s_base.h"
#include "Znk_vsnprintf.h"

#include <stdarg.h>
#include <string.h>

#define M_data( zkbfr )     ZnkBfr_data( zkbfr )
#define M_size( zkbfr )     ZnkBfr_size( zkbfr )
#define M_c_str( zkbfr )    ((char*)ZnkBfr_data( zkbfr ))

#define DECIDE_LENG( leng, str ) do if( (leng) == Znk_NPOS ){ (leng) = Znk_strlen(str); } while(0)

Znk_INLINE void
I_zkstr_releng( ZnkStr zkstr, size_t new_leng )
{
	ZnkBfr_resize( zkstr, new_leng+1 );
	M_data( zkstr )[ new_leng ] = '\0';
}

void
ZnkStr_releng( ZnkStr zkstr, size_t new_leng )
{
	I_zkstr_releng( zkstr, new_leng );
}

void
ZnkStr_terminate_null( ZnkBfr zkbfr, bool plus_one )
{
	const size_t size = M_size(  zkbfr );
	char*        data = M_c_str( zkbfr );
	if( size == 0 ){
		ZnkBfr_push_bk( zkbfr, '\0' );
		return;
	}
	if( plus_one ){
		if( data[size-1] != '\0' ){
			ZnkBfr_push_bk( zkbfr, '\0' );
		}
	} else {
		/* �������ɍŏI������ NULL�����ŏ㏑������ */
		data[ size-1 ] = '\0';
	}
}

void
ZnkStr_normalize( ZnkBfr zkbfr, bool plus_one )
{
	const size_t size = M_size(  zkbfr );
	char*        data = M_c_str( zkbfr );
	if( size == 0 ){
		/* NULL���������ɘA������ */
		ZnkBfr_push_bk( zkbfr, '\0' );
		return;
	} else {
		const char* null_p = memchr( data, '\0', size );
		const size_t null_pos = null_p ? null_p - data : Znk_NPOS;
		if( null_pos == Znk_NPOS ){
			/* �͈͓���NULL������������Ȃ��ꍇ */
			if( plus_one ){
				/* NULL���������ɘA������ */
				ZnkBfr_push_bk( zkbfr, '\0' );
			} else {
				/* �������ɍŏI������ NULL�����ŏ㏑������ */
				data[ size-1 ] = '\0';
			}
		} else if( null_pos < size-1 ){
			/* �͈͓���NULL����������A�����ꂪ�ŏI���������O�ɂ���ꍇ */
			ZnkBfr_resize( zkbfr, null_pos+1 ); /* ����ɂ��realloc�͔������Ȃ��͂� */
		}
	}
}

ZnkStr
ZnkStr_create_ex( const char* init_data, size_t leng, ZnkBfrType type )
{
	ZnkBfr zkbfr;
	if( init_data ){
		if( leng == Znk_NPOS ){ leng = Znk_strlen(init_data); }
		zkbfr = ZnkBfr_create( (uint8_t*)init_data, leng, false, type );
		ZnkStr_terminate_null( zkbfr, true );
	} else {
		if( leng == Znk_NPOS ){ leng = 0; }
		zkbfr = ZnkBfr_create( NULL, leng+1, false, type );
		I_zkstr_releng( zkbfr, 0 );
	}
	return zkbfr;
}
ZnkStr
ZnkStr_newf( const char* fmt, ... )
{
	ZnkStr zkstr = ZnkStr_new( "" );
	va_list ap;
	va_start(ap, fmt);
	ZnkStr_vsnprintf( zkstr, 0, Znk_NPOS, fmt, ap );
	va_end(ap);
	return zkstr;
}

void
ZnkStr_assign( ZnkStr zkstr_dst, size_t dst_pos, const char* src, size_t src_leng )
{
	size_t size = M_size(zkstr_dst);
	if( size == 0 ){
		dst_pos = 0;
	} else {
		dst_pos = Znk_clampSize( dst_pos, size-1 );
	}
	if( src_leng == 0 ){
		/* erase */
		ZnkBfr_resize( zkstr_dst, dst_pos );
		ZnkStr_terminate_null( zkstr_dst, true );
		return;
	} else {
		uint8_t* data;
		if( src_leng == Znk_NPOS ){ src_leng = Znk_strlen(src); }
		ZnkBfr_resize( zkstr_dst, dst_pos + src_leng + 1 );
		data = M_data(zkstr_dst);
		Znk_memmove( data+dst_pos, src, src_leng );
		data[ dst_pos + src_leng ] = '\0';
	}
}

void
ZnkStr_replace( ZnkStr str, size_t dst_pos, size_t dst_leng, const char* src, size_t src_leng )
{
	src_leng = ( src_leng == Znk_NPOS ) ? Znk_strlen(src) : src_leng;
	dst_pos  = Znk_clampSize( dst_pos,  ZnkStr_leng(str) );
	dst_leng = Znk_clampSize( dst_leng, ZnkStr_leng(str)-dst_pos );
	ZnkBfr_replace( str, dst_pos, dst_leng, (uint8_t*)src, src_leng );
	ZnkStr_terminate_null( str, true );
}
void
ZnkStr_insert( ZnkStr str, size_t dst_pos, const char* src, size_t src_leng )
{
	src_leng = ( src_leng == Znk_NPOS ) ? Znk_strlen(src) : src_leng;
	dst_pos  = Znk_clampSize( dst_pos,  ZnkStr_leng(str) );
	ZnkBfr_replace( str, dst_pos, 0, (uint8_t*)src, src_leng );
	ZnkStr_terminate_null( str, true );
}

void
ZnkStr_add_c( ZnkStr zkstr, char val )
{
	const size_t size = ZnkBfr_size( zkstr );
	uint8_t* data;
	if( size == 0 ){
		ZnkBfr_resize( zkstr, 2 );
		data = ZnkBfr_data( zkstr );
		data[ 0 ] = (uint8_t)val;
		data[ 1 ] = '\0';
		return;
	}
	ZnkBfr_resize( zkstr, size+1 );
	data = ZnkBfr_data( zkstr );
	data[ size-1 ] = (uint8_t)val;
	data[ size   ] = '\0';
}

/* sys and C99 ���Ή��� */
typedef int (*ZnkVSNPrintfFuncT)( char* buf, size_t buf_size, const char* fmt, va_list ap );
typedef enum {
	ZnkVSNPrintfType_e_C99=0,
	ZnkVSNPrintfType_e_Sys
} ZnkVSNPrintfType; 
static ZnkVSNPrintfType st_vsnprintf_type = ZnkVSNPrintfType_e_C99;
int
ZnkStr_vsnprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, va_list ap )
{
	/**
	 * �����Ŏw�肷��֐��� va_start <=> va_end �Ԃɂ����镡����Ăяo����
	 * �Ή��������̂łȂ���΂Ȃ�Ȃ�.
	 */
	ZnkVSNPrintfFuncT vsnprintf_func = st_vsnprintf_type == ZnkVSNPrintfType_e_C99 ?
		Znk_vsnprintf_C99 : ZnkS_vsnprintf_sys__;
	int  ret_len = -1;
	bool result  = true;

	if( size > 0 ){
		int ret_len;
		char* data;
		size_t str_leng = ZnkStr_leng( zkstr );
		pos = Znk_clampSize( pos, str_leng );
	
		if( size == Znk_NPOS ){
			size_t backword_capacity = 512;
			size_t backword_capacity_real;
			/***
			 * size�����Ȃ�.
			 * ���̏ꍇ�A�\�Ȍ���o�b�t�@�̊g�����s��.
			 */
			while( true ){
				ZnkBfr_reserve( zkstr, pos + backword_capacity );
				backword_capacity_real = ZnkBfr_capacity( zkstr ) - pos;
				data = M_c_str( zkstr ); /* Note : think realloc */
				ret_len = vsnprintf_func( data + pos, backword_capacity_real, fmt, ap );

				/***
				 * ret_len��ret_len < backword_capacity_real�̏ꍇ�A�w�肳�ꂽ�o�b�t�@����
				 * �W�J������̂��ׂĂ��������܂�Ă��芮���ł���.
				 * ret_len�� -1 �̏ꍇ�A�����G���[�������������Ƃ��������߁A
				 * ���̏ꍇ��result=false�ɂ�����Œ��~����.
				 */
				if( (size_t)ret_len < backword_capacity_real ){
					/* OK. */
					break;
				} else if( ret_len < 0 ){
					/* �����G���[. */
					result = false;
					break;
				}
				/***
				 * string is trucated. retry.
				 *
				 * C99�̎d�l�ł͖߂�l�ł���ret_len�́Abackword_capacity_real�̎w��l�Ɋւ�炸
				 * ���m�ɓW�J��̕�����T�C�Y������.
				 * ����AZnkS_vsnprintf_sys�ł�C99�̎d�l�Ƃ͈قȂ�Abackword_capacity_real�̎w��l��
				 * Over�����ꍇ�́A���̖߂�l�Ƃ��ė\�������񒷂�Ԃ�.
				 * ����͐��m�ɓW�J��̕�����T�C�Y�ł���Ƃ͌���Ȃ����A�����̏ꍇ�\���ȃT�C�Y�ł���.
				 *
				 * ������ɂ��掟��͗e�ʂ�ret_len + 1 �Ƃ��Ď��݂�΂悢.
				 */
				backword_capacity = ret_len + 1;
			}

		} else {
			ZnkBfr_resize( zkstr, pos + size );
			data = M_c_str( zkstr ); /* Note : think realloc */
			//ret_len = Znk_vsnprintf_C99( data + pos, size, fmt, ap );
			ret_len = vsnprintf_func( data + pos, size, fmt, ap );
			if( ret_len < 0 ){
				/* �����G���[. */
				result = false;
			}
		}
		/***
		 * ��L�ł�ZnkBfr�Ƃ��ď������Ă��邽�߁A�Ō��ZnkStr�Ƃ���releng���Ă���.
		 */
		if( result ){
			ZnkStr_releng( zkstr, pos + ret_len );
		} else {
			/* �����G���[�����������ꍇ��pos�ȍ~�̓��e�͔j�󂳂�Ă���\��������.
			 * �T�C�Y�� pos �ɐ؂�l�߂Ă����̂��]�܂����Ǝv����. */
			ZnkStr_releng( zkstr, pos );
		}
		/***
		 * �����̐�����A���ʂȃ������m�ۂ��������ꍇ�A���̍����T�C�Y���傫���̂ŁA
		 * capacity��������x�傫���P�[�X�ɂ��Ă� shrink to fit ���s���Ă���.
		 */
		if( ZnkBfr_capacity( zkstr ) > 4096 ){
			ZnkBfr_shrink_to_fit( zkstr );
		}
	}
	return ret_len;
}

int ZnkStr_snprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, pos, size, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_sprintf( ZnkStr zkstr, size_t pos, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, pos, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_addf( ZnkStr zkstr, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, Znk_NPOS, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}
int ZnkStr_setf( ZnkStr zkstr, const char* fmt, ... )
{
	int ret_len;
	va_list ap;
	va_start(ap, fmt);
	ret_len = ZnkStr_vsnprintf( zkstr, 0, Znk_NPOS, fmt, ap );
	va_end(ap);
	return ret_len;
}

/*****************************************************************************/
/* eq */
bool
ZnkStr_eq( const ZnkStr str, const char* cstr ){
	return (bool)( strcmp( ZnkStr_cstr(str), cstr ) == 0 );
}

bool
ZnkStr_eqEx( const ZnkStr str, size_t pos, const char* data, size_t data_leng )
{
	const size_t leng = ZnkStr_leng( str );
	DECIDE_LENG( data_leng, data );
	if( pos >= leng ){
		/* ���̂Ƃ�str���͈̔͂͋�Ƃ݂Ȃ�. �]����data_leng��0�̏ꍇ�̂ݐ^�Ƃ���. */
		return (bool) ( data_leng == 0 );
	}
	/* ���̎��_�� leng > 0 �ł��� */ 
	if( leng != data_leng ){ return false; }
	return (bool)( Znk_memcmp( M_data(str), data, leng ) == 0 );
}

bool
ZnkStr_isContain( const ZnkStr zkstr, size_t pos, const char* data, size_t data_size )
{
	const size_t leng = ZnkStr_leng( zkstr );
	DECIDE_LENG( data_size, data );
	if( data_size > leng ){
		/* ���炩�Ƀ}�b�`���Ȃ� */
		return false;
	}

	if( pos == Znk_NPOS ){
		/* ���̏ꍇ�Ō��data�ŏI����Ă��邩�ǂ����𒲂ׂ���̂Ƃ��� */
		pos = leng - data_size;
	} else if( pos > leng ){
		pos = leng;
	}

	if( pos == leng ){
		/* pos �͍Ō��NULL�������w���Ă���. �܂�󕶎���ƍl���� */
		return (bool)( data_size == 0 );
	} else {
		const size_t size = leng - pos;
		if( size < data_size ){ return false; }
	}
	return (bool)( Znk_memcmp( M_data(zkstr) + pos, data, data_size ) == 0 );
}

/* endof eq */
/*****************************************************************************/


void
ZnkStr_cut_back( ZnkStr zkstr, size_t cut_size )
{
	const size_t size = M_size( zkstr );
	switch( size ){
	case 0:  return;
	case 1:  M_data(zkstr)[ 0 ] = '\0'; return;
	default: break;
	}

	/***
	 * cut_size == 1 �̂Ƃ��A�ȉ��� ( cut_size >= size-1 ) �Ƃ�������͖��ʂł���.
	 * (���̂Ƃ��A�P�� I_zkstr_releng( zkstr, size-2 ) ���Ăяo���΂悢).
	 * �������A����������� switch( cut_size ) �ɂ��Acut_size �� 1 �̏ꍇ�Ƃ����łȂ��ꍇ�Ƃ�
	 * �������Ƃ��Ă��A�����������̏��������񕪂�switch�ɂ��I�[�o�[�w�b�h�ɂ͂قƂ�Ǎ��͂��߁A
	 * ���܂�Ӗ�������[�u�Ƃ͎v���Ȃ�. �܂��AZnkStr_cut_back_c �ɂ��Ă��̕����������Ȃ�����
	 * ��������ʂɗp�ӂ���̂͑傰�������邵�A�ނ���]�v�Ȏ����𑝂₷�����ɂȂ�.
	 * ����āA�����ł͒P�Ɉȉ��̂悤�ȌĂяo���ɗ��߂�`�Ƃ���.
	 */
	I_zkstr_releng( zkstr, 
			( cut_size >= size-1 ) ? 0 : size-1-cut_size );
}
void
ZnkStr_erase( ZnkStr zkstr, size_t pos, size_t cut_size )
{
	const size_t size = M_size( zkstr );
	switch( size ){
	case 0:  return;
	case 1:  M_data(zkstr)[ 0 ] = '\0'; return;
	default: break;
	}
	if( pos >= size-1 ){ return; }
	if( pos + cut_size >= size-1 ){
		/* ���̂Ƃ��Apos �� releng �ł悢 */
		I_zkstr_releng( zkstr, pos );
		return;
	}
	/***
	 * pos + cut_size <= size-2
	 * ���̂Ƃ��A�������͕̂K���I�[NULL���O�̕�����ł���.
	 * ����Č��ʂ͈ˑR�Ƃ���NULL�I�[�̏�Ԃ�ۂ�.
	 * �����ɂ� pos + cut_size <= size-1 �ł��A�ȉ��̏����͖��͂Ȃ��̂����A
	 * ZnkBfr_erase��ZnkStr_clear���ᑬ�ł��邽�߁A�����ł͂Ȃ�ׂ������������.
	 */
	ZnkBfr_erase( zkstr, pos, cut_size );
}

void
ZnkStr_chompC( ZnkStr zkstr, size_t pos, char ch )
{
	const size_t size = M_size( zkstr );
	if( size <= 1 ){
		/* ���̂Ƃ��͂ǂ�������Ԃł���A�����񒷂� 0 �Ƃ݂Ȃ��ĂȂɂ����Ȃ� */
		return;
	} else {
		const size_t last = size-2;
		uint8_t* data = M_data( zkstr );
		if( pos == Znk_NPOS || pos >= last ){
			if( data[ last ] == ch ){
				data[ last ] = '\0';
				ZnkBfr_resize( zkstr, size-1 );
			}
		} else {
			if( data[ pos ] == ch ){
				//ZnkBfr_replace( zkstr, pos, 1, NULL, 0 );
				ZnkBfr_erase( zkstr, pos, 1 );
			}
		}
	}
}
void
ZnkStr_chompNL( ZnkStr line )
{
	/***
	 * ���ʂ�
	 *   ZnkStr_chompC( line, Znk_NPOS, '\n' );
	 *   ZnkStr_chompC( line, Znk_NPOS, '\r' );
	 * �Ǝ��s�����̂Ɠ����ł��邪�A�����ł̎����͍œK�����Ă���.
	 */
	const size_t size = M_size( line );
	if( size <= 1 ){
		/* ���̂Ƃ��͂ǂ�������Ԃł���A�����񒷂� 0 �Ƃ݂Ȃ��ĂȂɂ����Ȃ� */
		return;
	} else {
		size_t last = size-2;
		uint8_t* data = M_data( line );

		if( data[ last ] == '\n' ){
			ZnkStr_releng( line, last );

			if( last == 0 ){ return; }
			--last;
			data = M_data( line );
		}
		if( data[ last ] == '\r' ){
			ZnkStr_releng( line, last );
		}
	}
}

char ZnkStr_at( const ZnkStr zkstr, size_t idx )
{
	const size_t size = M_size( zkstr );
	switch( size ){
		case 0: case 1: return '\0';
		default: break;
	}
	return (char)M_data(zkstr)[ idx ];
}
char ZnkStr_last( const ZnkStr zkstr )
{
	const size_t size = M_size( zkstr );
	switch( size ){
		case 0: case 1: return '\0';
		default: break;
	}
	/* size >= 2 */
	return (char)M_data( zkstr )[ size-2 ];
}

void
ZnkStr_swap( ZnkStr str1, ZnkStr str2 )
{
	ZnkBfr_swap( str1, str2 );
}
