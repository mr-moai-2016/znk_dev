#include <Znk_rgx.h>
#include "Znk_rgx_tree.h"
#include "Znk_rgx_nfa.h"
#include "Znk_rgx_dfa.h"
#include <Znk_stdc.h>
#include <Znk_str_fio.h>
#include <Znk_s_base.h>
#include <stdlib.h>
#include <string.h>


static bool st_verbose = false;

struct ZnkRgxInfoImpl {
	ZnkRgxNFATable nfa_ary_;
	ZnkRgxDFAStateAry dfa_ary_;
	int         nfa_entry_;
	int         nfa_exit_;
	ZnkRgxDFAState    initial_dfa_state_;
	ZnkUChar64FuncT_get  func_get_;
	ZnkIncrementU8PFuncT func_increment_;
};

struct ZnkRgxEncodeImpl {
	const char* id_;
	ZnkUChar64FuncT_get  func_get_;
	ZnkIncrementU8PFuncT func_increment_;
};

/* �\����tree�ɑ΂���NFA�𐶐�����B
   NFA�̓�����entry�A�o����way_out�Ƃ��� */
static bool
genNFA_byTree( ZnkRgxNFATable nfa_ary, ZnkRgxTree *tree, int entry, int way_out )
{
	static const ZnkUChar64 st_dot   = { ZnkRgx_DOT }; 
	static const ZnkUChar64 st_empty = { ZnkRgx_EMPTY }; 
	int  a1, a2;

	switch (tree->op) {
	case ZnkRgxOP_char:
		ZnkRgxNFATable_add_transition( nfa_ary, entry, way_out, tree->u.c_);
		break;
	case ZnkRgxOP_dot:
		ZnkRgxNFATable_add_transition( nfa_ary, entry, way_out, st_dot );
		break;
	case ZnkRgxOP_empty:
		ZnkRgxNFATable_add_transition( nfa_ary, entry, way_out, st_empty );
		break;
	case ZnkRgxOP_union:
		if( !genNFA_byTree( nfa_ary, tree->u.x.left_, entry, way_out) ){
			return false;
		}
		if( !genNFA_byTree( nfa_ary, tree->u.x.right_, entry, way_out) ){
			return false;
		}
		break;
	case ZnkRgxOP_closure:
		a1 = ZnkRgxNFATable_gen_node( nfa_ary );
		a2 = ZnkRgxNFATable_gen_node( nfa_ary );
		ZnkRgxNFATable_add_transition( nfa_ary, entry, a1, st_empty );
		if( !genNFA_byTree( nfa_ary, tree->u.x.left_, a1, a2) ){
			return false;
		}
		ZnkRgxNFATable_add_transition( nfa_ary, a2, a1, st_empty );
		ZnkRgxNFATable_add_transition( nfa_ary, a1, way_out, st_empty );
		break;
	case ZnkRgxOP_concat:     
		a1 = ZnkRgxNFATable_gen_node( nfa_ary );
		if( !genNFA_byTree( nfa_ary, tree->u.x.left_, entry, a1) ){
			return false;
		}
		if( !genNFA_byTree( nfa_ary, tree->u.x.right_, a1, way_out) ){
			return false;
		}
		break;
	default:
		return false;
	}
	return true;
}


/* �\����tree�ɑΉ�����NFA�𐶐����� */
static bool
initNFA( ZnkRgxNFATable nfa_ary, int* nfa_entry, int* nfa_exit, const char* regexp_str, ZnkUChar64FuncT_get func_get, ZnkStr rep_msg )
{
	bool result = false;
	/* �w�肳�ꂽ���K�\�����p�[�X���č\���؂𓾂� */
	ZnkRgxTree* tree = ZnkRgxTree_create( regexp_str, func_get, rep_msg );

	ZnkRgxNFATable_clear( nfa_ary );

	/* NFA�̏�����Ԃ̃m�[�h�����蓖�Ă� */
	*nfa_entry = ZnkRgxNFATable_gen_node( nfa_ary );
	
	/* NFA�̏I����Ԃ̃m�[�h�����蓖�Ă� */
	*nfa_exit  = ZnkRgxNFATable_gen_node( nfa_ary );
	
	/* NFA�𐶐����� */
	if( !genNFA_byTree( nfa_ary, tree, *nfa_entry, *nfa_exit ) ){
		goto FUNC_END;
	}

	result = true;
FUNC_END:
	ZnkRgxTree_destroy( tree, rep_msg );
	return result;
}


