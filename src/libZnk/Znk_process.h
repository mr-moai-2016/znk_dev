#ifndef INCLUDE_GUARD__Znk_process_h__
#define INCLUDE_GUARD__Znk_process_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN

typedef enum {
	 ZnkProcessConsole_e_Inherit=0
	,ZnkProcessConsole_e_CreateNew
	,ZnkProcessConsole_e_Detached
} ZnkProcessConsoleType;

unsigned int
ZnkProcess_getCurrentProcessId( void );

void
ZnkProcess_requestReprocess( void );

bool
ZnkProcess_isReqReproces( void );

/***
 * ���݂̃v���Z�X��exit����.
 */
void 
ZnkProcess_exit( bool result );

void 
ZnkProcess_execChild( size_t argc, const char** argv, bool is_wait, ZnkProcessConsoleType cons_type );

/***
 * ���݂̃v���Z�X���w�肵���v���O�����Ƃ��čċN������.
 */
void 
ZnkProcess_execRestart( size_t argc, const char** argv, ZnkProcessConsoleType cons_type );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
