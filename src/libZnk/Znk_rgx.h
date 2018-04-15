#ifndef INCLUDE_GUARD__Znk_rgx_h__
#define INCLUDE_GUARD__Znk_rgx_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkRgxInfoImpl* ZnkRgxInfo;
typedef struct ZnkRgxEncodeImpl* ZnkRgxEncode;

ZnkRgxInfo
ZnkRgxInfo_create( void );
bool
ZnkRgxInfo_setup( ZnkRgxInfo info, const char* regexp_str, const ZnkRgxEncode enc, ZnkStr rep_msg );
void
ZnkRgxInfo_destroy( ZnkRgxInfo info );

/**
 * DFA���g���ĕ�����begin�ɑ΂��ăp�^�[���}�b�`���s��.
 * from: �}�b�`���������̐擪�ʒu��Ԃ�.
 * to:   �}�b�`���������̒���̈ʒu��Ԃ�.
 * �p�^�[���}�b�`�Ɏ��s������from��NULL���Z�b�g����.
 */
void
ZnkRgxInfo_match( ZnkRgxInfo rgxinfo, const char* begin, size_t* from, size_t* to, ZnkStr rep_msg );

ZnkRgxEncode
ZnkRgxEncode_find( const char* id );

const char*
ZnkRgxEncode_id( const ZnkRgxEncode enc );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
