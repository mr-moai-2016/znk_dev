#include <Znk_myf.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <Znk_str_fio.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>
#include <Znk_str_path.h>
#include <Znk_liba_scan.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	 LibaDumper_e_Default=0 /* Use gslconv internal function */
	,LibaDumper_e_Dumpbin   /* Use VC dumpbin tool */
	,LibaDumper_e_NM        /* Use MinGW nm tool */
} LibaDumperType;

static int
gsl2def( ZnkMyf gsl, const char* gslmyf_filename, const char* deffile )
{
	bool result = false;

	result = ZnkMyf_load( gsl, gslmyf_filename );

	if( result ){
		ZnkVarpAry config = ZnkMyf_find_vars( gsl, "config" );
		ZnkStrAry export_symbols = ZnkMyf_find_lines( gsl, "export_symbols" );
		ZnkStrAry ignore_symbols = ZnkMyf_find_lines( gsl, "ignore_symbols" );
		ZnkStrAry export_symbols_additional = ZnkMyf_find_lines( gsl, "export_symbols_additional" );
		ZnkStrAry exports = ZnkStrAry_create( true );
		size_t size;
		size_t idx;
		const char* nl = "\r\n";
		ZnkVarp varp;
		const char* dlib_filename = NULL;
		ZnkFile fp = NULL;

		varp = ZnkVarpAry_find_byName_literal( config, "dlib_filename", false );
		if( varp ){ dlib_filename = ZnkVar_cstr( varp ); }

		fp = Znk_fopen( deffile, "wb" );
		if( fp ){
			ZnkStr line_ref = NULL;
			Znk_fprintf( fp, "LIBRARY \"%s\"%s", dlib_filename, nl );

			/* sanitize side space */
			size = ZnkStrAry_size( export_symbols );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( export_symbols, idx );
				ZnkStrEx_removeSideCSet( line_ref, " \t", 2 );
			}
			size = ZnkStrAry_size( ignore_symbols );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( ignore_symbols, idx );
				ZnkStrEx_removeSideCSet( line_ref, " \t", 2 );
			}
			size = ZnkStrAry_size( export_symbols_additional );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( export_symbols_additional, idx );
				ZnkStrEx_removeSideCSet( line_ref, " \t", 2 );
			}

			/* push back to exports list with duplicate checking */
			size = ZnkStrAry_size( export_symbols );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( export_symbols, idx );
				if( ZnkStrAry_find( ignore_symbols, 0, ZnkStr_cstr(line_ref), Znk_NPOS ) != Znk_NPOS ){
					continue; /* skip */
				}
				if( ZnkStrAry_find( exports, 0, ZnkStr_cstr(line_ref), Znk_NPOS ) == Znk_NPOS ){
					ZnkStrAry_push_bk_cstr( exports, ZnkStr_cstr(line_ref), Znk_NPOS );
				}
			}
			size = ZnkStrAry_size( export_symbols_additional );
			for( idx=0; idx<size; ++idx ){
				/* ���̃��X�g�Ɏw�肳��Ă���V���{���� ignore_symbols ���D�悷����̂Ƃ��� */
				line_ref = ZnkStrAry_at( export_symbols_additional, idx );
				if( ZnkStrAry_find( exports, 0, ZnkStr_cstr(line_ref), Znk_NPOS ) == Znk_NPOS ){
					ZnkStrAry_push_bk_cstr( exports, ZnkStr_cstr(line_ref), Znk_NPOS );
				}
			}

			/* output def file */
			Znk_fprintf( fp, "EXPORTS%s", nl );
			size = ZnkStrAry_size( exports );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( exports, idx );
				Znk_fprintf( fp, "\t%s%s", ZnkStr_cstr(line_ref), nl );
			}

			Znk_fclose( fp );
		}
		ZnkStrAry_destroy( exports );
	}
	return 0;
}

typedef bool (*FuncT_getOneSymbol)( ZnkStrAry exports, const char* line, size_t line_size, const char* target_machine );

