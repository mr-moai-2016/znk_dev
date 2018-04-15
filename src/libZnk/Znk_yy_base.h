#ifndef INCLUDE_GUARD__Znk_yy_base_h__
#define INCLUDE_GUARD__Znk_yy_base_h__

#include <Znk_base.h>
#include <stdio.h>

Znk_EXTERN_C_BEGIN

/* flex���f���R�[�h�ɂ����Ă̋����̊֐��X�^�C�����g��Ȃ��悤�ɂ���. */
#ifndef   YY_USE_PROTOS
#  define YY_USE_PROTOS
#endif

#if defined(_MSC_VER) || defined(__DMC__) || defined(__BORLANDC__)
#  ifndef   YY_NO_UNISTD_H
#    define YY_NO_UNISTD_H
#  endif
#endif

int yyparse( void );
int yylex( void );

/***
 * ZnkYYInfo_update_line_idx��Lex���̎���u\n�v���o���ɌĂяo���Ă�
 * �悢�Ǝv���邩������Ȃ����A����͂�낵���Ȃ�.
 * ���̗��R�́Ayyerror�֐��̌Ăяo����Lex���̎���u\n�v���o���̑O��o����
 * ��������\�������邩��ł���.
 * ���̈ʒu��ZnkYYInfo_update_line_idx��u�����Ƃɂ���āA
 * ���̃^�C�~���O�����ׂẴP�[�X�ɂ����Ċm���Ɂu\n�v���o������Ƃ��邱�Ƃ��ł���.
 */
void   ZnkYYInfo_reset_line_idx( void );
void   ZnkYYInfo_update_line_idx( void );
size_t ZnkYYInfo_the_line_idx( void );
void   ZnkYYInfo_print_error( const char* msg, const char* near_text );

#define ZnkYYInfo_set_yyin( fp ) do { \
	extern FILE* yyin; \
	yyin = fp; \
} while( 0 )

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
