#include "Znk_rgx_nfa.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG   1

/***
 * �\���؂���NFA�𐶐�����
 */

/* NFAState��NFA�ɂ�����J�ڂ�\���^�B
   ����c�ɂ���ď��toNFA�ւƑJ�ڂ���  */
typedef struct NFAState_tag {
	ZnkUChar64           c_;
	int                  toNFA;
	struct NFAState_tag* next;   /* ���̃f�[�^�ւ̃|�C���^ */
} NFAState;

/***
 * NFA�̏�ԑJ�ڕ\
 * from0 => (to1,c1) (to2,c2), (to3,c3), ...
 * from1 => (to1,c1) (to2,c2), (to3,c3), ...
 * from2 => (to1,c1) (to2,c2), (to3,c3), ...
 * �Ƃ����悤�ȍ\�����Ƃ�. (�n�b�V���e�[�u������������ꍇ�̃��X�g�̔z��Ɏ��Ă���)
 */

static void deleteNFAState( NFAState* ns )
{
	NFAState* next;
	while( ns ){
		next = ns->next;
		Znk_free( ns );
		ns = next;
	}
}
Znk_INLINE void
clearZnkRgxNFATable( ZnkRgxNFATable ary )
{
	size_t idx = 0;
	const size_t size = ZnkRgxNFATable_size( ary );
	for( idx=0; idx<size; ++idx ){
		NFAState* ns = (NFAState*)ZnkObjAry_at( (ZnkObjAry)ary, idx );
		deleteNFAState( ns );
	}
	ZnkObjAry_clear( (ZnkObjAry)ary );
}
ZnkRgxNFATable
ZnkRgxNFATable_create( void )
{
	/***
	 * ���̃|�C���^�̔z��ł́A�e�v�f��ZnkObjAry_set�ɂ��p�ɂɏ��������ɂ��ւ�炸�A
	 * ���̃^�C�~���O�ŋ��v�f�l��next�Ɉړ�����A���̎��Ԃ��܂���������Ɏc���Ă����Ȃ���΂Ȃ�Ȃ�.
	 * ���̂悤�ȓ���ȍ\���̂��߁AZnkObjAry_set�ȂǂŎ����I�ȉ�����Ȃ���Ă͂Ȃ�Ȃ�.
	 * �]����deleteElem�ɂ�NULL���w�肵�Ă���.
	 * �e�v�f�̉����destroy�̃^�C�~���O�݈̂ꊇ����next�̂��̂܂œ��܂��čs��.
	 */
	ZnkObjAry ary = ZnkObjAry_create( NULL );
	return (ZnkRgxNFATable)ary;
}
void
ZnkRgxNFATable_destroy( ZnkRgxNFATable ary )
{
	if( ary ){
		clearZnkRgxNFATable( ary );
		ZnkObjAry_destroy( (ZnkObjAry)ary );
	}
}
void
ZnkRgxNFATable_clear( ZnkRgxNFATable ary )
{
	if( ary ){
		clearZnkRgxNFATable( ary );
	}
}
size_t
ZnkRgxNFATable_size( const ZnkRgxNFATable ary )
{
	return ZnkObjAry_size( (ZnkObjAry)ary );
}
NFAState*
ZnkRgxNFATable_at( const ZnkRgxNFATable ary, size_t idx )
{
	return (NFAState*)ZnkObjAry_at( (ZnkObjAry)ary, idx );
}
void
ZnkRgxNFATable_set( ZnkRgxNFATable ary, size_t idx, NFAState* ns )
{
	ZnkObjAry_set( (ZnkObjAry)ary, idx, (ZnkObj)ns );
}


/* �m�[�h�ɔԍ������蓖�Ă� */
int ZnkRgxNFATable_gen_node( ZnkRgxNFATable nfa_ary )
{
	const size_t size = ZnkObjAry_size( (ZnkObjAry)nfa_ary );
	ZnkObjAry_push_bk( (ZnkObjAry)nfa_ary, NULL );
    return (int)size;
}

