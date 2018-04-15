#ifndef INCLUDE_GUARD__Znk_mem_find_h__
#define INCLUDE_GUARD__Znk_mem_find_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN


/*****************************************************************************/
/* lfind/rfind */

/**
 * @brief
 *   buf�� 16/32/64bit�f�[�^num����Ȃ�z��Ƃ���.
 *   val�ɓ������ŏ��̗v�f���̃C���f�b�N�X��Ԃ�.
 *   ������Ȃ������ꍇ��Znk_NPOS��Ԃ�.
 *
 * @note
 *   uint8_t* �̃|�C���^���L���X�g���Ă����̊֐��ɓn���ꍇ�A���̃A�h���X�l��
 *   �o�C�g���E�ɂȂ���΂Ȃ�Ȃ�. �Ⴆ�� uint8_t* ptr �� (const uint16_t*)ptr �Ȃ�
 *   �Ƃ��āAZnkAry_lfind_16�֓n���ꍇ�Aptr �̃A�h���X�l�͂Q�̔{���łȂ���΂Ȃ�Ȃ�.
 *   �����Ȃ���΁A�����n�ɂ���Ă͊��Ғʂ�̓�������邩������Ȃ��i���������̏ꍇ
 *   �ł����Ғʂ�̑��x�����@�B��ɖ|�󂳂��Ƃ͌���Ȃ�)���A�N���b�V�����邩��
 *   ����Ȃ�(�v����ɖ���`�̓���ƂȂ�). 
 */
size_t ZnkAry_lfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_sz( const size_t*   buf, size_t   val, size_t num, size_t delta );
size_t ZnkAry_rfind_sz( const size_t*   buf, size_t   val, size_t num, size_t delta );