static bool
getOneSymbol_nm( ZnkStrAry exports, const char* line, size_t line_size, const char* target_machine )
{
	const char* p = strstr( line, " T " );
	if( p ){
		if( p - line == 8 ){
			/* 32bit��lib�̃��O�ł��� */
			ZnkStrAry_push_bk_cstr( exports, p+3+1, Znk_NPOS );
		} else if( p - line == 16 ){
			/* 64bit��lib�̃��O�ł��� */
			/* 64bit��lib�ł̓O���[�o���V���{���� _�Ŏn�܂�Ȃ�. */
			ZnkStrAry_push_bk_cstr( exports, p+3, Znk_NPOS );
		} else {
			Znk_printf_e( "Error : nm_log is Broken.\n" );
			return false;
		}
	}
	return true;
}
static bool
getOneSymbol_dumpbin( ZnkStrAry exports, const char* line, size_t line_size, const char* target_machine )
{
	if( line_size > 17 ){
		const char* p = line + 13;
		const bool is_underbar = ZnkS_eq( target_machine, "x86" );
		if( ZnkS_isBegin( p, "SECT" ) ){
			/* dumpbin SECT line */
			p += 4;
			while( *p != ' ' ) ++p;
			while( *p == ' ' ) ++p;
			if( ZnkS_isBegin( p, "notype ()" ) ){
				/* this is function */
				p += 9;
				while( *p == ' ' ) ++p;
				if( ZnkS_isBegin( p, "External" ) ){
					/* this is Global function */
					p += 8;
					while( *p != '|' ) ++p;
					++p;
					while( *p == ' ' ) ++p;
					if( is_underbar ){
						ZnkStrAry_push_bk_cstr( exports, p+1, Znk_NPOS );
					} else {
						ZnkStrAry_push_bk_cstr( exports, p, Znk_NPOS );
					}
				}
			}
		}
	}
	return true;
}

static bool
autogetLibname( char* libname, size_t libname_size, const char* target_slib )
{
	size_t tail_pos = ZnkStrPath_getTailPos( target_slib );
	const char* ext = ZnkS_get_extension( target_slib, '.' );
	char buf[ 256 ] = "";
	char* libver = NULL;

	if( ZnkS_eqCase( ext, "lib" ) || ZnkS_eqCase( ext, "a" ) ){
		ZnkFile fp = NULL;
		const char* begin = target_slib + tail_pos;
		if( ZnkS_isBegin( begin, "lib" ) ){
			begin += 3;
		}
		fp = Znk_fopen( "Makefile_version.mak", "rb" );
		if( fp ){
			Znk_fgets( buf, sizeof(buf), fp );
			if( ZnkS_isBegin( buf, "DL_VER=" ) ){
				libver = buf + Znk_strlen_literal("DL_VER=");
				ZnkS_chompNL( libver );
			}
			Znk_fclose( fp );
		}
		if( libver ){
			Znk_snprintf( libname, libname_size, "%s-%s.dll", begin, ext-1-begin, libver );
		} else {
			Znk_snprintf( libname, libname_size, "%s.dll", begin, ext-1-begin );
		}
		return true;
	}
	return false;
}

