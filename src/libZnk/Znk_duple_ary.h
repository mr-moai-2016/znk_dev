#ifndef INCLUDE_GUARD__Znk_duple_ary_h__
#define INCLUDE_GUARD__Znk_duple_ary_h__

#include <Znk_prim.h>
#include <Znk_obj_ary.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkDupleAryImpl* ZnkDupleAry;

/**
 * @brief
 * val_ �Ɋւ��Ă͓ǂ݂͂������A���������Ă��悢.
 * (������\���̂̃����o�Œ��ڍs���Ă��悢���A���L�A�N�Z�T���g���Ă��悢).
 * ���̏���������set�����ɊY������. �Ⴆ�Ί���findElem�ɂ��ZnkDuple��
 * �����Ă���ꍇ�AZnkDupleAry_setVal(*)�̂悤�Ȋ֐����͂邩�ɍ����ł���.
 * �����ł͍������̂��߁AZnkDuple�̖{���̎���(ZnkDuplePrivateImpl)������
 * �ꕔ�N���`���Ƃ��Ă���.
 */
struct ZnkDupleImpl {
	const void* val_;
};
typedef struct ZnkDupleImpl* ZnkDuple;

Znk_INLINE const void*
ZnkDuple_val( const ZnkDuple elem ){
	return elem->val_;
}
Znk_INLINE void
ZnkDuple_set_val( ZnkDuple elem, const void* val ){
	elem->val_ = val;
}

/**
 * @brief
 * ���̊֐��͎g�p���@�ɂ���Ă�ZnkDupleAry�̃f�[�^��������j�󂷂鋰�ꂪ����.
 * �ł��邾���g�p�͎��d���A�g�p����ꍇ�ł��אS�̒��ӂ𕥂�����.
 * (���ɂ��̒l��ZnkStr�Ȃǂ̕�����ł������ꍇ�A���̓��e��ύX���Ă͂Ȃ�Ȃ�)
 */
const void*
ZnkDuple_ikey_unsafe( const ZnkDuple elem );


/**
 * @brief
 * ZnkDupleAry�̓�����K�肷�邽�߂̊֐��Q
 *
 * ZnkDupleKeyFuncT_eq:
 *   ��1�����ɂ�ikey(ZnkDuplePrivate���ŕێ�����L�[)���^������.
 *   ��2�����ɂ�key(���[�U�w��ɂ��L�[)���^������.
 *   ���̓�̃L�[�����������ǂ������r���邽�߂̊֐��ł���A
 *   �������ꍇ�ɂ�true�A�����łȂ��ꍇ�ɂ�false��Ԃ����̂Ƃ���.
 *
 * ZnkDupleKeyFuncT_makeIKey:
 *   �����ɂ�key(���[�U�w��ɂ��L�[)���^������.
 *   �߂�l�Ƃ���key��萶�����ꂽikey(ZnkDuplePrivate���ŕێ�����L�[)��
 *   �Ԃ����̂Ƃ���.
 *
 * ZnkDupleKeyFuncT_copyIKey:
 *   �����ɂ�ikey(ZnkDuplePrivate���ŕێ�����L�[)���^������.
 *   �߂�l�Ƃ���ikey���N���[���������ꂽikey��Ԃ����̂Ƃ���.
 *
 * ZnkDupleKeyFuncT_deleteIKey:
 *   �����ɂ�ikey(ZnkDuplePrivate���ŕێ�����L�[)���^������.
 *   �^����ꂽikey�����������@���K�肷�邽�߂̊֐��ł���.
 *
 *
 * ZnkDupleValFuncT_copyVal:
 *   �����ɂ�val���^������.
 *   �߂�l�Ƃ���val���N���[���������ꂽval�l��Ԃ����̂Ƃ���.
 *
 * ZnkDupleValFuncT_deleteVal:
 *   �����ɂ�val���^������.
 *   �^����ꂽval�����������@���K�肷�邽�߂̊֐��ł���.
 */
typedef bool        (*ZnkDupleKeyFuncT_eq)( const void* ikey, const void* key );
typedef const void* (*ZnkDupleKeyFuncT_makeIKey)( const void* );
typedef const void* (*ZnkDupleKeyFuncT_copyIKey)( const void* );
typedef void        (*ZnkDupleKeyFuncT_deleteIKey)( void* );

