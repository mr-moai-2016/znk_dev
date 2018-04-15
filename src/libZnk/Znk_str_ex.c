#include "Znk_str_ex.h"
#include "Znk_str_ary.h"
#include "Znk_s_base.h"
#include "Znk_mem_find.h"
#include "Znk_def_util.h"

#define DECIDE_STRLENG( str_leng, str ) Znk_setStrLen_ifNPos( &str_leng, str )


/*****************************************************************************/
/* Adapter */

Znk_DECLARE_HANDLE( StrListHandle );
typedef size_t      (*AdapterSizeFunc_T)(    StrListHandle list );
typedef void        (*AdapterReserveFunc_T)( StrListHandle list, size_t size );
typedef void        (*AdapterPushBkFunc_T)(  StrListHandle list, const char* data, size_t data_leng );
typedef const char* (*AdapterAtFunc_T)(      StrListHandle list, size_t idx );

struct AdapterImpl {
	AdapterSizeFunc_T    size_;
	AdapterReserveFunc_T reserve_;
	AdapterPushBkFunc_T  push_bk_;
	AdapterAtFunc_T      at_;
};

typedef struct AdapterImpl* Adapter;

static size_t      adp__VStr_size(    StrListHandle list )
{ return ZnkStrAry_size(      (ZnkStrAry)list ); }
static void        adp__VStr_reserve( StrListHandle list, size_t size )
{        ZnkStrAry_reserve(   (ZnkStrAry)list, size ); }
static void        adp__VStr_push_bk( StrListHandle list, const char* data, size_t data_leng )
{        ZnkStrAry_push_bk_cstr(   (ZnkStrAry)list, data, data_leng ); }
static const char* adp__VStr_at(      StrListHandle list, size_t idx )
{ return ZnkStrAry_at_cstr( (ZnkStrAry)list, idx ); }

static struct AdapterImpl st_adapter_vstr = {
	adp__VStr_size,
	adp__VStr_reserve,
	adp__VStr_push_bk,
	adp__VStr_at,
};

/* endof Adapter */
/*****************************************************************************/

static void
Adapter_addSplitC( Adapter adp, StrListHandle ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size )
{
	size_t token_size;
	size_t begin = 0;
	DECIDE_STRLENG( str_leng, str );

	adp->reserve_( ans_list, adp->size_(ans_list) + expect_size );

	while( true ){
		if( begin >= str_leng ){
			/***
			 * size == 0 �܂��� delimit_c �̈ʒu�����x size-1 (str�̍ŏI����) �ɂ���ꍇ�̂�
			 * �����ɓ��B����.
			 * count_void_str �� true �Ȃ�΁A�󕶎���������.
			 */
			if( count_void_str ){ adp->push_bk_( ans_list, "", 0 ); }
			break;
		}

		token_size = ZnkMem_lfind_8( (uint8_t*)str+begin, str_leng-begin, delimit_c, 1 );
		if(	token_size == Znk_NPOS ){
			/***
			 * not found 
			 * �c��S���� ans_list �ɒǉ���break����. str_leng-begin > 0 �͕K������.
			 */
			adp->push_bk_( ans_list, str + begin, str_leng-begin );
			break;
		}

		/***
		 * delimit_c �̒��O�ɂ���g�[�N���͈̔͊m��. ans_list�֒ǉ�.
		 */
		if( token_size > 0 ){
			adp->push_bk_( ans_list, str + begin, token_size );
		} else {
			/***
			 * token_size == 0 �̂Ƃ� count_void_str �� true �Ȃ�΁A�󕶎���������.
			 */
			if( count_void_str ){ adp->push_bk_( ans_list, "", 0 ); }
		}
		begin += (token_size + 1); /* �c�� */
	}
}

void
ZnkStrEx_addSplitC( ZnkStrAry ans_list,
		const char* str, size_t str_leng,
		char delimit_c, const bool count_void_str, size_t expect_size )
{
	Adapter_addSplitC( &st_adapter_vstr,
			(StrListHandle)ans_list, str, str_leng, delimit_c, count_void_str, expect_size );
}


/***
 * str.size �� 2�ȏ�Achset.size �� 2�ȏ�A
 * ���G�ȃP�[�X�݂̂�O��Ƃ��ď�������.
 */