/* NFA��\������i�f�o�b�O�p�j */
void ZnkRgxNFATable_dump( ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit )
{
	size_t i;
	NFAState *p;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );

	Znk_printf("--- NFA -----\n");
	Znk_printf("state entry=%3d exit=%3d\n", nfa_entry, nfa_exit);
	for( i = 0; i <size; ++i ) {
		p = ZnkRgxNFATable_at( nfa_ary, i );
		if( p ){
			Znk_printf("state %3zu: ", i);
			for( ; p != NULL; p = p->next) {
				if( p->c_.u64_ == ZnkRgx_DOT ){
					Znk_printf("(DOT :toNFA=%d) ", p->toNFA);
				} else if( p->c_.u64_ == ZnkRgx_EMPTY ){
					Znk_printf("(EMPTY :toNFA=%d) ", p->toNFA);
				} else {
					Znk_printf("(%s(%08I64x) :toNFA=%d) ", p->c_.s_, p->c_.u64_, p->toNFA);
				}
			}
			Znk_printf("\n");
		}
	}
	Znk_printf("--- NFA end-----\n");
}



/* NFA��ԏW��nset��NFA���s��ǉ�����B 
   ������NFA���s����ÑJ�ڂňړ��ł���NFA��Ԃ��ǉ����� */
static void mark_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset, int s )
{
	NFAState* p;
	ZnkNSet32_push(nset, s);
	p = ZnkRgxNFATable_at( nfa_ary, s );
	for( ; p != NULL; p = p->next ){
		if( p->c_.u64_ == ZnkRgx_EMPTY && !ZnkNSet32_exist(nset, p->toNFA) ){
			mark_empty_transition( nfa_ary, nset, p->toNFA );
		}
	}
}

/* NFA��ԏW��nset�ɑ΂��ă�-follow��������s����B
   �ÑJ�ڂőJ�ډ\�Ȃ��ׂĂ�NFA��Ԃ�ǉ����� */
void ZnkRgxNFATable_collect_empty_transition( ZnkRgxNFATable nfa_ary, ZnkNSet32 nset )
{
	/* TODO: nset�̃C�e���[�g��݂��A������C�e���[�g������΂悢�����ł͂Ȃ��� ? */
	size_t i;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );
	for ( i = 0; i < size; ++i ) {
		if( ZnkNSet32_exist(nset, i) ){
			mark_empty_transition( nfa_ary, nset, i);
		}
	}
}

void
ZnkRgxNFATable_add_transition( ZnkRgxNFATable nfa_ary, int from, int toNFA, ZnkUChar64 c )
{
	NFAState* new;
	new = (NFAState *)Znk_alloc0(sizeof(NFAState));
	new->c_    = c;
	new->toNFA = toNFA;
	new->next  = ZnkRgxNFATable_at( nfa_ary, from );
	ZnkRgxNFATable_set( nfa_ary, from, new );
}



/***
 * NFA2DFA �����W���\���@�Ɋւ���⏕���[�`��.
 */

struct ZnkRgxNFAReachable_tag {
	ZnkUChar64                  c_;
	ZnkNSet32                   nset_;
	struct ZnkRgxNFAReachable_tag* nextRNS; /* ���̃f�[�^�ւ̃|�C���^ */
};

/***
 * query_c�ɂ����toNFA�֑J�ڂ�����NFA(ZnkRgxNFAReachable)����ǉ�����.
 * ZnkRgxNFAReachable��c���L�[�Ƃ���悤�ȃ��X�g�ƂȂ��Ă���.
 * ���Ȃ킿ZnkRgxNFAReachable�̃m�[�h��rns(N)�Ƃ����
 * rns(0) c0 : nset(toNFA0, toNFA1, toNFA2...)
 * rns(1) c1 : nset(toNFA0, toNFA1, toNFA2...)
 * rns(2) c2 : nset(toNFA0, toNFA1, toNFA2...)
 * �Ƃ������悤�ȍ\��������.
 */
