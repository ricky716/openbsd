/*	$OpenBSD: src/sys/compat/sunos/Attic/sunos_syscall.h,v 1.21 2008/01/05 00:38:13 miod Exp $	*/

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	OpenBSD: syscalls.master,v 1.17 2008/01/05 00:36:13 miod Exp 
 */

/* syscall: "syscall" ret: "int" args: */
#define	SUNOS_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	SUNOS_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	SUNOS_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	SUNOS_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	SUNOS_SYS_write	4

/* syscall: "open" ret: "int" args: "char *" "int" "int" */
#define	SUNOS_SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	SUNOS_SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	SUNOS_SYS_wait4	7

/* syscall: "creat" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_creat	8

/* syscall: "link" ret: "int" args: "char *" "char *" */
#define	SUNOS_SYS_link	9

/* syscall: "unlink" ret: "int" args: "char *" */
#define	SUNOS_SYS_unlink	10

/* syscall: "execv" ret: "int" args: "char *" "char **" */
#define	SUNOS_SYS_execv	11

/* syscall: "chdir" ret: "int" args: "char *" */
#define	SUNOS_SYS_chdir	12

				/* 13 is obsolete time */
/* syscall: "mknod" ret: "int" args: "char *" "int" "int" */
#define	SUNOS_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_chmod	15

/* syscall: "lchown" ret: "int" args: "char *" "int" "int" */
#define	SUNOS_SYS_lchown	16

/* syscall: "break" ret: "int" args: "char *" */
#define	SUNOS_SYS_break	17

				/* 18 is obsolete stat */
/* syscall: "lseek" ret: "long" args: "int" "long" "int" */
#define	SUNOS_SYS_lseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	SUNOS_SYS_getpid	20

				/* 21 is obsolete sunos_old_mount */
/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	SUNOS_SYS_setuid	23

/* syscall: "getuid" ret: "uid_t" args: */
#define	SUNOS_SYS_getuid	24

/* syscall: "ostime" ret: "int" args: "int" */
#define	SUNOS_SYS_ostime	25

/* syscall: "ptrace" ret: "long" args: "int" "pid_t" "caddr_t" "int" "char *" */
#define	SUNOS_SYS_ptrace	26

/* syscall: "access" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_access	33

/* syscall: "sync" ret: "int" args: */
#define	SUNOS_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_kill	37

/* syscall: "stat" ret: "int" args: "char *" "struct stat43 *" */
#define	SUNOS_SYS_stat	38

/* syscall: "lstat" ret: "int" args: "char *" "struct stat43 *" */
#define	SUNOS_SYS_lstat	40

/* syscall: "dup" ret: "int" args: "u_int" */
#define	SUNOS_SYS_dup	41

/* syscall: "opipe" ret: "int" args: */
#define	SUNOS_SYS_opipe	42

/* syscall: "otimes" ret: "int" args: "struct tms *" */
#define	SUNOS_SYS_otimes	43

/* syscall: "profil" ret: "int" args: "caddr_t" "u_int" "u_int" "u_int" */
#define	SUNOS_SYS_profil	44

/* syscall: "setgid" ret: "int" args: "uid_t" */
#define	SUNOS_SYS_setgid	46

/* syscall: "getgid" ret: "gid_t" args: */
#define	SUNOS_SYS_getgid	47

/* syscall: "acct" ret: "int" args: "char *" */
#define	SUNOS_SYS_acct	51

/* syscall: "mctl" ret: "int" args: "caddr_t" "int" "int" "void *" */
#define	SUNOS_SYS_mctl	53

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "caddr_t" */
#define	SUNOS_SYS_ioctl	54

/* syscall: "reboot" ret: "int" args: "int" "char *" */
#define	SUNOS_SYS_reboot	55

				/* 56 is obsolete sunos_owait3 */
/* syscall: "symlink" ret: "int" args: "char *" "char *" */
#define	SUNOS_SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "char *" "char *" "int" */
#define	SUNOS_SYS_readlink	58

/* syscall: "execve" ret: "int" args: "char *" "char **" "char **" */
#define	SUNOS_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	SUNOS_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "char *" */
#define	SUNOS_SYS_chroot	61

/* syscall: "fstat" ret: "int" args: "int" "struct stat43 *" */
#define	SUNOS_SYS_fstat	62

/* syscall: "getpagesize" ret: "int" args: */
#define	SUNOS_SYS_getpagesize	64

/* syscall: "msync" ret: "int" args: "void *" "size_t" "int" */
#define	SUNOS_SYS_msync	65

/* syscall: "vfork" ret: "int" args: */
#define	SUNOS_SYS_vfork	66

				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
/* syscall: "sbrk" ret: "int" args: "int" */
#define	SUNOS_SYS_sbrk	69

