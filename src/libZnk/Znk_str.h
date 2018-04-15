#ifndef INCLUDE_GUARD__Znk_str_h__
#define INCLUDE_GUARD__Znk_str_h__

#include <Znk_bfr.h>
#include <stdarg.h>

Znk_EXTERN_C_BEGIN

/***
 * ZnkStr�́AZnkBfr�̕ʖ��ł��肱���͑S������̌^�ł���.
 * �������AZnkStr�ł͓����o�b�t�@�̏�Ԃɓ��L�̃|���V�[������.
 */
typedef ZnkBfr ZnkStr;


/**
 * @brief
 *   zkstr ��leng�� new_leng�Ŏw�肵�������ɂ���.
 *   ZnkBfr_resize�Ƃ̈Ⴂ�́Anew_leng�͎��ۂ�buffer size����1�������l�ł���Ƃ���
 *   ���ƂƁANULL�I�[���l������邱�Ƃł���.
 *   new_leng�ɂ�Znk_NPOS���w�肷�邱�Ƃ͂ł��Ȃ�.
 */
void ZnkStr_releng( ZnkStr zkstr, size_t new_leng );

/**
 * @brief
 * �����o�b�t�@��size�͈͓̔��Ŋm����NULL�I�[������.
 */
void
ZnkStr_terminate_null( ZnkBfr zkbfr, bool plus_one );

/**
 * @brief
 * �����o�b�t�@��size�͈͓̔��Ŋm����NULL�I�[�����A����ɕK�v�Ȃ��NULL�����̈ʒu�ɍ��킹��
 * size���k��������(���K������).
 */
void
ZnkStr_normalize( ZnkBfr zkbfr, bool plus_one );

/**
 * NULL�I�[���镶������쐬����.
 *
 * @param init_data
 *   �������̂��߂̕�������w�肷��.
 *   �܂��A�ȉ��̂悤�ȓ��ʂȏꍇ�����e����.
 *
 *   1. NULL�I�[���Ă��Ȃ��o�C�g�V�[�P���X��^���Ă��悢.
 *      ���̏ꍇleng�ɗL�����K�؂Ȓl���w�肷��K�v������.
 *      (Znk_NPOS���w�肵�Ă͂Ȃ�Ȃ�).
 *
 *   2. NULL���w�肵�Ă��悢.
 *      ���̏ꍇ�͕�����ł̏��������s��Ȃ�.
 *      �܂����̂Ƃ��Ɍ���Aleng�͓��ʂȈӖ��ɂȂ�(leng���Q��).
 *
 * @param leng
 *   ��{�I�ɂ͕�����̒���(�o�C�g)��^����.
 *   �����Ń������m�ۂ����size�́A���̒l�Ɠ������Ȃ�Ƃ͌���Ȃ�.
 *   (���̏ꍇ�ANULL�������T�C�Y���ǉ�����邽�߁A��������傫���T�C�Y�ƂȂ�)
 *   �������AZnkStr_leng�Ŏ擾�����l��(����ZnkStr�ɕύX���Ȃ�����)���̒l�ƂȂ�.
 *   �v��ZnkStr���ێ��������������ɑ΂���strlen���{�����Ƃ��̖߂�l�ɑ�������l�ł���.
 *
 *   �܂��A�ȉ��̂悤�ȓ��ʂȏꍇ�����e����.
 *
 *   1. init_data��NULL�I�[���Ă���C�̕�����ł���ꍇ�Ɍ���Aleng��Znk_NPOS���w�肵�Ă��悢.
 *     ���̏ꍇ�Astrlen( init_data )���w�肳�ꂽ�̂Ɠ����Ƃ݂Ȃ����.
 *
 *   2. init_data��NULL���w�肵���ꍇ�A�����Ɏw�肳�ꂽ�l�� 1 �𑫂����l�� ��������reserve�����.
 *     �������A��������reserve�ʂɂ�����炸�A���̏�Ԃł�ZnkStr_leng�̒l�͏�� 0 �ł���.
 *     (�����ZnkStr_clear���{������ԂƓ����ł���. �����Ɍ����΁A���̂Ƃ��̓���ZnkBfr�ɂ�����
 *     ������T�C�Y�͏�� 1 �ɐݒ肳��Ă���)
 */
