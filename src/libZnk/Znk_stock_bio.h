#ifndef INCLUDE_GUARD__Znk_stock_bio_h__
#define INCLUDE_GUARD__Znk_stock_bio_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/***
 * ���̊֐��͈ȉ��𖞂������̂Ƃ���.
 * 1. recv�����f�[�^��1byte�ȏ㑶�݂���ꍇ�͂��̃T�C�Y(byte)��Ԃ�.
 * 2. �f�[�^���I�������ꍇ��0��Ԃ�.
 * 3. �G���[�����������ꍇ��-1��Ԃ�.
 */
typedef int (*ZnkStockRecvFunc)( void* arg, uint8_t* buf, size_t buf_size );

typedef struct ZnkStockBIOImpl* ZnkStockBIO;

ZnkStockBIO
ZnkStockBIO_create( size_t stck_buf_size, ZnkStockRecvFunc recver_func, void* recver_arg );
void
ZnkStockBIO_destroy( ZnkStockBIO sbio );
void
ZnkStockBIO_clear( ZnkStockBIO sbio, ZnkStockRecvFunc recver_func, void* recver_arg );

size_t
ZnkStockBIO_getReadedSize( const ZnkStockBIO sbio );
int
ZnkStockBIO_getRecverResult( const ZnkStockBIO sbio );

bool
ZnkStockBIO_readLineCRLF( ZnkStockBIO sbio, char* dst_buf, size_t dst_buf_size );

/**
 * @return
 * �߂�l��fread�̊���ɕ키.
 * �����ɂ�fread�̖߂�l��byte�l�ł͂Ȃ�blk�̌��ł��邪�A
 * �����ł͎��ۂɓǂݍ��񂾃o�C�g����Ԃ����̂Ƃ���.
 */
size_t
ZnkStockBIO_read( ZnkStockBIO sbio, uint8_t* dst_buf, size_t dst_buf_size );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
