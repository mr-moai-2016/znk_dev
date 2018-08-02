#ifndef INCLUDE_GUARD__Znk_str_ary_h__
#define INCLUDE_GUARD__Znk_str_ary_h__

#include <Znk_obj_ary.h>
#include <Znk_str.h>
#include <Znk_s_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkStrAry );

/**
 * @brief Ary�𐶐�����.
 *
 * @param elem_responsibility:
 *  ���ꂪtrue�̏ꍇ�A�e�v�f�̎����ɂ��Ă���Ary�͐ӔC������.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�Ƃ��Ă��ꂪ�Ă΂��.
 *  ���̂Ƃ��A���[�U�͊O���Ŏ��������蓖�Ă�object��set, push_bk�Ȃǂœn���Ă��̂܂ܕ��u���Ă���
 *  �`�ɂȂ�(����������ΊO���ł����delete���Ă͂Ȃ�Ȃ�).
 *
 *  ���ꂪfalse�̏ꍇ�́A�P�ɂ���Ary���|�C���^�l�̈ꎞ�I�ȃR���e�i�Ƃ��Ďg�p���A
 *  �e�v�f�̎����͊O���̋@�\�ɂ���ĊǗ����Ă��邱�Ƃ��Ӗ�����.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�͌Ă΂�Ȃ�.
 */
ZnkStrAry
ZnkStrAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
ZnkStrAry_destroy( ZnkStrAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkStrAry_size( const ZnkStrAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkStr
ZnkStrAry_at( ZnkStrAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkStr );
}
Znk_INLINE ZnkStr*
ZnkStrAry_ary_ptr( ZnkStrAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkStr );
}

Znk_INLINE bool
ZnkStrAry_erase( ZnkStrAry ary, ZnkStr obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkStrAry_erase_byIdx( ZnkStrAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkStrAry_clear( ZnkStrAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
void
ZnkStrAry_resize( ZnkStrAry ary, size_t size, const char* init_val );

Znk_INLINE void
ZnkStrAry_push_bk( ZnkStrAry ary, ZnkStr obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkStrAry_pop_bk( ZnkStrAry ary ){
	ZnkObjAry_M_POP_BK( ary );
}
Znk_INLINE void
ZnkStrAry_set( ZnkStrAry ary, size_t idx, ZnkStr obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}
Znk_INLINE void
ZnkStrAry_swap( ZnkStrAry ary1, ZnkStrAry ary2 ){
	ZnkObjAry_M_SWAP( ary1, ary2 );
}

int
ZnkStrAry_push_bk_snprintf( ZnkStrAry ary, size_t size, const char* fmt, ... );

void
ZnkStrAry_push_bk_cstr( ZnkStrAry ary, const char* cstr, size_t cstr_leng );

size_t
ZnkStrAry_find( ZnkStrAry ary, size_t pos, const char* query, size_t query_leng );

size_t
ZnkStrAry_find_isMatch( ZnkStrAry ary, size_t pos, const char* query, size_t query_leng,
		ZnkS_FuncT_IsMatch is_match_func );

Znk_INLINE const char*
ZnkStrAry_at_cstr( ZnkStrAry ary, size_t idx ){
	return ZnkStr_cstr( ZnkStrAry_at( ary, idx ) );
}
Znk_INLINE size_t
ZnkStrAry_at_leng( ZnkStrAry ary, size_t idx ){
	return ZnkStr_leng( ZnkStrAry_at( ary, idx ) );
}

Znk_INLINE void
ZnkStrAry_reserve( ZnkStrAry ary, size_t size ){
	const size_t size_save = ZnkStrAry_size( ary );
	ZnkObjAry_M_RESIZE( ary, size );
	ZnkObjAry_M_RESIZE( ary, size_save );
}

Znk_INLINE void
ZnkStrAry_append( ZnkStrAry dst, const ZnkStrAry src, size_t src_begin, size_t src_end )
{
	size_t idx;
	ZnkStr str;
	const size_t src_size = ZnkStrAry_size(src);
	if( src_end >= src_size ){
		src_end  = src_size;
	}
	for( idx=src_begin; idx<src_end; ++idx ){
		str = ZnkStrAry_at( src, idx );
		ZnkStrAry_push_bk_cstr( dst, ZnkStr_cstr( str ), ZnkStr_leng( str ) );
	}
}

Znk_INLINE void
ZnkStrAry_copy( ZnkStrAry dst, const ZnkStrAry src )
{
	ZnkStrAry_clear( dst );
	ZnkStrAry_append( dst, src, 0, Znk_NPOS );
}

void
ZnkStrAry_sort( ZnkStrAry ary );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