ZnkStr
ZnkStr_create_ex( const char* init_data, size_t leng, ZnkBfrType type );

Znk_INLINE ZnkStr
ZnkStr_create( const char* init_data, size_t leng )
{
	return ZnkStr_create_ex( init_data, leng, ZnkBfr_Pad128 );
}

Znk_INLINE void
ZnkStr_destroy( ZnkStr zkstr ){
	ZnkBfr_destroy( zkstr );
}

/***
 * �����ZnkStr_create/ZnkStr_destroy�ƑS���������̂����A
 * �T�^�I�ȃp�^�[�����Ȍ��Ɍ`�����������̂ł���.
 */
Znk_INLINE ZnkStr
ZnkStr_new( const char* init_data ){
	return ZnkStr_create( init_data, Znk_NPOS );
}
ZnkStr
ZnkStr_newf( const char* fmt, ... );
Znk_INLINE void
ZnkStr_delete( ZnkStr zkbfr ){
	ZnkBfr_destroy( zkbfr );
}

Znk_INLINE size_t
ZnkStr_leng( const ZnkStr zkstr )
{
	/* �����o�b�t�@�͐��K������Ă�����̂Ƃ��Ĉ��� */
	const size_t size = ZnkBfr_size( zkstr );
	/* size == 0 �̏ꍇ�͓��ʂɋ��e����. */
	return ( size == 0 ) ? 0 : size-1;
}
Znk_INLINE bool ZnkStr_empty( const ZnkStr zkstr ){ return (bool)( ZnkStr_leng( zkstr ) == 0 ); }

/**
 * @brief
 * NULL�I�[������̓����|�C���^�𓾂�.
 */
Znk_INLINE const char*
ZnkStr_cstr( const ZnkStr zkstr )
{
	/* �����o�b�t�@�͐��K������Ă�����̂Ƃ��Ĉ��� */
	return (const char*)ZnkBfr_data( zkstr );
}

/**
 * @brief
 * �^����ZnkBfr���󕶎����Ӗ����鐳�K��������ɂ���.
 * (�܂�ANULL������������i�[���ꂽsize=1�̃o�b�t�@�Ƃ���)
 */
Znk_INLINE void
ZnkStr_clear( ZnkBfr zkbfr ){ ZnkStr_releng( zkbfr, 0 ); }

/**
 * @brief
 * src, src_leng�Ŏw�肵���������zkstr_dst��dst_pos�ȍ~�̓��e���㏑������.
 * zkstr_dst ����dst_pos�ȍ~�ɂ��鋌������̓��e�͂��̃T�C�Y�@���Ɋւ�炸�S�j������A
 * ���S�ɐV����������ɒu�������(dst_pos���O�̕�����͂Ȃ��e���͂Ȃ�).
 *
 * ���̊֐��̌Ăяo���ȑO�Ɏ擾���Ă���zkstr_dst�̐��f�[�^�|�C���^�́A����ralloc�Ăяo���ɂ��
 * �����ƂȂ�\�������邱�Ƃɒ��ӂ���.
 *
 * @param zkstr_dst
 *   ��{�I�ɂ͐��K����������w�肷��.
 *   ������size��0��ZnkBfr�̎w����F�߂�.
 *
 * @param dst_pos
 *   zkstr_dst �ł̒u���������n�߂�ʒu.
 *   Znk_NPOS���w�肵���ꍇ�A������̍ŏI�ʒu(�����o�b�t�@�T�C�Y-1)���w�肳�ꂽ���̂Ƃ݂Ȃ����.
 *   ���̍ŏI�ʒu�́Azkstr_dst���ł�NULL�������i�[����Ă���͂��̈ʒu�ł���A���̏ꍇ�A�������ƂȂ�.
 *
 * @param src
 *   �V�����u�������镶����̐擪���w���|�C���^.
 *
 * @param src_leng
 *   �V�����u�������镶����̒���.
 *   src��NULL�I�[���Ă���ꍇ�݂̂�����Znk_NPOS���w�肵�Ă��悭�A
 *   ���̏ꍇ�Asrc�ɑ΂��Astrlen�����s�������ʂ��w�肳�ꂽ���̂Ƃ݂Ȃ����.
 */
