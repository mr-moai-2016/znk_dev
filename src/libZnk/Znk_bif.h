#ifndef INCLUDE_GUARD__Znk_bif_h__
#define INCLUDE_GUARD__Znk_bif_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/***
 * byte�z����R���o�[�g���鏈���͑��̏ꍇ����炾���Ŏ����ł���.
 * erase �� replace �n�͂قƂ�ǎg���邱�Ƃ͂Ȃ�.
 * (���ꂱ��炪�K�v�ƂȂ����ꍇ�� data_��resize_��memmove�̑g�ݍ��킹�ŋ����Ɏ������邱�Ƃ��ł���)
 *
 * ���Apush_bk_�͖��炩��append_�̓��ʂȏꍇ�����A�����̂��߉����Ă���.
 * create�̒��ۉ��͂��܂������Ȃ�.
 * �����ɂ�낤�Ƃ��Ă��������C���^�[�t�F�[�X�����G�����A�������Ďg���ɂ����Ȃ邾���Ȃ̂ł����ł͒ǉ����Ȃ�.
 * �܂�����͎����Ǘ�����R���e�i�Ƃ����킯�ł��Ȃ��̂�destroy�̒��ۉ����s��Ȃ����̂Ƃ���.
 */
typedef void     (*ZnkBifFuncT_append )( void* bfr, const uint8_t* src, size_t src_size );
typedef void     (*ZnkBifFuncT_push_bk)( void* bfr, uint8_t u8 );
typedef void     (*ZnkBifFuncT_reserve)( void* bfr, size_t size );
typedef void     (*ZnkBifFuncT_resize )( void* bfr, size_t size );
typedef size_t   (*ZnkBifFuncT_size)( const void* bfr );
typedef uint8_t* (*ZnkBifFuncT_data)( void* bfr );
typedef void     (*ZnkBifFuncT_swap)( void* bfr1, void* bfr2 );

typedef struct ZnkBifModule_tag {
	ZnkBifFuncT_append  append_;
	ZnkBifFuncT_push_bk push_bk_;
	ZnkBifFuncT_reserve reserve_;
	ZnkBifFuncT_resize  resize_;
	ZnkBifFuncT_size    size_;
	ZnkBifFuncT_data    data_;
	ZnkBifFuncT_swap    swap_;
}* ZnkBifModule;

typedef struct ZnkBif_tag {
	ZnkBifModule module_;
	void*        bfr_;
}* ZnkBif;

Znk_INLINE void ZnkBif_append( ZnkBif bif, const uint8_t* src, size_t src_leng ){
	bif->module_->append_( bif->bfr_, src, src_leng );
}
Znk_INLINE void ZnkBif_push_bk( ZnkBif bif, uint8_t u8 ){
	bif->module_->push_bk_( bif->bfr_, u8 );
}
Znk_INLINE void ZnkBif_reserve( ZnkBif bif, size_t size ){
	bif->module_->reserve_( bif->bfr_, size );
}
Znk_INLINE void ZnkBif_resize( ZnkBif bif, size_t size ){
	bif->module_->resize_( bif->bfr_, size );
}
Znk_INLINE size_t ZnkBif_size( const ZnkBif bif ){
	return bif->module_->size_( bif->bfr_ );
}
Znk_INLINE uint8_t* ZnkBif_data( ZnkBif bif ){
	return bif->module_->data_( bif->bfr_ );
}
Znk_INLINE void ZnkBif_swap( ZnkBif bif1, ZnkBif bif2 ){
	bif1->module_->swap_( bif1->bfr_, bif2->bfr_ );
}

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
