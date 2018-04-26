#ifndef INCLUDE_GUARD__Est_assort_h__
#define INCLUDE_GUARD__Est_assort_h__

#include <Znk_varp_ary.h>
#include <Znk_bird.h>
#include <Est_box_base.h>

Znk_EXTERN_C_BEGIN

bool
EstAssort_renameFile_toMD5ofFile( const char* src_file_path, const char* dst_topdir, ZnkStr msg, size_t* processed_count,
		const char* file_tags, const char* comment, bool is_marge_tags,
		ZnkStr renamed_filename, bool* ans_is_moved_file );

bool
EstAssort_getEssential( ZnkStr comment,
		EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* box_fsys_dir, ZnkStr msg, ZnkStr renamed_filename );
bool
EstAssort_isExist( EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* favorite_dir, ZnkStr msg, ZnkStr renamed_filename );

size_t
EstAssort_doOneNewly( const char* file_tags, const char* comment, bool is_marge_tags,
		const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename );

size_t
EstAssort_addTags( const char* file_tags, const char* comment, const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename );
void
EstAssort_convertCachePath_toFilePath( ZnkStr cache_path, const char* cache_top_dir, const char* file_top_dir );

/**
 * @brief
 * �g�b�v�f�B���N�g�� src_top_dir �̔z��(�����Ƃ͌���Ȃ�)�ɑ��݂���t�@�C�� src_file_path ��
 * ���̃T�u�f�B���N�g���͈ێ������܂� dst_top_dir �ֈړ�����.
 * ���������src_top_dir�̕���������dst_top_dir�ɒu�����������̂��ړ���p�X�Ƃ��Ĉړ�����Ƃ�������.
 *
 * @param src_top_dir:
 *  �ړ����̃t�@�C�����܂ރg�b�v�f�B���N�g���̃p�X�ł���.
 *  �K�������ړ����̃t�@�C���𒼑��Ɏ��f�B���N�g���̃p�X�ł���Ƃ͌���Ȃ����Ƃɒ���.
 *
 * @param src_file_path:
 *  �ړ����̃t�@�C���̃p�X�ł���.
 *  src_file_path �͕K�� src_top_dir ����n�܂镶����łȂ���΂Ȃ�Ȃ�.
 *  �܂����̊֐��ł� src_top_dir �̏I�[���� src_file_path �̏I�[�܂łɃT�u�f�B���N�g���ɑ������镶���񂪑��݂���ꍇ
 *  ���̃T�u�f�B���N�g�����ێ������܂܂� dst_top_dir �z���ֈڂ�.
 *
 * @param src_top_dir:
 *  �ړ���̃t�@�C�����i�[����g�b�v�f�B���N�g���̃p�X�ł���.
 *  �K�������ړ���̃t�@�C���𒼑��Ɏ��f�B���N�g���̃p�X�ł���Ƃ͌���Ȃ����Ƃɒ���.
 *
 * @param dst_additional_under_dir:
 *  ���ꂪ�����I�Ɏw�肳��Ă����ꍇ�Adst_top_dir�����̃T�u�f�B���N�g���ւ��̃f�B���N�g������ʂɒǉ��}������.
 *  �Ⴆ�Έړ���̃t�@�C���̃p�X�� dst_top_dir/jpg/image.jpg �ƂȂ��Ă����ꍇ�A
 *  ����� dst_top_dir/dst_additional_under_dir/jpg/image.jpg �̂悤�ɕϊ�����.
 *  �t�@�C���V�X�e����ɂ��̂悤�ȃf�B���N�g�������݂��Ȃ��ꍇ�͎����I�ɍ쐬�����.
 *  ���ꂪ��܂���NULL�Ŏw�肳��Ă���ꍇ�̓T�u�f�B���N�g���ɂ��̂悤�Ȓǉ��I�}�����s��Ȃ�.
 *
 * @param title:
 *  �G���[/���|�[�g���b�Z�[�W�̍ۂɍs���ɂ����\������.
 *
 * @param ermsg:
 *  �G���[/���|�[�g���b�Z�[�W���i�[���邽�߂�ZnkStr.
 *  ���̎擾���s�v�ȏꍇ��NULL���w�肵�Ă��悢.
 */
size_t
EstAssort_moveSubdirFile( const char* src_top_dir, const char* src_file_path, const char* dst_top_dir, const char* dst_additional_under_dir,
		const char* title, ZnkStr ermsg );
bool
EstAssort_remove( EstBoxDirType ast_dir_type, const char* fsys_path, ZnkStr msg );
bool
EstAssort_remove_fromFInfList( EstBoxDirType box_dir_type, const char* md5filename, ZnkStr msg );

void
EstAssort_addImgURLList( ZnkStr EstCM_img_url_list, size_t file_count, size_t begin_idx, size_t end_idx, const char* path );

//EstBoxDirType
//EstAssort_convertAssortDir_toFSysDir( ZnkStr fsys_path, const char* src_assort_dir, size_t* ans_box_path_offset, ZnkStr msg );

bool
EstAssort_registNewTag( ZnkVarpAry post_vars, ZnkStr tag_editor_msg, ZnkStr tagid );
void
EstAssort_addPostVars_ifNewTagRegisted( ZnkVarpAry post_vars, ZnkStr tag_editor_msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