ZnkRgxInfo
ZnkRgxInfo_create( void )
{
	ZnkRgxInfo info = Znk_alloc0( sizeof(struct ZnkRgxInfoImpl) );
	info->nfa_ary_ = ZnkRgxNFATable_create();
	info->dfa_ary_ = ZnkRgxDFAStateAry_create();
	return info;
}
bool
ZnkRgxInfo_setup( ZnkRgxInfo info, const char* regexp_str, const ZnkRgxEncode enc, ZnkStr rep_msg )
{
	bool result = false;
	result = initNFA( info->nfa_ary_, &info->nfa_entry_, &info->nfa_exit_,
			regexp_str, enc->func_get_, rep_msg );
	if( !result ){
		return false;
	}
	info->func_get_       = enc->func_get_;
	info->func_increment_ = enc->func_increment_;

	if( st_verbose ){
		/* ��������NFA�̓��e��\������ */
		ZnkRgxNFATable_dump( info->nfa_ary_, info->nfa_entry_, info->nfa_exit_ );
	}

	ZnkRgxDFAStateAry_clear( info->dfa_ary_ );
	/* NFA����DFA�𐶐����� */
	info->initial_dfa_state_ = ZnkRgxDFAState_convertFromNFA( info->dfa_ary_,
			info->nfa_ary_, info->nfa_entry_, info->nfa_exit_ );
	return true;
}
void
ZnkRgxInfo_destroy( ZnkRgxInfo info )
{
	if( info ){
		ZnkRgxNFATable_destroy( info->nfa_ary_ );
		ZnkRgxDFAStateAry_destroy( info->dfa_ary_ );
		Znk_free( info );
	}
}



/**
 * DFA���g���ĕ�����begin�ɑ΂��ăp�^�[���}�b�`���s��.
 * from: �}�b�`���������̐擪�ʒu��Ԃ�.
 * to:   �}�b�`���������̒���̈ʒu��Ԃ�.
 * �p�^�[���}�b�`�Ɏ��s������from��NULL���Z�b�g����.
 */
void
ZnkRgxInfo_match( ZnkRgxInfo rgxinfo, const char* begin, size_t* from, size_t* to, ZnkStr rep_msg )
{
	const char* p;
	const char* q;
	const char* max_match;
	ZnkRgxDFAState state;
	size_t     toDFAS = Znk_NPOS;
	ZnkUChar64 uc = { 0 };

	/* ������begin�̐擪����A1���������炵�ăp�^�[���}�b�`���� */
	for( p = begin; *p != '\0'; ){
		/* DFA��������Ԃɂ��� */
		state = rgxinfo->initial_dfa_state_;
		max_match = NULL;
		q = p;
		
		/* DFA���J�ڂł������J��Ԃ� */
		while( state != NULL ){
			/* �����I����Ԃł���Ώꏊ���L�^���Ă����B
			   ���ʂƂ��āA�ł������p�^�[���Ƀ}�b�`�����������L�^����� */
			if( ZnkRgxDFAState_isAccepted( state ) ){
				max_match = q;
				if( rep_msg ){
					ZnkStr_addf2( rep_msg, "Accepted : pos=[%td]\n", q-p );
				}
			}
			
			/* ������begin�̖����܂Ń}�b�`���� */
			if( *q == '\0' ){
				break;
			}
			
			q = (char*)( rgxinfo->func_get_( &uc, (uint8_t*)q ) );

			/* DFA����ԑJ�ڂ����� */
			toDFAS = ZnkRgxDFAState_findNextState(state, uc);
			if( toDFAS == Znk_NPOS ){
				state = NULL;
			} else {
				state = ZnkRgxDFAStateAry_at( rgxinfo->dfa_ary_, toDFAS );
			}
		}
		
		/***
		 * �p�^�[���}�b�`���������Ă���΁Afrom��to�Ɉʒu���Z�b�g���ă��^�[������B
		 * �i�������A�󕶎���Ƀ}�b�`�����ꍇ�́A���^�[�����Ȃ��j
		 */
		if( max_match != NULL && max_match != p ){
			*from = p - begin;
			*to   = max_match - begin;
			return;
		}
		p = (char*)( rgxinfo->func_increment_( (uint8_t*)p ) );
	}
	
	/* �p�^�[���}�b�`�Ɏ��s�����̂�from��Znk_NPOS���Z�b�g */
	*from = Znk_NPOS;
}


/*****************************************************************************/

