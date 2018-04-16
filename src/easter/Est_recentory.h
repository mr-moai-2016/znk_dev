#ifndef INCLUDE_GUARD__Est_recentory_h__
#define INCLUDE_GUARD__Est_recentory_h__

#include <Znk_str.h>
#include <Est_finf.h>

Znk_EXTERN_C_BEGIN

typedef struct EstRecentoryImpl* EstRecentory;

EstRecentory 
EstRecentory_create( void );

void
EstRecentory_destroy( EstRecentory recent );

bool
EstRecentory_load( EstRecentory recent, const char* filename );
bool
EstRecentory_save( EstRecentory recent, const char* filename, bool is_marge_tags );

ZnkStr
EstRecentory_title( EstRecentory recent );

ZnkVarpAry
EstRecentory_finf_list( EstRecentory recent );

//void
//EstRecentory_add( EstRecentory recent, const char* filename, const char* dst_box, const char* fsys_path, ZnkStr msg );
/***
 * @param src_vpath :
 *   Recentory�ɒǉ�����finf������vpath�Ɠ����l��vpath��ێ����Ă����ꍇ�ɁA
 *   ��finf���폜���邽�߂Ɏw�肷��.
 *   �܂肱�̏ꍇ����finf���Ō���ɒP�Ɉړ����邾���Ƃ�����.
 */
void
EstRecentory_add2( EstRecentory recent, const char* filename, const char* dst_box, const char* fsys_path,
		const char* src_vpath, ZnkStr msg );
size_t
EstRecentory_removeOld( EstRecentory recent );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