Znk_INLINE void
Adapter_splitCSet_core( Adapter adp, StrListHandle ans_list,
		const char* str, size_t str_leng, const char* chset, size_t chset_leng,
		size_t expect_size )
{
	size_t idx;
	size_t begin = 0;

	adp->reserve_( ans_list, adp->size_(ans_list) + expect_size );

	while( true ){
		/***
		 * chset ��ǂݔ�΂�����.
		 */
		begin = ZnkS_lfind_one_not_of( str, begin, str_leng, chset, chset_leng );
		if( begin >= str_leng || begin == Znk_NPOS ){ return; } /* str�̏I�[�ɒB�����ꍇ */

		/***
		 * ����chset�������ꏊ(�E���ׂ��g�[�N���̍ŏI�����ʒu+1)�����߂�.
		 * str[begin] ���̂�chset�łȂ����Ƃ́A���O��ZnkS_lfind_one_not_of�ɂ��
		 * �ۏ؂���Ă���.
		 */
		idx = ZnkS_lfind_one_of( str, begin, str_leng, chset, chset_leng );
		if(	idx >= str_leng || idx == Znk_NPOS ){
			/*
			 * ���ǍŌ�܂�WS�����͌�����Ȃ������ꍇ.
			 * begin ���c��S������̃g�[�N���ƂȂ�.
			 **/
			if( begin < str_leng ){ adp->push_bk_( ans_list, str + begin, str_leng - begin ); }
			break;
		}

		/***
		 * �g�[�N���͈̔͊m��. ans_list�֒ǉ�.
		 *
		 * str[begin] ���̂�chset�łȂ����Ƃ��ۏ؂���Ă���
		 * ���� idx�� ZnkS_lfind_one_of �̌��ʂł��邽�߁A�K�� idx > begin �����藧�͂�.
		 * �܂� token_size = idx - begin > 0
		 */
		adp->push_bk_( ans_list, str + begin, idx - begin );

		begin = idx + 1; /* �c�� */
	}
}

static void
Adapter_addSplitCSet( Adapter adp, StrListHandle ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size )
{
	DECIDE_STRLENG( str_leng, str );
	/***
	 * str ����̏ꍇ
	 */
	if( str_leng == 0 ){ return; }

	DECIDE_STRLENG( chset_leng, chset );
	/***
	 * chset ����̏ꍇ
	 */
	if( chset_leng == 0 ){
		/***
		 * �����ł͎w�肳�ꂽ�͈͂���̉�Ƃ��ĒP�Ƀv�b�V��������̂Ƃ���.
		 * ���̂Ƃ��Astr ��S�� 1 �������o���o���ɕ�������Ƃ������߂��ł��邪�A
		 * string ���́A1 �������o���o���ɂ������̂̏W���ł���̂�
		 * ���̂悤�ȖړI�ŏ������s�����Ǝ��̂ɈӖ����Ȃ�.
		 */
		adp->push_bk_( ans_list, str, str_leng );
		return;
	}

	/***
	 * str ��1�����̏ꍇ
	 */
	if( str_leng == 1 ){
		/* �B��̕��� str[0] �� chset �łȂ���΂����P�Ƀv�b�V������΂悢.
		 * chset �Ȃ�΂Ȃɂ����Ȃ�. */
		if( ZnkMem_lfind_8( (uint8_t*)chset, chset_leng, (uint8_t)str[0], 1 ) == Znk_NPOS ){
			adp->push_bk_( ans_list, str, 1 );
		}
		return;
	}

	if( chset_leng == 1 ){
	 	/* ���̂Ƃ��͗v����� splitC�����ł���. */
		Adapter_addSplitC( adp, ans_list,
				str, str_leng, chset[ 0 ], false, expect_size );
		return;
	} else {
		/***
		 * chset �̂������ۂɎg�p�������̂�(str����)�������đI�т����A
		 * �V���� chset_modify �𓾂�.
		 */
		char   chset_modify[ 256 ]; /* 1byte�R�[�h�̏W���ł��邩��NULL�����������΍ő�ł�255�ʂ� */
		size_t chset_modify_count = 0;
		size_t idx;

		Znk_CLAMP_MAX( chset_leng, 256 ); /* 256�ȏ�ł������ꍇ�͖��炩�Ɏw�肪�ςł���̂őł��؂� */
		for(idx=0; idx<chset_leng; ++idx){
			if( ZnkMem_lfind_8( (uint8_t*)str, str_leng, (uint8_t)chset[idx], 1 ) != Znk_NPOS ){
				/***
				 * �����ŏd���̃`�F�b�N�͂Ȃ���Ȃ�.
				 */
				chset_modify[ chset_modify_count ] = chset[ idx ];
				++chset_modify_count;
			}
		}

		chset_leng = chset_modify_count;
		chset_modify[ chset_modify_count ] = '\0';
	
		if( chset_leng == 0 ){
			/***
			 * chset_modify ����̏ꍇ. ���炩�� str �͕��f����Ȃ�.
			 */
			adp->push_bk_( ans_list, str, str_leng );
		} else if( chset_leng == 1 ){
			/***
			 * chset_modify ��1�����̏ꍇ. ���̂Ƃ��͗v����� splitC�����ł���.
			 */
			Adapter_addSplitC( adp, ans_list, str, str_leng, chset_modify[ 0 ], false, expect_size );
		} else {
			/***
			 * �ł����G�ȃP�[�X. str_leng >= 2 ���� chset_modify leng >= 2 �̏ꍇ.
			 */
			Adapter_splitCSet_core( adp, ans_list, str, str_leng, chset_modify, chset_leng, expect_size );
		}
	}
}

