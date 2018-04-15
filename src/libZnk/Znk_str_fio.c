#include "Znk_str_fio.h"
#include "Znk_stdc.h"

bool
ZnkStrFIO_fgets( ZnkStr line, size_t pos, size_t block_size, ZnkFile fp )
{
	const size_t leng = ZnkStr_leng( line );
	uint8_t* data;
	char* ptr;
	size_t readed_len;
	if( pos == Znk_NPOS || pos > leng ){
		pos = leng;
	}
	ZnkStr_releng( line, pos );
	if( block_size < 4 ){ block_size = 4; }

	ZnkBfr_reserve( line, pos + block_size );
	data = ZnkBfr_data( line ); 
	ptr = (char*)( data + pos );

	while( true ){
		if( Znk_fgets( ptr, block_size, fp ) == NULL ){
			return false;
		}
		readed_len = Znk_strlen( ptr );
		pos += readed_len;
		ZnkStr_releng( line, pos );

		if( readed_len + 1 < block_size ){
			/* buf���t���Ɏg���Ă��Ȃ����.
			 * OK. ���炩�Ɉ�s�����ׂēǂ݂����Ă��� */
			break;
		}

		/* buf���t���Ɏg���Ă�����. */

		/* �ŏI������'\n'�ł��邩�ۂ����m�F����. */
		if( ZnkStr_last( line ) == '\n' ){
			break; /* OK. ��s�����ׂēǂ݂����Ă��� */
		}

		/* �ŏI������'\n'�ł͂Ȃ�. ��s�����ׂēǂ݂����Ă��Ȃ�.
		 * ���s����. */
		ZnkBfr_reserve( line, pos + block_size );
		data = ZnkBfr_data( line ); 
		ptr = (char*) data + pos;
	}
	return true;
}

