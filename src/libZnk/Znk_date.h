#ifndef INCLUDE_GUARD__Znk_date_h__
#define INCLUDE_GUARD__Znk_date_h__

#include <Znk_str.h>
#include <time.h>

Znk_EXTERN_C_BEGIN

typedef struct {
	/***
	 * d1_
	 *   ���16bit : year
	 *   ����8bit  : month(1����12�܂�)
	 *   ����8bit  : day
	 */
	uint32_t d1_;
	/***
	 * d2_
	 *   ���8bit : wday �j��(0:�� 1:�� ... 6:�y)
	 *   ����8bit : hour
	 *   ����8bit : min
	 *   ����8bit : sec
	 */
	uint32_t d2_;
}ZnkDate;

typedef enum {
	 ZnkDateStr_e_Std=0
	,ZnkDateStr_e_HttpLastModified
	,ZnkDateStr_e_JavascriptLastModified
} ZnkDateStrType;

Znk_INLINE uint16_t ZnkDate_year(   const ZnkDate* date ){ return (uint16_t)( date->d1_ >> 16 ); }
Znk_INLINE uint8_t  ZnkDate_month(  const ZnkDate* date ){ return (uint8_t)( (date->d1_ & 0x0000ff00) >> 8 ); }
Znk_INLINE uint8_t  ZnkDate_day(    const ZnkDate* date ){ return (uint8_t)(  date->d1_ & 0x000000ff ); }
Znk_INLINE uint8_t  ZnkDate_wday(   const ZnkDate* date ){ return (uint8_t)(  date->d2_ >> 24 ); }
Znk_INLINE uint8_t  ZnkDate_hour(   const ZnkDate* date ){ return (uint8_t)( (date->d2_ & 0x00ff0000) >> 16 ); }
Znk_INLINE uint8_t  ZnkDate_minute( const ZnkDate* date ){ return (uint8_t)( (date->d2_ & 0x0000ff00) >> 8 ); }
Znk_INLINE uint8_t  ZnkDate_second( const ZnkDate* date ){ return (uint8_t)(  date->d2_ & 0x000000ff ); }

Znk_INLINE void
ZnkDate_set_year(   ZnkDate* date, uint16_t year   ){ date->d1_ &= 0x0000ffff; date->d1_ |= (((uint32_t)year) <<16); }
Znk_INLINE void
ZnkDate_set_month(  ZnkDate* date, uint8_t  month  ){ date->d1_ &= 0xffff00ff; date->d1_ |= (((uint32_t)month)<< 8); }
Znk_INLINE void
ZnkDate_set_day(    ZnkDate* date, uint8_t  day    ){ date->d1_ &= 0xffffff00; date->d1_ |= (uint32_t)day; }
Znk_INLINE void
ZnkDate_set_wday(   ZnkDate* date, uint8_t  wday   ){ date->d2_ &= 0x00ffffff; date->d2_ |= (((uint32_t)wday)  <<24); }
Znk_INLINE void
ZnkDate_set_hour(   ZnkDate* date, uint8_t  hour   ){ date->d2_ &= 0xff00ffff; date->d2_ |= (((uint32_t)hour)  <<16); }
Znk_INLINE void
ZnkDate_set_minute( ZnkDate* date, uint8_t  minute ){ date->d2_ &= 0xffff00ff; date->d2_ |= (((uint32_t)minute)<< 8); }
Znk_INLINE void
ZnkDate_set_second( ZnkDate* date, uint8_t  second ){ date->d2_ &= 0xffffff00; date->d2_ |= (uint32_t)second; }

Znk_INLINE bool ZnkDate_eq( const ZnkDate* date1, const ZnkDate* date2 ){
	return (bool)( date1->d1_ == date2->d1_ && date1->d2_ == date2->d2_ );
}

/**
 * @brief
 * �����݂̓��t�Ǝ��Ԃ𓾂�.
 */
void ZnkDate_getCurrent( ZnkDate* date );

/**
 * @brief
 * �w�肵��ZnkDate�𕶎���ɕϊ�����.
 *
 * @param pos:
 *   ZnkStr_snprintf�ɂ�����pos�Ɠ����ł���A���ʂ��㏑�������ʒu������.
 *   �Ⴆ��0�̏ꍇ�Astr�̊J�n�ʒu���猋�ʂ��㏑�������. Znk_NPOS���w�肷��
 *   ���Ƃ��ł��A���̏ꍇ��str�̍Ō���Ɍ��ʂ��A�������.
 *
 * @param type:
 *   �o�͕�����̌`���̃^�C�v���w�肷��.
 */
void ZnkDate_getStr( ZnkStr str, size_t pos, const ZnkDate* date, ZnkDateStrType type );
/**
 * @brief
 * �w�肵����������X�L������ZnkDate�𓾂�.
 *
 * @param type:
 *   �w�蕶����̌`���̃^�C�v���w�肷��.
 */
bool ZnkDate_scanStr( ZnkDate* date, const char* str, ZnkDateStrType type );

/**
 * @brief
 * �w�肵���Q��ZnkDate�̓��t��(d1��)���r����.
 * ���̂Ƃ�d2���͊��S�ɖ��������.
 * date1�̕������t�������̏ꍇ�� 1��Ԃ�.
 * date2�̕������t�������̏ꍇ��-1��Ԃ�.
 * �o���̓��t���S�������Ȃ�� 0 ��Ԃ�.
 */