void
ZnkStrEx_addSplitCSet( ZnkStrAry ans_list,
		const char* str,   size_t str_leng,
		const char* chset, size_t chset_leng,
		size_t expect_size )
{
	Adapter_addSplitCSet( &st_adapter_vstr,
			(StrListHandle)ans_list, str, str_leng, chset, chset_leng, expect_size );
}


static void
Adapter_addSplitStr( Adapter adp, StrListHandle ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size )
{
	if( delimiter_leng == 1 ){
		Adapter_addSplitC( adp, ans_vstr, str, str_leng,
				delimiter[0], count_void_str, expect_size );
		return;
	} else {
		size_t token_size;
		size_t begin = 0;

		DECIDE_STRLENG( str_leng, str );
		DECIDE_STRLENG( delimiter_leng, delimiter );

		adp->reserve_( ans_vstr, expect_size );
	
		while( true ){

			if( begin >= str_leng ){
				if( count_void_str ){ adp->push_bk_( ans_vstr, "", 0 ); } /* ���ǉ� */
				break;
			}
	
			/***
			 * strstr�͎g���Ȃ�(�����ł�NULL�I�[���Ă���Ƃ͌���Ȃ�).
			 * begin < str_leng ���ۏ؂���Ă��邽�߁Astr_leng-begin �� 1 �ȏ�ƂȂ�.
			 * �܂��A�����Ŏ擾�����̂� str+begin ����̑��Έʒu�ł��邽�߁A
			 * ���Ȃ킿�g�[�N���̒������̂��̂ł���( Znk_NPOS �ł���ꍇ������ ).
			 */
			token_size = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
					(uint8_t*)delimiter, delimiter_leng, 1 );
			if(	token_size == Znk_NPOS ){
				/***
				 * begin ���c��S������̃g�[�N���Ƃ��Ēǉ�����.
				 */
				adp->push_bk_( ans_vstr, str+begin, str_leng-begin );
				break;
			}
	
			/***
			 * [ begin, begin+token_size )�̕�������̃g�[�N���Ƃ��Ēǉ�.
			 *
			 * begin�������Ȃ�delimiter�̊J�n�������w�������Ă���\���͂���.
			 * �Ⴆ�΁Adelimiter="ABC" �Ƃ��āAstr="ABD..." �� str="...ABCABC..." �ƂȂ��Ă���ꍇ�ł���.
			 * �]���āAtoken_size �� 0 �ɂȂ�\��������.
			 */
			if( token_size == 0 ){
				if( count_void_str ){ adp->push_bk_( ans_vstr, "", 0 ); } /* ���ǉ� */
			} else {
				adp->push_bk_( ans_vstr, str+begin, token_size );
			}
	
			/***
			 * �c��
			 *
			 * ���̎��_�ŁAbegin+token_size��delimiter�̊J�n�����̈ʒu�ł���.
			 * ����āA���̐V����begin�́Abegin+token_size+delimiter_leng�ƂȂ�.
			 *
			 * �V����begin�������Ȃ�delimiter�̊J�n�������w�������\���͂���.
			 * �Ⴆ�΁Adelimiter="ABC", str="...ABCABC..." �ƂȂ��Ă���ꍇ�ł���.
			 *
			 * ���Adelimiter�����xstr�̍ŏI�����ɂ���ꍇ(str="...ABC"�ƂȂ��Ă���ꍇ)
			 * ���̍X�V�ɂ���āAbegin �� str_leng(>0) �ɓ������Ȃ�.
			 * �t�� begin �� str_leng(>0) �ɓ������Ȃ�̂͂��̏ꍇ�������đ��ɂȂ�.
			 *
			 * ���L�̍X�V��A���̃��[�v�̎���n�߂ɂ����āAbegin >= str_leng �Ƃ�����r������
			 * ���ꂪ�^�ƂȂ�Ȃ�΁A����͗v����� begin == str_leng �ƒf�肵�Ă悭�A
			 * delimiter�����xstr�̍ŏI�����ɂ���ꍇ�Ɣ��f�ł���.
			 *
			 * count_void_str �� true�̏ꍇ�́A���̌��ɃT�C�Y0�̃g�[�N��������ƍl����K�v������.
			 */
			begin += ( token_size + delimiter_leng );
		}
	}
}

