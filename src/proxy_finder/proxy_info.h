#ifndef INCLUDE_GUARD__proxy_list_h__
#define INCLUDE_GUARD__proxy_list_h__

#include <Znk_obj_ary.h>
#include <Znk_stdc.h>

Znk_EXTERN_C_BEGIN

typedef struct ProxyInfoImpl* ProxyInfo;

/**
 * BREK : proxy�Ƃ��ċ@�\���Ă��Ȃ�(BREaK)
 * RFUS : �T�[�o���ڑ������₷��(ReFUSed)
 * NTJP : Not jp�ƕ\�������
 * ACNG : �A�N�Z�X�K����
 * ACOK : BBS���֏����݉\
 * UNKN : ������(UnKnown)
 * UCNT : �T�[�o���N�����Ă��Ȃ�(UnConnect)
 * MBON : Mobile(�g��/�X�}�z)�̂ݏ����݉�
 * PCON : Computer(PC)�̂ݏ����݉�
 */
typedef enum {
	 ProxyState_e_UNKN = 0
	,ProxyState_e_ACOK
	,ProxyState_e_ACNG
	,ProxyState_e_RFUS
	,ProxyState_e_NTJP
	,ProxyState_e_BREK
	,ProxyState_e_UCNT
	,ProxyState_e_MBON
	,ProxyState_e_PCON
}ProxyStateCode;

Znk_INLINE const char*
ProxyStateCode_toStr( const ProxyStateCode query_code )
{
	const char* str = "ProxyState_e_UNKN";
	switch( query_code ){
	case Znk_BIND_STR( :str=, ProxyState_e_UNKN ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_ACOK ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_ACNG ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_RFUS ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_NTJP ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_BREK ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_UCNT ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_MBON ); break;
	case Znk_BIND_STR( :str=, ProxyState_e_PCON ); break;
	default: break;
	}
	return str + Znk_strlen_literal( "ProxyState_e_" );
}

ProxyStateCode
ProxyStateCode_getCode_fromStr( const char* query_str );


Znk_DECLARE_HANDLE( ProxyInfoAry );

/**
 * @brief Ary�𐶐�����.
 *
 * @param elem_responsibility:
 *  ���ꂪtrue�̏ꍇ�A�e�v�f�̎����ɂ��Ă���Ary�͐ӔC������.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�Ƃ��Ă��ꂪ�Ă΂��.
 *  ���̂Ƃ��A���[�U�͊O���Ŏ��������蓖�Ă�object��set, push_bk�Ȃǂœn���Ă��̂܂ܕ��u���Ă���
 *  �`�ɂȂ�(����������ΊO���ł����delete���Ă͂Ȃ�Ȃ�). ���邢�͕ʓrregist�n�̊֐��Ȃǂ�
 *  �p�ӂ��ׂ��ł���.
 *
 *  ���ꂪfalse�̏ꍇ�́A�P�ɂ���Ary���|�C���^�l�̈ꎞ�I�ȃR���e�i�Ƃ��Ďg�p���A
 *  �e�v�f�̎����͊O���̋@�\�ɂ���ĊǗ����Ă��邱�Ƃ��Ӗ�����.
 *  �܂�destroy, clear, resize, erase, pop_bk���ɂ����āA�e�v�f�ɉ�����deleter�͌Ă΂�Ȃ�.
 *  �K�R�I�ɁA���[�U�͓����ŃI�u�W�F�N�g���蓖�Ă���������悤��regist�n�֐������̏ꍇ��
 *  �ǉ��ŗp�ӂ��ׂ��ł͂Ȃ�. ����� clear, resize, erase, push_bk, set �Ȃǂ������ČĂяo����
 *  �����Ǘ��ɕs�����������邩��ł���. �ǂ����Ă�regist�n�֐����K�v�ȏꍇ�͑f����deleter��
 *  �Z�b�g���ׂ��ł���.
 *
 * @note
 *   ��Lregist�n API�Ƃ� regist ������.
 */
ProxyInfoAry
ProxyInfoAry_create( bool elem_responsibility );

/**
 * @brief Ary��j������.
 */
Znk_INLINE void
ProxyInfoAry_destroy( ProxyInfoAry dary ){
	ZnkObjAry_M_DESTROY( dary );
}

Znk_INLINE size_t
ProxyInfoAry_size( const ProxyInfoAry dary ){
	return ZnkObjAry_M_SIZE( dary );
}
Znk_INLINE ProxyInfo
ProxyInfoAry_at( ProxyInfoAry dary, size_t idx ){
	return ZnkObjAry_M_AT( dary, idx, ProxyInfo );
}
Znk_INLINE ProxyInfo*
ProxyInfoAry_dary_ptr( ProxyInfoAry dary ){
	return ZnkObjAry_M_ARY_PTR( dary, ProxyInfo );
}

Znk_INLINE bool
ProxyInfoAry_erase( ProxyInfoAry dary, ProxyInfo obj ){
	return ZnkObjAry_M_ERASE( dary, obj );
}
Znk_INLINE bool
ProxyInfoAry_erase_byIdx( ProxyInfoAry dary, size_t idx ){
	return ZnkObjAry_M_ERASE_BY_IDX( dary, idx );
}
Znk_INLINE void
ProxyInfoAry_clear( ProxyInfoAry dary ){
	ZnkObjAry_M_CLEAR( dary );
}
Znk_INLINE void
ProxyInfoAry_resize( ProxyInfoAry dary, size_t size ){
	ZnkObjAry_M_RESIZE( dary, size );
}

Znk_INLINE void
ProxyInfoAry_push_bk( ProxyInfoAry dary, ProxyInfo obj ){
	ZnkObjAry_M_PUSH_BK( dary, obj );
}
Znk_INLINE void
ProxyInfoAry_set( ProxyInfoAry dary, size_t idx, ProxyInfo obj ){
	ZnkObjAry_M_SET( dary, idx, obj );
}


void
ProxyInfoAry_regist( ProxyInfoAry list,
		const char* ip, uint16_t port, ProxyStateCode state_code, const char* hostname );

bool
ProxyInfoAry_load( ProxyInfoAry list, const char* filename );

bool
ProxyInfoAry_save( const ProxyInfoAry list, const char* filename, bool with_statecode );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
