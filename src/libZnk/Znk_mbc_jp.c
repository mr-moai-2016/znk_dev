#include "Znk_mbc_jp.h"
#include "Znk_bfr_bif.h"
#include <string.h>


#ifdef __BORLANDC__
/**
 * �x�� W8071 hoge.cpp XX: �ϊ��ɂ���ėL������������(�֐�hoge) )
 *
 * ���̃��W���[���ł́A0xFF ����8bit�����Ƃ���uint8_t �ƈꏏ�Ɏg�������𑽗p���Ă��邽��.
 * ���̌x���������\�������.  ���̃��W���[���Ɍ���΂���͖��Ӗ��Ȍx���ł���.
 */
#  pragma warn -8071
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1300)
/***
 * VC6.0�̂�
 * warning C4761: �֐��̉������Ǝ��������ް��^���قȂ��Ă��܂��B���������������̌^�ɕϊ����܂��B 
 * ��}��(VC6.0�ł�0x80�Ȃǂ̃��e�����\�L��uint8_t�^�����֑������Ȃǂ���Ƃ��̌x�����o��).
 */
#  pragma warning(disable: 4761)
#endif



Znk_INLINE bool UTF8_isMidByte   ( uint8_t b ){ return (bool)((b & 0xc0) == 0x80); } /* 2�o�C�g�ڈȍ~�͕K�� 10xxxxxx */
Znk_INLINE bool UTF8_isFirstByte2( uint8_t b ){ return (bool)((b & 0xe0) == 0xc0); } /* 2byte�����̊J�n 110xxxxx 10xxxxxx (����11bit) */
Znk_INLINE bool UTF8_isFirstByte3( uint8_t b ){ return (bool)((b & 0xf0) == 0xe0); } /* 3byte�����̊J�n 1110xxxx 10xxxxxx, ... (����16bit) */
Znk_INLINE bool UTF8_isFirstByte4( uint8_t b ){ return (bool)((b & 0xf8) == 0xf0); } /* 4byte�����̊J�n 11110xxx 10xxxxxx, ... (����21bit) */
Znk_INLINE int  UTF8_getFirstByteType( uint8_t b )
{
	if( b < 0x80 ){ return 1; } /* ASCII. */
	if( UTF8_isMidByte(    b ) ){ return 0; }
	if( UTF8_isFirstByte2( b ) ){ return 2; }
	if( UTF8_isFirstByte3( b ) ){ return 3; }
	if( UTF8_isFirstByte4( b ) ){ return 4; }
	return -1; /* Invalid UTF8 */
}


/***
 * �w�肷��v32�̒l��Unicode��Ԃł̃X�J���[�l�ł���.
 * �����郏�C�h�����̐����l�ł͂Ȃ�.
 * ���C�g�����̐����l��UTF16�̃o�C�g�C���[�W�̕���2byte���邢��4byte�P�ʂŔ����o�������̂ł���A
 * �����Ŏw�肷��Unicode��Ԃł̃X�J���[�lv32�Ƃ͈قȂ�.
 */
Znk_INLINE bool
UTF8_addUnicodeVal32( ZnkBif utf8s, uint32_t v32 )
{
	if (v32 < 0x80) { /* 0x0000-0x007F */
		ZnkBif_push_bk( utf8s, (uint8_t)v32 );
	}
	else if (v32 < 0x800) { /* 0x0080-0x07FF */
		const uint8_t b2 = (uint8_t)((v32     ) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 6)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xc0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
	}
	else if (v32 < 0x10000) { /* 0x0800-0xFFFF */
		const uint8_t b3 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 12)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xe0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
	}
	else if (v32 < 0x200000) { /* 0x00010000-0x001FFFFF */
		const uint8_t b4 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 18)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xf0) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
	}
	else if (v32 < 0x4000000) { /* 0x00200000-0x03FFFFFF */
		const uint8_t b5 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b4 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 18) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 24)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xf8) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
		ZnkBif_push_bk( utf8s, (b5 | 0x80) );
	}
	else if (v32 < 0x80000000) { /* 0x04000000-0x7FFFFFFF */
		const uint8_t b6 = (uint8_t)((v32      ) & 0x3f);
		const uint8_t b5 = (uint8_t)((v32 >>  6) & 0x3f);
		const uint8_t b4 = (uint8_t)((v32 >> 12) & 0x3f);
		const uint8_t b3 = (uint8_t)((v32 >> 18) & 0x3f);
		const uint8_t b2 = (uint8_t)((v32 >> 24) & 0x3f);
		const uint8_t b1 = (uint8_t)((v32 >> 30)       );
		ZnkBif_push_bk( utf8s, (b1 | 0xfc) );
		ZnkBif_push_bk( utf8s, (b2 | 0x80) );
		ZnkBif_push_bk( utf8s, (b3 | 0x80) );
		ZnkBif_push_bk( utf8s, (b4 | 0x80) );
		ZnkBif_push_bk( utf8s, (b5 | 0x80) );
		ZnkBif_push_bk( utf8s, (b6 | 0x80) );
	}
	else {
		return false;
	}
	return true;
}

/***
 * UTF-16�������`��
 * UTF-16�ł́AUnicode�̕����ʒuU+0000..U+10FFFF���A16�r�b�g�����Ȃ������𕄍��P�ʂƂ��������P�ʗ�ŕ\���B
 * Unicode�̕����ʒu�̍ő傪U+10FFFF�Ȃ̂́A���ꂪUTF-16�ŕ\����ő傾����ł���B
 * �����P�ʗ��1�܂���2�̕����P�ʂ���Ȃ�B�܂�A���v��16�r�b�g�܂���32�r�b�g�ł���B
 *
 * BMP�Ɋ܂܂��U+0000..U+D7FF��U+E000..U+FFFF�́A���̂܂ܕ����P��1�ŕ\���B
 * U+D800 �` U+DFFF �̕����ʒu�͑�p�����ʒu�iSurrogate Code Point�j�ƌĂ΂�A���ʂȈӖ��Ŏg�p����邽�߁A
 * BMP�̂��̗̈�ɂ͕��������^����Ă��炸�AUTF-16�ȊO�̕������X�L�[���ł͒ʏ�͎g�p����Ȃ��B
 *
 * �X�J���l(16bit)          UTF-16(16bit)
 * xxxxxxxx xxxxxxxx        xxxxxxxx xxxxxxxx 
 *
 * BMP�ȊO��U+10000..U+10FFFF�́A�ȉ��̂悤�Ƀr�b�g��z�����āA�����P��2�ŕ\���B
 * BMP�O��1�̕����ʒu��\���A������2�̑�p�����ʒu�̃y�A���T���Q�[�g�y�A�ƌĂԁB
 * �ȉ��ł� 110110 �� 110111 ������ɂ�����.
 *
 * �X�J���l(21bit)              UTF-16(32bit)
 * 000uuuuu xxxxxxxx xxxxxxxx   110110ww wwxxxxxx 110111xx xxxxxxxx
 * ������ wwww = uuuuu - 1 �ł���. uuuuu �͂��Ȃ炸 1 �ȏ� 10000�ȉ��ł���A-1 ����̂�
 * 5bit��4bit�ɂ��邱�Ƃŗ̈�̖��ʂ�1bit���点�邩��ł���.
 */

/***
 * �w�肷��v16,v32�̒l��Unicode��Ԃł̃X�J���[�l�ł���.
 * �����郏�C�h�����̐����l�ł͂Ȃ�.
 * ���C�g�����̐����l��UTF16�̃o�C�g�C���[�W�̕���2byte���邢��4byte�P�ʂŔ����o�������̂ł���A
 * �����Ŏw�肷��Unicode��Ԃł̃X�J���[�lv16,v32�Ƃ͈قȂ�.
 */
