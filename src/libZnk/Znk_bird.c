#include "Znk_bird.h"
#include "Znk_mem_find.h"
#include "Znk_stdc.h"
#include "Znk_varp_ary.h"
#include "Znk_bms_find.h"

/***
 * ZnkBird (Basic Intrinsic Replacement Directive)
 */

struct ZnkBirdImpl {
	ZnkVarpAry         varp_ary_;
	ZnkBmsFinder       key_finder_;
	ZnkBirdProcessFunc func_;
	void*              func_arg_;
	ZnkStr             wk_key_;
	ZnkStr             wk_dst_;
};

ZnkBird
ZnkBird_create( const char* key_beginer, const char* key_ender )
{
	ZnkBird info = Znk_malloc( sizeof( struct ZnkBirdImpl ) );
	info->varp_ary_   = ZnkVarpAry_create( true );
	info->key_finder_ = ZnkBmsFinder_create( key_beginer, key_ender );
	info->func_       = NULL;
	info->func_arg_   = NULL;
	info->wk_key_     = ZnkStr_new( "" );
	info->wk_dst_     = ZnkStr_new( "" );
	return info;
}
void
ZnkBird_destroy( ZnkBird info )
{
	if( info ){
		ZnkBmsFinder_destroy( info->key_finder_ );
		ZnkVarpAry_destroy( info->varp_ary_ );
		ZnkStr_delete( info->wk_key_ );
		ZnkStr_delete( info->wk_dst_ );
		Znk_free( info );
	}
}
void
ZnkBird_regist( ZnkBird info, const char* key, const char* val )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	if( var ){
		ZnkVar_set_val_Str( var, val, Znk_strlen(val) );
	} else {
		ZnkVarp new_var = ZnkVarp_create( key, "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( new_var, val, Znk_strlen(val) );
		ZnkVarpAry_push_bk( info->varp_ary_, new_var );
	}
}

void
ZnkBird_setFunc( ZnkBird info, ZnkBirdProcessFunc func, void* func_arg )
{
	info->func_     = func;
	info->func_arg_ = func_arg;
}

const char*
ZnkBird_at( ZnkBird info, const char* key )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	return var ? ZnkVar_cstr( var ) : NULL;
}
const bool
ZnkBird_exist( ZnkBird info, const char* key )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	return (bool)( var != NULL );
}
void
ZnkBird_clear( ZnkBird info )
{
	ZnkVarpAry_clear( info->varp_ary_ );
}

/***
 * #[key]# Directive�����ߍ��܂ꂽ�������W�J����.
 *
 * ���̏�����src���p�ɂɕύX�����悤�ȏ����Ɍ����Ă���.
 * ����ŁAsrc�͏�������Œ�ł���Ainfo����hash��val�̒l(key�͕s��)���p�ɂɕς��悤�ȏ����̏ꍇ�A
 * ���̊֐��ł��ꉞ�Ή��ł��邪�A�O�����ɂ������ƍ����Ȏ������l���邱�Ƃ��ł���.
 * �����MsgObj�Ƃ������W���[���ŏ��������\��.
 *
 * ���Adst��src�͓����ZnkStr�ł����Ă͂Ȃ�Ȃ�.
 */
