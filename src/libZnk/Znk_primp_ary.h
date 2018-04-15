#ifndef Znk_primp_ary_h__INCLUDED__
#define Znk_primp_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_prim.h>

Znk_EXTERN_C_BEGIN

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
ZnkPrimpAry
ZnkPrimpAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
ZnkPrimpAry_destroy( ZnkPrimpAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkPrimpAry_size( const ZnkPrimpAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkPrimp
ZnkPrimpAry_at( ZnkPrimpAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkPrimp );
}
Znk_INLINE ZnkPrimp*
ZnkPrimpAry_ary_ptr( ZnkPrimpAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkPrimp );
}

Znk_INLINE bool
ZnkPrimpAry_erase( ZnkPrimpAry ary, ZnkPrimp obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkPrimpAry_erase_byIdx( ZnkPrimpAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkPrimpAry_clear( ZnkPrimpAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkPrimpAry_resize( ZnkPrimpAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkPrimpAry_push_bk( ZnkPrimpAry ary, ZnkPrimp obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkPrimpAry_set( ZnkPrimpAry ary, size_t idx, ZnkPrimp obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
