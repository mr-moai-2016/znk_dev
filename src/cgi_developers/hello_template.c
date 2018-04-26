#include <Rano_cgi_util.h>
#include <Znk_bird.h>
#include <Znk_dir.h>
#include <stdio.h>
#include <string.h>

static void
show_result( RanoCGIEVar* evar )
{
	/* ZnkBird�̐���. */
	ZnkBird bird = ZnkBird_create( "#[", "]#" );
	/* BIRD�^�O�̓o�^. */
	ZnkBird_regist( bird, "your_name", "Mr.Moai" );
	/* �e���v���[�gHTML��ǂݍ��݁ABIRD�^�O��W�J���ĕ\��. */
	RanoCGIUtil_printTemplateHTML( evar, bird, "../publicbox/hello_template.html" );
	/* ZnkBird�̔j��. */
	ZnkBird_destroy( bird );
}

int main(int argc, char **argv)
{
	/***
	 * Create+Get Environment Variables for CGI
	 */
	RanoCGIEVar* evar = RanoCGIEVar_create();

	/***
	 * for Windows : change stdin/stdout to binary-mode.
	 */
	Znk_Internal_setMode( 1, true );

	show_result( evar );

	RanoCGIEVar_destroy( evar );
	return 0;
}