Znk_INLINE void
UTF16_addUnicodeVal16( ZnkBif utf16s, uint16_t v16 ) {
	ZnkBif_append( utf16s, (uint8_t*)&v16, sizeof(uint16_t) );
}

Znk_INLINE bool
UTF16_addUnicodeVal32( ZnkBif utf16s, uint32_t v32 )
{
	if( v32 < 0x10000 ){
		/***
		 * U+0000..U+D7FF��U+E000..U+FFFF�͈̔�(BMP�Ɋ܂܂��).
		 * ���̂܂ܕ����P��1�ŕ\���B
		 */
		UTF16_addUnicodeVal16( utf16s, (uint16_t)(v32) );
	} else if (v32 < 0x110000) {
		/***
		 * U+10000..U+10FFFF�͈̔�(BMP�O)
		 * �ő�21bit
		 */
		uint16_t high =
			(uint16_t)((v32 - 0x10000) >> 10) | /* ���11bit(uuuuu xxxxxx�ɑ���)�̎擾. */
			0xd800; /* ��1�̃T���Q�[�g�y�A 110110 ��t�����邽�߁A1101 1000 0000 0000(=0xd800) ��bit�𗧂Ă� */
		uint16_t low =
			(uint16_t)(v32 & 0x03ff) | /* ����10bit�̎擾 */
			0xdc00; /* ��2�̃T���Q�[�g�y�A 110111 ��t�����邽�߁A1101 1100 0000 0000(=0xdc00) ��bit�𗧂Ă� */
		UTF16_addUnicodeVal16( utf16s, high );
		UTF16_addUnicodeVal16( utf16s, low );
	} else {
		/***
		 * 22bit�ȏ�̕����͎g���Ȃ�.
		 * ����������v32������ȏ�̒l�ł������ꍇ�ُ͈�.
		 */
		return false;
	}
	return true;
}


static bool st_shortest_flag   = true;
static int  st_replace_invalid = 0;

/**
 * Convert UTF-8 to UTF-16-LE (byte order: LittleEndian)
 */
bool
ZnkMbc_convertUTF8_toUTF16( const char* utf8s, size_t utf8s_leng, ZnkBif utf16s, ZnkErr* err )
{
	const uint8_t* utf8p = (const uint8_t*)utf8s;
	size_t         inlen = utf8s_leng;

	uint16_t u16 = 0;

	while( inlen > 0 ){
		const uint8_t c0 = utf8p[0];

		if( (c0 & 0x80) == 0 ){ /* 0xxxxxxx (7bit) : ASCII�R�[�h�݊� */
			if( c0 == 0 ){
				/***
				 * UTF-8��1-byte�ڂ�0:
				 * �ꉞUTF-8�̎d�l�ɂ͈ᔽ���Ă��Ȃ��������I�ɂ͒ʏ�L�蓾�Ȃ�Text�Ǝv����.
				 */
				ZnkErr_intern( err, "Warning : Input may not be UTF-8 text.\n");
			}

			/***
			 * UTF16_addUnicodeVal32���Ă�ł��悢���A�ǉ������UnicodeVal�͖��炩��16bit�ȉ��ł��邽�߁A
			 * �����ł����������邽��UTF16_addUnicodeVal16�𒼐ڌĂ�.
			 */
			UTF16_addUnicodeVal16( utf16s, c0 );
			++utf8p;
			--inlen;

		} else if( inlen >= 2 && UTF8_isFirstByte2(c0)
		  && UTF8_isMidByte( utf8p[1] )
		){
			const uint8_t c1 = utf8p[1];

			/***
			 * c0��0xc2�ȏ�ł���͂��ł���.
			 * 0xc1�ȉ��Ƃ���ƗL��bit�̎����l��0x7f�ȉ��ƂȂ��Ă��܂�
			 * �����UTF-8��1byte�n���J�o�[����͈͂Ɣ��.
			 */
			if( st_shortest_flag && (c0 == 0xc0 || c0 == 0xc1) ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 2;
					inlen -= 2;
					continue;
				} else {
					ZnkErr_internf( err, "Error : Non-shortest UTF-8 sequence (%02x).", c0);
					return false;
				}
			}
			u16 = ((c0 & 0x1f) << 6) | /* 1byte�ڂ̉���5bit���擾. */
			      (c1 & 0x3f);         /* 2byte�ڂ̉���6bit���擾. */

			/***
			 * UTF16_addUnicodeVal32���Ă�ł��悢���A�ǉ������UnicodeVal�͖��炩��16bit�ȉ��ł��邽�߁A
			 * �����ł����������邽��UTF16_addUnicodeVal16�𒼐ڌĂ�.
			 */
			UTF16_addUnicodeVal16( utf16s, u16 );
			utf8p += 2;
			inlen -= 2;

		} else if( inlen >= 3 && UTF8_isFirstByte3(c0)
		  && UTF8_isMidByte(utf8p[1]) && UTF8_isMidByte(utf8p[2]) )
		{
			const uint8_t c1 = utf8p[1];
			const uint8_t c2 = utf8p[2];

			if( st_shortest_flag && c0 == 0xe0 && c1 < 0xa0 ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 3;
					inlen -= 3;
					continue;
				} else {
					ZnkErr_internf( err, "Error : non-shortest UTF-8 sequence (%02x)", c0);
					return false;
				}
			}

			u16 = ((c0 & 0xf)  << 12) | /* 1byte�ڂ̉���4bit���擾. */
			      ((c1 & 0x3f) <<  6) | /* 2byte�ڂ̉���6bit���擾. */
			      ((c2 & 0x3f)      );  /* 3byte�ڂ̉���6bit���擾. */

			/* surrogate chars */
			if( u16 >= 0xd800 && u16 <= 0xdfff ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 3;
					inlen -= 3;
					continue;
				} else {
					ZnkErr_internf( err, "Error : none-UTF-16 char detected (%04x)", u16);
					return false;
				}
			}

			/***
			 * UTF16_addUnicodeVal32���Ă�ł��悢���A�ǉ������UnicodeVal�͖��炩��16bit�ȉ��ł��邽�߁A
			 * �����ł����������邽��UTF16_addUnicodeVal16�𒼐ڌĂ�.
			 */
			UTF16_addUnicodeVal16( utf16s, u16 );
			utf8p += 3;
			inlen -= 3;

		} else if( inlen >= 4 && UTF8_isFirstByte4(c0)
		  && UTF8_isMidByte(utf8p[1]) && UTF8_isMidByte(utf8p[2]) && UTF8_isMidByte(utf8p[3]) )
		{
			const uint8_t c1 = utf8p[1];
			const uint8_t c2 = utf8p[2];
			const uint8_t c3 = utf8p[3];
			uint32_t u32;

			if( st_shortest_flag && c0 == 0xf0 && c1 < 0x90 ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 4;
					inlen -= 4;
					continue;
				} else {
					ZnkErr_internf( err, "Error : non-shortest UTF-8 sequence (%02x)", c0);
					return false;
				}
			}

			u32 = ((c0 &  0x7) << 18) | /* ����3bit�� ��ʂ���21-19bit�̈ʒu�� */
			      ((c1 & 0x3f) << 12) | /* ����6bit�� ��ʂ���18-13bit�̈ʒu�� */
			      ((c2 & 0x3f) <<  6) | /* ����6bit�� ��ʂ���12-7bit�̈ʒu�� */
			      ((c3 & 0x3f)      );  /* ����6bit�� ��ʂ���6-1bit�̈ʒu�� */

			if( !UTF16_addUnicodeVal32( utf16s, u32 ) ){
				if( st_replace_invalid ){
					UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
					utf8p += 4;
					inlen -= 4;
					continue;
				} else {
					ZnkErr_internf( err, "Error : none-UTF-16 char detected (%04x)", u32);
					return false;
				}
			}

			utf8p += 4;
			inlen -= 4;

		} else {
			if( st_replace_invalid ){
				UTF16_addUnicodeVal32( utf16s, st_replace_invalid );
				++utf8p;
				--inlen;
			} else {
				ZnkErr_internf( err, "Error : illegal UTF-8 sequence (%02x)", c0);
				return false;
			}
		}
	}

	return true;
}