void
ZnkStr_assign( ZnkStr zkstr_dst, size_t dst_pos, const char* src, size_t src_leng );

/**
 * @brief
 *  ������C��������㏑���������.
 *
 * @note
 *  ��͂�C�������^���đ�����鏈������ԕp�x������.
 *  ����Ă����ł͂��̈�ԒZ��set�Ƃ������O��^����.
 */
Znk_INLINE void
ZnkStr_set( ZnkStr zkstr, const char* cstr )
{
	ZnkStr_assign( zkstr, 0, cstr, Znk_NPOS );
}

/***
 * @brief
 * �V���K�[�֐�.
 */
Znk_INLINE void
ZnkStr_copy( ZnkStr dst, const ZnkStr src )
{
	ZnkStr_assign( dst, 0, ZnkStr_cstr(src), ZnkStr_leng(src) );
}


/**
 * @brief
 * ����������֘A������.
 * ���̊֐��̌Ăяo���ȑO�Ɏ擾���Ă���zkstr�̐��f�[�^�|�C���^�́A����ralloc�Ăяo���ɂ��
 * �����ƂȂ�\�������邱�Ƃɒ��ӂ���.
 * �e�����ɂ��ẮAZnkStr_assign�ɏ�����.
 */
Znk_INLINE void
ZnkStr_append( ZnkStr zkstr, const char* src, size_t src_leng ){
	ZnkStr_assign( zkstr, Znk_NPOS, src, src_leng );
}

void
ZnkStr_replace( ZnkStr str, size_t dst_pos, size_t dst_leng, const char* src, size_t src_leng );
void
ZnkStr_insert( ZnkStr str, size_t dst_pos, const char* src, size_t src_leng );

/**
 * @brief
 *  C�����܂��͕���������֘A������.
 *
 * @note
 *  �����̊֐��̌Ăяo���ȑO�Ɏ擾���Ă���zkstr�̐��f�[�^�|�C���^�́A����ralloc�Ăяo���ɂ��
 *  �����ƂȂ�\�������邱�Ƃɒ��ӂ���.
 */
void
ZnkStr_add_c( ZnkStr zkstr, char ch );
Znk_INLINE void
ZnkStr_add( ZnkStr zkstr, const char* cstr )
{
	ZnkStr_append( zkstr, cstr, Znk_NPOS );
}
Znk_INLINE void
ZnkStr_add2( ZnkStr zkstr, const char* s1, const char* s2 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
}
Znk_INLINE void
ZnkStr_add3( ZnkStr zkstr, const char* s1, const char* s2, const char* s3 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
}
Znk_INLINE void
ZnkStr_add4( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
}
Znk_INLINE void
ZnkStr_add5( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
	ZnkStr_add( zkstr, s5 );
}
Znk_INLINE void
ZnkStr_add6( ZnkStr zkstr, const char* s1, const char* s2, const char* s3, const char* s4, const char* s5, const char* s6 )
{
	ZnkStr_add( zkstr, s1 );
	ZnkStr_add( zkstr, s2 );
	ZnkStr_add( zkstr, s3 );
	ZnkStr_add( zkstr, s4 );
	ZnkStr_add( zkstr, s5 );
	ZnkStr_add( zkstr, s6 );
}

