#ifndef INCLUDE_GUARD__Znk_rgx_nfa_h__
#define INCLUDE_GUARD__Znk_rgx_nfa_h__

#include <Znk_base.h>
#include <Znk_obj_ary.h>
#include <Znk_nset.h>
#include "Znk_rgx_base.h"

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( ZnkRgxNFATable );

ZnkRgxNFATable
ZnkRgxNFATable_create( void );
void
ZnkRgxNFATable_destroy( ZnkRgxNFATable ary );
void
ZnkRgxNFATable_clear( ZnkRgxNFATable ary );
size_t
ZnkRgxNFATable_size( const ZnkRgxNFATable ary );
/**
 * �V�����m�[�h�ԍ��𔭍s����.
 */
int
ZnkRgxNFATable_gen_node( ZnkRgxNFATable nfa_ary );

/**
 * NFA��\������i�f�o�b�O�p�j.
 */
void
ZnkRgxNFATable_dump( ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit );


/**
 * NFA��ԏW��nset�ɑ΂��ă�-follow��������s����.
 * �ÑJ�ڂőJ�ډ\�Ȃ��ׂĂ�NFA��Ԃ�ǉ�����.
 */
void
ZnkRgxNFATable_collect_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset );

/**
 * NFA�ɏ�ԑJ�ڂ�ǉ�����.
 * ���from�ɑ΂��āA����c�̂Ƃ��ɏ��toNFA�ւ̑J�ڂ�ǉ�����.
 */
void
ZnkRgxNFATable_add_transition( ZnkRgxNFATable nfa_ary, int from, int toNFA, ZnkUChar64 c );


typedef struct ZnkRgxNFAReachable_tag* ZnkRgxNFAReachable;

/***
 * �^����ꂽZnkNSet32 nset����J�ډ\��NFA��Ԃ̏W�������߂�.
 * ���ʂ͕���c�𓯒l�ނƂ������X�g�\���Ƃ��č쐬�����.
 */
ZnkRgxNFAReachable
ZnkRgxNFAReachable_create( ZnkRgxNFATable nfa_ary, const ZnkNSet32 nset );

void
ZnkRgxNFAReachable_destroy( ZnkRgxNFAReachable rns );

ZnkRgxNFAReachable
ZnkRgxNFAReachable_next( const ZnkRgxNFAReachable rns );

ZnkUChar64
ZnkRgxNFAReachable_c( const ZnkRgxNFAReachable rns );

ZnkNSet32
ZnkRgxNFAReachable_nset( const ZnkRgxNFAReachable rns );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

