#include <Rano_hash.h>

uint32_t
RanoHash_murmurhash3_32_gc( const char* key, size_t key_length, uint32_t seed )
{
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	const size_t remainder = key_length & 3; /* key_length % 4 */
	const size_t bytes = key_length - remainder;

	uint32_t h1 = seed;
	uint32_t i = 0;
	uint32_t h1b;
	uint32_t k1;
	
	while( i < bytes ){
		/***
		 * ISO/IEC 9899/1990 6.3 �ɂ��΁AC�ɂ����ĕ���p�����_(Sequence point)�Ƃ�
		 * ���ׂĂ̕���p�������������Ƃ��ۏ؂���Ă���_�ł���. ����͎��̖{���̏I���A
		 * �������̕]��������̊֐��̌ďo�����A&&�A||�A�O�����Z�q?:�A�J���}���Z�q�ɂ�����
		 * ���݂���. �����ȊO�ł̉��Z�q�ŋ�؂�ꂽ�͈͂ł́A���͈͓̔��ŕϐ�x��
		 * ������X�V�����ꍇ�́A���͈͓̔��ŕϐ�x�𕡐���X�V�����ꍇ��x�̕]��������
		 * ����`�ł���. �܂����Ƃ����̂悤�Ȕ͈͂ɂ�����x�̍X�V�����݂̂ł����Ă��A
		 * x�X�V�O�̒l���Ax�X�V��̒l�����肷�邽�߂����ɃA�N�Z�X����Ă���ꍇ(x = x+1�Ȃ�)�������A
		 * x�̕ύX�ƎQ�Ƃ������ɍs���Ă���ꍇ(a[x]=x++�Ȃ�)�̓���͖���`�ƂȂ�.
		 * 
		 * �]���ăI���W�i����Javascript�ɂ���悤�� ++i ����̎����ŕ����g���L�q��C�ł�NG�ł���
		 * ( | ���Z�q�ŋ敪����Ă͂��邪�A����͏�L�̏����𖞂������Z�q�ł͂Ȃ����Ƃɒ��� )�A
		 * �Ⴆ�Έȉ��̂悤�Ɍ�ł܂Ƃ߂ăC���N�������g����K�v������.
		 */
		k1 =
			( (key[ i   ] & 0xff)      ) |
			( (key[ i+1 ] & 0xff) << 8 ) |
			( (key[ i+2 ] & 0xff) << 16) |
			( (key[ i+3 ] & 0xff) << 24);
		i += 4;
		k1  = ((((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16))) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = ((((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16))) & 0xffffffff;
		h1 ^= k1;
		h1  = (h1 << 13) | (h1 >> 19);
		h1b = ((((h1 & 0xffff) * 5) + ((((h1 >> 16) * 5) & 0xffff) << 16))) & 0xffffffff;
		h1  = (((h1b & 0xffff) + 0x6b64) + ((((h1b >> 16) + 0xe654) & 0xffff) << 16));
	}

	k1 = 0;
	
	switch( remainder ){
	case 3: k1 ^= (key[ i + 2 ] & 0xff) << 16; /* fall-through */
	case 2: k1 ^= (key[ i + 1 ] & 0xff) << 8;  /* fall-through */
	case 1: k1 ^= (key[ i     ] & 0xff);
		k1  = (((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16)) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = (((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16)) & 0xffffffff;
		h1 ^= k1;
		break;
	default:
		break;
	}
	
	h1 ^= key_length;
	
	h1 ^= h1 >> 16;
	h1  = (((h1 & 0xffff) * 0x85ebca6b) + ((((h1 >> 16) * 0x85ebca6b) & 0xffff) << 16)) & 0xffffffff;
	h1 ^= h1 >> 13;
	h1  = ((((h1 & 0xffff) * 0xc2b2ae35) + ((((h1 >> 16) * 0xc2b2ae35) & 0xffff) << 16))) & 0xffffffff;
	h1 ^= h1 >> 16;
	
	return h1 >> 0;
}

