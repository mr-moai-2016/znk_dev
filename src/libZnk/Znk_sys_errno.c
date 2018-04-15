#include "Znk_sys_errno.h"
#include <errno.h>

/***
 * POSIX �W���� errno �̒l�Ƃ��ẮA���̂悤�Ȃ��̂�����.
 *
 * E2BIG:
 *   �֐��ɓn�����������X�g���������܂����B
 * EACCESS:
 *   �A�N�Z�X�����ۂ���܂����B���̃v���O���������s���Ă��郆�[�U�[�́A�t�@�C����f�B���N�g���[�Ȃǂւ̃A�N�Z�X���������Ă��܂���B
 * EAGAIN:
 *   �v�����ꂽ���\�[�X���ꎞ�I�ɗ��p�s�\�ł��B�����������ōēx�s���΁A�������邩������܂���B
 * EBADF:
 *   �֐����A�s�K�؂ȃt�@�C���L�q�q���g�����Ƃ��܂���.
 *   �Ⴆ�΁A�J���Ă���t�@�C�����Q�Ƃ��Ă��Ȃ��A���邢�́A
 *   �ǂݎ���p�Ƃ��ĊJ����Ă���t�@�C���ɏ������ނ��߂Ɏg��ꂽ�A�Ȃ�.
 * EBUSY:
 *   �v�����ꂽ���\�[�X�͗��p�ł��܂���B
 *   �Ⴆ�΁A�ʂ̃A�v���P�[�V�������ǂ�ł���Œ��Ƀf�B���N�g���[���폜���悤�Ƃ����A�ȂǁB
 *   EBUSY �� EAGAIN �Ƃ̔����ȈႢ�ɒ��ӂ��Ă��������B���R�ł����A�ǂݎ����s���Ă���v���O�������I��������Ȃ�A
 *   ���̃f�B���N�g���[���폜���邱�Ƃ��ł��܂��B
 * ECHILD:
 *   wait() �֐��A�܂��� waitpid() �֐����A�q�v���Z�X���I������̂�҂Ƃ��Ƃ��܂������A
 *   ���ׂĂ̎q�v���Z�X�͊��ɏI�����Ă��܂����B
 * EDEADLK:
 *   ���N�G�X�g���p�������ƁA���\�[�X�E�f�b�h���b�N���������܂��B
 *   ����́A�}���`�X���b�h�̃R�[�h�ŋN����悤�ȃf�b�h���b�N�ł͂Ȃ����Ƃɒ��ӂ��Ă��������B
 *   errno �Ƃ��̒��Ԃł́A�ƂĂ����������f�b�h���b�N�͒ǐՂł��܂���B
 * EDOM:
 *   ���͈������A���̐��w�֐��̑ΏۊO�ł��B
 * EEXIST:
 *   �t�@�C�������ɑ��݂��A���ꂪ���ł��B
 *   �Ⴆ�΁A�p�X��t���� mkdir() ���ĂԂƁA�����̃t�@�C����f�B���N�g���[��
 *   ���O��t���邱�ƂɂȂ��Ă��܂��悤�ȏꍇ�ł��B
 * EFAULT:
 *   �֐��̈����� 1 ���A�����ȃA�h���X���Q�Ƃ��Ă��܂��B�啔���̎����ł́A
 *   ����͌��o�ł��܂��� (�v���O������ SIGSEGFAULT �V�O�i�����󂯎��A�I�����܂�)�B
 * EFBIG:
 *   ���N�G�X�g���A�����Œ�`���ꂽ�ő�̃t�@�C���T�C�Y�𒴂��ăt�@�C�����g�����悤�Ƃ��܂����B
 *   ����͒ʏ�͖� 2 GB �ł����A�ŋ߂̂قƂ�ǂ̃t�@�C���V�X�e���͂����Ƒ傫�ȃt�@�C�����T�|�[�g���Ă���A
 *   �ꍇ�ɂ��� read()/write() �� lseek() �Ȃǂ̊֐��� 64 �r�b�g�ł��K�v�ɂȂ�܂��B
 * EINTR:
 *   �֐����V�O�i���Ɋ��荞�܂�A���荞�݂̓v���O�����̃V�O�i���E�n���h���[�ɂ���ăL���b�`����A
 *   �����ăV�O�i���E�n���h���[�̖߂�͐���ł����B
 * EINVAL:
 *   �֐��ɑ΂��Ė����Ȉ�����n���܂����B
 * EIO:
 *   I/O �G���[���������܂����B����͒ʏ�A�n�[�h�E�F�A�̖��ւ̔����Ƃ��Đ�������܂��B
 * EISDIR:
 *   �t�@�C��������v������֐����A�f�B���N�g���[������t���ČĂт܂����B
 * ENFILE:
 *   ���̃v���Z�X�Ŋ��ɊJ����Ă���t�@�C�����������܂��B
 *   �e�v���Z�X�� OPEN_MAX �t�@�C���L�q�q�������Ă���A�����[�U�[�� (OPEN_MAX + 1) �t�@�C�����J�����Ƃ��Ă��܂��B
 *   �t�@�C���L�q�q�ɂ́A�\�P�b�g�̂悤�Ȃ��̂��̊܂܂�邱�Ƃɒ��ӂ��Ă��������B
 * ENLINK:
 *   ���̊֐����ĂԂƁA���̃t�@�C���� LINK_MAX �ȏ�̃����N�������ƂɂȂ�܂��B
 * ENAMETOOLONG:
 *   PATH_MAX ���������p�X��������Ă��܂��܂����B
 *   ���邢�́ANAME_MAX ���������t�@�C������f�B���N�g���[��������Ă��܂��܂����B
 * ENFILE:
 *   ���̃V�X�e���œ����ɊJ���Ă���t�@�C�����������܂��B
 *   ����͈ꎞ�I�ȏ�Ԃ̂͂��ł���A�ŋ߂̃V�X�e���ł́A�ʏ�͋N����Ȃ��͂��ł��B
 * ENODEV:
 *   ���̂悤�ȃf�o�C�X�͂���܂���B
 *   ���邢�́A�w�肳�ꂽ�f�o�C�X�ɑ΂��ĕs�K�؂Ȃ��Ƃ����݂悤�Ƃ��Ă��܂�
 *   (�Ⴆ�΁A��̂̃��C���E�v�����^�[����ǂݎ�낤�Ƃ��Ă͂����܂���)�B
 * ENOENT:
 *   ���̂悤�ȃt�@�C���͌�����܂���B���邢�́A�w�肳�ꂽ�p�X�������݂��܂���B
 * ENOEXEC:
 *   ���s�\�ł͂Ȃ��t�@�C�������s���悤�Ƃ��܂����B
 * ENOLCK:
 *   ���b�N�����p�ł��܂���B�V�X�e���S�̂ł̃t�@�C�������A���邢�̓��R�[�h�E���b�N�ɒB���܂����B
 * ENOMEM:
 *   �V�X�e���̃������[������Ȃ��Ȃ�܂����B
 *   �]������A�A�v���P�[�V������ (������ OS ���̂��̂�)�A��������܂������܂���B
 *   ���̂��߂ɁA���Ƀf�B�X�N��̃X���b�v��Ԃ̃T�C�Y�𓮓I�ɑ����ł��Ȃ��V�X�e���ł́A
 *   �g�������� RAM �ʂ��������� RAM ���K�v�Ȃ̂ł��B
 * ENOSPC:
 *   �f�o�C�X��ɋ�Ԃ��c���Ă��܂���B���[�U�[���A���Ɉ�t�̃f�o�C�X�ɑ΂��ď����������Ƃ����A
 *   ���邢�̓t�@�C�����쐬���悤�Ƃ��܂����B������������A�A�v���P�[�V�����ł� OS �ł����܂������Ȃ��G���[�ł��B
 * ENOSYS:
 *   �V�X�e���́A���̊֐����T�|�[�g���܂���B�Ⴆ�΁A�W���u�E�R���g���[���������Ȃ��V�X�e���ɑ΂���
 *   setpgid() ���ĂԂƁAENOSYS �G���[���������܂��B
 * ENOTDIR:
 *   �w�肳�ꂽ�p�X���̓f�B���N�g���[�ł���K�v������܂����A�����Ȃ��Ă��܂���B
 *   ����� EISDIR �G���[�̋t�ł��B
 * ENOTEMPTY:
 *   �w�肳�ꂽ�f�B���N�g���[����ł͂���܂���. ��ł���K�v������܂��B
 *   ��̃f�B���N�g���[�ł��A�u.�v ��u..�v �Ȃǂ��܂܂�Ă��邱�Ƃɒ��ӂ��Ă��������B
 * ENOTTY:
 *   ���̑�����T�|�[�g���Ă��Ȃ��t�@�C���A���邢�͓��ʂȃt�@�C���ɑ΂���
 *   I/O �R���g���[����������悤�Ƃ��܂����B
 *   �Ⴆ�΁A�f�B���N�g���[�ɑ΂��ă{�[���[�g��ݒ肵�悤�Ƃ��Ă͂����܂���B
 * ENXIO:
 *   ���݂��Ȃ��f�o�C�X�̓��ʂȃt�@�C���ɑ΂��� I/O ���N�G�X�g�����݂܂����B
 * EPERM:
 *   ���̑���͋�����Ă��܂���B
 *   ���[�U�[�́A�w�肳�ꂽ���\�[�X�ւ̃A�N�Z�X���������Ă��܂���B
 * EPIPE:
 *   ���͂⑶�݂��Ȃ��p�C�v����ǂݎ�낤�Ƃ��܂����A���邢�̓p�C�v�ɏ����������Ƃ��܂����B
 *   �p�C�v�E�`�F�[���̒��̃v���O������ 1 ���A���̃X�g���[����������Ă��܂��܂��� (�Ⴆ�ΏI���ɂ����)�B
 * ERANGE:
 *   �֐����Ă΂�܂������A�߂�l�͖߂�^�ŕ\������ɂ͑傫�����܂��B
 *   �Ⴆ�΁A����֐��� unsigned char �̒l��Ԃ������̂́A���ʂ� 256 �ȏ� (���邢�� -1 �ȉ�)��
 *   �v�Z�����Ƃ���ƁAerrno �� ERANGE �ɐݒ肳��A���̊֐��͖��֌W�Ȓl��Ԃ��܂��B
 *   ���������ꍇ�ɂ́A���̓f�[�^���K�؂��ǂ����𒲂ׂ邩�A���邢�͊֐����Ă񂾂�K�� errno �𒲂ׂ邱�Ƃ��d�v�ł��B
 * EROFS:
 *   �ǂݎ���p�̃t�@�C���V�X�e�� (���邢�͓ǂݎ���p���[�h�Ń}�E���g���ꂽ�t�@�C���V�X�e��) ��
 *   �ۑ����ꂽ�t�@�C���A���邢�̓f�B���N�g���[��ύX���悤�Ƃ��܂����B
 * ESPIPE:
 *   �p�C�v�A���邢�� FIFO (First In, First Out) �ɑ΂��ăV�[�N���悤�Ƃ��܂����B
 * ESRCH:
 *   �����ȃv���Z�X ID�A���邢�̓v���Z�X�E�O���[�v���w�肵�܂����B
 * EXDEV:
 *   �f�o�C�X�ɂ܂������ă����N���ړ����鑀����s�����Ƃ��܂����B
 *   �Ⴆ�΁AUNIX �t�@�C���V�X�e���ł́A�t�@�C���V�X�e���ԂŃt�@�C�����ړ����邱�Ƃ������܂���
 *   (�t�@�C�����R�s�[���A���̌�ŃI���W�i�����폜����K�v������܂�)�B
 */

