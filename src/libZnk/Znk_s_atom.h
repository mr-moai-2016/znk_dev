#ifndef INCLUDE_GUARD__Znk_s_atom_h__
#define INCLUDE_GUARD__Znk_s_atom_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkSAtomDBImpl* ZnkSAtomDB;

typedef uintptr_t ZnkSAtomId;

ZnkSAtomDB ZnkSAtomDB_create( void );
void       ZnkSAtomDB_destroy( ZnkSAtomDB atomdb );

/**
 * @brief
 *   C������ key �́u���e�v�ɑΉ������ӂ� id (ZnkSAtomId)��Ԃ�.
 *   ���̂Ƃ��Akey �̓��e�́Aatomdb������������DB�֕ۑ������.
 *   �߂�l�ł��� id �̎��̂́A����DB�ɂ����� key �̓��e���ۑ����ꂽ�C���f�b�N�X��Ԃ����l�ł���A
 *   ���ɓ����u���e�v�� key ���w�肳�ꂽ�ꍇ�́A�O��o�^���ꂽ�Ƃ��Ɠ����l�� id ���Ԃ���邾����
 *   �V���ȓo�^�͍s���Ȃ�. �܂�A���̊֐��ł͎w�肳�ꂽkey�Ɠ����u���e�v�̂��̂����ɑ��݂���
 *   ���邩�ǂ����𖈉�����V�[�N���Ă��邽�߂��܂荂���ł͂Ȃ�.
 *   key�̃|�C���^�l�Ƃ��Ă̔�r�ł͂Ȃ��A�����܂ł��ꂪ����������̓��e�������ł��邩�ۂ���
 *   ���f���Ă���_�ɒ��ӂ���.
 *
 * @return
 *   intern�����������ʓ���ꂽ�Akey�̓��e�ɑΉ�����id(ZnkSAtomId)��Ԃ�.
 *
 * @note
 *   ���̏�����X Window System��Atom�ɂ�����intern������^�������̂ł���.
 */
ZnkSAtomId    ZnkSAtomDB_intern( ZnkSAtomDB atomdb, const char* key );

/**
 * @brief
 *   id ������������� atomdb ����Q�Ƃ��ĕԂ�.
 *   id ������������Ƃ́AZnkSAtomDB_intern �Ăяo�����ɓo�^����������ł���.
 *
 * @return
 *   id ������������̃|�C���^�ł���ANULL�I�[����C������ł���.
 *   ���ꂪ����������|�C���^�́Aid ���Ή����� atomdb ���AZnkSAtomDB_destroy�ɂ���Ĕj������Ȃ�����
 *   �����ɂȂ邱�Ƃ͂Ȃ�.
 *
 * @note
 *   �߂�l�ƂȂ镶����̃|�C���^�͓���ZnkVStr�̊e�v�f�ł���ZnkStr������ɓ����ŕێ�����|�C���^�ł���.
 *   �]���āA�P���ɂ���ZnkVStr�����T�C�Y�Ȃǂ��ꂽ�ꍇ�ɁA���̃|�C���^�ւ̉e����S�z����K�v�͂Ȃ�.
 */
const char* ZnkSAtomDB_cstr( ZnkSAtomDB atomdb, ZnkSAtomId id );

void
ZnkSAtomDB_swap( ZnkSAtomDB atomdb1, ZnkSAtomDB atomdb2 );

/***
 * ZnkSAtomId �� ZnkSAtomDB�̃Z�b�g.
 * �ȉ��̓��[�U�w���p�[�f�[�^�Ɗ֐��ł���.
 */
typedef struct {
	ZnkSAtomId id_;
	ZnkSAtomDB db_;
} ZnkSAtomPr;

Znk_INLINE void
ZnkSAtomPr_intern( ZnkSAtomPr* ans_atompr, ZnkSAtomDB atomdb, const char* key )
{
	ans_atompr->id_ = ZnkSAtomDB_intern( atomdb, key );
	ans_atompr->db_ = atomdb;
}
Znk_INLINE const char*
ZnkSAtomPr_cstr( const ZnkSAtomPr* atompr )
{
	return ZnkSAtomDB_cstr( atompr->db_, atompr->id_ );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