static ZnkRgxNFAReachable
internZnkRgxNFAReachable( ZnkRgxNFAReachable rns, ZnkUChar64 query_c, int toNFA )
{
	ZnkRgxNFAReachable new;
	ZnkRgxNFAReachable a = rns;
	bool found = false;

	if( query_c.u64_ == ZnkRgx_DOT ){
		/***
		 * ���ׂĂ̕����ɂ���toNFA�ւ̑J�ڂ�ǉ�����.
		 */
		for( ; a != NULL; a=a->nextRNS ){
			if( a->c_.u64_ == ZnkRgx_DOT ){
				/* found */
				found = true;
			}
			ZnkNSet32_push(a->nset_, toNFA);
		}
	} else {
		/* ����c�Ɋւ��� ZnkRgxNFAReachable��T�����A�����̂��̂����������ꍇ�͂���nset��toNFA����ǉ� */
		for( ; a != NULL; a=a->nextRNS ){
			if( a->c_.u64_ == query_c.u64_ ){
				/* found */
				ZnkNSet32_push(a->nset_, toNFA);
				found = true;
				break;
			}
		}
	}

	if( found ){
		return rns;
	}

	/* not found */
	
	/* ����c�Ɋւ��� ZnkRgxNFAReachable��V�K�Ɋ��蓖�āA���X�g�ɒǉ� */
	new = Znk_alloc0( sizeof(struct ZnkRgxNFAReachable_tag) );
	new->c_ = query_c;
	new->nset_ = ZnkNSet32_create();
	new->nextRNS = rns;
	
	/* nset��toNFA����ǉ� */
	ZnkNSet32_push(new->nset_, toNFA);
	
	return new;
}


/***
 * �^����ꂽZnkNSet32 nset����J�ډ\��NFA��Ԃ̏W�������߂�.
 * ���ʂ̓��X�g�\���Ƃ��č쐬�����.
 */
ZnkRgxNFAReachable
ZnkRgxNFAReachable_create( ZnkRgxNFATable nfa_ary, const ZnkNSet32 nset )
{
	size_t i;
	NFAState* p;
	ZnkRgxNFAReachable result = NULL;
	const size_t size = ZnkRgxNFATable_size( nfa_ary );
	
	/* TODO: nset�̃C�e���[�g��݂��A������C�e���[�g������΂悢�����ł͂Ȃ��� ? */
	
	/* NFA��ԃe�[�u�������ɒ��ׂ� */
	for( i=0; i<size; ++i ){
		/* NFA���i��nset�Ɋ܂܂�Ă���΁A�ȉ��̏������s�� */
		if( ZnkNSet32_exist(nset, i) ){
			/* NFA���i����J�ډ\��NFA��Ԃ����ׂăC�e���[�g.
			 * �������ÑJ�ڂ͏��O���� */
			p = ZnkRgxNFATable_at( nfa_ary, i );
			for( ; p != NULL; p = p->next ){
				if( p->c_.u64_ != ZnkRgx_EMPTY ){
					result = internZnkRgxNFAReachable( result, p->c_, p->toNFA );
				}
			}
		}
	}
	
	/* �쐬�������X�g��Ԃ� */
	return result;
}

void
ZnkRgxNFAReachable_destroy( ZnkRgxNFAReachable rns )
{
	if( rns ){
		ZnkNSet32_destroy( rns->nset_ );
		Znk_free( rns );
	}
}

ZnkRgxNFAReachable
ZnkRgxNFAReachable_next( const ZnkRgxNFAReachable rns )
{
	return rns->nextRNS;
}

ZnkUChar64
ZnkRgxNFAReachable_c( const ZnkRgxNFAReachable rns )
{
	return rns->c_;
}

ZnkNSet32
ZnkRgxNFAReachable_nset( const ZnkRgxNFAReachable rns )
{
	return rns->nset_;
}