static int
gslgen( ZnkMyf gsl, const char* gslmyf_filename, const char* target_slib, const char* target_machine )
{
	bool result = false;

	if( ZnkDir_getType( gslmyf_filename ) != ZnkDirType_e_File ){
		/* ���݂��Ȃ��ꍇ�͐V�K�쐬���� */
		ZnkFile fp = Znk_fopen( gslmyf_filename, "wb" );
		if( fp ){
			char libname[ 256 ] = "";
			autogetLibname( libname, sizeof(libname), target_slib );

			Znk_fprintf( fp, "@def_quote [' ']\r\n" );
			Znk_fprintf( fp, "\r\n" );
			Znk_fprintf( fp, "@@V config\r\n" );
			if( ZnkS_empty( libname ) ){
				Znk_fprintf( fp, "dlib_filename = ['gsl-1.0.dll']\r\n" );
			} else {
				Znk_fprintf( fp, "dlib_filename = ['%s']\r\n", libname );
			}
			Znk_fprintf( fp, "def_filename = ['gsl.def']\r\n" );
			Znk_fprintf( fp, "@@.\r\n" );
			Znk_fprintf( fp, "\r\n" );
			Znk_fprintf( fp, "@@L export_symbols\r\n" );
			Znk_fprintf( fp, "@@.\r\n" );
			Znk_fprintf( fp, "\r\n" );
			Znk_fprintf( fp, "@@L export_symbols_additional\r\n" );
			Znk_fprintf( fp, "@@.\r\n" );
			Znk_fprintf( fp, "\r\n" );
			Znk_fprintf( fp, "@@L ignore_symbols\r\n" );
			Znk_fprintf( fp, "@@.\r\n" );
			Znk_fclose( fp );
		}
	}
	result = ZnkMyf_load( gsl, gslmyf_filename );
	if( result ){
		ZnkVarpAry config = ZnkMyf_find_vars( gsl, "config" );
		ZnkStrAry export_symbols = ZnkMyf_find_lines( gsl, "export_symbols" );
		ZnkStrAry ignore_symbols = ZnkMyf_find_lines( gsl, "ignore_symbols" );
		ZnkStrAry exports = ZnkStrAry_create( true );
		ZnkFile fp;
		//char* p;
		size_t size;
		size_t idx;
		char cmd[ 1024 ] = "";

		/***
		 * @ gslconv�̓����̃V���{�����͂��g���ꍇ(�f�t�H���g):
		 *
		 * �ʏ��gslconv�����̃V���{����͋@�\��32/64bit��VC lib�A32/64bit��MinGW a�t�@�C��
		 * ����炷�ׂĂ𕪐͂ł��邽�߁A���ɊO���c�[���ɗ���K�v�͂Ȃ� liba_dumper_type �̒l�� LibaDumper_e_Default�ł悢.
		 * ���̂Ƃ�config�t�@�C����ɂ����Ă͉��L�̕ϐ��l�͓��ɉ����w�肷��K�v�͂Ȃ�.
		 *
		 *   liba_dumper
		 *   dumpbin_path
		 *   dumpbin_log
		 *   nm_path
		 *   nm_log
		 *
		 * �������Agslconv�ł͊����Ă��̑��̊O���c�[�����g���ăV���{����͂��s�����Ƃ��ł��A
		 * ���݁A�O���c�[���̏o�͂Ƃ��� VC dumpbin �� MinGW nm �ɑΉ����Ă���.
		 * ���̏ꍇ�� config�t�@�C���� liba_dumper�ϐ��ɂ����Ă��ꂼ�� dumpbin �܂��� nm ���w�肷�邱��.
		 */
		LibaDumperType liba_dumper_type = LibaDumper_e_Default;

		/***
		 * @ dumpbin���g�p����ꍇ�̒���:
		 *
		 * �I�v�V�����Ƃ��� -symbols��Y�ꂸ�ɂ��邱��.
		 * dumpbin�� 32bit�ł��邢��64bit�ł�lib �o������͂ł��邪.
		 * a �t�@�C���͕��͂ł��Ȃ�.
		 */
		ZnkStr dumpbin_path = ZnkStr_new( "dumpbin -symbols" );
		ZnkStr dumpbin_log  = ZnkStr_new( "./out_dir/gsl_dumpbin.log" );
		/***
		 * @ nm���g�p����ꍇ�̒���:
		 *
		 * 32bit��MinGW�ł�nm�ł́A32bit�ł�lib, a ���Ƃ��ɔF���ł���.
		 * �������Ȃ��� 64bit�ł�lib, a �ɂ��Ă͂Ƃ��ɔF���ł����A�����𕪐͂���ɂ�
		 * 64bit��MinGW�ł�nm���K�v�ł���.
		 * ���̂���MACHINE�̒l�ɉ����āA32bi�ł��邢��64bit�ł� nm ���U�蕪���Ď��s�����悤
		 * �K�؂ɐݒ肷��K�v������.
		 *
		 * ���Ƃ��΁AMACHINE�̒l�ɉ����āAnm�����݂���PATH��K�؂ɐ؂�ւ���悤�ɂ���.
		 * ���邢��nm�ւ̃p�X�𒼐ڎw�肷��Ȃ�A���Ƃ��� nm_path = ['%%MKFSYS_DIR%%/bin%%MACHINE%%/nm']
		 * �ȂǂƂ��āAMACHINE�̒l�ɉ�����nm�����s�����悤�ɂ��Ă����Ƃ悢.
		 */
		ZnkStr nm_path      = ZnkStr_new( "nm" );
		ZnkStr nm_log       = ZnkStr_new( "./out_dir/gsl_nm.log" );

		ZnkVarp varp;
		const char dsp = '\\';

		/* Optional */
		varp = ZnkVarpAry_find_byName_literal( config, "liba_dumper", false );
		if( varp ){
			const char* liba_dumper = ZnkVar_cstr( varp );
			if( ZnkS_eq( liba_dumper, "dumpbin" ) ){
				liba_dumper_type = LibaDumper_e_Dumpbin;
			}
			if( ZnkS_eq( liba_dumper, "nm" ) ){
				liba_dumper_type = LibaDumper_e_NM;
			}
		}
		varp = ZnkVarpAry_find_byName_literal( config, "dumpbin_path", false );
		if( varp ){
			ZnkStr_set( dumpbin_path, ZnkVar_cstr( varp ) );
			ZnkStrPath_replaceDSP( dumpbin_path, dsp );
		}
		varp = ZnkVarpAry_find_byName_literal( config, "dumpbin_log", false );
		if( varp ){
			ZnkStr_set( dumpbin_log, ZnkVar_cstr( varp ) );
			ZnkStrPath_replaceDSP( dumpbin_log, dsp );
		}
		varp = ZnkVarpAry_find_byName_literal( config, "nm_path", false );
		if( varp ){
			ZnkStr_set( nm_path, ZnkVar_cstr( varp ) );
			ZnkStrPath_replaceDSP( nm_path, dsp );
		}
		varp = ZnkVarpAry_find_byName_literal( config, "nm_log", false );
		if( varp ){
			ZnkStr_set( nm_log, ZnkVar_cstr( varp ) );
			ZnkStrPath_replaceDSP( nm_log, dsp );
		}

		if( ZnkStr_empty( dumpbin_path ) ){
			Znk_printf_e( "Error : dumpbin_path is not specified in %s\n", gslmyf_filename );
			goto FUNC_END;
		}
		if( ZnkStr_empty( dumpbin_log ) ){
			Znk_printf_e( "Error : dumpbin_log is not specified in %s\n", gslmyf_filename );
			goto FUNC_END;
		}
		if( ZnkStr_empty( nm_path ) ){
			Znk_printf_e( "Error : nm_path is not specified in %s\n", gslmyf_filename );
			goto FUNC_END;
		}
		if( ZnkStr_empty( nm_log ) ){
			Znk_printf_e( "Error : nm_log is not specified in %s\n", gslmyf_filename );
			goto FUNC_END;
		}

		if( ZnkDir_getType( target_slib ) != ZnkDirType_e_File ){
			Znk_printf_e( "Error : target_slib=[%s] is not file or does not exist.\n", target_slib );
			goto FUNC_END;
		}

		if( liba_dumper_type == LibaDumper_e_Default ){
			bool debug = false;
			ZnkStr line_ref = NULL;
			ZnkLibaScan_getExportFunctionSymbols( target_slib, exports, debug );

			ZnkStrAry_clear( export_symbols );
			/* push back to exports list with duplicate checking */
			size = ZnkStrAry_size( exports );
			for( idx=0; idx<size; ++idx ){
				line_ref = ZnkStrAry_at( exports, idx );
				if( ZnkStrAry_find( ignore_symbols, 0, ZnkStr_cstr(line_ref), Znk_NPOS ) != Znk_NPOS ){
					continue; /* skip */
				}
				ZnkStrAry_push_bk_cstr( export_symbols, ZnkStr_cstr(line_ref), Znk_NPOS );
			}

			/* sort */
			ZnkStrAry_sort( export_symbols );

			ZnkMyf_save( gsl, gslmyf_filename );

		} else {
			const char* cmd_path =
				liba_dumper_type == LibaDumper_e_Dumpbin ?
				ZnkStr_cstr(dumpbin_path) : ZnkStr_cstr(nm_path);
			const char* cmd_log =
				liba_dumper_type == LibaDumper_e_Dumpbin ?
				ZnkStr_cstr(dumpbin_log) : ZnkStr_cstr(nm_log);
			FuncT_getOneSymbol get_one_symbol =
				liba_dumper_type == LibaDumper_e_Dumpbin ? 
				getOneSymbol_dumpbin : getOneSymbol_nm;

			Znk_snprintf( cmd, sizeof(cmd), "%s %s > %s", cmd_path, target_slib, cmd_log );
			{
				int ret = system( cmd );
				if( ret != 0 ){
					Znk_printf_e( "Error : [%s] exit without status 0.\n", cmd_path );
					goto FUNC_END;
				}
			}
			fp = Znk_fopen( cmd_log, "rb" );
			if( fp ){
				ZnkStr line = ZnkStr_new( "" );
				ZnkStr line_ref = NULL;
				while( true ){
					if( !ZnkStrFIO_fgets( line, 0, 4096, fp ) ){
						break;
					}
					ZnkStr_chompNL( line );

					if( !get_one_symbol( exports, ZnkStr_cstr(line), ZnkStr_leng(line), target_machine ) ){
						break;
					}
				}
				Znk_fclose( fp );
				ZnkStr_delete( line );

				/* sanitize side space */
				size = ZnkStrAry_size( ignore_symbols );
				for( idx=0; idx<size; ++idx ){
					line_ref = ZnkStrAry_at( ignore_symbols, idx );
					ZnkStrEx_removeSideCSet( line_ref, " \t", 2 );
				}

				ZnkStrAry_clear( export_symbols );
				/* push back to exports list with duplicate checking */
				size = ZnkStrAry_size( exports );
				for( idx=0; idx<size; ++idx ){
					line_ref = ZnkStrAry_at( exports, idx );
					if( ZnkStrAry_find( ignore_symbols, 0, ZnkStr_cstr(line_ref), Znk_NPOS ) != Znk_NPOS ){
						continue; /* skip */
					}
					ZnkStrAry_push_bk_cstr( export_symbols, ZnkStr_cstr(line_ref), Znk_NPOS );
				}

				/* sort */
				ZnkStrAry_sort( export_symbols );

				ZnkMyf_save( gsl, gslmyf_filename );
			}
		}
		ZnkStrAry_destroy( exports );

		ZnkStr_delete( dumpbin_path );
		ZnkStr_delete( dumpbin_log );
		ZnkStr_delete( nm_path );
		ZnkStr_delete( nm_log );
	}

FUNC_END:
	return 0;
}