/* syscall: "sstk" ret: "int" args: "int" */
#define	SUNOS_SYS_sstk	70

/* syscall: "mmap" ret: "int" args: "caddr_t" "size_t" "int" "int" "int" "long" */
#define	SUNOS_SYS_mmap	71

				/* 72 is obsolete vadvise */
/* syscall: "munmap" ret: "int" args: "caddr_t" "size_t" */
#define	SUNOS_SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "caddr_t" "size_t" "int" */
#define	SUNOS_SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "caddr_t" "size_t" "int" */
#define	SUNOS_SYS_madvise	75

/* syscall: "vhangup" ret: "int" args: */
#define	SUNOS_SYS_vhangup	76

/* syscall: "mincore" ret: "int" args: "caddr_t" "size_t" "char *" */
#define	SUNOS_SYS_mincore	78

/* syscall: "getgroups" ret: "int" args: "u_int" "gid_t *" */
#define	SUNOS_SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "u_int" "gid_t *" */
#define	SUNOS_SYS_setgroups	80

/* syscall: "getpgrp" ret: "int" args: */
#define	SUNOS_SYS_getpgrp	81

/* syscall: "setpgrp" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_setpgrp	82

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval *" "struct itimerval *" */
#define	SUNOS_SYS_setitimer	83

/* syscall: "swapon" ret: "int" args: "char *" */
#define	SUNOS_SYS_swapon	85

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval *" */
#define	SUNOS_SYS_getitimer	86

/* syscall: "gethostname" ret: "int" args: "char *" "u_int" */
#define	SUNOS_SYS_gethostname	87

/* syscall: "sethostname" ret: "int" args: "char *" "u_int" */
#define	SUNOS_SYS_sethostname	88

/* syscall: "getdtablesize" ret: "int" args: */
#define	SUNOS_SYS_getdtablesize	89

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	SUNOS_SYS_dup2	90

/* syscall: "fcntl" ret: "int" args: "int" "int" "void *" */
#define	SUNOS_SYS_fcntl	92

/* syscall: "select" ret: "int" args: "u_int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	SUNOS_SYS_select	93

/* syscall: "fsync" ret: "int" args: "int" */
#define	SUNOS_SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	SUNOS_SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	SUNOS_SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "caddr_t" "int" */
#define	SUNOS_SYS_connect	98

/* syscall: "accept" ret: "int" args: "int" "caddr_t" "int *" */
#define	SUNOS_SYS_accept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_getpriority	100

/* syscall: "send" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	SUNOS_SYS_send	101

/* syscall: "recv" ret: "int" args: "int" "caddr_t" "int" "int" */
#define	SUNOS_SYS_recv	102

/* syscall: "bind" ret: "int" args: "int" "caddr_t" "int" */
#define	SUNOS_SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int" */
#define	SUNOS_SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_listen	106

/* syscall: "sigvec" ret: "int" args: "int" "struct sigvec *" "struct sigvec *" */
#define	SUNOS_SYS_sigvec	108

/* syscall: "sigblock" ret: "int" args: "int" */
#define	SUNOS_SYS_sigblock	109

/* syscall: "sigsetmask" ret: "int" args: "int" */
#define	SUNOS_SYS_sigsetmask	110

/* syscall: "sigsuspend" ret: "int" args: "int" */
#define	SUNOS_SYS_sigsuspend	111

/* syscall: "sigstack" ret: "int" args: "struct sigstack *" "struct sigstack *" */
#define	SUNOS_SYS_sigstack	112

/* syscall: "recvmsg" ret: "int" args: "int" "struct omsghdr *" "int" */
#define	SUNOS_SYS_recvmsg	113

/* syscall: "sendmsg" ret: "int" args: "int" "caddr_t" "int" */
#define	SUNOS_SYS_sendmsg	114

				/* 115 is obsolete vtrace */
/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	SUNOS_SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	SUNOS_SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "caddr_t" "int *" */
#define	SUNOS_SYS_getsockopt	118

/* syscall: "readv" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	SUNOS_SYS_readv	120

/* syscall: "writev" ret: "int" args: "int" "struct iovec *" "u_int" */
#define	SUNOS_SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	SUNOS_SYS_settimeofday	122

/* syscall: "fchown" ret: "int" args: "int" "int" "int" */
#define	SUNOS_SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_fchmod	124

/* syscall: "recvfrom" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int *" */
#define	SUNOS_SYS_recvfrom	125

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	SUNOS_SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	SUNOS_SYS_setregid	127

/* syscall: "rename" ret: "int" args: "char *" "char *" */
#define	SUNOS_SYS_rename	128

/* syscall: "truncate" ret: "int" args: "char *" "long" */
#define	SUNOS_SYS_truncate	129

/* syscall: "ftruncate" ret: "int" args: "int" "long" */
#define	SUNOS_SYS_ftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_flock	131

