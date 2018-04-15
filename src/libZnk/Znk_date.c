#include "Znk_date.h"
#include "Znk_base.h"
#include "Znk_str.h"
#include "Znk_s_base.h"

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
#  include <unistd.h>
#endif

#include <stdio.h>
#include <assert.h>

void ZnkDate_getCurrent( ZnkDate* date )
{
	time_t timer;
	struct tm *t_st;
	
	time(&timer); /* ���ݎ����̎擾 */
	
	/***
	 * ctime�֐��͌��ݎ��������̂悤�ȕ�����ɕϊ�����.
	 *  Wed Mar 06 00:35:26 2002
	 *  �g�p�� : printf("%s\n", ctime(&timer));
	 * ����͂��܂�悢�\�����@�ł͂Ȃ����߂����ł͍̗p���Ȃ�.
	 */
	
	t_st = localtime(&timer); /* ���ݎ������\���̂ɕϊ� */

	/***
	 * struct tm {
	 *   int tm_sec;   // �b [0-61] �ő�2�b�܂ł̂��邤�b���l��
	 *   int tm_min;   // �� [0-59]
	 *   int tm_hour;  // �� [0-23]
	 *   int tm_mday;  // �� [1-31]
	 *   int tm_mon;   // �� [0-11] 0����n�܂邱�Ƃɒ���
	 *   int tm_year;  // �N [1900����̌o�ߔN��]
	 *   int tm_wday;  // �j�� [0:�� 1:�� ... 6:�y]
	 *   int tm_yday;  // �N���̒ʂ����� [0-365] 0����n�܂邱�Ƃɒ���
	 *   int tm_isdst; // �Ď��Ԃ������ł���� 0
	 * };
	 */
	ZnkDate_set_year(   date, (uint16_t)( t_st->tm_year + 1900 ) );
	ZnkDate_set_month(  date, (uint8_t) ( t_st->tm_mon  + 1 ) ); /* ����+1 */
	ZnkDate_set_day(    date, (uint8_t) ( t_st->tm_mday ) );
	ZnkDate_set_wday(   date, (uint8_t) ( t_st->tm_wday ) ); /* �j��(0:�� 1:�� ... 6:�y) */
	ZnkDate_set_hour(   date, (uint8_t) ( t_st->tm_hour ) );
	ZnkDate_set_minute( date, (uint8_t) ( t_st->tm_min ) );
	ZnkDate_set_second( date, (uint8_t) ( t_st->tm_sec ) );
}