static void
printUsage( void )
{
	Znk_printf_e( "Usage : \n" );
	Znk_printf_e( "  gslconv -g gslmyf_file target_slib target_machine\n" );
	Znk_printf_e( "    Generate gsl.myf from static library.\n" );
	Znk_printf_e( "  gslconv -d gslmyf_file target_def\n" );
	Znk_printf_e( "    Convert gsl.myf to def file.\n" );
}

static ZnkMyf st_gsl = NULL;

int main(int argc, char **argv)
{
	int result_code = 0;
	const char* option = NULL;
	const char* gslmyf_filename = NULL;

	if( argc < 3 ){
		printUsage();
		return 1;
	}
	option          = argv[1];
	gslmyf_filename = argv[2];

	if( ZnkS_eq( option, "-g" ) ){
		const char* target_slib    = NULL;
		const char* target_machine = NULL;
		if( argc < 5 ){
			printUsage();
			return 1;
		}
		target_slib    = argv[3];
		target_machine = argv[4];

		Znk_printf_e( "gslconv : gslgen slib=[%s] machine=[%s]\n", target_slib, target_machine );
		if( st_gsl == NULL ){
			st_gsl = ZnkMyf_create();
		}
		result_code = gslgen( st_gsl, gslmyf_filename, target_slib, target_machine );
		ZnkMyf_destroy( st_gsl );
	} else if( ZnkS_eq( option, "-d" ) ){
		const char* target_def = NULL;
		if( argc < 4 ){
			printUsage();
			return 1;
		}
		target_def    = argv[3];

		Znk_printf_e( "gslconv : gsl2def\n" );
		if( st_gsl == NULL ){
			st_gsl = ZnkMyf_create();
		}
		result_code = gsl2def( st_gsl, gslmyf_filename, target_def );
		ZnkMyf_destroy( st_gsl );
	} else {
		printUsage();
		return 1;
	}
	return result_code;
}
