#ifndef INCLUDE_GUARD__Rano_dir_util_h__
#define INCLUDE_GUARD__Rano_dir_util_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef bool (*RanoDirUtilFuncT)( const char* file_path, void* arg );

typedef bool (*RanoDirUtilFilterFuncT)( const char* in_file_path, const char* out_file_path, void* arg );

/**
 * @brief
 *   src_dir�f�B���N�g���z�����ċA�I��dst_dir�ֈړ�����.
 */
void
RanoDirUtil_moveDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg );

/**
 * @brief
 *   top_dir�f�B���N�g���z�������ׂč폜����.
 */
void
RanoDirUtil_removeDir( const char* topdir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg );

/**
 * @brief
 *   src_dir�f�B���N�g���z�����ċA�I��filt�������A
 *   filt�����ς݂̌��ʂ�dst_dir�֍ċA�I�ɏo�͂���.
 *   (src_dir�f�B���N�g���z���͉����ύX����Ȃ�)
 */
void
RanoDirUtil_filterDir( const char* src_dir, const char* dst_dir,
		const char* title, ZnkStr ermsg,
		RanoDirUtilFuncT is_processFile_func, void* is_processFile_arg,
		RanoDirUtilFilterFuncT filtFile_func, void* filtFile_arg );

/**
 * @brief
 *   path��tail�������܂���f�B���N�g��(�v��tail��荶��)�����ׂ�mkdir����.
 */
bool
RanoDirUtil_mkdirOfTailContain( const char* path, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