static ZnkSysErrnoInfo st_sys_errno_info_table[] = {

	/* POSIX */
	{ 0,        "0",       "Unknown error", }, /* 0 ( not POSIX ) */
	{ EPERM,    "EPERM",   "Operation not permitted" }, /* EPERM=1 */
	{ ENOENT,   "ENOENT",  "No such file or directory" },
	{ ESRCH,    "ESRCH",   "No such process" },
	{ EINTR,    "EINTR",   "Interrupted system call" },
	{ EIO,      "EIO",     "Input/output error" },
	{ ENXIO,    "ENXIO",   "No such device or address" },
	{ E2BIG,    "E2BIG",   "Argument list too long" },
	{ ENOEXEC,  "ENOEXEC", "Exec format error" },
	{ EBADF,    "EBADF",   "Bad file descriptor" },
	{ ECHILD,   "ECHILD",  "No child processes" },
	{ EAGAIN,   "EAGAIN",  "Resource temporarily unavailable" },
	{ ENOMEM,   "ENOMEM",  "Cannot allocate memory" },
	{ EACCES,   "EACCES",  "Permission denied" },
	{ EFAULT,   "EFAULT",  "Bad address" },
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	/* ENOTBLK is not defined on VC and MinGW, but number 15 is reserved as Unknown. */
	{ 15,       "ENOTBLK", "Unknown error" },
#elif defined(ENOTBLK)
	/* not POSIX but Cygwin, Linux, dmc, bcc55 can use. */
	{ ENOTBLK,  "ENOTBLK", "Block device required" },
#endif
	{ EBUSY,    "EBUSY",   "Device or resource busy" },
	{ EEXIST,   "EEXIST",  "File exists" },
	{ EXDEV,    "EXDEV",   "Invalid cross-device link" },
	{ ENODEV,   "ENODEV",  "No such device" },
	{ ENOTDIR,  "ENOTDIR", "Not a directory" },
	{ EISDIR,   "EISDIR",  "Is a directory" },
	{ EINVAL,   "EINVAL",  "Invalid argument" },
	{ ENFILE,   "ENFILE",  "Too many open files in system" },
	{ EMFILE,   "EMFILE",  "Too many open files" },
	{ ENOTTY,   "ENOTTY",  "Inappropriate ioctl for device" },
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	/* ETXTBSY is not defined on VC and MinGW, but number 26 is reserved as Unknown. */
	{ 26,       "ETXTBSY", "Unknown error" },
#elif defined(ETXTBSY)
	/* not POSIX but Cygwin, Linux, dmc, bcc55 can use. */
	{ ETXTBSY,  "ETXTBSY", "Text file busy" },
#endif
	{ EFBIG,    "EFBIG",   "File too large" },
	{ ENOSPC,   "ENOSPC",  "No space left on device" },
	{ ESPIPE,   "ESPIPE",  "Illegal seek" },
	{ EROFS,    "EROFS",   "Read-only file system" },
	{ EMLINK,   "EMLINK",  "Too many links" },
	{ EPIPE,    "EPIPE",   "Broken pipe" },
	{ EDOM,     "EDOM",    "Numerical argument out of domain" },
	{ ERANGE,   "ERANGE",  "Numerical result out of range" }, /* 34 */

	/*
	 * �����܂ł͂قڂ��ׂĂ̊��Œ�`����Ă���Ƃ݂Ă悢.
	 ***/

	/***
	 * �ȉ��� EUCLEAN, UNKNOWN �������Ă� Windows�ȊO�ɂ��l�͑��݂��邪�A���̎��l���قȂ�.
	 * (������Linux�ɂ͂��ׂĂ��邪�A���L�Ń��X�g���Ă���̂ł����ł͊܂߂Ȃ�)
	 * �ȉ����L���Ȃ̂� VC��MinGW�݂̂ł���.
	 */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__DMC__)
	/* EUCLEAN is not defined on VC and MinGW, but number 35 and 37 is reserved as Unknown. */
	{ 35,           "EUCLEAN",      "Unknown error" },  /* not POSIX. */
	{ EDEADLK,      "EDEADLK",      "Resource deadlock avoided" }, /* not bcc55 */
	{ 37,           "UNKNOWN",      "Unknown error" },  /* MS only */
	{ ENAMETOOLONG, "ENAMETOOLONG", "Filename too long" },
	{ ENOLCK,       "ENOLCK",       "No locks available" }, /* not bcc55 */
	{ ENOSYS,       "ENOSYS",       "Function not implemented" }, /* not bcc55 */
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ EILSEQ,       "EILSEQ",       "Illegal byte sequence" }, /* not bcc55 */
#endif
#if defined(__BORLANDC__)
	{ EDEADLOCK,    "EDEADLOCK",    "Locking violation" },
	{ EUCLEAN,      "EUCLEAN",      "?" },  /* not POSIX. */
	{ ENAMETOOLONG, "ENAMETOOLONG", "Filename too long" },
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ EINVFNC,      "EINVFNC",      "Invalid function number" },
	{ ENOPATH,      "ENOPATH",      "Path not found" },
	{ ECONTR ,      "ECONTR",       "Memory blocks destroyed" },
	{ EINVMEM,      "EINVMEM",      "Invalid memory block address" },
	{ EINVENV,      "EINVENV",      "Invalid environment" },
	{ EINVFMT,      "EINVFMT",      "Invalid format" },
	{ EINVACC,      "EINVACC",      "Invalid access code" },
	{ EINVDAT,      "EINVDAT",      "Invalid data" },
	{ ECURDIR,      "ECURDIR",      "Attempt to remove CurDir" },
	{ ENOTSAM,      "ENOTSAM",      "Not same device" },
	{ ENMFILE,      "ENMFILE",      "No more files" },
