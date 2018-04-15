#include "Znk_rgx_dfa.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <Znk_str.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG   1

/* DFAPath�͑J�ڐ�̃��X�g��\���^�B
   ����c�ɂ���ď��toDFAS�֑J�ڂ��� */
typedef struct DFAPath_tag {
	ZnkUChar64          c_;
	int                 toDFAS;
	struct DFAPath_tag* nextP; /* ���̃f�[�^�ւ̃|�C���^ */
} DFAPath;

/* ZnkRgxDFAState��DFA��Ԃ�\���^ */
struct ZnkRgxDFAState_tag {
	ZnkNSet32  nset_;    /* ����DFA��Ԃ��\��NFA��ԏW�� */
	int        visited;  /* �����ς݂Ȃ�1 */
	int        accepted; /* �I����Ԃ��܂ނȂ�1 */
	DFAPath*   nextP;    /* �J�ڐ�������p�X�̏W��(���X�g�Ŏ���) */
};



static void
deleteZnkRgxDFAState( ZnkRgxDFAState ds )
{
	if( ds ){
		DFAPath* p = ds->nextP;
		DFAPath* nextP;
		while( p ){
			nextP = p->nextP;
			Znk_free( p );
			p = nextP;
		}
		Znk_free( ds );
	}
}
static void
clearZnkRgxDFAStateAry( ZnkRgxDFAStateAry ary )
{
	size_t idx = 0;
	const size_t size = ZnkRgxDFAStateAry_size( ary );
	ZnkRgxDFAState ds;
	for( idx=0; idx<size; ++idx ){
		ds = (ZnkRgxDFAState)ZnkObjAry_at( (ZnkObjAry)ary, idx );
		deleteZnkRgxDFAState( ds );
	}
	ZnkObjAry_clear( (ZnkObjAry)ary );
}
ZnkRgxDFAStateAry
ZnkRgxDFAStateAry_create( void )
{
	ZnkObjAry ary = ZnkObjAry_create( NULL );
	return (ZnkRgxDFAStateAry)ary;
}
void
ZnkRgxDFAStateAry_destroy( ZnkRgxDFAStateAry ary )
{
	if( ary ){
		clearZnkRgxDFAStateAry( ary );
		ZnkObjAry_destroy( (ZnkObjAry)ary );
	}
}
void
ZnkRgxDFAStateAry_clear( ZnkRgxDFAStateAry ary )
{
	if( ary ){
		clearZnkRgxDFAStateAry( ary );
	}
}
size_t
ZnkRgxDFAStateAry_size( const ZnkRgxDFAStateAry ary )
{
	return ZnkObjAry_size( (ZnkObjAry)ary );
}
ZnkRgxDFAState
ZnkRgxDFAStateAry_at( const ZnkRgxDFAStateAry ary, size_t idx )
{
	return (ZnkRgxDFAState)ZnkObjAry_at( (ZnkObjAry)ary, idx );
}
static void
ZnkRgxDFAStateAry_push_bk( ZnkRgxDFAStateAry ary, ZnkRgxDFAState ds )
{
	ZnkObjAry_push_bk( (ZnkObjAry)ary, (ZnkObj)ds );
}



bool
ZnkRgxDFAState_isAccepted( ZnkRgxDFAState dfas )
{
	return (bool)( dfas->accepted != 0 );
}


/* DFA��\������i�f�o�b�O�p�j */
static void
dumpDFA( ZnkRgxDFAStateAry dfa_ary, ZnkRgxNFATable nfa_ary, ZnkStr msg )
{
	DFAPath *path;
	size_t i;
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	ZnkRgxDFAState ds;

	ZnkStr_add( msg, "--- DFA -----\n" );
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		ZnkStr_addf2( msg, "%2zu%c: ", i, ZnkRgxDFAState_isAccepted( ds ) ? 'A' : ' ' );
		for( path = ds->nextP; path != NULL; path = path->nextP ){
			if( path->c_.u64_ == ZnkRgx_DOT ){
				ZnkStr_addf2( msg, "DOT=>%d ", path->toDFAS );
			} else {
				ZnkStr_addf2( msg, "%c=>%d ", path->c_, path->toDFAS );
			}
		}
		ZnkStr_add( msg, "\n" );
	}
	
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		ZnkStr_addf2( msg, "state %2zu%c = {", i, ZnkRgxDFAState_isAccepted( ds ) ? 'A' : ' ' );
		//ZnkNSet32_print_e( ds->nset_ );
		ZnkStr_add( msg, "}\n" );
	}
	ZnkStr_add( msg, "--- DFA end-----\n" );
}

/***
 * NFA��ԏW��nset��DFA�ɓo�^���āADFA��Ԃւ̃|�C���^��Ԃ��B
 * nset���I����Ԃ��܂�ł���΁Aaccepted�t���O���Z�b�g����B
 * ���ł�nset��DFA�ɓo�^����Ă����牽�����Ȃ�.
 */