/***
 * Convert UTF-16-LE (st. byte order is reverse) to UTF-8
 * ���ʂ�utf8s�̍Ō���֒ǉ������.
 */
bool
ZnkMbc_convertUTF16_toUTF8( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif utf8s, bool eliminate_bom, ZnkErr* err )
{
	size_t len = utf16s_leng; /* utf16s_leng �̓o�C�g�T�C�Y�ł͂Ȃ��z��̌� */
	size_t i;
	if( len == 0 ) return true;

	for( i=0; i<len; ++i ) {
		uint16_t u16 = utf16s[i];

		if (eliminate_bom && u16 == 0xfeff) { /* byte order mark */
			continue;
		} else if (u16 < 0x80) { /* ASCII etc */
			ZnkBif_push_bk( utf8s, (uint8_t)(u16) );

		} else if (u16 < 0x0800) { /* 0x0100-0x07FF */
			uint8_t b1 = (uint8_t)(u16 >> 6);   // ���2bit
			uint8_t b2 = (uint8_t)(u16 & 0x3f); // ����6bit
			ZnkBif_push_bk( utf8s, (b1 | 0xc0) ); /* 0b110000nn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */

		} else if (u16 >= 0xdc00 && u16 <= 0xdfff) { /* sole low surrogate(1024) */
			if( st_replace_invalid ){
				UTF8_addUnicodeVal32( utf8s, st_replace_invalid );
				continue;
			} else {
				ZnkErr_intern( err, "Error : invalid surrogate detected\n");
				return false;
			}

		} else if (u16 >= 0xd800 && u16 <= 0xdbff) { /* high surrogate(1024) */
			uint16_t low;
			uint32_t u32;
			uint8_t  b1, b2, b3, b4;

			if( i+1 >= len ) { /* not enough length */
				if( st_replace_invalid ){
					UTF8_addUnicodeVal32(utf8s, st_replace_invalid);
					continue;
				} else {
					ZnkErr_intern( err, "Error : invalid surrogate detected\n");
					return false;
				}
			}

			low = utf16s[i+1];
			if (low < 0xdc00 || low > 0xdfff) { /* not low surrogate */
				if (st_replace_invalid) {
					UTF8_addUnicodeVal32(utf8s, st_replace_invalid);
					continue;
				} else {
					ZnkErr_intern( err, "Error : invalid surrogate detected\n");
					return false;
				}
			}

			/***
			 * u32�͍ő�ł�21bit
			 * ���ꂩ��4byteUTF8��g�ݗ��Ă�.
			 */
			u32 = ((u16 & 0x3ff) << 10) | /* ����10bit�����10bit�� */
			      (low & 0x3ff);          /* ����10bit������10bit�� */
			u32 += 0x10000;

			b1 = (uint8_t)((u32 >> 18)       ); /* u32 19-21bit(3bit) */
			b2 = (uint8_t)((u32 >> 12) & 0x3f); /* u32 13-18bit(6bit) */
			b3 = (uint8_t)((u32 >>  6) & 0x3f); /* u32 7-12bit(6bit) */
			b4 = (uint8_t)((u32      ) & 0x3f); /* u32 1-6bit(6bit) */
			ZnkBif_push_bk( utf8s, (b1 | 0xf0) ); /* 0b11110nnn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b3 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b4 | 0x80) ); /* 0b10nnnnnn */

			++i;

		} else { /* 0x0800-0xFFFF (BMP) */
			/**
			 * 3byteUTF8��g�ݗ��Ă�.
			 */
			uint8_t b1 = (uint8_t)((u16 >> 12)       ); // u16 13-16bit(4bit)
			uint8_t b2 = (uint8_t)((u16 >>  6) & 0x3f); // u16 7-12bit(6bit)
			uint8_t b3 = (uint8_t)((u16      ) & 0x3f); // u16 1-6bit(6bit)
			ZnkBif_push_bk( utf8s, (b1 | 0xe0) ); /* 0b1110nnnn */
			ZnkBif_push_bk( utf8s, (b2 | 0x80) ); /* 0b10nnnnnn */
			ZnkBif_push_bk( utf8s, (b3 | 0x80) ); /* 0b10nnnnnn */
		}
	}
	return true;
}


/*****************************************************************************/


/*
 * Round trip incompatibility 
 *   0xa1c0   -> U+005c -> 0x5c
 *   0x8fa2b7 -> U+007e -> 0x7e
 */

/*
 *  0x0000-0x007f : ASCII
 *  0x00a1-0x00df : JIS X 0201 (�ʏ�ANK)
 *    191��
 *    ANK�i�A���N�GAlphabetic Numeric and Kana�j�́A
 *    ASCII�R�[�h��8�r�b�g�i256��ށj�Ɋg�����A
 *    �Ǝ��̃J�^�J�i�������Ĕ��p������\�����Ă���.
 *    �A���t�@�x�b�g������ASCII���̂��̂����AJIS���[�}���Ƃ��Ă�.
 *    �J�i������JIS�J�i�ƌĂ�.
 *
 *  0x8181-0xfefe : JIS X 0208 (JIS��{����)
 *    6,879���i����6,355�A�񊿎�524�j
 *    �Ђ炪�ȁA�������ȁA�����A�S�p�L���Ȃǂ�2�o�C�g����.
 *    �����R�[�h�����́A�悭�g�����1�����̊����ƁA
 *    ���܂�g���Ȃ����A�Ȃ��ƕs�ւȑ�2�����̊�����
 *    16�r�b�g�i65,536��ށj�ŕ\������悤�ɋK�肵������.
 *
 *  0x2121-0x7e7e : JIS X 0212 (JIS�⏕����)
 *    6,067���i����5,801�A�񊿎�266�j
 *    �g�p�p�x�̒Ⴂ�����ō\��������3�����̊�������є񊿎������^����Ă���.
 *    �قƂ�ǎg���邱�Ƃ��Ȃ�.
 */
/**
 * size is 65536
 */
static const uint16_t st_cctbl_ucs2_to_euc[] = {
#include "cc_table/ucs2_to_euc.cctbl"
};
/**
 * size is 9120
 */
static const uint16_t st_cctbl_euc_jisx0212_to_ucs2[] = {
#include "cc_table/euc_jisx0212_to_ucs2.cctbl"
};
/**
 * size is 8192
 *   0x005c, // 0x2140 (REVERSE SOLIDUS)
 *   0xff3c, // 0x2140 (FULLWIDTH REVERSE SOLIDUS)
 */
static const uint16_t st_cctbl_euc_to_ucs2[] = {
#include "cc_table/euc_to_ucs2.cctbl"
};


