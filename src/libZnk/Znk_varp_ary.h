#ifndef Znk_varp_ary_h__INCLUDED__
#define Znk_varp_ary_h__INCLUDED__

#include <Znk_obj_ary.h>
#include <Znk_var.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkVarpAry );


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
ZnkVarpAry
ZnkVarpAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
ZnkVarpAry_destroy( ZnkVarpAry ary ){
	ZnkObjAry_M_DESTROY( ary );
}

Znk_INLINE size_t
ZnkVarpAry_size( const ZnkVarpAry ary ){
	return ZnkObjAry_M_SIZE( ary );
}
Znk_INLINE ZnkVarp
ZnkVarpAry_at( ZnkVarpAry ary, size_t idx ){
	return ZnkObjAry_M_AT( ary, idx, ZnkVarp );
}
Znk_INLINE ZnkVarp*
ZnkVarpAry_ary_ptr( ZnkVarpAry ary ){
	return ZnkObjAry_M_ARY_PTR( ary, ZnkVarp );
}

Znk_INLINE bool
ZnkVarpAry_erase( ZnkVarpAry ary, ZnkVarp obj ){
	return ZnkObjAry_M_ERASE( ary, obj );
}
Znk_INLINE bool
ZnkVarpAry_erase_byIdx( ZnkVarpAry ary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( ary, idx );
}
Znk_INLINE void
ZnkVarpAry_clear( ZnkVarpAry ary ){
	ZnkObjAry_M_CLEAR( ary );
}
Znk_INLINE void
ZnkVarpAry_resize( ZnkVarpAry ary, size_t size ){
	ZnkObjAry_M_RESIZE( ary, size );
}

Znk_INLINE void
ZnkVarpAry_push_bk( ZnkVarpAry ary, ZnkVarp obj ){
	ZnkObjAry_M_PUSH_BK( ary, obj );
}
Znk_INLINE void
ZnkVarpAry_set( ZnkVarpAry ary, size_t idx, ZnkVarp obj ){
	ZnkObjAry_M_SET( ary, idx, obj );
}
size_t
ZnkVarpAry_findIdx_byName( ZnkVarpAry ary,
		const char* query_name, size_t query_name_leng, bool use_eqCase );
Znk_INLINE ZnkVarp
ZnkVarpAry_findObj_byName( ZnkVarpAry ary,
		const char* query_name, size_t query_name_leng, bool use_eqCase )
{
	const size_t idx = ZnkVarpAry_findIdx_byName( ary, query_name, query_name_leng, use_eqCase );
	return ( idx == Znk_NPOS ) ? NULL : ZnkVarpAry_at( ary, idx );
}
size_t
ZnkVarpAry_findIdx_byStrVal( ZnkVarpAry ary,
		const char* query_val, size_t query_val_leng );

/* ���̓��n����Ary���W���[���Ɏd�l�ɂ��킹��Ȃ�{��find��idx��Ԃ��悤�ɂ��ׂ��ł�����.
 * �ڍs�Ɏ��Ԃ������邽�߁A���΂炭���̃}�N�������Ԃ��� */
#define ZnkVarpAry_find_byName ZnkVarpAry_findObj_byName
#define ZnkVarpAry_find_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_find_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )

/* �V�����L�q����ꍇ�͂�������g������.
 */
#define ZnkVarpAry_findIdx_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_findIdx_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )
#define ZnkVarpAry_findObj_byName_literal( ary, query_name, use_eqCase ) \
	ZnkVarpAry_findObj_byName( ary, query_name, Znk_strlen_literal(query_name), use_eqCase )

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