static const char* st_month_table[] = {
	"", /* dummy */
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};
static const char* st_wday_table[] = {
	"Sun", "Man", "Tue", "Wed", "Thu", "Fri", "Sat",
};
void
ZnkDate_getStr( ZnkStr str, size_t pos, const ZnkDate* date, ZnkDateStrType type )
{
	static const char* tz = "GMT"; /* �Ƃ肠�����Œ� */

	switch( type ){
	case ZnkDateStr_e_HttpLastModified:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%s, %02d %s %d %02d:%02d:%02d %s",
				st_wday_table[ (int)ZnkDate_wday( date ) ],
				(int)ZnkDate_day( date ),
				st_month_table[ (int)ZnkDate_month( date ) ],
				(int)ZnkDate_year( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ),
				tz );
		break;
	case ZnkDateStr_e_JavascriptLastModified:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%02d/%02d/%d %02d:%02d:%02d",
				(int)ZnkDate_month( date ),
				(int)ZnkDate_day( date ),
				(int)ZnkDate_year( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ) );
		break;
	default:
		ZnkStr_snprintf( str, pos, Znk_NPOS, 
				"%d/%02d/%02d %02d:%02d:%02d",
				(int)ZnkDate_year( date ),
				(int)ZnkDate_month( date ),
				(int)ZnkDate_day( date ),
				(int)ZnkDate_hour( date ),
				(int)ZnkDate_minute( date ),
				(int)ZnkDate_second( date ) );
		break;
	}

}
static int 
findTableStr( const char** table, size_t table_size, const char* query_str )
{
	size_t i;
	for( i=0; i<table_size; ++i ){
		if( ZnkS_eq( table[ i ], query_str ) ){
			break;
		}
	}
	return (int)i;
}
bool
ZnkDate_scanStr( ZnkDate* date, const char* str, ZnkDateStrType type )
{
	char wday_str[ 16 ]      = "";
	char month_str[ 16 ] = "";
	char tz[ 16 ]        = "GMT";
	int year = 0, month  = 0, day = 0;
	int hour = 0, minute = 0, second = 0;
	int wday = 0;

	switch( type ){
	case ZnkDateStr_e_HttpLastModified:
		sscanf( str, "%s, %02d %s %d %02d:%02d:%02d %s",
				wday_str, &day, month_str, &year,
				&hour, &minute, &second,
				tz );
		month = findTableStr( st_month_table, Znk_NARY(st_month_table), month_str );
		if( month >= Znk_NARY(st_month_table) ){
			month = 0;
		}
		wday = findTableStr( st_wday_table, Znk_NARY(st_wday_table), wday_str );
		if( wday >= Znk_NARY(st_wday_table) ){
			wday = 0;
		}
		ZnkDate_set_wday(  date, (uint8_t)wday );
		break;
	case ZnkDateStr_e_JavascriptLastModified:
		sscanf( str, "%02d/%02d/%d %02d:%02d:%02d",
				&month, &day, &year,
				&hour, &minute, &second );
		break;
	default:
		sscanf( str, "%d/%02d/%02d %02d:%02d:%02d",
				&year, &month, &day,
				&hour, &minute, &second );
		break;
	}

	ZnkDate_set_year(  date, (uint16_t)year );
	ZnkDate_set_month( date,  (uint8_t)month );
	ZnkDate_set_day(   date,  (uint8_t)day );
	ZnkDate_set_hour(   date, (uint8_t)hour );
	ZnkDate_set_minute( date, (uint8_t)minute );
	ZnkDate_set_second( date, (uint8_t)second );
	return true;

}

int ZnkDate_compareDay( const ZnkDate* date1, const ZnkDate* date2 )
{
	/***
	 * year, month, day �ƃ��x�����傫�����ɏ��bit���犄��U���Ă���.
	 * ����Ă����uint32_t�^�̐����Ƃ��ď����ɔ�r���Ă��A���̌��ʂ�
	 * year, month, day �ł̔�r���ʂƈ�v����͂��ł���.
	 */
	int64_t diff = (int64_t)date1->d1_ - (uint64_t)date2->d1_;
	return ( diff == 0 ) ? 0 : ( diff > 0 ) ? 1 : -1; 
}
long
ZnkDate_diffSecond( const ZnkDate* date1, const ZnkDate* date2, uint32_t delta_day )
{
	const int hour1   = (int)ZnkDate_hour( date1 );
	const int hour2   = (int)ZnkDate_hour( date2 );
	const int minute1 = (int)ZnkDate_minute( date1 );
	const int minute2 = (int)ZnkDate_minute( date2 );
	const int second1 = (int)ZnkDate_second( date1 );
	const int second2 = (int)ZnkDate_second( date2 );
	/* compare == 0 �̂Ƃ��� ���t(d1)�͓��� */
	const int compare = ZnkDate_compareDay( date1, date2 );
	/***
	 * compare == 1 �̂Ƃ�
	 *   �܂�date1����date2�������v�Z�����A�����  compare*delta_day*24*3600 ��������.
	 * compare == -1�̂Ƃ�
	 *   �܂�date2����date1�������v�Z�����A����� -compare*delta_day*24*3600 ��������.
	 *   ����ɂ��̌��ʂ̎��̕������t�]������΁A���� compare == 1 �̎��ƈ�v����.
	 * compare == 0 �̂Ƃ�
	 *   �P��date1����date2�������v�Z������΂悢.
	 *   ���̂Ƃ����炩�� compare == 1 �̎��ƈ�v����.
	 *
	 * ���ǂǂ̂悤�ȏꍇ�ł��ȉ��̎�(compare==1�̏ꍇ�̌`��)�ɓ��ꂳ��邱�Ƃ��킩��.
	 */
	return ( hour1 - hour2 ) * 3600 + ( minute1 - minute2 ) * 60 + second1 - second2
		+ compare * delta_day * 24 * 3600;
}