void
ZnkStrEx_addSplitStr( ZnkStrAry ans_vstr, const char* str, size_t str_leng,
		const char* delimiter, size_t delimiter_leng,
		const bool count_void_str, size_t expect_size )
{
	Adapter_addSplitStr( &st_adapter_vstr, (StrListHandle)ans_vstr,
			str, str_leng,
			delimiter, delimiter_leng,
			count_void_str, expect_size );
}

static void
Adapter_addJoin( Adapter adp,
		ZnkStr ans, const StrListHandle str_list,
		size_t begin, size_t end,
		const char* connector, size_t connector_leng, size_t expect_elem_leng )
{
	const size_t num = adp->size_( str_list );
	end = Znk_MIN( num, end );
	if( end > begin ){
		size_t idx;

		DECIDE_STRLENG( connector_leng, connector );

		/* �����܂��ȗ\�z�\��T�C�Y */
		ZnkBfr_reserve( ans, ZnkBfr_size(ans) + ( expect_elem_leng + connector_leng ) * (end-begin) );
	
		/***
		 * ZnkStr_add, ���邢�� ZnkStr_append�ŏ\�������ł���.
		 * �������g�킸�ɍH�v���Â炵���Ƃ��Ă��قƂ�Ǎ��͂Ȃ�.
		 */
		for( idx=begin; idx<end-1; ++idx ){
			ZnkStr_add( ans, adp->at_( str_list, idx ) );
			ZnkStr_append( ans, connector, connector_leng );
		}
		ZnkStr_add( ans, adp->at_( str_list, end-1 ) );
	}
}

void
ZnkStrEx_addJoin( ZnkStr ans, const ZnkStrAry str_list,
		size_t begin, size_t end,
		const char* connector, size_t connector_leng, size_t expect_elem_leng )
{
	Adapter_addJoin( &st_adapter_vstr, ans, (StrListHandle)str_list,
			begin, end,
			connector, connector_leng, expect_elem_leng );
}

/***
 * ���_�Ƃ��ẮA�����Ȃ�ꍇ�ł���A���ɃV�[�N���Ă������@���ő��ł���.
 *
 * begin_quot �� end_quot �̗������񂪑S�������P�[�X�̏ꍇ�A
 * �ȉ��̂悤�ȓ���ȃA���S���Y���ŏ�������̂͂ǂ����ƍl���邩������Ȃ�.
 * ���Ȃ킿�A�Ⴆ�� str=[ " abc"  "de" "" "fgh" "" ] �̏ꍇ�ɂ܂�
 * ZnkStrEx_addSplitStr(C) �Ȃǂ��ĂсA�ꎞ�R���e�i�ł��� splited_strs ��
 * splited_strs={[ ],[ abc],[  ],[de],[ ],[],[ ],[fgh],[ ],[],[ ]} �Ƃ������ʂ𓾂�.
 * ���̌�Asplited_strs �̊�Ԗڂ݂̂��Ƃ���{[ abc],[de],[],[fgh],[]}�����ʂƂ���
 * �i�[����Ƃ������̂ł���.
 *
 * �������Ȃ��炱�̂悤�ȕ��@�͂ǂ̂悤�ȏꍇ�ł���ᑬ�Ȃ̂ł����ł͋p���Ƃ���.
 * �܂�splited_strs���R���e�i�Ƃ��Ċm�ۉ�����鏈���ɃI�[�o�[�w�b�h��������.
 * ( �����ȃV�[�N�ł���� �g�p����̂�find���������ł���A���̂悤�ȍ\�z/�j���̏�����
 * �K�v�Ȃ� ). ����ɂ���Ƀv�b�V�����鏈����(�{���K�v�Ƃ��鏈���̓�{�̉�)�s������A
 * �����ans�Ɋ�Ԗڂ݂̂��v�b�V�����Ȃ���΂Ȃ�Ȃ�. ����͏����ɃV�[�N���Ă���
 * �����Ɣ�ׂĖ��炩�ɖ��ʂł���.
 *
 * begin_quot �� end_quot ���قȂ�ꍇ�̃P�[�X�ɂ��ẮA����������L�̕��@�͎g�p�ł��Ȃ�.
 * �֋X��Abegin_quot = "<" �Aend_quot = ">" �Ƃ���.
 *
 * ��1. str=[ < abc>  <de> <> <fgh> <> ] �̏ꍇ�A
 *   ���� begin_quot �ɂ��Split�������s�����ꍇ�A
 *   splited_strs={[ ],[ abc>  ],[de> ],[> ],[fgh> ],[> ]}
 *   �ƂȂ�.
 *   �����̂Q�Ԗڈȍ~���Ƃ�A���ꂼ��� end_quot �܂Œ��o���� ans_list �� push ���Ă�����
 *   ans_list = {[ abc],[de],[],[fgh],[]}�ƂȂ�.
 *   ���̏ꍇ�͂��܂�����.
 *
 * ��2. str=[< abc<>] �̏ꍇ�A
 *   ���̏ꍇ�A��L�̃A���S���Y���ł͂��܂������Ȃ�.
 *   ans_list = {[ abc<]} �ƂȂ��Ăق����Ƃ��낾���A
 *   ��L�̃A���S���Y���ł� ans_list = {[ abc], []} �ƂȂ��Ă��܂�.
 */