static const uint8_t*
incrementP_a( const uint8_t* p )
{
	return p+1;
}
static const uint8_t*
getUChar64_a( ZnkUChar64* uc, const uint8_t* p )
{
	uc->u64_ = *p++;
	return p;
}

static const uint8_t*
getUChar64_sjis( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p & 0x80 ){
		uc->u64_ =  *p++;
		uc->u64_ <<= 8;
		uc->u64_ |= *p++;
	} else {
		uc->u64_ =  *p++;
	}
	return p;
}
static const uint8_t*
incrementP_sjis( const uint8_t* p )
{
	if( *p & 0x80 ){
		p += 2;
	} else {
		++p;
	}
	return p;
}

Znk_INLINE bool UTF8_isMidByte   ( uint8_t b ){ return (bool)((b & 0xc0) == 0x80); } /* 2�o�C�g�ڈȍ~�͕K�� 10xxxxxx */
Znk_INLINE bool UTF8_isFirstByte2( uint8_t b ){ return (bool)((b & 0xe0) == 0xc0); } /* 2byte�����̊J�n 110xxxxx 10xxxxxx (����11bit) */
Znk_INLINE bool UTF8_isFirstByte3( uint8_t b ){ return (bool)((b & 0xf0) == 0xe0); } /* 3byte�����̊J�n 1110xxxx 10xxxxxx, ... (����16bit) */
Znk_INLINE bool UTF8_isFirstByte4( uint8_t b ){ return (bool)((b & 0xf8) == 0xf0); } /* 4byte�����̊J�n 11110xxx 10xxxxxx, ... (����21bit) */
Znk_INLINE int  UTF8_getFirstByteType( uint8_t b )
{
	if( b < 0x80 ){ return 1; } /* ASCII. */
	if( UTF8_isMidByte(    b ) ){ return 0; }
	if( UTF8_isFirstByte2( b ) ){ return 2; }
	if( UTF8_isFirstByte3( b ) ){ return 3; }
	if( UTF8_isFirstByte4( b ) ){ return 4; }
	return -1; /* Invalid UTF8 */
}

static const uint8_t*
getUChar64_utf8( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p == '\0' ){
		uc->u64_ = 0;
	} else {
		size_t i;
		const int delta = UTF8_getFirstByteType( *p );

		switch( delta ){
		case 1:
			/* ASCII */
			uc->u64_ =  *p++;
			break;
		case 2:
			uc->u64_ =  *p++;

			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			break;
		case 3:
			uc->u64_ =  *p++;

			/* 3�̕p�x�͍����Ǝv����̂ł��������͓W�J���Ă��� */
			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			break;
		case 4: case 5: case 6:
			uc->u64_ =  *p++;

			/* ����ȍ~�͕p�x�͏��Ȃ��Ǝv����̂�for���� */
			for( i=0; i<(size_t)delta-2; ++i ){
				if( *p == '\0' ){ break; }
				uc->u64_ <<= 8;
				uc->u64_ |= *p++;
			}
			break;
		default:
			/* maybe broken utf8 */
			uc->u64_ = '?';
			++p;
			break;
		}
	}
	return p;
}
static const uint8_t*
incrementP_utf8( const uint8_t* p )
{
	const int delta = UTF8_getFirstByteType( *p );
	if( delta > 0 ){
		p += delta;
	} else {
		/* broken utf8 : anyway increment +1 */
		++p;
	}
	return p;
}


static const uint8_t*
getUChar64_utf16( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p ){
		uc->s_[0] =  *p++;
		uc->s_[1] =  *p++;
	}
	return p;
}
static const uint8_t*
incrementP_utf16( const uint8_t* p )
{
	if( *p ){
		p += 2;
	}
	return p;
}

static struct ZnkRgxEncodeImpl st_encode_info_table[] = {
	{ "sjis",  getUChar64_sjis,  incrementP_sjis },
	{ "utf8",  getUChar64_utf8,  incrementP_utf8 },
	{ "utf16", getUChar64_utf16, incrementP_utf16 },
	{ "a",     getUChar64_a,     incrementP_a },
};

ZnkRgxEncode
ZnkRgxEncode_find( const char* id )
{
	ZnkRgxEncode enc = NULL;
	size_t i;
	for( i=0; i<Znk_NARY(st_encode_info_table); ++i ){
		enc = st_encode_info_table + i;
		if( ZnkS_eq( enc->id_, id ) ){
			break;
		}
	}
	return enc;
}

const char*
ZnkRgxEncode_id( const ZnkRgxEncode enc )
{
	return enc->id_;
}