/* syscall: "sendto" ret: "int" args: "int" "caddr_t" "size_t" "int" "caddr_t" "int" */
#define	SUNOS_SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	SUNOS_SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "char *" */
#define	SUNOS_SYS_rmdir	137

/* syscall: "utimes" ret: "int" args: "char *" "struct timeval *" */
#define	SUNOS_SYS_utimes	138

/* syscall: "sigreturn" ret: "int" args: "struct sigcontext *" */
#define	SUNOS_SYS_sigreturn	139

/* syscall: "adjtime" ret: "int" args: "struct timeval *" "struct timeval *" */
#define	SUNOS_SYS_adjtime	140

/* syscall: "getpeername" ret: "int" args: "int" "caddr_t" "int *" */
#define	SUNOS_SYS_getpeername	141

/* syscall: "gethostid" ret: "int" args: */
#define	SUNOS_SYS_gethostid	142

/* syscall: "getrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	SUNOS_SYS_getrlimit	144

/* syscall: "setrlimit" ret: "int" args: "u_int" "struct orlimit *" */
#define	SUNOS_SYS_setrlimit	145

/* syscall: "killpg" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_killpg	146

/* syscall: "getsockname" ret: "int" args: "int" "caddr_t" "int *" */
#define	SUNOS_SYS_getsockname	150

/* syscall: "poll" ret: "int" args: "struct pollfd *" "unsigned long" "int" */
#define	SUNOS_SYS_poll	153

/* syscall: "nfssvc" ret: "int" args: "int" */
#define	SUNOS_SYS_nfssvc	155

/* syscall: "getdirentries" ret: "int" args: "int" "char *" "u_int" "long *" */
#define	SUNOS_SYS_getdirentries	156

/* syscall: "statfs" ret: "int" args: "char *" "struct sunos_statfs *" */
#define	SUNOS_SYS_statfs	157

/* syscall: "fstatfs" ret: "int" args: "int" "struct sunos_statfs *" */
#define	SUNOS_SYS_fstatfs	158

/* syscall: "unmount" ret: "int" args: "char *" */
#define	SUNOS_SYS_unmount	159

/* syscall: "async_daemon" ret: "int" args: */
#define	SUNOS_SYS_async_daemon	160

/* syscall: "getfh" ret: "int" args: "char *" "fhandle_t *" */
#define	SUNOS_SYS_getfh	161

/* syscall: "getdomainname" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_getdomainname	162

/* syscall: "setdomainname" ret: "int" args: "char *" "int" */
#define	SUNOS_SYS_setdomainname	163

/* syscall: "quotactl" ret: "int" args: "int" "char *" "int" "caddr_t" */
#define	SUNOS_SYS_quotactl	165

/* syscall: "exportfs" ret: "int" args: "char *" "char *" */
#define	SUNOS_SYS_exportfs	166

/* syscall: "mount" ret: "int" args: "char *" "char *" "int" "caddr_t" */
#define	SUNOS_SYS_mount	167

/* syscall: "ustat" ret: "int" args: "int" "struct sunos_ustat *" */
#define	SUNOS_SYS_ustat	168

/* syscall: "semsys" ret: "int" args: "int" "int" "int" "int" "int" */
#define	SUNOS_SYS_semsys	169

/* syscall: "msgsys" ret: "int" args: "int" "int" "int" "int" "int" "int" */
#define	SUNOS_SYS_msgsys	170

/* syscall: "shmsys" ret: "int" args: "int" "int" "int" "int" */
#define	SUNOS_SYS_shmsys	171

/* syscall: "auditsys" ret: "int" args: "char *" */
#define	SUNOS_SYS_auditsys	172

/* syscall: "getdents" ret: "int" args: "int" "char *" "int" */
#define	SUNOS_SYS_getdents	174

/* syscall: "setsid" ret: "int" args: */
#define	SUNOS_SYS_setsid	175

/* syscall: "fchdir" ret: "int" args: "int" */
#define	SUNOS_SYS_fchdir	176

/* syscall: "fchroot" ret: "int" args: "int" */
#define	SUNOS_SYS_fchroot	177

/* syscall: "sigpending" ret: "int" args: "int *" */
#define	SUNOS_SYS_sigpending	183

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	SUNOS_SYS_setpgid	185

/* syscall: "pathconf" ret: "long" args: "char *" "int" */
#define	SUNOS_SYS_pathconf	186

/* syscall: "fpathconf" ret: "long" args: "int" "int" */
#define	SUNOS_SYS_fpathconf	187

/* syscall: "sysconf" ret: "int" args: "int" */
#define	SUNOS_SYS_sysconf	188

/* syscall: "uname" ret: "int" args: "struct sunos_utsname *" */
#define	SUNOS_SYS_uname	189

#define	SUNOS_SYS_MAXSYSCALL	190