void
ZnkMbc_convertUTF16_toEUC( const uint16_t* utf16s, size_t utf16s_leng, ZnkBif eucs )
{
	size_t len = utf16s_leng; /* ����̓o�C�g�T�C�Y�ł͂Ȃ��z��̌� */
	size_t i;
	for( i=0; i<len; ++i ){

		uint16_t idx = utf16s[i];
		uint16_t e16 = st_cctbl_ucs2_to_euc[ idx ];
		if( e16 == 0 ){ /* Unknown char */
			/***
			 * ���߂ł��Ȃ�UTF16�����������ꍇ.
			 * �Ƃ肠���� '?' ������ǉ����Ă���.
			 */
			ZnkBif_push_bk( eucs, '?' );

		} else if( e16 < 0x80 ){ /* ASCII */
			ZnkBif_push_bk( eucs, (uint8_t)e16 );

		} else if( e16 > 0xa0 && e16 <= 0xdf ){ /* EUC SS2 (JIS X 0201 kana) */
			ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			ZnkBif_push_bk( eucs, (uint8_t)( e16 & 0xff ) );

		} else if( e16 >= 0x2121 && e16 <= 0x7e7e ){ /* EUC SS3 (JIS X 0212(0x2121-0x6d63) + ��) */
			ZnkBif_push_bk( eucs, (uint8_t)0x8f );
			ZnkBif_push_bk( eucs, (uint8_t)( (e16 >> 8)   | 0x80 ) ); /* e16�̏��8bit�̉���7bit + 0x80 */
			ZnkBif_push_bk( eucs, (uint8_t)( (e16 & 0xff) | 0x80 ) ); /* e16�̉���8bit�̉���7bit + 0x80 */

		} else if( e16 != 0xffff ){ /* JIS X 0208(��{����) */
			ZnkBif_push_bk( eucs, (uint8_t)( e16 >> 8 ) );   /* e16�̏��8bit */
			ZnkBif_push_bk( eucs, (uint8_t)( e16 & 0xff ) ); /* e16�̉���8bit */

		} else {
		}
	}
	return;
}

bool
ZnkMbc_convertEUC_toUTF16( const char* eucs, size_t eucs_leng, ZnkBif utf16s )
{
	const uint8_t* euc = (const uint8_t*)eucs;
	const size_t len = eucs_leng;
	
	size_t i;
	for( i=0; i<len; ++i ){
		if( euc[i] < 0x80 ){ /* ASCII */
			UTF16_addUnicodeVal16( utf16s, euc[i] );
		} else if( euc[i] == 0x8e ){ /* EUC SS2(JIS X 0201 kana) */
			uint8_t u8 = euc[i+1];
			uint16_t u16;
			if( u8 >= 0xa1 && u8 <= 0xdf ){
				u8 -= 0x40;
			} else {
				u8 = 0;
			}
			u16 = 0xff00 | u8;
			UTF16_addUnicodeVal16( utf16s, u16 );
			++i;

		} else if( euc[i] == 0x8f ){ /* EUC SS3(JIS X 0212) */
			uint8_t  hi  = euc[i+1] & 0x7f;
			uint8_t  low = euc[i+2] & 0x7f;
			size_t   idx = (hi - 0x20) * 96 + (low - 0x20);
			uint16_t u16 = 0;

			if( idx < Znk_NARY( st_cctbl_euc_jisx0212_to_ucs2 ) ){
				u16 = st_cctbl_euc_jisx0212_to_ucs2[ idx ];
			}

			if( u16 == 0 ){
				/***
				 * ���m��EUC�R�[�h�������ꍇ.
				 * �Ƃ肠���� '?' �����Ƃ��Ă���.
				 */
				u16 = '?';
			}
			UTF16_addUnicodeVal16( utf16s, u16 );
			i += 2;

		} else if( euc[i] < 0xa0 ){  /* C1 */
		} else { /* JIX X 0208 */
			uint8_t  hi  = euc[i  ] &  0x7f;
			uint8_t  low = euc[i+1] &  0x7f;
			size_t   idx = (hi - 0x20) * 96 + (low - 0x20);
			uint16_t u16 = 0;

			if ( idx < Znk_NARY( st_cctbl_euc_to_ucs2 ) ){
				u16 = st_cctbl_euc_to_ucs2[ idx ];
			}

			if (u16 == 0) {
				/***
				 * ���m��EUC�R�[�h�������ꍇ.
				 * �Ƃ肠���� '?' �����Ƃ��Ă���.
				 */
				u16 = '?';
			}
			UTF16_addUnicodeVal16( utf16s, u16 );
			++i;
		}
	}
	return true;
}



/***
 * UTF8�ł���utf8s��^���A�����EUC�ɕϊ��������ʂ�eucs�̍Ō���ɒǉ�����.
 *
 * utf8s �� ZnkBif_data(eucs) �Ɠ����aggregate�������|�C���^�ł��ꉞ����͂���.
 * ���Ƃ��� �ŏ���eucs���ɂ�UTF8�����񂪊i�[����Ă����Ƃ��āA���̖����������
 * EUC�ϊ����ꂽ�����񂪘A�������Ƃ������������ꉞ�\�ł͂���.
 * (���̌��ʂɎ��p��̈Ӗ������邩�ۂ��͒u���Ƃ���).
 *
 * ws�͗^����ꂽ���̂����Ȃ�����Ƃ������aggregate���������̂ł����Ă͂Ȃ�Ȃ�.
 */
bool
ZnkMbc_convertUTF8_toEUC( const char* utf8s, size_t utf8s_leng, ZnkBif eucs, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	if( utf8s_leng == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* �܂��N���A */
	ZnkBif_reserve( utf16s, utf8s_leng );

	result = ZnkMbc_convertUTF8_toUTF16( utf8s, utf8s_leng, utf16s, err );
	if( result ){ 
		ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2, eucs );
	}
	return result;
}
/***
 * UTF8�ł���self��^���A�����EUC�ɕϊ��������ʂ�self�ɏ㏑������.
 */
bool
ZnkMbc_convertUTF8_toEUC_self( ZnkBif self, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	if( ZnkBif_size(self) == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* �܂��N���A */
	ZnkBif_reserve( utf16s, ZnkBif_size(self) );

	result = ZnkMbc_convertUTF8_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), utf16s, err );
	if( result ){ 
		ZnkBif_resize( self, 0 ); /* �����ň�U�N���A */
		ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2, self );
	}
	return result;
}

/***
 * EUC�ł���eucs��^���A�����UTF8�ɕϊ��������ʂ�utf8s�̍Ō���ɒǉ�����.
 *
 * eucs �� utf8s �͓���̃I�u�W�F�N�g�ł��悭�A���̏ꍇeucs�̌���UTF8�ϊ����ꂽ������
 * �A������邱�ƂɂȂ�. (���̌��ʂɎ��p��̈Ӗ������邩�ۂ��͒u���Ƃ���).
 *
 * eucs �� ZnkBif_data(utf8s) �Ɠ����aggregate�������|�C���^�ł��ꉞ����͂���.
 * ���Ƃ��� �ŏ���utf8s���ɂ�EUC�����񂪊i�[����Ă����Ƃ��āA���̖����������
 * UTF8�ϊ����ꂽ�����񂪘A�������Ƃ������������ꉞ�\�ł͂���.
 * (���̌��ʂɎ��p��̈Ӗ������邩�ۂ��͒u���Ƃ���).
 *
 * ws�͗^����ꂽ���̂����Ȃ�����Ƃ������aggregate���������̂ł����Ă͂Ȃ�Ȃ�.
 */
bool
ZnkMbc_convertEUC_toUTF8( const char* eucs, size_t eucs_leng, ZnkBif utf8s, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	bool eliminate_bom = true;
	if( eucs_leng == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* �܂��N���A */
	ZnkBif_reserve( utf16s, eucs_leng );

	ZnkMbc_convertEUC_toUTF16( eucs, eucs_leng, utf16s );

	result = ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2,
			utf8s, eliminate_bom, err );
	return result;
}
/***
 * EUC�ł���self��^���A�����UTF8�ɕϊ��������ʂ�self�ɏ㏑������.
 */
bool
ZnkMbc_convertEUC_toUTF8_self( ZnkBif self, ZnkBif ws, ZnkErr* err )
{
	ZnkBif utf16s = ws;
	bool result;
	bool eliminate_bom = true;
	if( ZnkBif_size(self) == 0 ){ return true; }

	ZnkBif_resize( utf16s, 0 ); /* �܂��N���A */
	ZnkBif_reserve( utf16s, ZnkBif_size(self) );

	ZnkMbc_convertEUC_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), utf16s );
	ZnkBif_resize( self, 0 ); /* self����U�N���A����. */

	result = ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(utf16s), ZnkBif_size(utf16s)/2,
			self, eliminate_bom, err );
	return result;
}


