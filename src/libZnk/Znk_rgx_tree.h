#ifndef INCLUDE_GUARD__Znk_rgx_tree_h__
#define INCLUDE_GUARD__Znk_rgx_tree_h__

#include <Znk_rgx_base.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

/* ZnkRgxOP_t�̓m�[�h�̑����\���^ */
typedef enum {
	ZnkRgxOP_char,            /* �������̂���  */
	ZnkRgxOP_concat,          /* XY */
	ZnkRgxOP_union,           /* X|Y */
	ZnkRgxOP_closure,         /* X* */
	ZnkRgxOP_dot,             /* dot */
	ZnkRgxOP_empty            /* �� */
} ZnkRgxOP_t;

typedef const uint8_t* (*ZnkIncrementU8PFuncT)( const uint8_t* );
typedef const uint8_t* (*ZnkUChar64FuncT_get)( ZnkUChar64* uc, const uint8_t* p );

/* ZnkRgxTree�͍\���؂̃m�[�h��\���^ */
typedef struct ZnkRgxTree_tag {
	ZnkRgxOP_t op; /* ���̃m�[�h�̑��� */
	union {
		ZnkUChar64 c_; /* op == ZnkRgxOP_char ZnkRgxOP_dot �̂Ƃ�: ���� */
		struct { /* op == ZnkRgxOP_char ZnkRgxOP_dot �ȊO�̂Ƃ�: */
			struct ZnkRgxTree_tag* left_;  /* ���̎q */
			struct ZnkRgxTree_tag* right_; /* �E�̎q */
		} x;
	} u;
} ZnkRgxTree;

size_t
ZnkRgxTree_getAllocatedNodeCount( void );

ZnkRgxTree*
ZnkRgxTree_create( const char* str, ZnkUChar64FuncT_get func_get, ZnkStr rep_msg );

void
ZnkRgxTree_destroy( ZnkRgxTree* tree, ZnkStr rep_msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