#endif

	/***
	 * UNIX(cygwin or linux).
	 * linux �̏ꍇ�Aerrno.h �ɋɈꕔ�A
	 * �����đ啔���́A34�ȉ�(POSIX?)�� asm-generic/errno-base.h,
	 * 35�ȍ~�� asm-generic/errno.h �ɒ�`���Ă���.
	 * �Ȃ��A���я��� Cygwin��linux�ň�v���Ă��Ȃ�.
	 * �A���������Ƃ��Ĉ�v���Ă��镔�������邪�A���ꂩ���������ł���Ƃ͌���Ȃ�.
	 * �o���o���ł���Ƃ݂Ȃ��������悢.
	 */
#if defined( Znk_TARGET_UNIX)
	/* Linux/Cygwin����(���������l�Ƃ��Ă̐����͈قȂ�) */
	{ ENOMSG,       "ENOMSG",       "No message of desired type" },
	{ EIDRM,        "EIDRM",        "Identifier removed" },
	{ ECHRNG,       "ECHRNG",       "Channel number out of range" },
	{ EL2NSYNC,     "EL2NSYNC",     "Level 2 not synchronized" },
	{ EL3HLT,       "EL3HLT",       "Level 3 halted" },
	{ EL3RST,       "EL3RST",       "Level 3 reset" },
	{ ELNRNG,       "ELNRNG",       "Link number out of range" },
	{ EUNATCH,      "EUNATCH",      "Protocol driver not attached" },
	{ ENOCSI,       "ENOCSI",       "No CSI structure available" },
	{ EL2HLT,       "EL2HLT",       "Level 2 halted" },
	{ EDEADLK,      "EDEADLK",      "Resource deadlock avoided" },
	{ ENOLCK,       "ENOLCK",       "No locks available" },
	{ EBADE,        "EBADE",        "Invalid exchange" },
	{ EBADR,        "EBADR",        "Invalid request descriptor" },
	{ EXFULL,       "EXFULL",       "Exchange full" },
	{ ENOANO,       "ENOANO",       "No anode" },
	{ EBADRQC,      "EBADRQC",      "Invalid request code" },
	{ EBADSLT,      "EBADSLT",      "Invalid slot" },
	{ EDEADLOCK,    "EDEADLOCK",    "Resource deadlock avoided" },
	{ EBFONT,       "EBFONT",       "Bad font file format" },
	{ ENOSTR,       "ENOSTR",       "Device not a stream" },
	{ ENODATA,      "ENODATA",      "No data available" },
	{ ETIME,        "ETIME",        "Timer expired" },
	{ ENOSR,        "ENOSR",        "Out of streams resources" },
	{ ENONET,       "ENONET",       "Machine is not on the network" },
	{ ENOPKG,       "ENOPKG",       "Package not installed" },
	{ EREMOTE,      "EREMOTE",      "Object is remote" },
	{ ENOLINK,      "ENOLINK",      "Link has been severed" },
	{ EADV,         "EADV",         "Advertise error" },
	{ ESRMNT,       "ESRMNT",       "Srmount error" },
	{ ECOMM,        "ECOMM",        "Communication error on send" },
	{ EPROTO,       "EPROTO",       "Protocol error" },
	{ EMULTIHOP,    "EMULTIHOP",    "Multihop attempted" },
	{ EDOTDOT,      "EDOTDOT",      "RFS specific error" },
	{ EBADMSG,      "EBADMSG",      "Bad message" },
	{ ENOTUNIQ,     "ENOTUNIQ",     "Name not unique on network" },
	{ EBADFD,       "EBADFD",       "File descriptor in bad state" },
	{ EREMCHG,      "EREMCHG",      "Remote address changed" },
	{ ELIBACC,      "ELIBACC",      "Can not access a needed shared library" },
	{ ELIBBAD,      "ELIBBAD",      "Accessing a corrupted shared library" },
	{ ELIBSCN,      "ELIBSCN",      ".lib section in a.out corrupted" },
	{ ELIBMAX,      "ELIBMAX",      "Attempting to link in too many shared libraries" },
	{ ELIBEXEC,     "ELIBEXEC",     "Cannot exec a shared library directly" },
	{ ENOSYS,       "ENOSYS",       "Function not implemented" },
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ ENAMETOOLONG, "ENAMETOOLONG", "File name too long" },
	{ ELOOP,        "ELOOP",        "Too many levels of symbolic links" },
	{ EOPNOTSUPP,   "EOPNOTSUPP",   "Operation not supported" },
	{ EPFNOSUPPORT, "EPFNOSUPPORT", "Protocol family not supported" },
	{ ECONNRESET,   "ECONNRESET",   "Connection reset by peer" },
	{ ENOBUFS,      "ENOBUFS",      "No buffer space available" },
	{ EAFNOSUPPORT, "EAFNOSUPPORT", "Address family not supported by protocol" },
	{ EPROTOTYPE,   "EPROTOTYPE",   "Protocol wrong type for socket" },
	{ ENOTSOCK,     "ENOTSOCK",     "Socket operation on non-socket" },
	{ ENOPROTOOPT,  "ENOPROTOOPT",  "Protocol not available" },
	{ ESHUTDOWN,    "ESHUTDOWN",    "Cannot send after transport endpoint shutdown" },
	{ ECONNREFUSED, "ECONNREFUSED", "Connection refused" },
	{ EADDRINUSE,   "EADDRINUSE",   "Address already in use" },
	{ ECONNABORTED, "ECONNABORTED", "Software caused connection abort" },
	{ ENETUNREACH,  "ENETUNREACH",  "Network is unreachable" },
	{ ENETDOWN,     "ENETDOWN",     "Network is down" },
	{ ETIMEDOUT,    "ETIMEDOUT",    "Connection timed out" },
	{ EHOSTDOWN,    "EHOSTDOWN",    "Host is down" },
	{ EHOSTUNREACH, "EHOSTUNREACH", "No route to host" },
	{ EINPROGRESS,  "EINPROGRESS",  "Operation now in progress" },
	{ EALREADY,     "EALREADY",     "Operation already in progress" },
	{ EDESTADDRREQ, "EDESTADDRREQ", "Destination address required" },
	{ EMSGSIZE,     "EMSGSIZE",     "Message too long" },
	{ EPROTONOSUPPORT, "EPROTONOSUPPORT", "Protocol not supported" },
	{ ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "Socket type not supported" },
	{ EADDRNOTAVAIL, "EADDRNOTAVAIL", "Cannot assign requested address" },
	{ ENETRESET,    "ENETRESET",    "Network dropped connection on reset" },
	{ EISCONN,      "EISCONN",      "Transport endpoint is already connected" },
	{ ENOTCONN,     "ENOTCONN",     "Transport endpoint is not connected" },
	{ ETOOMANYREFS, "ETOOMANYREFS", "Too many references: cannot splice" },
	{ EUSERS,       "EUSERS",       "Too many users" },
	{ EDQUOT,       "EDQUOT",       "Disk quota exceeded" },
	{ ESTALE,       "ESTALE",       "Stale NFS file handle" },
	{ ENOTSUP,      "ENOTSUP",      "Operation not supported" },
	{ ENOMEDIUM,    "ENOMEDIUM",    "No medium found" },
	{ EILSEQ,       "EILSEQ",       "Invalid or incomplete multibyte or wide character" },
	{ EOVERFLOW,    "EOVERFLOW",    "Value too large for defined data type" },