static void
Adapter_addQuotedTokens( Adapter adp, StrListHandle ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size )
{
	size_t tmp_size;
	size_t pos = 0;

	adp->reserve_( ans_vstr, adp->size_(ans_vstr) + expect_size );

	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( begin_quot_leng, begin_quot );
	DECIDE_STRLENG( end_quot_leng,   end_quot );

	while( pos < str_leng ){
		/***
		 * ���̎��_�ł� pos �� �N�H�[�g�O���w���Ă���͂��ł��邩��A
		 * ���� begin_quot ��T��.
		 * begin_quot�̍ŏ��̕������w���Ă���\�������邪�A���̏ꍇ���N�H�[�g�O�Ƃ݂Ȃ�.
		 *
		 * begin_quot_leng �� 1 �̏ꍇ�́AZnkMem_lfind_data_BF ���� ZnkMem_lfind_8 �̕���
		 * �킸���ɖ��ʂ����Ȃ�. ������ZnkMem_lfind_data_BF�ł� leng �� 1 �̏ꍇ�͂�������
		 * ZnkMem_lfind_8 ���Ăяo�����߁A���ۏケ�̍��͂قƂ�ǂȂ�. ���ɂ��̌Ăяo����
		 * leng �ɉ����Ċ֐�table�ɕ������Ƃ��Ă��A���̒��ۉ��̂��߂̃��b�p�[�֐��̌Ăяo��
		 * �̃I�[�o�[�w�b�h���������߁AZnkMem_lfind_data_BF�𒼐ڌĂяo���̂Ɠ�����
		 * ���邢��(leng>=2�̏ꍇ)����������ʂȌĂяo�����������ƂɂȂ�.
		 *
		 * ���̏ꍇ�A������ tmp_size �� str+pos����̑��Έʒu�ł��邱�Ƃɒ��ӂ���.
		 */
		tmp_size = ZnkMem_lfind_data_BF( (uint8_t*)str+pos, str_leng-pos,
				(uint8_t*)begin_quot, begin_quot_leng, 1 );
		if( tmp_size == Znk_NPOS ){
			/***
			 * begin_quot �����͂⑶�݂��Ȃ��ꍇ�A�I������.
			 */
			return;
		}
		/***
		 * �N�H�[�g����
		 * ���̌���ˑR�Ƃ��� pos <= str_leng�͕ۂ����͂��ł���.
		 */
		pos += ( tmp_size + begin_quot_leng );

		/***
		 * ���̎��_�ł� pos �� �N�H�[�g�����w���Ă���͂��ł��邩��A
		 * ���� end_quot ��T��.
		 * end_quot�̍ŏ��̕������w���Ă���\�������邪�A���̏ꍇ���N�H�[�g���Ƃ݂Ȃ�.
		 *
		 * end_quot �����݂��Ȃ��ꍇ�Astr���s���ȕ��@�ƂȂ��Ă���ƍl�����邪�A
		 * �ꉞ�Ō�܂ŏE���ďI������.
		 *
		 * ���̏ꍇ�A������ tmp_size �� str+pos����̑��Έʒu�ł��邪�A
		 * ����͓����ɁA�N�H�[�g�ň͂܂ꂽ���g�̃g�[�N���T�C�Y���̂��̂ł��邱�Ƃɒ��ӂ���.
		 */
		tmp_size = ZnkMem_lfind_data_BF( (uint8_t*)str+pos, str_leng-pos,
				(uint8_t*)end_quot, end_quot_leng, 1 );
		if( tmp_size == Znk_NPOS ){
			/***
			 * end_quot ���݂���Ȃ���ԂŃV�[�N���Ō�܂œ��B.
			 * �Ƃ肠�����Ō�܂ł̕����� push ���ďI��.
			 */
			adp->push_bk_( ans_vstr, str+pos, str_leng-pos );
			return; /* �����ł������ɏI������ */
		} else {
			/***
			 * �ʏ�̃g�[�N���ł���ꍇ.
			 */
			adp->push_bk_( ans_vstr, str+pos, tmp_size );
		}

		/***
		 * �N�H�[�g�O��
		 * ���̌���ˑR�Ƃ��� pos <= str_leng�͕ۂ����͂��ł���.
		 * ���� pos == str_leng�Ȃ�Ύ��̃��[�v�̎n�߂ŏI������`�ɂȂ�.
		 */
		pos += ( tmp_size + end_quot_leng );
	}
}
void
ZnkStrEx_addQuotedTokens( ZnkStrAry ans_vstr,
		const char* str, size_t str_leng,
		const char* begin_quot, size_t begin_quot_leng,
		const char* end_quot,   size_t end_quot_leng,
		size_t expect_size )
{
	Adapter_addQuotedTokens( &st_adapter_vstr, (StrListHandle) ans_vstr,
			str, str_leng,
			begin_quot, begin_quot_leng,
			end_quot,   end_quot_leng,
			expect_size );
}

