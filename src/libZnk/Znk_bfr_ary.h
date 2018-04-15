#ifndef Znk_bfr_ary_h__INCLUDED__
#define Znk_bfr_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_bfr.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkBfrAry );


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
ZnkBfrAry
ZnkBfrAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
ZnkBfrAry_destroy( ZnkBfrAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkBfrAry_size( const ZnkBfrAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkBfr
ZnkBfrAry_at( ZnkBfrAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkBfr );
}
Znk_INLINE ZnkBfr*
ZnkBfrAry_ary_ptr( ZnkBfrAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkBfr );
}

Znk_INLINE bool
ZnkBfrAry_erase( ZnkBfrAry ary, ZnkBfr obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkBfrAry_erase_byIdx( ZnkBfrAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkBfrAry_clear( ZnkBfrAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkBfrAry_resize( ZnkBfrAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkBfrAry_push_bk( ZnkBfrAry ary, ZnkBfr obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkBfrAry_set( ZnkBfrAry ary, size_t idx, ZnkBfr obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