/*****************************************************************************/

/**
 * @brief
 * �^���� uint8_t c �̒l���ASJIS��1�o�C�g�ڂ���蓾��l���ۂ��𒲂ׂ�.
 *
 * @note
 * @code
 * SJIS�̐擪�o�C�g����蓾��l
 * ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc )
 * �������A�����ł͍������̂��߁A�����ό`���������Ŕ��肵�Ă���B
 * @endcode
 */

/***
 * SJIS�̐擪�o�C�g����蓾��l
 *
 *   ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc )
 *
 * ���̔���́A�ȉ��Ɠ��l�Ƃ�������������.
 * (uint8_t�ɂ��L���X�g�͕K�{.
 * C�ł́A���Ƃ�uint8_t���m�ł����Ă��A�����Z���������_��int�ֈÖٕϊ������)
 *
 *   ( static_cast<uint8_t>( ( c^0x20 ) - 0xa1 ) <= 0x3b )
 *
 *
 * �ؖ�:
 *   ��L�̊�{������0x20��XOR���Z������ƈȉ��ɂȂ�.
 *    ( cx>=0xa1 && cx<=0xbf ) ||  ( cx>=0xc0 && cx<=0xdc )
 *   �������Acx = c^0x20 �Ƃ���.
 *   (�ʏ�A0x20��XOR���Z������ƁA�}0x20 ����邪�A
 *   ��L�͈͓̔��ł́A(���E�}0x20�ߖT�̑������l�������)�s�������ω����Ȃ�)
 *
 *    0xbf+1 = 0xc0 �Ȃ̂Ō��ǈȉ��ɂȂ�.
 *    ( cx>=0xa1 && cx<=0xdc )
 *
 *   ��L���� 0xa1 �������ƁA
 *    ( cy>=0x00 && cy<=0x3b )
 *   �������Acy = c^0x20 - 0xa1 �Ƃ���.
 *
 *   ����āAcy �� uint8_t �ɃL���X�g���Ă���̏������Ŕ���\�ƂȂ�.
 */
Znk_INLINE bool is1stByte_ofSJIS( uint8_t c )
{
	/**
	 * ��{�`:
	 * return ( c>=0x81 && c<=0x9f ) || ( c>=0xe0 && c<=0xfc );
	 */
	/* �����` */
	return (bool)( (uint8_t)( ( c^0x20 ) - 0xa1 ) <= 0x3b );
}

/***
 * Char Conversion
 */

/***
 * �Q�l:
 * http://www.tohoho-web.com/wwwkanji.htm
 * http://www.tohoho-web.com/lng/200001/00010045.htm
 */

/***
 * EUC��JIS�ꕶ���ϊ�
 */
#define M_convertEUC_toJIS( c1, c2 ) (c1) -= 0x80; (c2) -= 0x80
#define M_convertJIS_toEUC( c1, c2 ) (c1) += 0x80; (c2) += 0x80

/***
 * JIS��SJIS�ꕶ���ϊ�
 */
#define M_convertJIS_toSJIS( c1, c2 ) do{ \
	/* if (c1 % 2) */ \
	if (c1 & 0x01) { \
		/* c1 = ((c1 + 1) / 2) + 0x70; */ \
		c1 = ((c1 + 1) >> 1) + 0x70; \
		c2 += 0x1f; \
	} else { \
		/* c1 = (c1 / 2) + 0x70; */ \
		c1 = (c1 >> 1) + 0x70; \
		c2 += 0x7d; \
	} \
	if (c1 >= 0xa0) { c1 += 0x40; } \
	if (c2 >= 0x7f) { ++c2; } \
}while(0)

#define M_convertSJIS_toJIS( c1, c2 ) do{ \
	if (c1 >= 0xe0) { c1 -= 0x40; } \
	if (c2 >= 0x80) { --c2; } \
 \
	if (c2 >= 0x9e) { \
		/* c1 = (c1 - 0x70) * 2; */ \
		c1 = ( (c1 - 0x70) << 1 ); \
		c2 -= 0x7d; \
	} else { \
		/* c1 = ((c1 - 0x70) * 2) - 1; */ \
		c1 = ( (c1 - 0x70) << 1 ) - 1; \
		c2 -= 0x1f; \
	} \
}while(0)

/***
 * EUC��SJIS�ꕶ���ϊ�
 * �Ƃ肠������UJIS�ϊ�����̂���΂炵��.
 */
#define M_convertEUC_toSJIS( c1, c2 ) \
	M_convertEUC_toJIS ( c1, c2 ); M_convertJIS_toSJIS( c1, c2 )

#define M_convertSJIS_toEUC( c1, c2 ) \
	M_convertSJIS_toJIS( c1, c2 ); M_convertJIS_toEUC ( c1, c2 )

/*
 * endof Char Conversion
 ***/

/***
 * EUC��SJIS�ꕶ���ϊ�
 * �Ƃ肠������UJIS�ϊ�����̂���΂炵��.
 */
#define M_convertEUC_toSJIS( c1, c2 ) \
	M_convertEUC_toJIS ( c1, c2 ); M_convertJIS_toSJIS( c1, c2 )

#define M_convertSJIS_toEUC( c1, c2 ) \
	M_convertSJIS_toJIS( c1, c2 ); M_convertJIS_toEUC ( c1, c2 )