/**
 * @brief
 *   8/16/24/32/64bit��f�z��size�o�C�g�͈͓̔���padding�Ȃ��ɃV���A���C�Y���ꂽ
 *   �C���[�W(buf)����Aval�ɓ������ŏ��̉�f�̃C���f�b�N�X��Ԃ�.
 *   (������24bit�ɂ����ẮAval�̉���24bit�ɓ��������̂�T��. val�̏��8bit�ɂ���
 *   bit�l�͒P�ɖ��������. �����Ȏd�l�ɂ��Ă�param is_swap�̐������Q��).
 *   �����Ō����C���f�b�N�X�Ƃ́A�����̉�f�̘A���������Ȕz��Ƃ݂Ȃ����ꍇ�ɂ�����
 *   �C���f�b�N�X�ł���16bit�ȏ�̏ꍇ�o�C�g�ʒu�Ƃ͓������Ȃ�Ȃ����Ƃɒ��ӂ���.
 *
 *   ������Ȃ������ꍇ��Znk_NPOS��Ԃ�.
 *
 * @param buf :
 *   buf�̊J�n�A�h���X.
 *   ���̊֐��ł́Abuf�̃A�h���X�l�̓o�C�g���E���C�ɂ��Ȃ��Ă��悢.
 *   (�C�ӂ̔{���A�h���X���J�n�ʒu�Ƃ��邱�Ƃ��ł���).
 *   �Ⴆ�Ί�A�h���X����16bit�v�f�̘A�����n�܂��Ă��Ă��悢.
 *
 * @param size :
 *   size��buf�̃o�C�g�T�C�Y���w�肷�邱�Ƃɒ��ӂ���.
 *   �܂� size ���AU=sizeof(val)�̔{��(24bit�̏ꍇ��U=3)�łȂ��ꍇ�́A
 *   �ŏI�̒[���o�C�g���������������̂��w�肳�ꂽ�Ƃ݂Ȃ�. 
 *
 * @param delta :
 *   �����Ŏw�肵�������V�[�N�ɂ����鑝���Ƃ���.
 *   �Ⴆ�Ε��ʂɂP�v�f�����ׂĂ��������ꍇ�� 1 ���w�肷��.
 *   2 ���w�肵���ꍇ�A0, 2, 4, 6, ... �ԖځA3 �̏ꍇ 0, 3, 6, 9,... �Ƃ����悤��
 *   delta�̔{���Ԗڂ̗v�f�݂̂����ׂ��A����ȊO�̗v�f�ɂ͌����l�͂Ȃ����̂Ƃ���
 *   �X�L�b�v�����(���Ƃ����݂��Ă�����͖��������).
 *   delta �� 0 ���w�肵���ꍇ�A1 ���w�肳�ꂽ�̂Ɠ����Ƃ݂Ȃ����.
 *
 * @param is_LE :
 *   8/24bit�ňȊO�ł݈̂Ӗ�������.
 *   ���̏ꍇ�Aval�̓p�^�[���Ƃ��������Z�p�I�Ȑ����l�Ƃ��Ă̈Ӗ����������L�[�ł���A
 *   buf�������l�Ƃ��ăV���A���C�Y����Ă�����̂ł���ƍl����.
 *   buf���̗v�f�l�� LE(Little Endian)�ŃV���A���C�Y����Ă�����̂Ƃ݂Ȃ��ꍇ��true,
 *   BE(Big Endian)�ŃV���A���C�Y����Ă�����̂Ƃ݂Ȃ��ꍇ��false���w�肷��.
 *
 *   �w�肷��val�͂��̏����n�l�C�e�B�u�̒l�����̂܂ܗ^����΂悢.
 *   �Ⴆ�� uint32_t �^�� 1 �Ƃ����l��T���ꍇ�A�����n��Endian���ǂ��Ȃ��Ă��邩��
 *   ���̊֐��������Ŏ����I�ɔ��f���A�܂����̌��ʂ�is_LE�̎w��ɉ����ĕK�v�ɉ�����
 *   �����I�� byte swap �����s����. ����ɂ��buf�� LE, BE�̂�����ŃV���A���C�Y
 *   ����Ă��悤�Ƃ� uint32_t �^ 1 �����Ғʂ� find ���邱�Ƃ��ł���.
 *
 * @param is_swap :
 *   24bit�łł݈̂Ӗ�������.
 *   ���̏ꍇ�Aval�̓p�^�[���Ƃ��Ă̈Ӗ����������L�[�ł���A�Z�p�I�Ȑ����l�Ƃ��Ă̈Ӗ�������
 *   �Ȃ����̂ƍl����.
 *   4byte ���� val �l��C�����bit���Z�I�ɏ�ʃo�C�g���� A B C D �ō\������Ă���悤�Ɍ�������̂Ƃ���B
 *   is_swap��false�̂Ƃ��A�z��Ō�����Key[3] = { B, C, D } �ō\�������o�C�g���buf����T������.
 *   is_swap��true�̂Ƃ��A����Ƃ͋t�� Key[3] = { D, C, B } �ō\�������o�C�g���buf����T������.
 *   ������ɂ���A�ŏ�ʃo�C�g��A�͖��������.
 *
 *   �J��Ԃ��ɂȂ邪�A�w�肷��val�̏��,���ʂ̊T�O��C�ł�bit���Z�̈����ōl����.
 *   �Ⴆ�� uint32_t �^�� 1 �Ƃ����l���w�肵�Ais_swap��false�̏ꍇ�A
 *   BE���Ȃ��val���ۃ�������ɂ� { 0, 0, 0, 1 } �ƃV���A���C�Y����Ă��邪
 *   C��bit���Z��ł̏��,���ʂ��l�����ꍇ�AA=0, B=0, C=0, D=1 �ƂȂ�A
 *   �܂� 0 0 1 �Ƃ����V�[�P���X���T������邱�ƂɂȂ�.
 *   LE���Ȃ��val���ۃ�������ɂ� { 1, 0, 0, 0 } �ƃV���A���C�Y����Ă��邪
 *   C��bit���Z��ł̏��,���ʂ��l�����ꍇ�ABE�Ɠ����� A=0, B=0, C=0, D=1 �ƂȂ�A
 *   ��͂� 0 0 1 �Ƃ����V�[�P���X���T������邱�ƂɂȂ�.
 *   �v����ɁA���� 1 �Ƃ����l��val��^�����ꍇ�ABE/LE�ɂ���ċ����̍��͂Ȃ�.
 *
 * @note
 *   �����̊֐��ł́A�A�h���X�l���o�C�g���E�ɐ������Ă��邩�ǂ����������
 *   �����I�ɔ��肷��. �o�C�g���E�ɂ���ꍇ�͂�荂����ZnkAry_*�֐��������I��
 *   �Ăяo����A�����ł͂Ȃ��ꍇ��shift���Z�Ȃǂ���g�����m���ȏ������s����.
 *
 * ��
 * buf = { 9, 9, 0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 8 } �Ƃ���.
 * �ȉ��ł� val ��byte-image�� {1,0} �Ȃǂƕ\�L����.
 * �Ⴆ�΁ALE��uint16_t val=1 �̂Ƃ��Aval={1,0}�ȂǂƏ���.
 * BE�̏ꍇ�ł���΁A���ꂪ{0,1}�ƂȂ�. �V���A���C�Y����Ă���o�C�g��̊e��
 * LE��BE�̂ǂ���̌`���Ŋi�[����Ă��邩��is_LE�t���O�Ŏw�肷��.
 *
 * ZnkMem_lfind_16 �ł� {9,9}, {0,1}, {2,3}, {4,5}, {6,7}, {9,9} �Ƃ������uint16_t�Ƃ݂āA
 * ���ɒ��ׂ�. �Ō�� 8 �͗]��ɂȂ邽�߁A���������.
 * �����Ǝw�肵�� val �����������ۂ��𒲂ׁA���������̂����������ŏ��̃C���f�b�N�X��Ԃ�.
 * �Ⴆ�΁A�ȉ��̂悤�ɂȂ�.
 *
 *   val={2,3}�̏ꍇ�A2 ���Ԃ����(�z��̗v�f�Ƃ��Ă�2�Ԗڂɂ��邽��).
 *   val={1,2}�̏ꍇ�AZnk_NPOS���Ԃ����. 
 *                    �o�C�g��Ƃ��Ă�[3,5)�̈ʒu�ɑ��݂�����̂́A
 *                    ��L�̉�̒��Ɋ܂܂�Ȃ����߁A���m����Ȃ�.
 *   val={9,9}�̏ꍇ�A0 ���Ԃ����.
 *
 * ����� delta=2 ���w�肵���ꍇ�A��L�̉� {9,9}, {2,3}, {6,7} �݂̂ɂȂ�.
 * (delta�̔{���Ԗ�(������0���琔����)�ȊO�̗v�f�̓X�L�b�v�����)
 * ���̂Ƃ��Aval={2,3}�̏ꍇ�́A2 ���Ԃ����(���������ꍇ�ł����
 * ���ʂ� delta=1�̂Ƃ��Ɠ����ł���). val={1,0}�̏ꍇ�́A�X�L�b�v�����̂Ō��m����Ȃ�.
 *
 * ZnkMem_rfind_16 �ł� {9,9}, {0,1}, {2,3}, {4,5}, {6,7}, {9,9} �Ƃ������uint16_t�Ƃ݂āA
 * �t�ɒ��ׂ�. �Ō�� 8 �͗]��ɂȂ邽�߁A���������i���̓_�� lfind �Ɠ����ł���).
 * �����Ǝw�肵�� val �����������ۂ��𒲂ׁA���������̂����������Ō�̃C���f�b�N�X��Ԃ�.
 * �Ⴆ�΁A�ȉ��̂悤�ɂȂ�.
 *
 *   val={2,3}�̏ꍇ�A2 ���Ԃ����(lfind�Ɠ���)
 *   val={1,2}�̏ꍇ�AZnk_NPOS���Ԃ����(lfind�Ɠ���).
 *                    ( ������ {9,8},{7,9},{5,6},{3,4},{1,2}...�̂悤�ɒ��ׂ����
 *                    ���Ⴂ���Ă͂����Ȃ� )
 *   val={9,9}�̏ꍇ�A5 ���Ԃ����(lfind�ƈقȂ�).
 *
 * ����� delta=2 ���w�肵���ꍇ�A��L�̉� {9,9}, {2,3}, {6,7} �݂̂ɂȂ�.
 * (delta�̔{���Ԗ�(������0���琔����)�ȊO�̗v�f�̓X�L�b�v�����).
 * �����܂ł� lfind�Ɠ����ŁA���̏�ł������t�ɒT������( {6,7}, {2,3}, {9,9} �̏� ).
 * ���̂Ƃ��Aval={2,3}�̏ꍇ�́A2 ���Ԃ����( rfind �̏ꍇ�ł����������ꍇ�ł����
 * ���ʂ͂�͂� delta=1 �̂Ƃ��Ɠ����ł���). val={1,0}�̏ꍇ�́A�X�L�b�v�����̂Ō��m
 * ����Ȃ�(�����lfind�Ɠ��l�ł���). �܂� val={9,9}�̏ꍇ�A0 ���Ԃ����(��ԏ��߂ɂ���
 * {9,9}�̕����݂̂��q�b�g����).
 *
 * ZnkMem_lfind_32, ZnkMem_rfind_32, ZnkMem_rfind_64, ZnkMem_rfind_64 �̏ꍇ�����l�ł���.
 * ZnkMem_lfind_24, ZnkMem_rfind_24 �̏ꍇ�́A�w�肷��val�̉���24bit�������l�Ƃ��Ďg����
 * �Ƃ����_�ȊO�͓��l�ł���(is_LE�̈Ӗ��Ɏ኱���ӂ���K�v������. 24bit�łł� 3�o�C�g����
 * �Ƃ������̂����z�I�ɑ��݂���Ƃ��āA����̃o�C�g�I�[�_�[�ōl����.)
 */