uint8_t
ZnkDate_getEndDayOfManth( unsigned int year, unsigned int month )
{
	switch( month ){
	case 1:  return 31;
	case 2:  return ZnkDate_isGregorianLeapYear( year ) ? 29 : 28;
	case 3:  return 31;
	case 4:  return 30;
	case 5:  return 31;
	case 6:  return 30;
	case 7:  return 31;
	case 8:  return 31;
	case 9:  return 30;
	case 10: return 31;
	case 11: return 30;
	case 12: return 31;
	default: break;
	}
	assert( 0 );
	return 0;
}

uint8_t
ZnkDate_calcWDay( unsigned int year, unsigned int month, unsigned int day )
{
	/***
	 * table of [ (13*month+8)/5 ] mod 7
	 * ������month=1, 2 �̏ꍇ��13, 14 �ɓǂݑւ��Čv�Z���Ă���.
	 */
	static const unsigned int st_tbl13[] = {
		0,    /* dummy */
		0, 3, /* as month=13, month=14 */
		2, 5, 0, 3,
		5, 1, 4, 6, 2, 4,
	};
	/***
	 * 1, 2���͑O�N�x��13��, 14�����^����ꂽ���̂Ƃ݂Ȃ�.
	 */
	year -= month < 3;
	return (uint8_t)( ( year + year/4 - year/100 + year/400 + st_tbl13[ month ] + day ) % 7 );
}

void
ZnkDate_getNDaysAgo( ZnkDate* ans, ZnkDate* current, unsigned int ndays )
{
	uint16_t year  = ZnkDate_year(  current );
	uint8_t  month = ZnkDate_month( current );
	uint8_t  day   = ZnkDate_day(   current );
	if( (unsigned int)day > ndays ){
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(day-ndays) );
	} else {
		unsigned int remain_day = ndays - (unsigned int)day;
		unsigned int end_day    = 0;
		while( true ){
			/* ��O�̌��̏����擾 */
			if( month > 1 ){
				--month;
			} else {
				--year;
				month = 12;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day ����O�̌��̓����ȏ�Ȃ炳��Ɍ���O�ւƒH�� */
			if( remain_day >= end_day ){
				remain_day -= end_day;
			} else {
				break;
			}
		}
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(end_day - remain_day) );
	}
	/* �j���̍X�V */
	ZnkDate_updateWDay( ans );
}
void
ZnkDate_getNDaysLater( ZnkDate* ans, ZnkDate* current, unsigned int ndays )
{
	uint16_t year  = ZnkDate_year(  current );
	uint8_t  month = ZnkDate_month( current );
	uint8_t  day   = ZnkDate_day(   current );
	unsigned int end_day = ZnkDate_getEndDayOfManth( year, month );

	if( end_day >= day + ndays ){
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)(day+ndays) );
	} else {
		unsigned int remain_day = day + ndays - end_day;
		while( true ){
			/* ���̌��̏����擾 */
			if( month < 12 ){
				++month;
			} else {
				++year;
				month = 1;
			}
			end_day = ZnkDate_getEndDayOfManth( year, month );

			/* remain_day �����̌��̓������傫���Ȃ炳��Ɍ�����ւƒH�� */
			if( remain_day > end_day ){
				remain_day -= end_day;
			} else {
				break;
			}
		}
		*ans = *current;
		ZnkDate_set_year(  ans, year );
		ZnkDate_set_month( ans, month );
		ZnkDate_set_day(   ans, (uint8_t)remain_day );
	}
	/* �j���̍X�V */
	ZnkDate_updateWDay( ans );
}