size_t
ZnkStrEx_getFront_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );

	/***
	 * ���ꂪ�������Ȃ��ꍇ�AFront �͏�ɋ�ł���Ƃ݂Ȃ�.
	 * (���Ȃ킿 begin ��Ԃ�)
	 */
	if( begin < str_leng && ptn_leng ){
		size_t end = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
			(uint8_t*)ptn, ptn_leng, 1 );
		if( end == Znk_NPOS ){
			/***
			 * not found.
			 * str[begin] �ȍ~���ׂĂ� Front �Ƃ݂Ȃ�.
			 */
			if( ans ){ ZnkStr_append( ans, str+begin, str_leng-begin ); }
			return Znk_NPOS;
		} else {
			end += begin; /* ���Έʒu�����Έʒu�֒��� */
			if( ans ){ ZnkStr_append( ans, str+begin, end-begin ); }
			return end;
		}
	}
	return begin;
}

size_t
ZnkStrEx_getBack_byPattern( const char* str, size_t str_leng,
		size_t begin, const char* ptn, size_t ptn_leng,
		ZnkStr ans )
{
	DECIDE_STRLENG( str_leng, str );
	DECIDE_STRLENG( ptn_leng, ptn );

	/***
	 * ���̏ꍇ�A���� [ begin, str_leng ) �͈̔͂� Back�Ƃ݂Ȃ�.
	 */
	if( ptn_leng == 0 ){
		if( ans ){ ZnkStr_append( ans, str+begin, str_leng-begin ); }
		return begin;
	}

	/***
	 * ���ꂪ�������Ȃ��ꍇ�ABack �͏�ɋ�ł���Ƃ݂Ȃ�.
	 * (���Ȃ킿 Znk_NPOS ��Ԃ�)
	 */
	if( begin < str_leng ){
		size_t end = ZnkMem_lfind_data_BF( (uint8_t*)str+begin, str_leng-begin,
			(uint8_t*)ptn, ptn_leng, 1 );
		if( end == Znk_NPOS ){
			/***
			 * not found.
			 * Back �͋�ł���Ƃ݂Ȃ�.
			 */
			return Znk_NPOS;
		} else {
			end += begin; /* ���Έʒu�����Έʒu�֒��� */
			end += ptn_leng; /* end �� ptn �̊J�n�ʒu�ł��������߁A���̉��Z���ʂ� str_leng�ȉ��ł��� */
			if( ans ){ ZnkStr_append( ans, str+end, str_leng-end ); }
			return end;
		}
	}
	return Znk_NPOS;
}