/***
 * eucs => sjis �ւƃR���o�[�g����.
 * eucs �� sjis �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
Znk_INLINE void
convertStr_EUC_toSJIS( const char* eucs, size_t eucs_leng, ZnkBif sjis )
{
	uint8_t ch1;
	uint8_t ch2;
	size_t idx = 0;
	if( eucs_leng == 0 ){ return; }

	while( idx < eucs_leng ){
		ch1 = eucs[ idx ];

		/***
		 * EUC�Ŕ��p�J�^�J�i�́AEUC(SS2)�ƌĂ΂�Ă���.
		 * �R�[�h�́A0x8E 0xA1 ... 0x8E 0xDF �܂łɊ��蓖�Ă�ꂽ�}���`�o�C�g�����ŁA
		 * SJIS �̂P�o�C�g�����ł��锼�p�J�i(0xA1�`0xDF)�ɑ�������.
		 * ���̂��߁A���� EUC��SJIS �ϊ��́A��1�o�C�g���̂āA��2�o�C�g���c���悤�ɂ���΂悢.
		 */
		if( ch1 == 0x8E ){
			if( idx+1 < eucs_leng ){
				ch2 = eucs[ idx+1 ];
				if( ch2 >= 0xA1 && ch2 <= 0xDF ){
					ZnkBif_push_bk( sjis, ch2 );
					idx += 2;
					continue;
				}
			}
		}

		if( ch1 < 0xA1 ){
			/***
			 * �}���`�o�C�g�ł͂Ȃ�.
			 * �Ƃ肠����ASCII �Ƃ݂Ȃ�.
			 */
			ZnkBif_push_bk( sjis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1 �� EUC �̃}���`�o�C�g�����̑�1����.
		 */
		if( idx+1 >= eucs_leng ){ break; }
		ch2 = eucs[ idx+1 ];
		M_convertEUC_toSJIS( ch1, ch2 );

		ZnkBif_push_bk( sjis, ch1 );
		ZnkBif_push_bk( sjis, ch2 );
		idx += 2;
	}
	return;
}


/***
 * sjis => eucs �ւƃR���o�[�g����.
 * sjis �� eucs �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
Znk_INLINE void
convertStr_SJIS_toEUC( const char* sjis, size_t sjis_leng, ZnkBif eucs )
{
	uint8_t ch1;
	uint8_t ch2;
	size_t idx = 0;
	if( sjis_leng == 0 ){ return; }

	while( idx < sjis_leng ){
		ch1 = sjis[ idx ];

		/***
		 * SJIS ���p�J�^�J�i
		 *
		 * EUC�Ŕ��p�J�^�J�i�́AEUC(SS2)�ƌĂ΂�Ă���.
		 * �R�[�h�́A0x8E 0xA1 ... 0x8E 0xDF �܂łɊ��蓖�Ă��A
		 * SJIS �̔��p�J�i(0xA1...0xDF)�ɑ�������.
		 * ���̂��߁A���� SJIS��EUC �ϊ��́A��1�o�C�g�̑O�ɐV���� 0x8E ��ǉ�����΂悢.
		 */
		if( ch1 >= 0xA1 && ch1 <= 0xDF ){
			ZnkBif_push_bk( eucs, (uint8_t)0x8E );
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		/***
		 * SJIS �̃}���`�o�C�g�ł͂Ȃ�.
		 * ASCII �Ƃ݂Ȃ�.
		 */
		if( !is1stByte_ofSJIS(ch1) ){
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1 �� SJIS �̃}���`�o�C�g�����̑�1����.
		 */
		if( idx+1 >= sjis_leng ){ break; }
		ch2 = sjis[ idx+1 ];
		M_convertSJIS_toEUC( ch1, ch2 );

		ZnkBif_push_bk( eucs, ch1 );
		ZnkBif_push_bk( eucs, ch2 );
		idx += 2;
	}
}


typedef enum {
	 JIS_ES_NONE=0
	,JIS_ES_ASCII
	,JIS_ES_0201_ROMAN
	,JIS_ES_0201_KATAKANA
	,JIS_ES_0208_1978
	,JIS_ES_0208_1983
	,JIS_ES_0208_1990
	,JIS_ES_0212_1990
} JIS_ES_Type;

/***
 * @brief
 *   str��JIS������Ƃ��Aidx��EscapeSequence�̊J�n�ʒu�������Ƃ���.
 *   ���̂Ƃ��A���̈ʒu�������������}���`�o�C�g���ۂ�(1byte������2byte������)��
 *   �����t���O�l�� is_multibyte �Ƃ��Ď擾���AEscapeSequence�̎��(����͗v�����
 *   �����R�[�h�̎�ނ�����)��Ԃ�. �܂� idx �̒l��EscapeSequence�̍Ō�̈ʒu��
 *   ���̈ʒu���w���悤�ɏ㏑���X�V�����.
 *
 * �Q�l: http://www.tohoho-web.com/wwwkanji.htm
 *
 * ESC : 0x1b
 * $   : 0x24
 * &   : 0x26
 * (   : 0x28
 * @   : 0x40
 * B   : 0x42
 * D   : 0x44
 * J   : 0x4a
 *
 * ASCII            : ESC ( B
 * JIS X 0201 Roman : ESC ( J
 * JIS X 0201 ���p�J�^�J�i : ESC ( I
 * JIS X 0208 1978  : ESC $ @
 * JIS X 0208 1983  : ESC $ B 
 * JIS X 0208 1990  : ESC & @ ESC $ B
 * JIS X 0212 1990  : ESC $ ( D 
 *
 * ��: JIS X 0201 Roman �� ASCII�Ƃقړ��������A
 *     �o�b�N�X���b�V���� �~�}�[�N�ɂȂ��Ă���̂� 
 *     �`���_���I�[�o�[���C���ɂȂ��Ă���_���قȂ�.
 */
Znk_INLINE JIS_ES_Type
JIS_processEscapeSequence( const char* str, size_t str_leng, size_t* idx, bool* is_multibyte )
{
	const uint8_t ec0 = (uint8_t)(str[ *idx ]);
	const size_t size = str_leng;

	if( ec0 != 0x1b ){ return JIS_ES_NONE; }
	if( (*idx)+2 >= size ){
		return JIS_ES_NONE;
	} else {
		const uint8_t ec1 = str[ (*idx)+1 ];
		const uint8_t ec2 = str[ (*idx)+2 ];
	
		switch( ec1 ){
		case '$' :
			switch( ec2 ){
			case '@':
				/***
				 * 2�o�C�g�����̊J�n
				 *   JIS X 0208 1978  : ESC $ @
				 */
				(*idx) += 3;
				*is_multibyte = true;
				return JIS_ES_0208_1978;
			case 'B':
				/***
				 * 2�o�C�g�����̊J�n
				 *   JIS X 0208 1983  : ESC $ B 
				 */
				(*idx) += 3;
				*is_multibyte = true;
				return JIS_ES_0208_1983;
			case '(':
				if( (*idx)+3 < size && str[ (*idx)+3 ] == 'D' ){
					/***
					 * 2�o�C�g�����̊J�n
					 *   JIS X 0212 1990  : ESC $ ( D 
					 */
					(*idx) += 4;
					*is_multibyte = true;
					return JIS_ES_0212_1990;
				}
			default:
				break;
			}
			break;
	
		case '(' :
			switch( ec2 ){
			case 'B':
				/***
				 * 1�o�C�g�����̊J�n
				 *   ASCII            : ESC ( B
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_ASCII;
			case 'J':
				/***
				 * 1�o�C�g�����̊J�n
				 *   JIS X 0201 Roman : ESC ( J
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_0201_ROMAN;
			case 'I':
				/***
				 * 1�o�C�g�����̊J�n
				 *   JIS X 0201 ���p�J�^�J�i : ESC ( I
				 */
				(*idx) += 3;
				*is_multibyte = false;
				return JIS_ES_0201_KATAKANA;
			default:
				break;
			}
			break;
		case '&':
			if( (*idx)+5 < size ){
				if ( ec2 == '@'
				  && str[ (*idx)+3 ] == 0x1b
				  && str[ (*idx)+4 ] == '$'
				  && str[ (*idx)+5 ] == 'B' )
				{
					/***
					 * 2�o�C�g�����̊J�n
					 *   JIS X 0208 1990  : ESC & @ ESC $ B
					 */
					(*idx) += 6;
					*is_multibyte = true;
					return JIS_ES_0208_1990;
				}
			}
			break;
	
		default:
			break;
		}
	}

	return JIS_ES_NONE;
}

#define M_DEFINE_JIS_BEGINER_ASCII     static const uint8_t st_beginer_ascii[]     = { 0x1b, '(', 'B', }      /* ASCII���̗p���� */ 
#define M_DEFINE_JIS_BEGINER_0208_1983 static const uint8_t st_beginer_0208_1983[] = { 0x1b, '$', 'B', }      /* JIS X 0208-1983���̗p����. */
#define M_DEFINE_JIS_BEGINER_0212_1990 static const uint8_t st_beginer_0212_1990[] = { 0x1b, '$', '(', 'D', } /* JIS X 0212 1990 */
#define M_DEFINE_JIS_BEGINER_KATAKANA  static const uint8_t st_beginer_katakana[]  = { 0x1b, '(', 'I', }      /* JIS X 0201 KATAKANA */

#define M_APPEND_BEGINER( msl, beginer ) \
	ZnkBif_append( (msl), beginer, sizeof(beginer) )

/***
 * sjis => jis �ւƃR���o�[�g����.
 * sjis �� jis �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
Znk_INLINE void
convertStr_SJIS_toJIS( const char* sjis, size_t sjis_leng, ZnkBif jis )
{
	M_DEFINE_JIS_BEGINER_ASCII;
	M_DEFINE_JIS_BEGINER_0208_1983;

	uint8_t ch1;
	uint8_t ch2;
	JIS_ES_Type conv_state = JIS_ES_ASCII;
	size_t idx = 0;

	if( sjis_leng == 0 ){ return; }

	while( idx < sjis_leng ){
		ch1 = sjis[ idx ];

		/***
		 * SJIS ���p�J�^�J�i
		 *
		 * JIS(iso-2022-jp)�ɂ͔��p�J�^�J�i���Ȃ����A
		 * JIS X 0201 KATAKANA �ɕϊ������邱�Ƃɂ���.
		 */
		if( ch1 >= 0xA1 && ch1 <= 0xDF ){
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * �}���`�o�C�g���ۂ��𔻒�
		 */
		if( is1stByte_ofSJIS( ch1 ) ){
			if( conv_state != JIS_ES_0208_1983 ){
				conv_state  = JIS_ES_0208_1983;
				/***
				 * �}���`�o�C�g�̊J�n.
				 */
				M_APPEND_BEGINER( jis, st_beginer_0208_1983 );
			}
		} else {
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				/***
				 * ASCII�̊J�n.
				 */
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
		}

		/***
		 * ch1��ASCII
		 */
		if( conv_state == JIS_ES_ASCII ){
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1,ch2 �� SJIS 2byte����
		 */
		if( idx+1 >= sjis_leng ){ break; }
		ch2 = sjis[ idx+1 ];
		M_convertSJIS_toJIS( ch1, ch2 );

		ZnkBif_push_bk( jis, ch1 );
		ZnkBif_push_bk( jis, ch2 );
		idx += 2;
	}

	/***
	 * ASCII(1byte����)�Ŋ������Ă��Ȃ��ꍇ�́A�Ō��ASCII�̊J�n��ǉ�.
	 */
	if( ZnkBif_size(jis) > 0 && conv_state != JIS_ES_ASCII ){
		M_APPEND_BEGINER( jis, st_beginer_ascii );
	}
}


/***
 * eucs => jis �ւƃR���o�[�g����.
 * eucs �� jis �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
void Znk_INLINE
convertStr_EUC_toJIS( const char* eucs, size_t eucs_leng, ZnkBif jis )
{
	M_DEFINE_JIS_BEGINER_ASCII;
	M_DEFINE_JIS_BEGINER_0208_1983;
	M_DEFINE_JIS_BEGINER_0212_1990;

	uint8_t ch1;
	uint8_t ch2;
	uint8_t ch3;
	JIS_ES_Type conv_state = JIS_ES_ASCII;
	size_t idx = 0;

	if( eucs_leng == 0 ){ return; }

	while( idx < eucs_leng ){
		ch1 = eucs[ idx ];

		if( ch1 == 0x8e ){
			/***
			 * EUC�Ŕ��p�J�^�J�i(SS2)�̔���.
			 *
			 * JIS(iso-2022-jp)�ɂ͔��p�J�^�J�i���Ȃ����A
			 * JIS X 0201 KATAKANA �ɕϊ������邱�Ƃɂ���.
			 * 
			 * vim(libiconv)�Ō��ʂ��m�F����ƁAcp932 �Ƃ݂Ȃ���邩������Ȃ�.
			 */
			if( idx+1 < eucs_leng ){ 
				ch2 = eucs[ idx+1 ];
				if( ch2 >= 0xA1 && ch2 <= 0xDF ){
#if 0
					if( conv_state != JIS_ES_0201_KATAKANA ){
						conv_state  = JIS_ES_0201_KATAKANA;
						M_APPEND_BEGINER( jis, st_beginer_katakana );
					}
#endif
					if( conv_state != JIS_ES_ASCII ){
						conv_state  = JIS_ES_ASCII;
						M_APPEND_BEGINER( jis, st_beginer_ascii );
					}
					ZnkBif_push_bk( jis, ch2 );
					idx += 2;
					continue;
				}
			}
		}

		/***
		 * EUC 3byte����(SS3)�̔���.
		 *   ���̂Ƃ��AJIS X 0212 1990 �ɕϊ������邱�Ƃɂ���.
		 *
		 * vim(libiconv)�Ō��ʂ��m�F����ꍇ�́Aencoding=iso-2022-jp-2 �Ǝw��
		 * ���Ȃ���΂Ȃ�Ȃ���������Ȃ�.
		 */
		if( ch1 == 0x8f ){
			if( idx+2 < eucs_leng ){ 
				ch2 = eucs[ idx+1 ];
				ch3 = eucs[ idx+2 ];
				if( conv_state != JIS_ES_0212_1990 ){
					conv_state  = JIS_ES_0212_1990;
					M_APPEND_BEGINER( jis, st_beginer_0212_1990 );
				}
				ZnkBif_push_bk( jis, (uint8_t)(ch2 - 0x80) );
				ZnkBif_push_bk( jis, (uint8_t)(ch3 - 0x80) );
				idx += 3;
				continue;
			}
		}

		/***
		 * �}���`�o�C�g���ۂ��𔻒�
		 */
		if( ch1 >= 0xa1 ){
			if( conv_state != JIS_ES_0208_1983 ){
				conv_state  = JIS_ES_0208_1983;
				/***
				 * �}���`�o�C�g�̊J�n.
				 */
				M_APPEND_BEGINER( jis, st_beginer_0208_1983 );
			}
		} else {
			if( conv_state != JIS_ES_ASCII ){
				conv_state  = JIS_ES_ASCII;
				/***
				 * ASCII�̊J�n.
				 */
				M_APPEND_BEGINER( jis, st_beginer_ascii );
			}
		}

		/***
		 * 1byte����.
		 */
		if( conv_state == JIS_ES_ASCII ){
			ZnkBif_push_bk( jis, ch1 );
			++idx;
			continue;
		}

		/***
		 * ch1, ch2 �� EUC 2byte����.
		 */
		if( idx+1 >= eucs_leng ){ break; }
		ch2 = eucs[ idx+1 ];
		M_convertEUC_toJIS( ch1, ch2 );

		ZnkBif_push_bk( jis, ch1 );
		ZnkBif_push_bk( jis, ch2 );
		idx += 2;

	}

	/***
	 * ASCII�Ŋ������Ă��Ȃ��ꍇ�́A�Ō��ASCII�̊J�n��ǉ�.
	 */
	if( ZnkBif_size(jis) > 0 && conv_state != JIS_ES_ASCII ){
		M_APPEND_BEGINER( jis, st_beginer_ascii );
	}
}


/***
 * jis => sjis �ւƃR���o�[�g����.
 * jis �� sjis �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
Znk_INLINE void
convertStr_JIS_toSJIS( const char* jis, size_t jis_leng, ZnkBif sjis )
{
	uint8_t ch1;
	uint8_t ch2;
	bool    is_multibyte = false;
	size_t idx = 0;

	if( jis_leng == 0 ){ return; }

	while( idx<jis_leng ){
		JIS_processEscapeSequence( jis, jis_leng, &idx, &is_multibyte );

		if( idx >= jis_leng ){ break; }
		ch1 = jis[ idx ];
		/***
		 * JIS 1byte����.
		 * ���p�J�^�J�i�̏ꍇ�͂��̂܂�(SJIS�Ƌ���).
		 */
		if( !is_multibyte ){
			ZnkBif_push_bk( sjis, ch1 );
			++idx;
			continue;
		}

		if( idx+1 >= jis_leng ){ break; }
		ch2 = jis[ idx+1 ];
		/***
		 * ch1,ch2 �� JIS 2byte����
		 */
		M_convertJIS_toSJIS( ch1, ch2 );

		ZnkBif_push_bk( sjis, ch1 );
		ZnkBif_push_bk( sjis, ch2 );
		idx += 2;

	}
}


/***
 * jis => eucs �ւƃR���o�[�g����.
 * jis �� eucs �͓���I�u�W�F�N�g�ł����Ă͂Ȃ�Ȃ�.
 */
Znk_INLINE void
convertStr_JIS_toEUC( const char* jis, size_t jis_leng, ZnkBif eucs )
{
	uint8_t ch1;
	uint8_t ch2;
	bool    is_multibyte = false;
	JIS_ES_Type es_type = JIS_ES_NONE;
	size_t idx = 0;

	if( jis_leng == 0 ){ return; }

	while( idx<jis_leng ){
		es_type = JIS_processEscapeSequence( jis, jis_leng, &idx, &is_multibyte );

		if( idx >= jis_leng ){ break; }
		ch1 = jis[ idx ];
		/***
		 * JIS 1byte����.
		 * ���p�J�^�J�i�̏ꍇ�� 0x8e ��ǉ�����.
		 */
		if( !is_multibyte ){
			if( es_type == JIS_ES_0201_KATAKANA ){
				/***
				 * JIS7bit���p�J�^�J�i�Ƃ݂Ȃ�.
				 */
				ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			} else if( ch1 >= 0xA1 && ch1 <= 0xDF ){
				/***
				 * JIS8bit���p�J�^�J�i�Ƃ݂Ȃ�.
				 */
				ZnkBif_push_bk( eucs, (uint8_t)0x8e );
			}
			ZnkBif_push_bk( eucs, ch1 );
			++idx;
			continue;
		}

		if( idx+1 >= jis_leng ){ break; }
		ch2 = jis[ idx+1 ];
		/***
		 * ch1,ch2 �� JIS 2byte����
		 */
		M_convertJIS_toEUC( ch1, ch2 );

		ZnkBif_push_bk( eucs, ch1 );
		ZnkBif_push_bk( eucs, ch2 );
		idx += 2;
	}
}


#define M_DIFFER_SELF_CONVERT( conv_func ) \
	ZnkBif_resize( tmp, 0 ); \
	ZnkBif_reserve( tmp, ZnkBif_size(self) ); \
	conv_func( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp ); \
	ZnkBif_swap( self, tmp )

#define M_PREPARE_TMP \
	ZnkBif_resize( tmp, 0 ); \
	ZnkBif_reserve( tmp, ZnkBif_size(self) )

void
ZnkMbc_convert_self( ZnkBif self, ZnkMbcType src_mbc_type, ZnkMbcType dst_mbc_type, ZnkBif tmp, ZnkErr* err )
{
	switch( src_mbc_type ){
	case ZnkMbcType_EUC:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  break;
		case ZnkMbcType_SJIS: M_DIFFER_SELF_CONVERT( convertStr_EUC_toSJIS ); break;
		case ZnkMbcType_JIS:  M_DIFFER_SELF_CONVERT( convertStr_EUC_toJIS  ); break;
		case ZnkMbcType_UTF8: ZnkMbc_convertEUC_toUTF8_self( self, tmp, err ); /* ��p�̊֐���p����̂��ő� */ break;
		case ZnkMbcType_UTF16:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertEUC_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_swap( tmp, self );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_SJIS:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  M_DIFFER_SELF_CONVERT( convertStr_SJIS_toEUC ); break;
		case ZnkMbcType_SJIS: break;
		case ZnkMbcType_JIS:  M_DIFFER_SELF_CONVERT( convertStr_SJIS_toJIS ); break;
		case ZnkMbcType_UTF8:
#if 0
			/* �ЂƂ܂� SJIS=>EUC�ϊ��������̂�tmp�֊i�[ */
			M_PREPARE_TMP;
			convertStr_SJIS_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_resize( self, 0 ); /* �����ň�U self���N���A */
			ZnkMbc_convertEUC_toUTF8( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self, tmp2, err );
#else
			ZnkMbc_convert_self( self, ZnkMbcType_SJIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convertEUC_toUTF8_self( self, tmp, err );
#endif
			break;
		case ZnkMbcType_UTF16:
			ZnkMbc_convert_self( self, ZnkMbcType_SJIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,  ZnkMbcType_UTF16, tmp, err );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_JIS:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:  M_DIFFER_SELF_CONVERT( convertStr_JIS_toEUC  ); break;
		case ZnkMbcType_SJIS: M_DIFFER_SELF_CONVERT( convertStr_JIS_toSJIS ); break;
		case ZnkMbcType_JIS:  break;
		case ZnkMbcType_UTF8:
#if 0
			/* �ЂƂ܂� JIS=>EUC�ϊ��������̂�tmp�֊i�[ */
			M_PREPARE_TMP;
			convertStr_JIS_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp );
			ZnkBif_resize( self, 0 ); /* �����ň�U self���N���A */
			ZnkMbc_convertEUC_toUTF8( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self, tmp2, err );
#else
			ZnkMbc_convert_self( self, ZnkMbcType_JIS, ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convertEUC_toUTF8_self( self, tmp, err );
#endif
			break;
		case ZnkMbcType_UTF16:
			ZnkMbc_convert_self( self, ZnkMbcType_JIS,  ZnkMbcType_EUC,   tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,  ZnkMbcType_UTF16, tmp, err );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_UTF8:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC: ZnkMbc_convertUTF8_toEUC_self( self, tmp, err ); /* ��p�̊֐���p����̂��ő� */ break;
		case ZnkMbcType_SJIS:
#if 0
			/* �ЂƂ܂� UTF8=>EUC�ϊ��������̂�tmp�֊i�[ */
			M_PREPARE_TMP;
			if( !ZnkMbc_convertUTF8_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, tmp2, err ) ){
				break;
			}
			ZnkBif_resize( self, 0 ); /* �����ň�U self���N���A */
			convertStr_EUC_toSJIS( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self );
#else
			ZnkMbc_convertUTF8_toEUC_self( self, tmp, err );
			M_DIFFER_SELF_CONVERT( convertStr_EUC_toSJIS );
#endif
			break;
		case ZnkMbcType_JIS:
#if 0
			/* �ЂƂ܂� UTF8=>EUC�ϊ��������̂�tmp�֊i�[ */
			M_PREPARE_TMP;
			if( !ZnkMbc_convertUTF8_toEUC( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, tmp2, err ) ){
				break;
			}
			ZnkBif_resize( self, 0 ); /* �����ň�U self���N���A */
			convertStr_EUC_toJIS( (char*)ZnkBif_data(tmp), ZnkBif_size(tmp), self );
#else
			ZnkMbc_convertUTF8_toEUC_self( self, tmp, err );
			M_DIFFER_SELF_CONVERT( convertStr_EUC_toJIS );
#endif
			break;
		case ZnkMbcType_UTF8: break;
		case ZnkMbcType_UTF16:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF8_toUTF16( (char*)ZnkBif_data(self), ZnkBif_size(self), tmp, err );
			ZnkBif_swap( self, tmp );
			break;
		default: break;
		}
		break;

	case ZnkMbcType_UTF16:
		switch( dst_mbc_type ){
		case ZnkMbcType_EUC:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF16_toEUC( (uint16_t*)ZnkBif_data(self), ZnkBif_size(self)/2, tmp );
			ZnkBif_swap( self, tmp );
			break;
		case ZnkMbcType_SJIS:
			ZnkMbc_convert_self( self, ZnkMbcType_UTF16, ZnkMbcType_EUC,  tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,   ZnkMbcType_SJIS, tmp, err );
			break;
		case ZnkMbcType_JIS:
			ZnkMbc_convert_self( self, ZnkMbcType_UTF16, ZnkMbcType_EUC, tmp, err );
			ZnkMbc_convert_self( self, ZnkMbcType_EUC,   ZnkMbcType_JIS, tmp, err );
			break;
		case ZnkMbcType_UTF8:
			ZnkBif_resize( tmp, 0 );
			ZnkBif_reserve( tmp, ZnkBif_size(self) );
			ZnkMbc_convertUTF16_toUTF8( (uint16_t*)ZnkBif_data(self), ZnkBif_size(self)/2, tmp, true, err );
			ZnkBif_swap( self, tmp );
			break;
		case ZnkMbcType_UTF16:
			break;
		default: break;
		}
		break;

	default: break;
	}
}