#endif

#if defined(__CYGWIN__)
	/* Cygwin�ŗL */
	{ ELBIN,        "ELBIN",        "Inode is remote (not really error)" },
	{ EFTYPE,       "EFTYPE",       "Inappropriate file type or format" },
	{ ENMFILE,      "ENMFILE",      "No more files" },
	{ EPROCLIM,     "EPROCLIM",     "EPROCLIM" },
	{ ENOSHARE,     "ENOSHARE",     "No such host or network path" },
	{ ECASECLASH,   "ECASECLASH",   "Filename exists with different case" },
#endif

#if defined(__linux__)
	/* Linux�ŗL */
	{ ECANCELED,    "ECANCELED",    "Operation canceled" },
	{ ERESTART,     "ERESTART",     "Interrupted system call should be restarted" },
	{ ESTRPIPE,     "ESTRPIPE",     "Streams pipe error" },
	{ ENOTNAM,      "ENOTNAM",      "Not a XENIX named type file" },
	{ ENAVAIL,      "ENAVAIL",      "No XENIX semaphores available" },
	{ EISNAM,       "EISNAM",       "Is a named type file" },
	{ EREMOTEIO,    "EREMOTEIO",    "Remote I/O error" },
	{ EMEDIUMTYPE,  "EMEDIUMTYPE",  "Wrong medium type" },
	{ ENOKEY,       "ENOKEY",       "Required key not available" },
	{ EKEYEXPIRED,  "EKEYEXPIRED",  "Key has expired" },
	{ EKEYREVOKED,  "EKEYREVOKED",  "Key has been revoked" },
	{ EKEYREJECTED, "EKEYREJECTED", "Key was rejected by service" },
	{ EOWNERDEAD,   "EOWNERDEAD",   "Owner died" },
	{ ENOTRECOVERABLE, "ENOTRECOVERABLE", "State not recoverable" },
	/* �ȉ���Ubuntu(�Ƃ�����gcc4.2.4)�ł͑��݂��Ȃ��悤�Ȃ̂ŃR�����g�A�E�g */
	//{ ERFKILL,      "ERFKILL",      "Operation not possible due to RF-kill" },