size_t
ZnkStrEx_getKeyAndValEx( const char* str, size_t begin, size_t end,
		size_t* ans_key_begin, size_t* ans_key_end,
		size_t* ans_val_begin, size_t* ans_val_end,
		const char* delimiter,  size_t delimiter_leng,
		const char* skip_chset, size_t skip_chset_leng,
		ZnkStr ans_key, ZnkStr ans_val )
{
	size_t key_begin, key_end, val_begin, val_end;
	const size_t delimiter_pos = ZnkS_find_key_and_val( str, begin, end,
			&key_begin, &key_end,
			&val_begin, &val_end,
			delimiter,  delimiter_leng,
			skip_chset, skip_chset_leng );

	if( ans_key_begin ){ *ans_key_begin = key_begin; }
	if( ans_key_end   ){ *ans_key_end   = key_end; }
	if( ans_val_begin ){ *ans_val_begin = val_begin; }
	if( ans_val_end   ){ *ans_val_end   = val_end; }

	if( ans_key && key_begin != Znk_NPOS ){
		const size_t token_size = ( key_end == Znk_NPOS ) ?  end - key_begin : key_end - key_begin;
		ZnkStr_append( ans_key, str + key_begin, token_size );
	}
	if( ans_val && val_begin != Znk_NPOS ){
		const size_t token_size = ( val_end == Znk_NPOS ) ?  end - val_begin : val_end - val_begin;
		ZnkStr_append( ans_val, str + val_begin, token_size );
	}
	return delimiter_pos;
}


bool
ZnkStrEx_removeFrontCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng )
{
	DECIDE_STRLENG( skip_chset_leng, skip_chset );

	if( skip_chset_leng != 0 && !ZnkStr_empty(str) ){
		size_t pos = ZnkS_lfind_one_not_of( ZnkStr_cstr(str), 0, ZnkStr_leng(str), skip_chset, skip_chset_leng );
		if( pos == 0 ){
			return false; /* remove�̕K�v���Ȃ�. */
		} else if( pos == Znk_NPOS ){
			/* str �̂��ׂĂ̕����� skip_chset ����Ȃ�. */
			ZnkStr_clear( str );
			return true;
		} else {
			/* [0, pos) �܂ł��폜. */
			ZnkStr_cut_front( str, pos );
			return true;
		}
	}
	return false; /* remove�̕K�v���Ȃ�. */
}
bool
ZnkStrEx_removeBackCSet( ZnkStr str, const char* skip_chset, size_t skip_chset_leng )
{
	DECIDE_STRLENG( skip_chset_leng, skip_chset );

	if( skip_chset_leng != 0 && !ZnkStr_empty(str) ){
		const size_t str_leng = ZnkStr_leng( str );
		size_t pos = ZnkS_rfind_one_not_of( ZnkStr_cstr(str), 0, str_leng, skip_chset, skip_chset_leng );
		if( pos == str_leng-1 ){
			return false; /* remove�̕K�v���Ȃ�. */
		} else if( pos == Znk_NPOS ){
			/* str �̂��ׂĂ̕����� skip_chset ����Ȃ�. */
			ZnkStr_clear( str );
			return true;
		} else {
	 		/* [pos+1, ...) �܂ł��폜. */
			ZnkStr_releng( str, pos+1 );
			return true;
		}
	}
	return false; /* remove�̕K�v���Ȃ�. */
}


bool 
ZnkStrEx_chompStr( ZnkStr str, size_t pos, const char* ptn, size_t ptn_leng )
{
	const size_t leng = ZnkStr_leng( str );
	DECIDE_STRLENG( ptn_leng, ptn );
	if( ptn_leng == 0 || ptn_leng > leng ){
		/***
		 * ptn_leng == 0 �̏ꍇ�̓}�b�`����Ƃ݂͂Ȃ����A���ǉ������ʂ͂Ȃ�.
		 * ptn_leng > leng �̏ꍇ�͖��炩�Ƀ}�b�`���Ȃ�.
		 */
		return false;
	}
	if( ZnkStr_isContain( str, pos, ptn, ptn_leng ) ){
		if( pos == Znk_NPOS || pos + ptn_leng == leng ){
			ZnkStr_cut_back( str, ptn_leng );
		} else {
			ZnkStr_erase( str, pos, ptn_leng ); 
		}
		return true;
	}
	return false;
}


void
ZnkStrEx_addRepeatC( ZnkStr ans, char c, size_t num )
{
	/***
	 * ���̊֐��� std::string �� append( num, c ) �����o�֐��ɑΉ�������̂ł���.
	 */
	const size_t old_leng = ZnkStr_leng(ans);
	uint8_t* data;
	ZnkStr_releng( ans, old_leng + num );
	data = ZnkBfr_data( ans );
	Znk_memset( data + old_leng, c, num );
}


/***
 * �^����ꂽ uint32_t �^���� num �� bit�ɕ������čl����.
 * ���̏����������琔���čŌ��bit�������Ă���ʒu�܂ł̌����܂ł�en_bit��
 * �z��Ƃ��ẴT�C�Y�Ƃ���
 *
 * �� �ȒP�̂���8bit�ōl����.
 *   00000100 �̂Ƃ� bit = 100    (en_bit �̓T�C�Y3��uint8_t�z��)
 *   00101101 �̂Ƃ� bit = 101101 (en_bit �̓T�C�Y6��uint8_t�z��)
 *   00000000 �̂Ƃ� bit = ��     (en_bit �̓T�C�Y0��uint8_t�z��)
 */
