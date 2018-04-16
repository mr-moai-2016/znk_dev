#include "cgi_util.h"
#include <stdio.h>

int main( void )
{
	/***
	 * �C�L�i���ł������肪����܂�.
	 * Moai CGI �`���[�g���A���ɂ�����T���v���͑S�Ĉ�т��āAWindows/Linux�Ƃ��ɓ��삷��悤�ɍ쐬���Ă���܂�.
	 *
	 * C�����CGI���쐬����ꍇ�A���̈ꕔ�ɂ�����HTTP�w�b�_���L�q����K�v������܂�.
	 * ������HTTP�w�b�_�ɂ�������s�� \r\n �ŋL�q���Ȃ���΂Ȃ�Ȃ��d�l�ƂȂ��Ă��܂�.
	 * Linux�ɂ����ẮA��������̂܂܋L�q����Ή������Ȃ��̂ł����A
	 * ���Windows�̃f�t�H���g�ł� \n ������Ƃ��ꂪ \r\n �ւƓ����Ŏ����I�ɕϊ�����Ă��܂��܂�.
	 * �܂�Windows�ɂ����Ă� \r\n �Ə����Ƃ��̂܂܂ł� \r\r\n �̈Ӗ��ɂȂ��Ă��܂��܂�.
	 * ���ƌ����� \n �ŋL�q����΍��x�͂��̎����ϊ����s���Ȃ�Linux�Ŗ��ɂȂ�܂�.
	 *
	 * ���s�� \n �� \r\n �̂ǂ���ŏ����ׂ��ł��傤���H�����[���炱��͂Ȃ��Ȃ��̓��ł�.
	 * CGI�J���ɂ����Ă�Windows�ɂ����邱�̎����ϊ��͎ז��ł�������܂���̂ŁA����𖳌������Ă��܂��܂��傤.
	 * ���������Windows�ł��AHTTP�w�b�_�̎d�l�ʂ肻�̂܂� \r\n �ŋL�q���邱�Ƃ��ł��܂�.
	 *
	 * cgi_util.h�ɂ�Windows�ɂ����邱�̎����ϊ��𖳌��ɂ��邽�߂̊֐����p�ӂ���Ă��܂�.
	 * CGIUtil_Internal_setMode �֐�������ɂ�����܂�.
	 * ���Ȃ݂ɂ����ł̑�1������ 1 �͕W���o�͂��Ӗ����܂�.
	 * ��2������ true �̓o�C�i�����[�h�ւ̕ϊ����Ӗ����܂�.
	 *
	 * ��libZnk�ɂ�����ƑS�����l�̊֐����p�ӂ���Ă��܂�.
	 * Znk_stdc.h �Ő錾����Ă��� Znk_Internal_setMode �֐�������ɂ�����܂�.
	 * libZnk���g�����J���ɂ��ẮA��̏͂̃T���v���ŉ�����܂�.
	 */
	{
		static const int is_binary_mode = 1;
		CGIUtil_Internal_setMode( 1, is_binary_mode );
	}


	/***
	 * Output HTTP Header for CGI
	 * (���s�R�[�h�͕K�� \r\n �Ƃ��ċL�q���悤)
	 */

	/* Content-Type�̎w��͕K�{�ł�.
	 * charset �͍ŋ߂ł� utf-8 ���w�肷��̂����ȏ��I�ɂ͐������̂ł��傤���A
	 * ���݂�Moai���[�U�̃^�[�Q�b�g�w���l�����ꍇ Shift_JIS�̕����ǂ��P�[�X�������Ǝv���܂��̂�
	 * �����ł͂������Ă����܂�. */
	printf( "Content-Type: text/html; charset=Shift_JIS\r\n" );

	/* CGI�̏ꍇ�u���E�U�ɓ��e���L���b�V�����������Ȃ��ꍇ���قƂ�ǂł��傤.
	 * �u���E�U�ɂ�����w�����邽�߁A���̂Q�s�������܂�. */
	printf( "Pragma: no-cache\r\n" );
	printf( "Cache-Control: no-cache\r\n" );

	/* ���̉��s��HTTP�w�b�_���̏I�����Ӗ����A�K�{�ł�. */
	printf( "\r\n" );


	/***
	 * Output HTML for CGI
	 * (���������͉��s�R�[�h�͂��͂�ǂ���ł��悢�� \n �ɂ��Ă����܂�)
	 */
	printf( "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n" );
	printf( "<html>\n" );
	printf( "<head>\n" );
	printf( "<META http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\n" );
	printf( "<META http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n" );
	printf( "<META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n" );
	printf( "</head>\n" );

	printf( "<body>\n" );

	printf( "Hello World on <b>Moai CGI</b>.\n" );

	printf( "</body></html>\n" );
	fflush( stdout );

	return 0;
}