/**
 * @brief
 * zkbfr��snprintf�����s����.
 * �����ł�Znk_vsnprintf���Ăяo���Ă���.
 * size �̎w��ɂ���āA�W�J���������񂪓r���ł��؂邱�Ƃ��ł��邵�A
 * ���������������莩���I�Ɋg�����邱�Ƃ��ł���.
 *
 * @param zkstr
 *   ���K��������ZnkStr���w�肵�Ȃ���΂Ȃ�Ȃ�.
 *
 * @param pos
 *   zkbfr���̈ʒu���w�肷��.
 *   ���̈ʒu���J�n�ʒu�Ƃ��ēW�J�������assign����.
 *   snprintf�Ƃ͑ΏƓI�ɁA���̑�2�����ɂ�buf_size���w�肷��̂ł͂Ȃ����Ƃɒ���.
 *   pos �� Znk_NPOS �╶����(ZnkStr_leng�œ�����l)�ȏ�̒l���w�肵���ꍇ�A
 *   �����񒷂��w�肳�ꂽ���̂Ƃ݂Ȃ���A���̏ꍇ�A���Ɍ��ʂ�A�����邱�ƂɂȂ�.
 *
 * @param size
 *   �W�J����镶����̌��E�T�C�Y���w�肷��.
 *   size �� Znk_NPOS �ȊO�̒l���w�肵���ꍇ�Asnprintf�Ƃقړ����Ӗ��ƂȂ�.
 *   ���Ȃ킿�Apos ���� size �܂ł͈̔͂ɓW�J�����񂪎��߂��(����𒴂���ꍇ��
 *   �r���ł��؂��)�A�ŏI�����͋����I��NULL�����ƂȂ�.
 *   size �� Znk_NPOS ���w�肵���ꍇ�A�W�J������ƌ����ɓ������̂�pos�ȍ~��
 *   assign�����. ���̏ꍇ�A�o�b�t�@�̃T�C�Y�͎����I�ɓK�؂Ȃ��̂Ɋg�������.
 *   ���̏ꍇ�A�����ł̓������m�ہ{snprintf���g���C���G���[�ŌJ��Ԃ����߁A
 *   �W�J������̃T�C�Y���傫���ꍇ�A�ŏ�����œK�ȃT�C�Y���w�肵���ꍇ�Ɣ��
 *   ���x�ቺ�������ꍇ������.
 *
 * @param fmt
 *   ������printf�t�H�[�}�b�g���w�肷��.
 *
 * @return
 *   Znk_vsnprintf �̖߂�l�ɏ�����.
 *   �������Asize �� Znk_NPOS �̏ꍇ�A���ꂪ -1 ��Ԃ����Ƃ͂Ȃ�.
 */
