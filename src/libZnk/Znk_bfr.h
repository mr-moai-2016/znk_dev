#ifndef INCLUDE_GUARD__Znk_bfr_h__
#define INCLUDE_GUARD__Znk_bfr_h__

/***
 * ZnkBfr
 * �ėp��binary stream��ێ����� uint8_t �̓��I�z��ł���.
 */

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkBfr_Exp2  =  0
	,ZnkBfr_Pad8  =  3
	,ZnkBfr_Pad16
	,ZnkBfr_Pad32
	,ZnkBfr_Pad64
	,ZnkBfr_Pad128
	,ZnkBfr_Pad256
	,ZnkBfr_Pad512
	,ZnkBfr_Pad1024
	,ZnkBfr_Pad2048
	,ZnkBfr_Pad4096
} ZnkBfrType;

typedef struct ZnkBfrImpl* ZnkBfr;

ZnkBfr
ZnkBfr_create( const uint8_t* init_data, size_t size, bool with_zero, ZnkBfrType type );

Znk_INLINE ZnkBfr
ZnkBfr_create_null( void ){
	return ZnkBfr_create( NULL, 0, false, ZnkBfr_Exp2 );
}

void
ZnkBfr_destroy( ZnkBfr bfr );

void
ZnkBfr_clear_and_minimize( ZnkBfr bfr );

bool
ZnkBfr_reserve( ZnkBfr bfr, size_t req_capacity );

bool
ZnkBfr_resize( ZnkBfr bfr, size_t size );

Znk_INLINE void ZnkBfr_clear( ZnkBfr bfr ){ ZnkBfr_resize( bfr, 0 ); }

bool
ZnkBfr_resize_fill( ZnkBfr bfr, size_t size, uint8_t val );

bool
ZnkBfr_shrink_to_fit( ZnkBfr bfr );

uint8_t*
ZnkBfr_data( ZnkBfr bfr );

Znk_INLINE void*
ZnkBfr_at_ptr( ZnkBfr bfr, size_t idx ){
	const uintptr_t* ary = (uintptr_t*)ZnkBfr_data(bfr);
	return (void*)( ary[ idx ] );
}
#define ZnkBfr_at_type( type, bfr, idx )  ( (type*)( ZnkBfr_data(bfr) + (idx) * sizeof(type) ) )

size_t
ZnkBfr_size( const ZnkBfr bfr );

size_t
ZnkBfr_capacity( const ZnkBfr bfr );

void
ZnkBfr_swap( ZnkBfr zkbfr1, ZnkBfr zkbfr2 );


/**
 * @brief
 *   bfr �ɂ�����dst_pos����dst_leng�o�C�g���̗̈�(�ȉ��ł�hole�ƌĂ�)��new_leng�Ŏw�肳�ꂽ
 *   �o�C�g�T�C�Y�Ɋg��/�k������. ���̑���ł��͈͈̔ȊO�̕����̒l���ω����邱�Ƃ͂Ȃ����A
 *   hole �����ɂ��镔���� new_leng �̑傫���ɉ����ēK�؂ɃX���C�h�����.
 *   ���̃X���C�h�����replace�������ōs���������ړ�����Ɠ����ł���.
 *
 * @return
 *   ���̊֐������������ꍇ��true��Ԃ�.
 *   ������realloc�Ɏ��s�����ꍇ�͂��̊֐��͎��s���Afalse��Ԃ�.
 *   ���̂Ƃ�����bfr�͔j�󂳂ꂸ�ێ�����A���e����ؕύX����Ȃ�.
 */
bool
ZnkBfr_slide_hole( ZnkBfr bfr, const size_t dst_pos, const size_t dst_leng, const size_t new_leng );

void
ZnkBfr_replace( ZnkBfr bfr, size_t dst_pos, size_t dst_leng, const uint8_t* src, size_t src_leng );

Znk_INLINE void
ZnkBfr_insert( ZnkBfr bfr, size_t dst_pos, const uint8_t* src, size_t src_leng ){
	ZnkBfr_replace( bfr, dst_pos, 0, src, src_leng );
}

Znk_INLINE void
ZnkBfr_copy( ZnkBfr dst, const ZnkBfr src ){
	ZnkBfr_replace( dst, 0, ZnkBfr_size(dst), ZnkBfr_data(src), ZnkBfr_size(src) );
}

Znk_INLINE void
ZnkBfr_erase_ptr( ZnkBfr bfr, size_t pos, size_t num ){
	ZnkBfr_slide_hole( bfr, pos, sizeof(void*)*num, 0 );
}
Znk_INLINE void
ZnkBfr_erase( ZnkBfr bfr, size_t pos, size_t leng ){
	ZnkBfr_slide_hole( bfr, pos, leng, 0 );
}

/**
 * @brief
 *   src �� src_leng �Ŏ������̈�̃f�[�^�� bfr �̌��֘A������.
 *   bfr->data_��src�͈قȂ�Aggregation�łȂ���΂Ȃ�Ȃ�.
 */
bool
ZnkBfr_append_dfr( ZnkBfr bfr, const uint8_t* src, size_t src_leng );
Znk_INLINE void
ZnkBfr_set_dfr( ZnkBfr bfr, const uint8_t* src, size_t src_leng ){
	ZnkBfr_clear( bfr );
	ZnkBfr_append_dfr( bfr, src, src_leng );
}

bool
ZnkBfr_push_bk( ZnkBfr bfr, uint8_t val );
bool
ZnkBfr_push_bk_16( ZnkBfr bfr, uint16_t val, bool is_LE );
bool
ZnkBfr_push_bk_32( ZnkBfr bfr, uint32_t val, bool is_LE );
bool
ZnkBfr_push_bk_64( ZnkBfr bfr, uint64_t val, bool is_LE );
bool
ZnkBfr_push_bk_ptr( ZnkBfr bfr, void* ptr );

size_t
ZnkBfr_pop_bk_ex( ZnkBfr bfr, uint8_t* data, size_t data_size );

Znk_INLINE void
ZnkBfr_pop_bk_ptr( ZnkBfr bfr ){
	ZnkBfr_pop_bk_ex( bfr, NULL, sizeof(void*) );
}

void
ZnkBfr_fill( ZnkBfr bfr, size_t pos, uint8_t val, size_t size );
void
ZnkBfr_fill_16( ZnkBfr bfr, size_t pos, uint16_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_32( ZnkBfr bfr, size_t pos, uint32_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_64( ZnkBfr bfr, size_t pos, uint64_t val, size_t num, bool is_LE );
void
ZnkBfr_fill_ptr( ZnkBfr bfr, size_t pos, void* ptr, size_t num );
void
ZnkBfr_fill_block( ZnkBfr bfr, size_t pos,
		uint8_t* block, size_t block_size, size_t num );

size_t
ZnkBfr_transfer( ZnkBfr zkbfr_dst, size_t dst_pos, const uint8_t* src, size_t src_leng );

bool
ZnkBfr_set_aryval_8( ZnkBfr bfr, size_t idx, uint8_t val );
bool
ZnkBfr_set_aryval_16( ZnkBfr bfr, size_t idx, uint16_t val, bool is_LE );
bool
ZnkBfr_set_aryval_32( ZnkBfr bfr, size_t idx, uint32_t val, bool is_LE );
bool
ZnkBfr_set_aryval_64( ZnkBfr bfr, size_t idx, uint64_t val, bool is_LE );
bool
ZnkBfr_set_aryval_ptr( ZnkBfr bfr, size_t idx, void* ptr );
bool
ZnkBfr_set_aryval_block( ZnkBfr bfr, size_t idx, uint8_t* block, size_t block_bytesize );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
