/***
 * �|��P�ʂ���̏ꍇ�x���𔭂���R���p�C�������݂���(VC�Ȃ�)
 * ����͂��̂��߂̑΍�.
 */
typedef int dummy_for_warning__translation_unit_is_empty__;

/***
 * dll�쐬�̍ہADllMain�̒�`���K�{�ȃR���p�C�������݂���(DMC�Ȃ�)
 * ����͂��̂��߂̑΍�.
 * �������A���ɑ��̕�����DllMain���`���Ă���ꍇ�͂�����X�L�b�v���邱�Ƃ��ł���.
 * ���̏ꍇ��DLLMAIN_DEFINED��p���邱��.
 */
#if !defined(DLLMAIN_DEFINED) && defined(__DMC__)

#  define WIN32_LEAN_AND_MEAN
#  include<windows.h>

#  ifdef __cplusplus
extern "C" {
#  endif

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
	switch(fdwReason){
	case DLL_PROCESS_ATTACH: break;
	case DLL_PROCESS_DETACH: break;
	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	}
	return  TRUE;
}

#  ifdef __cplusplus
}
#  endif

#endif
