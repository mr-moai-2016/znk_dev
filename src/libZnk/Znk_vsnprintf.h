#ifndef INCLUDE_GUARD__Znk_vsnprintf_h__
#define INCLUDE_GUARD__Znk_vsnprintf_h__

#include <Znk_base.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkVSNPrintfCtx_tag ZnkVSNPrintfCtx;

typedef size_t (*ZnkVSNPrintfAppendFunc_T)( struct ZnkVSNPrintfCtx_tag* vsnp_ctx, const void* src, size_t req_size );
typedef void   (*ZnkVSNPrintfTerminateNullFunc_T)( struct ZnkVSNPrintfCtx_tag* vsnp_ctx );

struct ZnkVSNPrintfCtx_tag {
	ZnkVSNPrintfAppendFunc_T add_func_;
	ZnkVSNPrintfTerminateNullFunc_T terminate_null_func_;
	uintptr_t pos_;
	void* user_info_;
};

/**
 * @brief:
 *  C99 ���w������vsnprintf
 *
 * @note
 *  �T�|�[�g����Ă���length_modifier�͈ȉ��ł���.
 *    h   : short�^(C�W��)
 *    z   : size_t�^(C99�W��)
 *    j   : intmax_t�^(C99�W��)
 *    t   : ptrdiff_t�^(C99�W��)
 *    L   : long double�^(C�W��)
 *    l   : long �^(C�W��)
 *    ll  : long long �^(C99�W��)
 *    I16 : 16bit����(�����Znk_vsnprintf�Ǝ��̊g���ƂȂ�)
 *    I32 : 32bit����(�����Znk_vsnprintf�Ǝ��̊g���ƂȂ�)
 *    I64 : 64bit����(VC,bcc�ŃT�|�[�g����Ă���g��)
 */
bool
Znk_vsnprintf_general( ZnkVSNPrintfCtx* vsnp_ctx, const char* fmt, va_list ap );

int
Znk_vsnprintf_C99( char* buf, size_t buf_size, const char* fmt, va_list ap );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
