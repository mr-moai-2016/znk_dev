#ifndef INCLUDE_GUARD__Znk_envvar_h__
#define INCLUDE_GUARD__Znk_envvar_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

/**
 * @brief
 * ZnkEnvVar_free���Ă΂��܂ł̊ԁA
 * �߂�l�͕�����Ƃ��Ď��R�Ɏg�p�ł���.
 */
char*
ZnkEnvVar_get( const char* varname );

/**
 * @brief
 * val�Ƃ���NULL��^�����ꍇ�A
 * varname�Ƃ������O�̊��ϐ�����������.
 */
void
ZnkEnvVar_set( const char* varname, const char* val );

/**
 * @brief
 * ZnkEnvVar_get�œ����l�͕K�����̊֐��ŉ�����Ȃ���΂Ȃ�Ȃ�.
 */
void
ZnkEnvVar_free( char* val );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
