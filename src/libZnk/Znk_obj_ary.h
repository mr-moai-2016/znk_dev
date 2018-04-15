#ifndef INCLUDE_GUARD__Znk_obj_ary_h__
#define INCLUDE_GUARD__Znk_obj_ary_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkObj );
typedef struct ZnkObjAryImpl* ZnkObjAry;

typedef bool (*ZnkObjQueryFunc)( ZnkObj, void* arg );
typedef ZnkObj (*ZnkObjAllocFunc)( void* arg );
typedef void (*ZnkElemDeleterFunc)( void* );

/**
 * @brief Ary�𐶐�����.
 *
 * @param elem_deleter:
 *  ���ꂪ��NULL�̏ꍇ�A�e�v�f�̎����ɂ��Ă���Ary�͐ӔC������.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�Ƃ��Ă��ꂪ�Ă΂��.
 *  ���̂Ƃ��A���[�U�͊O���Ŏ��������蓖�Ă�object��set, push_bk�Ȃǂœn���Ă��̂܂ܕ��u���Ă���
 *  �`�ɂȂ�(����������ΊO���ł����delete���Ă͂Ȃ�Ȃ�).
 *
 *  ���ꂪNULL�̏ꍇ�́A�P�ɂ���Ary���|�C���^�l�̈ꎞ�I�ȃR���e�i�Ƃ��Ďg�p���A
 *  �e�v�f�̎����͊O���̋@�\�ɂ���ĊǗ����Ă��邱�Ƃ��Ӗ�����.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�͌Ă΂�Ȃ�.
 */
ZnkObjAry
ZnkObjAry_create( ZnkElemDeleterFunc elem_deleter );

/**
 * @brief Ary��j������.
 *  Ary��elem_deleter����NULL�ɐݒ肳��Ă���ꍇ�́A
 *  �S�v�f�ɂ��Ă���deleter���Ă΂��.
 */
void
ZnkObjAry_destroy( ZnkObjAry obj_ary );

size_t
ZnkObjAry_size( const ZnkObjAry obj_ary );

ZnkObj
ZnkObjAry_at( const ZnkObjAry obj_ary, size_t idx );
ZnkObj*
ZnkObjAry_ary_ptr( ZnkObjAry obj_ary );

ZnkElemDeleterFunc
ZnkObjAry_getElemDeleter( const ZnkObjAry obj_ary );

/**
 * @brief Ary�̗v�f���폜����.
 *  Ary��elem_deleter����NULL�ɐݒ肳��Ă���ꍇ�́A
 *  �Ώۗv�f�ɂ��Ă���deleter���Ă΂��.
 *
 * @note
 *  �폜�v�f��idx�����m�ł���ꍇ��byIdx�ł̕����T�����s��Ȃ��������ł���.
 */
bool
ZnkObjAry_erase( ZnkObjAry obj_ary, ZnkObj obj );
bool
ZnkObjAry_erase_byIdx( ZnkObjAry obj_ary, size_t idx );
/**
 * @brief Ary�̗v�f�̂����A�����ɍ��v������̂����ׂč폜����.
 *
 * @note
 *  ���̊֐����R���p�N�V���������Ƃ��ė��p���邱�Ƃ��ł���.
 *  �����A���A���^�C�������̓r���Ȃǂ�erase���Ă΂��Ȃ�炩�̈�����邾���ɂ��Ă����A
 *  ��ŗ]�T������Ƃ��ɂ��̊֐����Ăяo���Ĉ�̂������̂��ꊇ�폜���邱�Ƃō�������
 *  �ȃ����������}���.
 */
size_t
ZnkObjAry_erase_withQuery( ZnkObjAry obj_ary, ZnkObjQueryFunc query_func, void* query_arg );
bool
ZnkObjAry_pop_bk( ZnkObjAry obj_ary );

/**
 * @brief Ary�̃T�C�Y��0�Ƃ���.
 *  Ary��elem_deleter����NULL�ɐݒ肳��Ă���ꍇ�́A
 *  ���ׂĂ̗v�f�ɂ��Ă���deleter���Ă΂��.
 */
void
ZnkObjAry_clear( ZnkObjAry obj_ary );
/**
 * @brief Ary�̃T�C�Y��size�Ƃ���.
 *  Ary��elem_deleter����NULL�ɐݒ肳��Ă���ꍇ�́A
 *  �����ɂ����erase����邷�ׂĂ̗v�f�ɂ��Ă���deleter���Ă΂��.
 *  ����A����ɂ����ẮA�V�K�̈�ɂ�NULL�|�C���^���Z�b�g�����.
 */
void
ZnkObjAry_resize( ZnkObjAry obj_ary, size_t size );

void
ZnkObjAry_push_bk( ZnkObjAry obj_ary, ZnkObj obj );
void
ZnkObjAry_set( ZnkObjAry obj_ary, size_t idx, ZnkObj obj );

void
ZnkObjAry_swap( ZnkObjAry obj_ary1, ZnkObjAry obj_ary2 );

ZnkObj
ZnkObjAry_intern( ZnkObjAry obj_ary, size_t* ans_idx,
		ZnkObjQueryFunc query_func, void* query_arg,
		ZnkObjAllocFunc alloc_func, void* alloc_arg,
		bool* is_newly );

/***
 * helper macros.
 */
#define ZnkObjAry_M_DESTROY( ary )            ZnkObjAry_destroy( (ZnkObjAry)ary )
#define ZnkObjAry_M_SIZE( ary )               ZnkObjAry_size( (ZnkObjAry)ary )
#define ZnkObjAry_M_AT( ary, idx, elem_type ) (elem_type) ZnkObjAry_at( (ZnkObjAry)ary, idx )
#define ZnkObjAry_M_ARY_PTR( ary, elem_type ) (elem_type*)ZnkObjAry_ary_ptr( (ZnkObjAry)ary )
#define ZnkObjAry_M_ERASE( ary, obj )         ZnkObjAry_erase( (ZnkObjAry)ary, (ZnkObj)obj )
#define ZnkObjAry_M_ERASE_BY_IDX( ary, idx )  ZnkObjAry_erase_byIdx( (ZnkObjAry)ary, idx )
#define ZnkObjAry_M_POP_BK( ary )             ZnkObjAry_pop_bk( (ZnkObjAry)ary )
#define ZnkObjAry_M_CLEAR( ary )              ZnkObjAry_clear( (ZnkObjAry)ary )
#define ZnkObjAry_M_RESIZE( ary, size )       ZnkObjAry_resize( (ZnkObjAry)ary, size )
#define ZnkObjAry_M_PUSH_BK( ary, obj )       ZnkObjAry_push_bk( (ZnkObjAry)ary, (ZnkObj)obj )
#define ZnkObjAry_M_SET( ary, idx, obj )      ZnkObjAry_set( (ZnkObjAry)ary, idx, (ZnkObj)obj )
#define ZnkObjAry_M_SWAP( ary1, ary2 )        ZnkObjAry_swap( (ZnkObjAry)ary1, (ZnkObjAry)ary2 )


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