static size_t
ZnkRgxDFAState_regist( ZnkRgxDFAStateAry dfa_ary, const ZnkNSet32 nset, int nfa_exit )
{
	size_t i;
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	ZnkRgxDFAState ds;

	/* NFA���nset�����ł�DFA�ɓo�^����Ă�����A�������Ȃ��Ń��^�[������ */
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		if( ds && ZnkNSet32_eq( ds->nset_, nset) ){
			return i;
		}
	}
	
	/* ZnkRgxDFAState ��V�K�쐬 */
	ds = Znk_alloc0(sizeof(struct ZnkRgxDFAState_tag));
	/* DFA�ɕK�v�ȏ����Z�b�g���� */
	ds->nset_    = ZnkNSet32_create();
	ZnkNSet32_copy( ds->nset_, nset );
	ds->visited  = 0;
	ds->accepted = ZnkNSet32_exist(nset, nfa_exit) ? 1 : 0;
	ds->nextP    = NULL;
	ZnkRgxDFAStateAry_push_bk( dfa_ary, ds );
	return dfa_size;
}

/* �����ς݂̈󂪂��Ă��Ȃ�DFA��Ԃ�T���B
   ������Ȃ����NULL��Ԃ� */
static ZnkRgxDFAState
ZnkRgxDFAState_fetch_unvisited( ZnkRgxDFAStateAry dfa_ary )
{
	const size_t dfa_size = ZnkRgxDFAStateAry_size( dfa_ary );
	size_t i;
	ZnkRgxDFAState ds;
	for( i=0; i<dfa_size; ++i ){
		ds = ZnkRgxDFAStateAry_at( dfa_ary, i );
		if( ds && ds->visited == 0 ){
			return ds;
		}
	}
	return NULL;
}

/* ���dfas���當��c�ɂ���đJ�ڂ��āA�J�ڌ�̏�Ԃ�Ԃ��B
   ����c�ɂ���đJ�ڂł��Ȃ����NULL��Ԃ� */
size_t
ZnkRgxDFAState_findNextState( ZnkRgxDFAState dfas, ZnkUChar64 c )
{
	DFAPath* p;
	for( p = dfas->nextP; p != NULL; p = p->nextP ){
		if( c.u64_ == p->c_.u64_ || p->c_.u64_ == ZnkRgx_DOT ){
			return p->toDFAS;
		}
	}
	return Znk_NPOS;
}


/* NFA�𓙉���DFA�ւƕϊ����� */
ZnkRgxDFAState
ZnkRgxDFAState_convertFromNFA( ZnkRgxDFAStateAry dfa_ary, ZnkRgxNFATable nfa_ary, int nfa_entry, int nfa_exit )
{
	ZnkNSet32          initial_nset;
	ZnkRgxDFAState     ds;
	ZnkRgxNFAReachable rns;
	DFAPath*           dpath;
	size_t initial_dfa_state = 0;
#if DEBUG
	ZnkStr msg = ZnkStr_new( "" );
#else
	ZnkStr msg = NULL;
#endif
	
	/* DFA�̏�����Ԃ�o�^���� */
	initial_nset = ZnkNSet32_create();
	ZnkNSet32_push( initial_nset, nfa_entry );
	ZnkRgxNFATable_collect_empty_transition( nfa_ary, initial_nset );
	initial_dfa_state = ZnkRgxDFAState_regist( dfa_ary, initial_nset, nfa_exit );
	
	/* ��������DFA��Ԃ�����΁A��������o���ď�������B 
	   �i���ڂ��Ă���DFA��Ԃ�ds�Ƃ���j */
	while( (ds = ZnkRgxDFAState_fetch_unvisited(dfa_ary)) != NULL ){
		/* �����ς݂̈������ */
		ds->visited = 1;

		/* ���ds����J�ډ\��DFA��Ԃ����ׂ�DFA�ɓo�^����B
		   rns��(����, NFA��ԏW��)�̑΂ɂȂ��Ă��� */
		rns = ZnkRgxNFAReachable_create( nfa_ary, ds->nset_ );

		for( ; rns != NULL; rns = ZnkRgxNFAReachable_next(rns) ){
			/* NFA��ԏW���̃�-closure�����߂� */
			ZnkRgxNFATable_collect_empty_transition( nfa_ary, ZnkRgxNFAReachable_nset(rns) );
			/* dpath��V�K�쐬 */
			dpath = Znk_alloc0(sizeof(DFAPath));
			dpath->c_ = ZnkRgxNFAReachable_c( rns );
			/* NFA��ԏW����DFA�ɓo�^���� */
			dpath->toDFAS = ZnkRgxDFAState_regist( dfa_ary, ZnkRgxNFAReachable_nset(rns), nfa_exit );
			/* ds��dpath��ǉ� */
			dpath->nextP = ds->nextP;
			ds->nextP = dpath;
		}
		ZnkRgxNFAReachable_destroy( rns );
	}

	if( msg ){
		dumpDFA( dfa_ary, nfa_ary, msg );
		ZnkStr_delete( msg );
	}

	return ZnkRgxDFAStateAry_at( dfa_ary, initial_dfa_state );
}


