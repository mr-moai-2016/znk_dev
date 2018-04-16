#ifndef INCLUDE_GUARD__CB_wgt_prim_h__
#define INCLUDE_GUARD__CB_wgt_prim_h__

#include <Znk_prim.h>
#include <Znk_obj_ary.h>

/**
 * Weighted Primitive for CustomBoy
 * Author : Zenkaku
 *
 * This implementation is the holder of ZnkPrim with probabilistic decision making.
 */

Znk_EXTERN_C_BEGIN

typedef struct CBWgtPrimImpl* CBWgtPrim;

void
CBWgtRand_init( void );
double
CBWgtRand_getRandomReal( double min_real, double max_real );

CBWgtPrim
CBWgtPrim_create( ZnkPrimType pm_type, double rate, ZnkElemDeleterFunc elem_deleter );

void
CBWgtPrim_destroy( CBWgtPrim wpm );

ZnkPrim*
CBWgtPrim_prim( CBWgtPrim wpm );
double
CBWgtPrim_rate( const CBWgtPrim wpm );

Znk_DECLARE_HANDLE( CBWgtPrimAry );

/**
 * @brief Ary�𐶐�����.
 *
 * @param elem_responsibility:
 *  ���ꂪtrue�̏ꍇ�A�e�v�f�̎����ɂ��Ă���Ary�͐ӔC������.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�Ƃ��Ă��ꂪ�Ă΂��.
 *  ���̂Ƃ��A���[�U�͊O���Ŏ��������蓖�Ă�object��set, push_bk�Ȃǂœn���Ă��̂܂ܕ��u���Ă���
 *  �`�ɂȂ�(����������ΊO���ł����delete���Ă͂Ȃ�Ȃ�). ���邢�͕ʓrregist�n�̊֐��Ȃǂ�
 *  �p�ӂ��ׂ��ł���.
 *
 *  ���ꂪfalse�̏ꍇ�́A�P�ɂ���Ary���|�C���^�l�̈ꎞ�I�ȃR���e�i�Ƃ��Ďg�p���A
 *  �e�v�f�̎����͊O���̋@�\�ɂ���ĊǗ����Ă��邱�Ƃ��Ӗ�����.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�͌Ă΂�Ȃ�.
 *  �K�R�I�ɁA���[�U�͓����ŃI�u�W�F�N�g���蓖�Ă���������悤��regist�n�֐������̏ꍇ��
 *  �ǉ��ŗp�ӂ��ׂ��ł͂Ȃ�. ����� clear, resize, erase, push_bk, set �Ȃǂ������ČĂяo����
 *  �����Ǘ��ɕs�����������邩��ł���. �ǂ����Ă�regist�n�֐����K�v�ȏꍇ�͑f����deleter��
 *  �Z�b�g���ׂ��ł���.
 *
 * @note
 *   ��Lregist�n API�Ƃ� regist ������.
 */
CBWgtPrimAry
CBWgtPrimAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
CBWgtPrimAry_destroy( CBWgtPrimAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
CBWgtPrimAry_size( const CBWgtPrimAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE CBWgtPrim
CBWgtPrimAry_at( CBWgtPrimAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, CBWgtPrim );
}
Znk_INLINE CBWgtPrim*
CBWgtPrimAry_ary_ptr( CBWgtPrimAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, CBWgtPrim );
}

Znk_INLINE bool
CBWgtPrimAry_erase( CBWgtPrimAry ary, CBWgtPrim obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
CBWgtPrimAry_erase_byIdx( CBWgtPrimAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
CBWgtPrimAry_clear( CBWgtPrimAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
CBWgtPrimAry_resize( CBWgtPrimAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
CBWgtPrimAry_push_bk( CBWgtPrimAry ary, CBWgtPrim obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
CBWgtPrimAry_set( CBWgtPrimAry ary, size_t idx, CBWgtPrim obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}

CBWgtPrim
CBWgtPrimAry_registStr( CBWgtPrimAry ary, const char* str, double rate );

ZnkPrim*
CBWgtPrimAry_select( const CBWgtPrimAry ary );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