int ZnkStr_vsnprintf( ZnkStr zkstr, size_t pos, size_t size, const char* fmt, va_list ap );
int ZnkStr_snprintf(  ZnkStr zkstr, size_t pos, size_t size, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_vsnprintf �� �� 3 ���� size �ɂ����� Znk_NPOS ���w�肵���̂Ɠ����ł���.
 *  �䂦�� sprintf�Ƃ������O�ł͂��邪�A�o�b�t�@�I�[�o�[�t���[�̋���͂Ȃ�.
 */
int ZnkStr_sprintf( ZnkStr zkstr, size_t pos, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_sprintf �� �� 2 ���� pos �ɂ����� Znk_NPOS ���w�肵���̂Ɠ����ł���.
 *  ���Ȃ킿�A�w�肵����������t�H�[�}�b�g�������̂�zkstr�̍Ō�ɘA������`�ɂȂ�.
 */
int ZnkStr_addf( ZnkStr zkstr, const char* fmt, ... );
/**
 * @brief
 *  ZnkStr_sprintf �� �� 2 ���� pos �ɂ����� 0 ���w�肵���̂Ɠ����ł���.
 *  ���Ȃ킿�A�w�肵����������t�H�[�}�b�g�������̂�zkstr��set����`�ɂȂ�.
 */
int ZnkStr_setf( ZnkStr zkstr, const char* fmt, ... );
/* ���A��Ƃ��I���܂ł��΂炭�c�� */
#define ZnkStr_vsnprintf2 ZnkStr_vsnprintf
#define ZnkStr_snprintf2  ZnkStr_snprintf
#define ZnkStr_sprintf2   ZnkStr_sprintf
#define ZnkStr_addf2 ZnkStr_addf
#define ZnkStr_setf2 ZnkStr_setf


/**
 * str�̑S���e��cstr�Ŏ������C�����񂪓��������ǂ�����Ԃ�.
 */
bool
ZnkStr_eq( const ZnkStr str, const char* cstr );

/**
 * @brief
 * str��[ pos, ZnkStr_leng(str) ) (���Ȃ킿pos����Ō�܂�)�͈̔͂�data�Ŏ������o�C�g��
 * ���S�ɓ��������ǂ�����Ԃ�.
 *
 * @note
 * ZnkStr_isContain�Ƃ̈Ⴂ�ɒ��ӂ���.
 * ���̊֐��̏ꍇ�Apos�ȍ~����NULL�����܂ł̕����񒷂�data_leng�ƈ�v���Ă��Ȃ��ꍇ��
 * ���Ƃ�pos����data_leng�Ԗڂ܂ł̕�����data�Ɠ������Ƃ��A�߂�l��true�Ƃ͂Ȃ�Ȃ�.
 */
bool
ZnkStr_eqEx( const ZnkStr str, size_t pos, const char* data, size_t data_leng );
/**
 * @brief
 * str��[ pos, pos+data_leng )�͈̔͂�data�Ŏ������o�C�g�񂪓��������ǂ�����Ԃ�.
 * str�ɂ����� pos+data_leng �ȍ~�͈͎̔͂Q�Ƃ���Ȃ����߁A���̕����͂ǂ̂悤�ȓ��e
 * �ł��\��Ȃ�.
 */
bool
ZnkStr_isContain( const ZnkStr zkstr, size_t pos, const char* data, size_t data_leng );
/**
 * str�� data �Ŏ������o�C�g�񂩂�J�n���Ă��邩�ۂ���Ԃ�.
 */
Znk_INLINE bool
ZnkStr_isBegin( const ZnkStr zkstr, const char* cstr ){
	return ZnkStr_isContain( zkstr, 0, cstr, Znk_NPOS );
}
/**
 * str�� data �Ŏ������o�C�g��ŏI�[���Ă��邩�ۂ���Ԃ�.
 */
Znk_INLINE bool
ZnkStr_isEnd( const ZnkStr zkstr, const char* cstr ){
	return ZnkStr_isContain( zkstr, Znk_NPOS, cstr, Znk_NPOS );
}


/**
 * @brief
 *   zkstr �̌��̕������ cut_size �Ŏw�肵�����������J�b�g����.
 *   cut_size �� zkstr �̕����񒷈ȏ�̐��� Znk_NPOS���w�肳�ꂽ�ꍇ�A
 *   ���̊֐���ZnkStr_clear�Ɠ����ɂȂ�.
 *
 * @param zkstr
 *   ��{�I�ɂ͐��K����������w�肷��.
 *   ������size��0��ZnkBfr�̎w����F�߂�.
 *   ������realloc�͔������Ȃ�.
 */
void ZnkStr_cut_back( ZnkStr zkstr, size_t cut_size );
/**
 * @brief
 *   ZnkStr_cut_back �� cut_size �ɂ����� 1 ���w�肵���̂ƑS�������ł���.
 */
Znk_INLINE void
ZnkStr_cut_back_c( ZnkStr zkstr ){ ZnkStr_cut_back( zkstr, 1 ); }


/**
 * @brief
 *  zkstr �� pos �̈ʒu�ɂ��镶����� cut_size �Ŏw�肵�����������폜����.
 *  cut_size �� zkstr �̕����񒷈ȏ�̐��� Znk_NPOS���w�肳�ꂽ�ꍇ�A
 *  ���̊֐���ZnkStr_releng �� pos ���w�肵���̂Ɠ����ɂȂ�.
 *  �܂� pos �� zkstr �̕����񒷈ȏ�̐��� Znk_NPOS���w�肳�ꂽ�ꍇ�A
 *  ���̊֐��͉������Ȃ�.
 *
 * @param zkstr
 *  ��{�I�ɂ͐��K����������w�肷��.
 *  ������size��0��ZnkBfr�̎w����F�߂�.
 *  ������realloc�͔������Ȃ�.
 */
void ZnkStr_erase( ZnkStr zkstr, size_t pos, size_t cut_size );
/**
 * @brief
 *  ZnkStr_erase �� pos �ɂ����� 0 ���w�肵���̂ƑS�������ł���.
 */
Znk_INLINE void
ZnkStr_cut_front( ZnkStr zkstr, size_t cut_size ){ ZnkStr_erase( zkstr, 0, cut_size ); }
/**
 * @brief
 *  ZnkStr_cut_front �� cut_size �ɂ����� 1 ���w�肵���̂ƑS�������ł���.
 */
Znk_INLINE void
ZnkStr_cut_front_c( ZnkStr zkstr ){ ZnkStr_cut_front( zkstr, 1 ); }

/**
 * @brief
 *   zkstr �ɂ����� pos �Ŏw�肵���ʒu�ɂ��镶���� ch �Ɠ������ꍇ�A
 *   ������J�b�g����. pos �� Znk_NPOS �� zkstr �̃T�C�Y�ȏ�̒l���w�肵���ꍇ�A
 *   �ŏI�����̂���ʒu���w�肳�ꂽ�Ƃ݂Ȃ����. �܂� zkstr�̕����񒷂� 0 �̏ꍇ��
 *   ���̊֐��͉������Ȃ�.
 */
void ZnkStr_chompC( ZnkStr zkstr, size_t pos, char ch );

/**
 * @brief
 *   line �̍Ō���̕��������s�R�[�h�ł������ꍇ�A�������J�b�g����.
 *   �����łȂ��ꍇ�͉������Ȃ�.
 */
void ZnkStr_chompNL( ZnkStr line );


/**
 * @brief
 *   ������� idx �Ԗڂ̕�����Ԃ�.
 *   �������A�����񂪋�̏ꍇ�� '\0' ��Ԃ�.
 *
 * @note
 *   ZnkBfr_at �Ƃ̈Ⴂ�́A�����񂪋�ł���ꍇ���l������Ă��邱�Ƃł���.
 *   �܂��A����ȊO�̗Ⴆ�Δ͈̓`�F�b�N�Ȃǂ͈�؍l������Ă��Ȃ�.
 *   �]���āAZnkStr_leng(zkstr) �̒l���傫�� idx ��^�����ꍇ�̓���ɂ��Ă�
 *   ����`�ƂȂ�( ZnkStr_leng(zkstr) �Ɠ����l�ɂ��Ă͔F�߂� ).
 *   idx��Znk_NPOS��^���邱�Ƃ͂ł��Ȃ�.
 */
char ZnkStr_at( const ZnkStr zkstr, size_t idx );

/**
 * @brief
 *   ������̈�Ԑ擪�̕�����Ԃ�.
 *   �������A�����񂪋�̏ꍇ�� '\0' ��Ԃ�.
 */
Znk_INLINE char ZnkStr_first( const ZnkStr zkstr ){
	return ZnkStr_at( zkstr, 0 );
}
/**
 * @brief
 *   ������̈�ԍŌ�̕�����Ԃ�.
 *   �������A�����񂪋�̏ꍇ�� '\0' ��Ԃ�.
 */
char ZnkStr_last( const ZnkStr zkstr );

void
ZnkStr_swap( ZnkStr str1, ZnkStr str2 );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