int  ZnkDate_compareDay( const ZnkDate* date1, const ZnkDate* date2 );
/**
 * @brief
 * �w�肵���Q��ZnkDate�̎��ԕ�(d2��)�̍���b���Ōv�Z���Ď擾����.
 * ���̂Ƃ�date1��date2���������t(d1)�͑S�������ł��邩�܂��� delta_day ���̍������Ȃ����Ƃ�O��Ƃ���.
 * �����̍��� delta_day ���ł͂Ȃ��ꍇ�A���������ʂƂ͂Ȃ�Ȃ����ߒ��ӂ���.
 * date1��date2�̂ǂ���𖢗��ɂ��Ă��悢���Adate1�������̏ꍇ���̕b�����Adate2�������̏ꍇ���̕b����Ԃ�.
 * �قƂ�ǂ̃P�[�X�ł�delta_day�ɂ�1���w�肷��`�ɂȂ�Ǝv����.
 * d1���S�������ꍇ��delta_day�̎w��͒P�ɖ��������.
 *
 * @note
 *  ��L�Ō��� delta_day ���̓��t�̍��Ƃ́A���҂̌����Ȏ�����24*delta_day���ԂƂ����Ӗ��ł͂Ȃ��A
 * �����܂œ��t(d1)�l�݂̂Ŕ�r�����ꍇ�̍����Ӗ�����. �Ⴆ�΁A2000/01/11 23:10:00 �� 2000/01/10 1:10:00 
 * �̂Q���w�肳��A�܂� delta_day�Ƃ��� 1 ���w�肵���ꍇ�A�i�����͌����Ȏ����ł�����46���Ԃ̍�������A
 * 24���Ԃ𒴂��Ă��邯��ǂ�)�Ad1�݂̂ɒ��ڂ����ꍇ�̓��t����1���ł���̂ŁA�F�߂�����͂������Ƃł���.
 * d1�̍��̌����Ȍv�Z������ōs���Ă��Ȃ����R�́A������s���ɂ͌����Ƃ̓�������̗݌v�Ȃǂ��l�����邽�߂�
 * ���[�v�����܂킷�K�v������A���̏����ɂ͎��Ԃ���������̂ƂȂ邽�߂ł���. 
 * ����䂦�����ł͒P�ɂǂ��炪�������������(ZnkDate_compareDay���Ăяo����)���f���Ă͂�����̂́A
 * ���̌����ȓ��t���ɂ��Ă�delta_day���Q�Ƃ��邱�Ƃɂ��Ă���.
 */
long ZnkDate_diffSecond( const ZnkDate* date1, const ZnkDate* date2, uint32_t delta_day );

/**
 * @brief 
 *  �[�N����.
 *
 * @param year:
 *   1582�N�ȍ~���L���ł���.
 *
 * @note
 *  ���̔��莮�̓O���S���I��Ɍ���L���ł���.
 *  �O���S���I����肳�ꂽ�̂�1582�N�ł��邩��Ayear�ɂ͂���ȍ~�̔N���^�����Ȃ���΂Ȃ�Ȃ�.
 *  ���������p��͉������Ȃ��ł��낤.
 */
Znk_INLINE bool
ZnkDate_isGregorianLeapYear( unsigned int year )
{
	/***
	 * 400 => true, 100 => false, 4 => true, else false �̏��ɔ��肷��̂��ꌩ����Ƃ����Ƃ��킩��Ղ������ł���.
	 * �����������̕p�x����l���Ă܂�4�Ŕ��肷�ׂ�.
	 * ������{�A���S���Y���͈ȉ��ƂȂ�.
	 *
	 * if( year % 4 == 0 ){
	 *   if( year % 100 == 0 ){
	 *     if( year % 400 == 0 ){
	 *       return true;
	 *     }
	 *     return false;
	 *   }
	 *   return true;
	 * }
	 * return false;
	 *
	 * �ȉ��͂���Ɠ����Ȏ��ɕϊ���������.
	 */
	return (bool)( ( year % 4 == 0 ) && ( ( year % 100 != 0 ) || ( year % 400 == 0 ) ) );
}
Znk_INLINE size_t
ZnkDate_getNumDayOfYear( unsigned int year )
{
	return ZnkDate_isGregorianLeapYear( year ) ? 366 : 365;
}

/**
 * @brief
 *  �^����ꂽ�N����肻�̌��̍ŏI��(�܂�����ł�����)�𓾂�.
 *
 * @param year:
 *   1582�N�ȍ~���L���ł���.
 *
 * @param month:
 *   1 ���� 12 ��^����.
 */
uint8_t
ZnkDate_getEndDayOfManth( unsigned int year, unsigned int month );

/**
 * @brief
 *  �^����ꂽ���t���j���𓾂�.
 *
 * @param year:
 *   ���j�I�Ȍo�܂ɂ��1752�N�ȍ~���L���ł���.
 *
 * @param month:
 *   1 ���� 12 ��^����.
 *
 * @param day:
 *   �w�肳�ꂽyear, month �ɖ������Ȃ��͈͂�^���Ȃ���΂Ȃ�Ȃ�.
 *
 * @return:
 *   0=��, 1=��, ..., 6=�y.
 *
 * @note:
 *   �Q�l: Zeller�̌���, C FAQ
 */
uint8_t
ZnkDate_calcWDay( unsigned int year, unsigned int month, unsigned int day );

Znk_INLINE void
ZnkDate_updateWDay( ZnkDate* date ){
	ZnkDate_set_wday( date,
			ZnkDate_calcWDay( ZnkDate_year( date ), ZnkDate_month( date ), ZnkDate_day( date ) ) );
}

/**
 * @brief
 *  �^����ꂽ���tcurrent���ndays���O��ZnkDate�𓾂�.
 */
void
ZnkDate_getNDaysAgo( ZnkDate* ans, ZnkDate* current, unsigned int ndays );
/**
 * @brief
 *  �^����ꂽ���tcurrent���ndays�����ZnkDate�𓾂�.
 */
void
ZnkDate_getNDaysLater( ZnkDate* ans, ZnkDate* current, unsigned int ndays );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