size_t ZnkMem_lfind_8(  const uint8_t* buf, size_t size, uint8_t  val, size_t delta );
size_t ZnkMem_rfind_8(  const uint8_t* buf, size_t size, uint8_t  val, size_t delta );
size_t ZnkMem_lfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap );
size_t ZnkMem_rfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap );

/**
 * @brief
 *   �w�肳�ꂽval�� buf�ɂ����� [ begin, end ) �͈̔͂ɂ�����begin�̈ʒu���珇�����ɒ��ׁA
 *   ���ꂪ���݂���ʒu��Ԃ�. �͈͂��w��ł���_�ȊO�� ZnkMem_lfind_8 �Ɠ����ł���.
 *   �߂�l�ƂȂ�ʒu�́Abegin����̈ʒu�ł͂Ȃ��Abuf�̐擪�ʒu����Ƃ����ʒu�ł���.
 *
 *   buf�̃T�C�Y�́Aend �o�C�g�ȏオ�m�ۂ���Ă��Ȃ���΂Ȃ�Ȃ�.
 *   ����buf�̍Ō�̈ʒu�܂Œ��ׂ����ꍇ�� end �� buf�̃o�C�g�T�C�Y���w�肷��΂悢.
 */
Znk_INLINE size_t ZnkMem_lfind_pos_8( const uint8_t* buf, size_t begin, size_t end, uint8_t val, size_t delta ){
	if( begin < end ){
		size_t offset = ZnkMem_lfind_8( buf + begin, end - begin, val, delta );
		return ( offset == Znk_NPOS ) ? Znk_NPOS : ( begin + offset );
	}
	return Znk_NPOS;
}
/**
 * @brief
 *   �w�肳�ꂽval�� buf�ɂ����� [ begin, end ) �͈̔͂ɂ����čŌ�(end�̈�O)����t�����ɒ��ׁA
 *   ���ꂪ���݂���ʒu��Ԃ�. �͈͂��w��ł���_�ȊO�� ZnkMem_rfind_8 �Ɠ����ł���.
 *   �߂�l�ƂȂ�ʒu�́Abegin����̈ʒu�ł͂Ȃ��Abuf�̐擪�ʒu����Ƃ����ʒu�ł���.
 *
 *   buf�̃T�C�Y�́Aend �o�C�g�ȏオ�m�ۂ���Ă��Ȃ���΂Ȃ�Ȃ�.
 *   ����buf�̍Ō�̈ʒu���璲�ׂ����ꍇ�� end �� buf�̃o�C�g�T�C�Y���w�肷��΂悢.
 */