bool ZnkBird_extend( const ZnkBird info,
		ZnkStr dst, const char* src, size_t expected_size )
{
	/***
	 * �R�[�h���key_beginer, key_ender �Ƃ����`�Œ��ۉ����Ă��邪�A
	 * �R�����g�ł͕֋X��A#[, ]# ���g��.
	 */
	const size_t key_beginer_size = ZnkBmsFinder_begin_ptn_leng( info->key_finder_ );
	const size_t key_ender_size   = ZnkBmsFinder_end_ptn_leng(   info->key_finder_ );
	const size_t src_leng = Znk_strlen( src );

	bool status = true;
	bool result = true;
	size_t src_pos = 0;
	size_t btwn_begin;
	size_t btwn_leng;
	ZnkStr key = info->wk_key_;

	if( ZnkS_empty( src ) ){
		return true;
	}
	if( src_leng <= key_beginer_size ){
		ZnkStr_add( dst, src );
		return true;
	}

	ZnkBfr_reserve( key, 16 );
	if( expected_size == 0 ){
		ZnkBfr_reserve( dst, src_leng + ZnkBfr_size( dst ) );
	} else {
		ZnkBfr_reserve( dst, expected_size + ZnkBfr_size( dst ) );
	}

	while( true ){
		if( src_pos >= src_leng ){
			/* �I�[�ɓ��B����. */
			break;
		}

		/***
		 * ���݂̈ʒu(pos) ���� #[ ... ]# �����݂���͈͂𒲂ׂ�.
		 *   ....#[....]#
		 *   ^     ^   ^
		 *   p     b   e
		 *
		 * p = src_pos
		 * b = p + btwn_begin
		 * e = p + btwn_begin + btwn_leng
		 */
		if( ZnkBmsFinder_getBetweenRange( info->key_finder_,
				src+src_pos, src_leng-src_pos, 0, &btwn_begin, &btwn_leng ) )
		{
			if( btwn_leng == Znk_NPOS ){
				/**
				 * #[ �͓o�ꂵ�Ă��邪�A]# ���o�ꂹ���ɏI�[���}���Ă���.
				 * ���̏ꍇ #[ ���܂߂��̂܂ܒǉ�.
				 */
				const char* key_ender = ZnkBmsFinder_end_ptn( info->key_finder_ );
				ZnkStr_append( dst, src + src_pos, src_leng - src_pos );
				Znk_printf_e( "ZnkBird_extend : Error : [%s] src_leng=[%zu] src_pos=[%zu] key_ender=[%s] key_ender_size=[%zu]\n",
						src+src_pos, src_leng, src_pos, key_ender, key_ender_size );
				return false;
			}
			/**
			 * ���݂̈ʒu(pos) ���� #[ ���n�܂�O�܂ł͈̔͂� dst �֒ǉ�����.
			 */
			ZnkStr_append( dst, src + src_pos, btwn_begin-key_beginer_size );
		} else {
			/* #[ �����݂��Ȃ��ꍇ�́A�c������̂܂ܑS���ǉ����A�I������ */
			ZnkStr_append( dst, src + src_pos, src_leng - src_pos );
			return result;
		}

		/***
		 * �V����src_pos�֍X�V.
		 * ....#[....]#
		 *       ^
		 *       p
		 */
		src_pos += btwn_begin;
		/***
		 * src �ɂ����� Between�͈̔͂� key�ɑ������.
		 */
		ZnkStr_assign( key, 0, src + src_pos, btwn_leng );

		/***
		 * key �����ɏ������s��.
		 */
		if( info->func_ ){
			/***
			 * �Y������key�����݂��Ȃ��ꍇ�ł��J�X�^�������������ꍇ������.
			 * ���̂��߁A�����ł͑��ݐ��̃`�F�b�N���X�L�b�v���Afunc_�𑦎��ĂԎd�l�Ƃ���.
			 *
			 * �Ⴆ�Ί��ϐ�����W�J�������ꍇ�Ȃǂ́A�\��key��regist���Ă����̂�����Ȃ�.
			 * ����͂ǂ̂悤��key�����s���Őݒ肳��Ă���̂���\�ߎ擾�ł���ڐA���̂���
			 * ���@�����݂��Ȃ����߂ł���.
			 * ���̏ꍇ�Akey���^����ꂽ���getenv�Ŏ擾�����݂�ȂǁA�x���I��key���菈����
			 * �s���K�v������.
			 */
			status = (*info->func_)( info, dst, src, ZnkStr_cstr(key), src_pos, info->func_arg_ );
		} else {
			if( ZnkBird_exist( info, ZnkStr_cstr(key) ) ){
				ZnkStr_add( dst, ZnkBird_at( info, ZnkStr_cstr(key) ) );
				status = true;
			} else {
				status = false;
			}
		}
		if( !status ){
			result = false;
			/***
			 * #[...]# �̕��������̂܂ܒǉ�.
			 */
			ZnkStr_append( dst, src + src_pos-key_beginer_size,
					key_beginer_size + btwn_leng + key_ender_size );
		}

		/***
		 * �V����src_pos�֍X�V.
		 * ...]#...
		 *      ^
		 *      p
		 */
		src_pos += btwn_leng+key_ender_size;
	}
	return result;
}

bool ZnkBird_extend_self( const ZnkBird info, ZnkStr self, size_t expected_size )
{
	bool result;
	ZnkStr dst = info->wk_dst_;
	ZnkStr_clear( dst );
	result = ZnkBird_extend( info, dst, ZnkStr_cstr(self), expected_size );
	ZnkBfr_swap( dst, self );
	return result;
}