#endif

};
/***
 * linux�ł̍ő�ԍ��͌���132�ł���.
 */

/* From cygwin32. linux�ɂ�����͗l.  */
//#define EWOULDBLOCK EAGAIN	/* Operation would block */

/* cygwin only */
//#define __ELASTERROR 2000	/* Users can add values starting here */


uint32_t
ZnkSysErrno_errno( void )
{
	return errno; /* std global variable : errno */
}

ZnkSysErrnoInfo*
ZnkSysErrno_getInfo( uint32_t sys_errno )
{
	/***
	 * POSIX�̕��� 1����34( EPERM����ERANGE )�܂ł̊Ԃ�
	 * (bcc55������)�ǂ̊��ł������Ă����̏��ԂɂȂ��Ă͂���悤�����A
	 * ����ȊO�̕����͊��ɂ���ăo���o���ł���ƍl���Ă悢.
	 * �����ł����ł̓��[�v�ɂ�荇�v����l���X�L�������Ȃ���΂Ȃ�Ȃ�.
	 */
	ZnkSysErrnoInfo* info;
	size_t idx;
	for( idx=0; idx<Znk_NARY( st_sys_errno_info_table ); ++idx ){
		info = st_sys_errno_info_table + idx;
		if( sys_errno == info->sys_errno_ ){
			return info;
		}
	}
	return st_sys_errno_info_table + 0 ; /* not found */
}