typedef struct ZnkDupleKeyFuncs_tag {
	ZnkDupleKeyFuncT_eq         eq_;
	ZnkDupleKeyFuncT_makeIKey   make_ikey_;
	ZnkDupleKeyFuncT_copyIKey   copy_ikey_;
	ZnkDupleKeyFuncT_deleteIKey delete_ikey_;
}ZnkDupleKeyFuncs;

typedef const void* (*ZnkDupleValFuncT_copyVal)( const void* );
typedef void        (*ZnkDupleValFuncT_deleteVal)( void* );

typedef struct ZnkDupleValFuncs_tag {
	ZnkDupleValFuncT_copyVal   copy_val_;
	ZnkDupleValFuncT_deleteVal delete_val_;
}ZnkDupleValFuncs;


ZnkDupleAry
ZnkDupleAry_create(
		const ZnkDupleKeyFuncs* key_funcs,
		const ZnkDupleValFuncs* val_funcs );
void
ZnkDupleAry_destroy( ZnkDupleAry dupa );

size_t
ZnkDupleAry_size( const ZnkDupleAry dupa );

ZnkDuple
ZnkDupleAry_atElem( ZnkDupleAry dupa, size_t idx );

ZnkDuple
ZnkDupleAry_findElem( ZnkDupleAry dupa, const void* key );

ZnkDuple
ZnkDupleAry_regist( ZnkDupleAry dupa, const void* key, const void* val, bool is_update, bool* is_newly );

void
ZnkDupleAry_swap( ZnkDupleAry ht1, ZnkDupleAry ht2 );
void
ZnkDupleAry_copy( ZnkDupleAry dst, const ZnkDupleAry src );

void
ZnkDupleAry_clear( ZnkDupleAry dupa );

bool
ZnkDupleAry_erase( ZnkDupleAry dupa, const void* key );


/**
 * @brief
 * ZnkDupleAry_create�̈����Ƃ��Ďg����T�^�I��ZnkDupleKeyFuncs���W�߂�����.
 * �ȉ���key�Ƃ̓��[�U���w�肷�����key���Ӗ�����.
 *
 * uval : 
 *   key��ikey���Ƃ���unsigned�Ȑ����ł������
 * cstr : 
 *   key��ikey���Ƃ���const char* �ł������
 *   key��ikey�͊��S�ɓ��ꂩ�����ꏊ���������|�C���^�ł���Aikey�ւ�strdup�͍s���Ȃ�.
 *   ���Ȃ��Ƃ�����HashTable�̎����������Ԃ�key�̎����������K�v������.
 * strdup : 
 *   key��ikey���Ƃ���const char* �ł������
 *   key�Ƃ͕ʂ�ikey�Ƃ���strdup�ɂ��R�s�[�ƃ������m�ۂ��s����.
 *   �w�肵�����[�Ukey�́A���[�U�������ɔj�����Ă����͂Ȃ�.
 * ZnkStr : 
 *   key��const char*�ł���Aikey��ZnkStr �ł������
 *   �w�肵�����[�Ukey�́A���[�U�������ɔj�����Ă����͂Ȃ�.
 */
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_uval( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_cstr( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_strdup( void );
const ZnkDupleKeyFuncs* ZnkDupleKeyFuncs_get_ZnkStr( void );

/**
 * @brief
 * ZnkDupleAry_create�̈����Ƃ��Ďg����T�^�I��ZnkDupleValFuncs���W�߂�����.
 *
 * uval : 
 *   val��unsigned�Ȑ����ł������.
 *   �������Auval�Ƃ������O�ł͂��邪�A���ۂɂ� unsigned int�Ɍ��炸
 *   C����I�� = ���Z�q�ő���\������ɓ��ʂȊ֐���K�v�Ƃ��Ȃ��X�J���[�ʂȂ��
 *   ���ׂĂ���ł��܂�����.
 */
const ZnkDupleValFuncs* ZnkDupleValFuncs_get_uval( void );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