Znk_INLINE size_t
getSizeTBits( uint8_t* en_bit, size_t val )
{
	static const size_t bit_num = sizeof(size_t) * 8;
	size_t enable_num;
	size_t idx, ridx;
	for( idx=0; idx<bit_num; ++idx ){
		ridx = bit_num - 1 - idx;
		if( val & (size_t)(0x1 << ridx) ){ break; }
	}
	enable_num = bit_num - idx;
	for( idx=0; idx<enable_num; ++idx ){
		en_bit[ idx ] = (uint8_t)( ( val & (size_t)(0x1 << idx) ) != 0 );
	}
	return enable_num;
}


void
ZnkStrEx_addEmbededStr( ZnkStr ans,
		char c, size_t num,
		const char* emb_str, size_t emb_str_leng,
		char positioning_mode )
{
	DECIDE_STRLENG( emb_str_leng, emb_str );

	if( emb_str_leng >= num ){
		/* add "emb_str" */
		ZnkStr_append( ans, emb_str, emb_str_leng );
	} else {
		const size_t repeat_num = num - emb_str_leng;
		switch( positioning_mode ){
		case 'L':
			/* add "emb_str   " */
			ZnkStr_append( ans, emb_str, emb_str_leng );
			ZnkStrEx_addRepeatC( ans, c, repeat_num );
			break;
		case 'R':
			/* add "   emb_str" */
			ZnkStrEx_addRepeatC( ans, c, repeat_num );
			ZnkStr_append( ans, emb_str, emb_str_leng );
			break;
		case 'C':
			/* add " emb_str  " */
			ZnkStrEx_addRepeatC( ans, c, repeat_num/2 );
			ZnkStr_append( ans, emb_str, emb_str_leng );
			ZnkStrEx_addRepeatC( ans, c, repeat_num/2 );
			if( repeat_num % 2 ){ ZnkStr_add_c( ans, c ); }
			break;
		default :
			/* none */
			break;
		}
	}
	return;
}


size_t
ZnkStrEx_replace_BF( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next )
{
	if( seek_depth ){
		size_t count=0;
		size_t pos_from_begin;
		size_t str_leng = ZnkStr_leng(str);

		DECIDE_STRLENG( old_ptn_leng, old_ptn );
		DECIDE_STRLENG( new_ptn_leng, new_ptn );
		if( delta_to_next == Znk_NPOS ){ delta_to_next = new_ptn_leng; }

		while( begin < str_leng ){
			pos_from_begin = ZnkMem_lfind_data_BF( (uint8_t*)ZnkStr_cstr(str)+begin, str_leng-begin,
					(uint8_t*)old_ptn, old_ptn_leng, 1 );
			if( pos_from_begin == Znk_NPOS ){ break; }
			begin += pos_from_begin;
			++count;

			ZnkBfr_replace( str, begin, old_ptn_leng, (uint8_t*)new_ptn, new_ptn_leng );
			begin += delta_to_next;

			if( count == seek_depth ){ break; }
			str_leng = ZnkStr_leng(str);
		}
		return count;
	}
	return 0;
}

size_t
ZnkStrEx_replace_BMS( ZnkStr str, size_t begin,
		const char* old_ptn, size_t old_ptn_leng, const size_t* old_ptn_occ_table,
		const char* new_ptn, size_t new_ptn_leng,
		size_t seek_depth, size_t delta_to_next )
{
	if( seek_depth ){
		size_t count=0;
		size_t pos_from_begin;
		size_t str_leng = ZnkStr_leng(str);

		DECIDE_STRLENG( old_ptn_leng, old_ptn );
		DECIDE_STRLENG( new_ptn_leng, new_ptn );
		if( delta_to_next == Znk_NPOS ){ delta_to_next = new_ptn_leng; }

		while( begin < str_leng ){
			pos_from_begin = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+begin, str_leng-begin,
					(uint8_t*)old_ptn, old_ptn_leng, 1, old_ptn_occ_table );
			if( pos_from_begin == Znk_NPOS ){ break; }
			begin += pos_from_begin;
			++count;

			ZnkBfr_replace( str, begin, old_ptn_leng, (uint8_t*)new_ptn, new_ptn_leng );

			begin += delta_to_next;

			if( count == seek_depth ){ break; }
			str_leng = ZnkStr_leng(str);
		}
		return count;
	}
	return 0;
}