Znk_INLINE size_t ZnkMem_rfind_pos_8( const uint8_t* buf, size_t begin, size_t end, uint8_t val, size_t delta ){
	if( begin < end ){
		size_t offset = ZnkMem_rfind_8( buf + begin, end - begin, val, delta );
		return ( offset == Znk_NPOS ) ? Znk_NPOS : ( begin + offset );
	}
	return Znk_NPOS;
}

/***
 * ��Uocc_tbl�����������ꂽ�ꍇ�A���̒l��0�ɂ͂Ȃ蓾�Ȃ����߁A
 * �l�� 0 ���ۂ��𒲂ׂ邱�Ƃŏ���������Ă��邩�ۂ��𔻒�ł���.
 * �g�p��.
 *
 *   static ZnkOccTable_D( st_occ_tbl );
 *   if( !ZnkOccTable_isInitialized(st_occ_tbl) ){
 *     ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"\r\n", 2 );
 *   }
 */
#define ZnkOccTable_D( name ) size_t name[ 256 ] = { 0 }
#define ZnkOccTable_isInitialized( occ_tbl ) ( (occ_tbl)[ 0 ] )

/***
 * @brief
 *   buf���ɂ���Ɨ\�z�����data��̈ʒu��T�����A���̃o�C�g�ʒu��Ԃ�.
 *   ���̊֐��ł́Adelta ���w�肷�邱�Ƃɂ��A�����������������buf����delta�̔{����
 *   �ʒu�ɂ�����݂̂̂Ɍ��肷�邱�Ƃ��ł���.�i��������΁A����delta�̔{���ł͂Ȃ��ʒu��
 *   �p�^�[�������������ꍇ�͂���͖�������). ���Ȃ݂� buf �̃A�h���X�l�ɂ��Ă� delta
 *   �̔{���ł���K�v�͂Ȃ�. delta == 1 �̂Ƃ��́A�����ł��ꑊ���̍����ȃo�[�W�������Ăяo�����.
 *
 * @param buf:
 *   �������s���鑤�̃o�b�t�@.
 *   (�R���s���[�^�T�C�G���X�ł�Haystack(����)�ƕ\�L�����ꍇ������)
 *   �o�C�g���E�͋C�ɂ���K�v�͂Ȃ�.
 *
 * @param data:
 *   �������s���L�[�ƂȂ�f�[�^��.
 *   (�R���s���[�^�T�C�G���X�ł�Needle(�j)�ƕ\�L�����ꍇ������)
 *
 * @param occ_tbl:
 *   BMS( Boyer-Moore Sunday ) �@�ɂ�錟�����s�������ꍇ�݂̂Ɏw�肷��.
 *   ������ NULL ���w�肷�邱�Ƃ��ł��A���̏ꍇ�͊֐������� OccTable�������I�ɍ쐬����.
 *   occ_tbl �͗v�f�����K��256�ƂȂ�size_t�^�z��̐擪�������|�C���^�ł���.
 *   �ڍׂɂ��ẮAZnkMem_getLOccTable_forBMS���Q��.
 *   BMS�@�̏ꍇ�AZnkMem_getLOccTable_forBMS�ł��̃e�[�u����data���Ɉ�x������������ɁA
 *   ���̊֐����Ăяo���ƌ����I�ł���. �ȉ��ɃR�[�h�������.
 *
 *   size_t occ_tbl[256];
 *   ZnkMem_getLOccTable_forBMS( occ_tbl, data1, data1_size );
 *   ZnkMem_lfind_data_BMS( buf1, buf1_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf2, buf2_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf3, buf3_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_getLOccTable_forBMS( occ_tbl, data2, data2_size );
 *   ZnkMem_lfind_data_BMS( buf1, buf1_size, data2, data2_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf2, buf2_size, data2, data2_size, delta, occ_tbl );
 *
 *   // Make occ_tbl internal
 *   ZnkMem_lfind_data_BMS( buf, buf_size, data, data_size, delta, NULL );
 *
 *   // Use Brute Force
 *   ZnkMem_lfind_data_BF( buf, buf_size, data, data_size, delta );
 *
 * @param delta:
 *   ���o�C�g�P�ʂ̔{���Ō������邩���w�肷��.
 *   0 ���w�肵���ꍇ�� 1 �Ɠ����Ƃ݂Ȃ����.
 *
 * @note
 *   ��1.
 *   �ȉ���24bit�C���[�W���� rgb �Ƃ����p�^�[��������������.
 *   �������A��̉�f�Ԃ� ..r gbx ... �Ƃ����p�^�[���������ꍇ��
 *   ����͖������Ăق���.
 *
 *     rxy abg xyr gbb xyz rgb yyr
 *               ^         ^
 *
 *   ��L�ł́A15 �����҂��铚���ł���. �܂��A8 �͗\�����Ȃ������ƂȂ�.
 *   ���̊֐��ł� delta = 3 �̏ꍇ���Ғʂ�A15 ��Ԃ��Adelta = 1 �̏ꍇ�A
 *   �ʏ��byte�P�ʌ����Ƃ��� 8 ��Ԃ�.
 *
 *   ��2.
 *   �ȉ��̃}���`�o�C�g(2byte)�����񂩂� 1234 �Ƃ����p�^�[��������������.
 *   �������A�قȂ�}���`�o�C�g�����Ԃ� xx12 34yy �Ƃ����p�^�[���������ꍇ��
 *   ����͖������Ăق���.
 *
 *     xxxx xxxx xx12 34yy yyyy 1234 xxxx
 *
 *   ��L�ł́Adelta = 2�̂Ƃ����Ғʂ� 20 ��Ԃ��Adelta = 1 �̂Ƃ� 10 ��Ԃ�.
 *
 *   ���̂悤��2byte�̃P�[�X�ł́AZnkAry_lfind_16���g����󋵂Ȃ�΂��̕��������ł���.
 *   �������AZnkAry_lfind_16 �ł� buf����A�h���X�̏ꍇ���Ŏg�p���ł��Ȃ�.
 *   (�����n�ɂ���Ă͂��܂����삷��\���͂��邪���댯�x������).
 *   ���̊֐��ł͂��̐������Ȃ�.
 *
 *   ���Ȃ݂�UTF8�͑S�p������3byte�ō\������邱�Ƃ��������A���Ƃ����ׂđS�p����
 *   ����\������镶����ł��e���������3byte�Ƃ͌���Ȃ��̂ŁAdelta = 3 �Ƃ���
 *   �w��͎g���Ȃ�. �����AUTF8�̏ꍇ��1byte�ڂ�2byte�ڈȍ~�Ƃ��Ԃ�Ȃ��R�[�h��
 *   �Ȃ��Ă���͂��Ȃ̂ŁAdelta = 1 �Ƃ����w��ł����̎�̕������ׂ����͔���
 *   ���Ȃ��͂��ł���.
 *
 *   ��3.
 *   �ȉ���padding�̂Ȃ����z�I�ȍ\���̂��V���A���C�Y�����V�[�P���X��������̂Ƃ��āA
 *   ���̒��������̃f�[�^������������.
 *   �Ⴆ�΁Atype = 10 �ƂȂ��Ă���f�[�^�̈ʒu��m�肽��.
 *   struct{ uint16_t type; int data; } // size �͑S��6byte�Ƃ���.
 *
 *     11 0234  2a 1076  10 abcd  00 xxxx
 *      ^          ^     ^
 *
 *   ��L�̏ꍇ�Adelta = 6�Adata = 10 �Ƃ���� ���Ғʂ�̈ʒu12��Ԃ�.
 *   delta = 1, data = 10 �̏ꍇ�͈ʒu1��Ԃ�.
 *   ����� delta = 2, data = 10 �Ƃ����ꍇ�� �ʒu8 ��Ԃ�.
 */
void ZnkMem_getLOccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size );
void ZnkMem_getROccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size );

size_t
ZnkMem_lfind_data_BF(  const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta );
size_t
ZnkMem_rfind_data_BF(  const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta );
size_t
ZnkMem_lfind_data_BMS( const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta, const size_t* occ_tbl );
size_t
ZnkMem_rfind_data_BMS( const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta, const size_t* occ_tbl );

/* endof lfind/rfind */
/*****************************************************************************/


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
