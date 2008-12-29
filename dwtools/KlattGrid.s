# 1 "KlattGrid.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "KlattGrid.c"
# 23 "KlattGrid.c"
# 1 "KlattGrid.h" 1
# 27 "KlattGrid.h"
# 1 "../sys/Collection.h" 1
# 53 "../sys/Collection.h"
# 1 "../sys/Simple.h" 1
# 27 "../sys/Simple.h"
# 1 "../sys/Data.h" 1
# 29 "../sys/Data.h"
# 1 "../sys/Thing.h" 1
# 39 "../sys/Thing.h"
# 1 "../sys/melder.h" 1
# 26 "../sys/melder.h"
# 1 "/usr/include/stdio.h" 1 3 4
# 28 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 330 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 348 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 349 "/usr/include/sys/cdefs.h" 2 3 4
# 331 "/usr/include/features.h" 2 3 4
# 354 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4


# 1 "/usr/include/gnu/stubs-32.h" 1 3 4
# 8 "/usr/include/gnu/stubs.h" 2 3 4
# 355 "/usr/include/features.h" 2 3 4
# 29 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 214 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 3 4
typedef unsigned int size_t;
# 35 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/bits/types.h" 1 3 4
# 28 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;




__extension__ typedef signed long long int __int64_t;
__extension__ typedef unsigned long long int __uint64_t;







__extension__ typedef long long int __quad_t;
__extension__ typedef unsigned long long int __u_quad_t;
# 131 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 132 "/usr/include/bits/types.h" 2 3 4


__extension__ typedef __u_quad_t __dev_t;
__extension__ typedef unsigned int __uid_t;
__extension__ typedef unsigned int __gid_t;
__extension__ typedef unsigned long int __ino_t;
__extension__ typedef __u_quad_t __ino64_t;
__extension__ typedef unsigned int __mode_t;
__extension__ typedef unsigned int __nlink_t;
__extension__ typedef long int __off_t;
__extension__ typedef __quad_t __off64_t;
__extension__ typedef int __pid_t;
__extension__ typedef struct { int __val[2]; } __fsid_t;
__extension__ typedef long int __clock_t;
__extension__ typedef unsigned long int __rlim_t;
__extension__ typedef __u_quad_t __rlim64_t;
__extension__ typedef unsigned int __id_t;
__extension__ typedef long int __time_t;
__extension__ typedef unsigned int __useconds_t;
__extension__ typedef long int __suseconds_t;

__extension__ typedef int __daddr_t;
__extension__ typedef long int __swblk_t;
__extension__ typedef int __key_t;


__extension__ typedef int __clockid_t;


__extension__ typedef void * __timer_t;


__extension__ typedef long int __blksize_t;




__extension__ typedef long int __blkcnt_t;
__extension__ typedef __quad_t __blkcnt64_t;


__extension__ typedef unsigned long int __fsblkcnt_t;
__extension__ typedef __u_quad_t __fsblkcnt64_t;


__extension__ typedef unsigned long int __fsfilcnt_t;
__extension__ typedef __u_quad_t __fsfilcnt64_t;

__extension__ typedef int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


__extension__ typedef int __intptr_t;


__extension__ typedef unsigned int __socklen_t;
# 37 "/usr/include/stdio.h" 2 3 4
# 45 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 65 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 75 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 32 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 78 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 53 "/usr/include/_G_config.h" 3 4
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 33 "/usr/include/libio.h" 2 3 4
# 53 "/usr/include/libio.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stdarg.h" 1 3 4
# 43 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 54 "/usr/include/libio.h" 2 3 4
# 170 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 180 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 203 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 271 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 319 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 328 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 364 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 416 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 458 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 488 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 76 "/usr/include/stdio.h" 2 3 4
# 89 "/usr/include/stdio.h" 3 4


typedef _G_fpos_t fpos_t;




# 141 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 142 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));














extern FILE *tmpfile (void) ;
# 186 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__)) ;





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__)) ;
# 204 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 229 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 243 "/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes) ;




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 272 "/usr/include/stdio.h" 3 4

# 283 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__)) ;
# 304 "/usr/include/stdio.h" 3 4



extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__));








extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 398 "/usr/include/stdio.h" 3 4





extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));
# 441 "/usr/include/stdio.h" 3 4

# 504 "/usr/include/stdio.h" 3 4





extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 528 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 539 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 572 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 653 "/usr/include/stdio.h" 3 4





extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s) ;

# 706 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream) ;








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 742 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 761 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
# 784 "/usr/include/stdio.h" 3 4

# 793 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;








extern void perror (__const char *__s);






# 1 "/usr/include/bits/sys_errlist.h" 1 3 4
# 27 "/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];
# 823 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
# 842 "/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
# 882 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
# 912 "/usr/include/stdio.h" 3 4

# 27 "../sys/melder.h" 2
# 1 "/usr/include/stdlib.h" 1 3 4
# 33 "/usr/include/stdlib.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 326 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 3 4
typedef int wchar_t;
# 34 "/usr/include/stdlib.h" 2 3 4


# 96 "/usr/include/stdlib.h" 3 4


typedef struct
  {
    int quot;
    int rem;
  } div_t;



typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;



# 140 "/usr/include/stdlib.h" 3 4
extern size_t __ctype_get_mb_cur_max (void) __attribute__ ((__nothrow__)) ;




extern double atof (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern int atoi (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

extern long int atol (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





__extension__ extern long long int atoll (__const char *__nptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;





extern double strtod (__const char *__restrict __nptr,
        char **__restrict __endptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 182 "/usr/include/stdlib.h" 3 4


extern long int strtol (__const char *__restrict __nptr,
   char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

extern unsigned long int strtoul (__const char *__restrict __nptr,
      char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




__extension__
extern long long int strtoq (__const char *__restrict __nptr,
        char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtouq (__const char *__restrict __nptr,
           char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





__extension__
extern long long int strtoll (__const char *__restrict __nptr,
         char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

__extension__
extern unsigned long long int strtoull (__const char *__restrict __nptr,
     char **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;

# 311 "/usr/include/stdlib.h" 3 4
extern char *l64a (long int __n) __attribute__ ((__nothrow__)) ;


extern long int a64l (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;




# 1 "/usr/include/sys/types.h" 1 3 4
# 29 "/usr/include/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 62 "/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;





typedef __off_t off_t;
# 100 "/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;




typedef __id_t id_t;




typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 133 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 75 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 93 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 105 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/usr/include/sys/types.h" 2 3 4
# 147 "/usr/include/sys/types.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 148 "/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 195 "/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 217 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/endian.h" 1 3 4
# 38 "/usr/include/endian.h" 2 3 4
# 218 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/select.h" 1 3 4
# 31 "/usr/include/sys/select.h" 3 4
# 1 "/usr/include/bits/select.h" 1 3 4
# 32 "/usr/include/sys/select.h" 2 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 24 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 35 "/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 121 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
# 45 "/usr/include/sys/select.h" 2 3 4

# 1 "/usr/include/bits/time.h" 1 3 4
# 69 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 47 "/usr/include/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 99 "/usr/include/sys/select.h" 3 4

# 109 "/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 121 "/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



# 221 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/sysmacros.h" 1 3 4
# 30 "/usr/include/sys/sysmacros.h" 3 4
__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__));
# 224 "/usr/include/sys/types.h" 2 3 4
# 235 "/usr/include/sys/types.h" 3 4
typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 270 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/pthreadtypes.h" 2 3 4
# 50 "/usr/include/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


typedef union
{
  char __size[36];
  long int __align;
} pthread_attr_t;
# 67 "/usr/include/bits/pthreadtypes.h" 3 4
typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;





typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;





    int __kind;





    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __pthread_slist_t __list;
    };

  } __data;
  char __size[24];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{
# 170 "/usr/include/bits/pthreadtypes.h" 3 4
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;


    unsigned char __flags;
    unsigned char __shared;
    unsigned char __pad1;
    unsigned char __pad2;
    int __writer;
  } __data;

  char __size[32];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[20];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 271 "/usr/include/sys/types.h" 2 3 4



# 321 "/usr/include/stdlib.h" 2 3 4






extern long int random (void) __attribute__ ((__nothrow__));


extern void srandom (unsigned int __seed) __attribute__ ((__nothrow__));





extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));



extern char *setstate (char *__statebuf) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));







struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };

extern int random_r (struct random_data *__restrict __buf,
       int32_t *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char *__restrict __statebuf,
   size_t __statelen,
   struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char *__restrict __statebuf,
         struct random_data *__restrict __buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern int rand (void) __attribute__ ((__nothrow__));

extern void srand (unsigned int __seed) __attribute__ ((__nothrow__));




extern int rand_r (unsigned int *__seed) __attribute__ ((__nothrow__));







extern double drand48 (void) __attribute__ ((__nothrow__));
extern double erand48 (unsigned short int __xsubi[3]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int lrand48 (void) __attribute__ ((__nothrow__));
extern long int nrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern long int mrand48 (void) __attribute__ ((__nothrow__));
extern long int jrand48 (unsigned short int __xsubi[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern void srand48 (long int __seedval) __attribute__ ((__nothrow__));
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };


extern int drand48_r (struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        double *__restrict __result) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int lrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int mrand48_r (struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *__restrict __buffer,
        long int *__restrict __result)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));









extern void *malloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;

extern void *calloc (size_t __nmemb, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;










extern void *realloc (void *__ptr, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__warn_unused_result__));

extern void free (void *__ptr) __attribute__ ((__nothrow__));




extern void cfree (void *__ptr) __attribute__ ((__nothrow__));



# 1 "/usr/include/alloca.h" 1 3 4
# 25 "/usr/include/alloca.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 26 "/usr/include/alloca.h" 2 3 4







extern void *alloca (size_t __size) __attribute__ ((__nothrow__));






# 498 "/usr/include/stdlib.h" 2 3 4




extern void *valloc (size_t __size) __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;




extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern void abort (void) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



extern int atexit (void (*__func) (void)) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern void exit (int __status) __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));

# 543 "/usr/include/stdlib.h" 3 4


extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;




extern char *__secure_getenv (__const char *__name)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;





extern int putenv (char *__string) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));





extern int setenv (__const char *__name, __const char *__value, int __replace)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));


extern int unsetenv (__const char *__name) __attribute__ ((__nothrow__));






extern int clearenv (void) __attribute__ ((__nothrow__));
# 583 "/usr/include/stdlib.h" 3 4
extern char *mktemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 594 "/usr/include/stdlib.h" 3 4
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
# 614 "/usr/include/stdlib.h" 3 4
extern char *mkdtemp (char *__template) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 640 "/usr/include/stdlib.h" 3 4





extern int system (__const char *__command) ;

# 662 "/usr/include/stdlib.h" 3 4
extern char *realpath (__const char *__restrict __name,
         char *__restrict __resolved) __attribute__ ((__nothrow__)) ;






typedef int (*__compar_fn_t) (__const void *, __const void *);
# 680 "/usr/include/stdlib.h" 3 4



extern void *bsearch (__const void *__key, __const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;



extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
# 699 "/usr/include/stdlib.h" 3 4
extern int abs (int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern long int labs (long int __x) __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;












extern div_t div (int __numer, int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     __attribute__ ((__nothrow__)) __attribute__ ((__const__)) ;

# 735 "/usr/include/stdlib.h" 3 4
extern char *ecvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *fcvt (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;




extern char *gcvt (double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern char *qecvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3))) ;




extern int ecvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int *__restrict __decpt,
     int *__restrict __sign, char *__restrict __buf,
     size_t __len) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
      int *__restrict __decpt, int *__restrict __sign,
      char *__restrict __buf, size_t __len)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (3, 4, 5)));







extern int mblen (__const char *__s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int mbtowc (wchar_t *__restrict __pwc,
     __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__)) ;


extern int wctomb (char *__s, wchar_t __wchar) __attribute__ ((__nothrow__)) ;



extern size_t mbstowcs (wchar_t *__restrict __pwcs,
   __const char *__restrict __s, size_t __n) __attribute__ ((__nothrow__));

extern size_t wcstombs (char *__restrict __s,
   __const wchar_t *__restrict __pwcs, size_t __n)
     __attribute__ ((__nothrow__));








extern int rpmatch (__const char *__response) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
# 840 "/usr/include/stdlib.h" 3 4
extern int posix_openpt (int __oflag) ;
# 875 "/usr/include/stdlib.h" 3 4
extern int getloadavg (double __loadavg[], int __nelem)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 891 "/usr/include/stdlib.h" 3 4

# 28 "../sys/melder.h" 2
# 1 "/usr/include/string.h" 1 3 4
# 28 "/usr/include/string.h" 3 4





# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 34 "/usr/include/string.h" 2 3 4




extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 82 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));

# 130 "/usr/include/string.h" 3 4
extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 165 "/usr/include/string.h" 3 4


extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 181 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 240 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));

# 254 "/usr/include/string.h" 3 4


extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 270 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));
# 294 "/usr/include/string.h" 3 4
extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));



extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 331 "/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 354 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 432 "/usr/include/string.h" 3 4

# 29 "../sys/melder.h" 2
# 37 "../sys/melder.h"
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stdarg.h" 1 3 4
# 105 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stdarg.h" 3 4
typedef __gnuc_va_list va_list;
# 38 "../sys/melder.h" 2
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 152 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 3 4
typedef int ptrdiff_t;
# 39 "../sys/melder.h" 2
# 1 "/usr/include/wchar.h" 1 3 4
# 42 "/usr/include/wchar.h" 3 4
# 1 "/usr/include/bits/wchar.h" 1 3 4
# 43 "/usr/include/wchar.h" 2 3 4
# 52 "/usr/include/wchar.h" 3 4
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 1 3 4
# 355 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stddef.h" 3 4
typedef unsigned int wint_t;
# 53 "/usr/include/wchar.h" 2 3 4
# 99 "/usr/include/wchar.h" 3 4


typedef __mbstate_t mbstate_t;

# 124 "/usr/include/wchar.h" 3 4





struct tm;









extern wchar_t *wcscpy (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncpy (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wcscat (wchar_t *__restrict __dest,
   __const wchar_t *__restrict __src) __attribute__ ((__nothrow__));

extern wchar_t *wcsncat (wchar_t *__restrict __dest,
    __const wchar_t *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__));


extern int wcscmp (__const wchar_t *__s1, __const wchar_t *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern int wcsncmp (__const wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

# 181 "/usr/include/wchar.h" 3 4



extern int wcscoll (__const wchar_t *__s1, __const wchar_t *__s2) __attribute__ ((__nothrow__));



extern size_t wcsxfrm (wchar_t *__restrict __s1,
         __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));

# 211 "/usr/include/wchar.h" 3 4


extern wchar_t *wcschr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcsrchr (__const wchar_t *__wcs, wchar_t __wc)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

# 227 "/usr/include/wchar.h" 3 4



extern size_t wcscspn (__const wchar_t *__wcs, __const wchar_t *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern size_t wcsspn (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcspbrk (__const wchar_t *__wcs, __const wchar_t *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

extern wchar_t *wcsstr (__const wchar_t *__haystack, __const wchar_t *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern wchar_t *wcstok (wchar_t *__restrict __s,
   __const wchar_t *__restrict __delim,
   wchar_t **__restrict __ptr) __attribute__ ((__nothrow__));


extern size_t wcslen (__const wchar_t *__s) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));

# 265 "/usr/include/wchar.h" 3 4


extern wchar_t *wmemchr (__const wchar_t *__s, wchar_t __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern int wmemcmp (__const wchar_t *__restrict __s1,
      __const wchar_t *__restrict __s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__));


extern wchar_t *wmemcpy (wchar_t *__restrict __s1,
    __const wchar_t *__restrict __s2, size_t __n) __attribute__ ((__nothrow__));



extern wchar_t *wmemmove (wchar_t *__s1, __const wchar_t *__s2, size_t __n)
     __attribute__ ((__nothrow__));


extern wchar_t *wmemset (wchar_t *__s, wchar_t __c, size_t __n) __attribute__ ((__nothrow__));

# 297 "/usr/include/wchar.h" 3 4



extern wint_t btowc (int __c) __attribute__ ((__nothrow__));



extern int wctob (wint_t __c) __attribute__ ((__nothrow__));



extern int mbsinit (__const mbstate_t *__ps) __attribute__ ((__nothrow__)) __attribute__ ((__pure__));



extern size_t mbrtowc (wchar_t *__restrict __pwc,
         __const char *__restrict __s, size_t __n,
         mbstate_t *__p) __attribute__ ((__nothrow__));


extern size_t wcrtomb (char *__restrict __s, wchar_t __wc,
         mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));


extern size_t __mbrlen (__const char *__restrict __s, size_t __n,
   mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));
extern size_t mbrlen (__const char *__restrict __s, size_t __n,
        mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

# 352 "/usr/include/wchar.h" 3 4



extern size_t mbsrtowcs (wchar_t *__restrict __dst,
    __const char **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));



extern size_t wcsrtombs (char *__restrict __dst,
    __const wchar_t **__restrict __src, size_t __len,
    mbstate_t *__restrict __ps) __attribute__ ((__nothrow__));

# 394 "/usr/include/wchar.h" 3 4



extern double wcstod (__const wchar_t *__restrict __nptr,
        wchar_t **__restrict __endptr) __attribute__ ((__nothrow__));

# 412 "/usr/include/wchar.h" 3 4



extern long int wcstol (__const wchar_t *__restrict __nptr,
   wchar_t **__restrict __endptr, int __base) __attribute__ ((__nothrow__));



extern unsigned long int wcstoul (__const wchar_t *__restrict __nptr,
      wchar_t **__restrict __endptr, int __base)
     __attribute__ ((__nothrow__));

# 687 "/usr/include/wchar.h" 3 4





extern wint_t fgetwc (__FILE *__stream);
extern wint_t getwc (__FILE *__stream);





extern wint_t getwchar (void);






extern wint_t fputwc (wchar_t __wc, __FILE *__stream);
extern wint_t putwc (wchar_t __wc, __FILE *__stream);





extern wint_t putwchar (wchar_t __wc);







extern wchar_t *fgetws (wchar_t *__restrict __ws, int __n,
   __FILE *__restrict __stream);





extern int fputws (__const wchar_t *__restrict __ws,
     __FILE *__restrict __stream);






extern wint_t ungetwc (wint_t __wc, __FILE *__stream);

# 798 "/usr/include/wchar.h" 3 4




extern size_t wcsftime (wchar_t *__restrict __s, size_t __maxsize,
   __const wchar_t *__restrict __format,
   __const struct tm *__restrict __tp) __attribute__ ((__nothrow__));

# 838 "/usr/include/wchar.h" 3 4

# 40 "../sys/melder.h" 2
# 1 "/usr/lib/gcc/i486-linux-gnu/4.3.2/include/stdbool.h" 1 3 4
# 41 "../sys/melder.h" 2
_Bool Melder_wcsequ_firstCharacterCaseInsensitive (const wchar_t *string1, const wchar_t *string2);
# 1 "../sys/enums.h" 1
# 43 "../sys/melder.h" 2

# 1 "../sys/melder_enums.h" 1
# 24 "../sys/melder_enums.h"
enum kMelder_number { kMelder_number_MIN = 1,
 kMelder_number_EQUAL_TO = 1,
 kMelder_number_NOT_EQUAL_TO = 2,
 kMelder_number_LESS_THAN = 3,
 kMelder_number_LESS_THAN_OR_EQUAL_TO = 4,
 kMelder_number_GREATER_THAN = 5,
 kMelder_number_GREATER_THAN_OR_EQUAL_TO = 6,







kMelder_number_MAX = 6, kMelder_number_DEFAULT = kMelder_number_EQUAL_TO }; const wchar_t * kMelder_number_getText (int value); int kMelder_number_getValue (const wchar_t *text);

enum kMelder_string { kMelder_string_MIN = 1,
 kMelder_string_EQUAL_TO = 1,
 kMelder_string_NOT_EQUAL_TO = 2,
 kMelder_string_CONTAINS = 3,
 kMelder_string_DOES_NOT_CONTAIN = 4,
 kMelder_string_STARTS_WITH = 5,
 kMelder_string_DOES_NOT_START_WITH = 6,
 kMelder_string_ENDS_WITH = 7,
 kMelder_string_DOES_NOT_END_WITH = 8,
 kMelder_string_MATCH_REGEXP = 9,










kMelder_string_MAX = 9, kMelder_string_DEFAULT = kMelder_string_EQUAL_TO }; const wchar_t * kMelder_string_getText (int value); int kMelder_string_getValue (const wchar_t *text);

enum kMelder_textInputEncoding { kMelder_textInputEncoding_MIN = 1,
 kMelder_textInputEncoding_UTF8 = 1,
 kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1 = 2,
 kMelder_textInputEncoding_ISO_LATIN1 = 3,
 kMelder_textInputEncoding_UTF8_THEN_WINDOWS_LATIN1 = 4,
 kMelder_textInputEncoding_WINDOWS_LATIN1 = 5,
 kMelder_textInputEncoding_UTF8_THEN_MACROMAN = 6,
 kMelder_textInputEncoding_MACROMAN = 7,













kMelder_textInputEncoding_MAX = 7, kMelder_textInputEncoding_DEFAULT = kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1 }; const wchar_t * kMelder_textInputEncoding_getText (int value); int kMelder_textInputEncoding_getValue (const wchar_t *text);


enum kMelder_textOutputEncoding { kMelder_textOutputEncoding_MIN = 1,
 kMelder_textOutputEncoding_UTF8 = 1,
 kMelder_textOutputEncoding_UTF16 = 2,
 kMelder_textOutputEncoding_ASCII_THEN_UTF16 = 3,
 kMelder_textOutputEncoding_ISO_LATIN1_THEN_UTF16 = 4,





kMelder_textOutputEncoding_MAX = 4, kMelder_textOutputEncoding_DEFAULT = kMelder_textOutputEncoding_ASCII_THEN_UTF16 }; const wchar_t * kMelder_textOutputEncoding_getText (int value); int kMelder_textOutputEncoding_getValue (const wchar_t *text);

enum kMelder_asynchronicityLevel { kMelder_asynchronicityLevel_MIN = 0,
 kMelder_asynchronicityLevel_SYNCHRONOUS = 0,
 kMelder_asynchronicityLevel_CALLING_BACK = 1,
 kMelder_asynchronicityLevel_INTERRUPTABLE = 2,
 kMelder_asynchronicityLevel_ASYNCHRONOUS = 3,





kMelder_asynchronicityLevel_MAX = 3, kMelder_asynchronicityLevel_DEFAULT = kMelder_asynchronicityLevel_ASYNCHRONOUS }; const wchar_t * kMelder_asynchronicityLevel_getText (int value); int kMelder_asynchronicityLevel_getValue (const wchar_t *text);
# 45 "../sys/melder.h" 2
# 62 "../sys/melder.h"
const wchar_t * Melder_integer (long value);
const wchar_t * Melder_bigInteger (double value);
const wchar_t * Melder_boolean (_Bool value);
const wchar_t * Melder_double (double value);
const wchar_t * Melder_single (double value);
const wchar_t * Melder_half (double value);
const wchar_t * Melder_fixed (double value, int precision);
const wchar_t * Melder_fixedExponent (double value, int exponent, int precision);

const wchar_t * Melder_percent (double value, int precision);

const wchar_t * Melder_float (const wchar_t *number);




int Melder_isStringNumeric (const wchar_t *string);
double Melder_atof (const wchar_t *string);
# 88 "../sys/melder.h"
void Melder_writeToConsole (wchar_t *message, _Bool useStderr);



int Melder_error (const char *format, ...);
_Bool Melder_error1 (const wchar_t *s1);
_Bool Melder_error2 (const wchar_t *s1, const wchar_t *s2);
_Bool Melder_error3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
_Bool Melder_error4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
_Bool Melder_error5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5);
_Bool Melder_error6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6);
_Bool Melder_error7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
_Bool Melder_error8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
_Bool Melder_error9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
_Bool Melder_error10 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10);
_Bool Melder_error11 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11);
_Bool Melder_error12 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12);
_Bool Melder_error13 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13);
_Bool Melder_error14 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14);
_Bool Melder_error15 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15);
_Bool Melder_error16 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16);
_Bool Melder_error17 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17);
_Bool Melder_error18 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
 const wchar_t *s18);
_Bool Melder_error19 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
 const wchar_t *s18, const wchar_t *s19);
void * Melder_errorp1 (const wchar_t *s1);
void * Melder_errorp2 (const wchar_t *s1, const wchar_t *s2);
void * Melder_errorp3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void * Melder_errorp4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void * Melder_errorp5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5);
void * Melder_errorp6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6);
void * Melder_errorp7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void * Melder_errorp8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void * Melder_errorp9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void * Melder_errorp10 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10);
void * Melder_errorp11 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11);
void * Melder_errorp12 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12);
void * Melder_errorp13 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13);
void * Melder_errorp14 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14);
void * Melder_errorp15 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15);
void * Melder_errorp16 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16);
void * Melder_errorp17 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17);
void * Melder_errorp18 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
 const wchar_t *s18);
void * Melder_errorp19 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5,
 const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9, const wchar_t *s10, const wchar_t *s11,
 const wchar_t *s12, const wchar_t *s13, const wchar_t *s14, const wchar_t *s15, const wchar_t *s16, const wchar_t *s17,
 const wchar_t *s18, const wchar_t *s19);
# 202 "../sys/melder.h"
void * Melder_errorp (const char *format, ...);
void * Melder_errorpW (const wchar_t *format, ...);


void Melder_flushError (const char *format, ...);
void Melder_flushErrorW (const wchar_t *format, ...);



int Melder_hasError (void);




void Melder_clearError (void);


wchar_t * Melder_getError (void);


int Melder_fatal (const char *format, ...);
# 234 "../sys/melder.h"
int _Melder_assert (const char *condition, const char *fileName, int lineNumber);
# 244 "../sys/melder.h"
void * _Melder_malloc (unsigned long size);

void * Melder_realloc (void *pointer, long size);
void * _Melder_calloc (long numberOfElements, long elementSize);

char * Melder_strdup (const char *string);
wchar_t * Melder_wcsdup (const wchar_t *string);
int Melder_strcmp (const char *string1, const char *string2);
int Melder_wcscmp (const wchar_t *string1, const wchar_t *string2);
int Melder_strncmp (const char *string1, const char *string2, unsigned long n);
int Melder_wcsncmp (const wchar_t *string1, const wchar_t *string2, unsigned long n);
wchar_t * Melder_wcsdecompose (const wchar_t *string);
wchar_t * Melder_wcsprecompose (const wchar_t *string);
wchar_t * Melder_wcsExpandBackslashSequences (const wchar_t *string);
wchar_t * Melder_wcsReduceBackslashSequences (const wchar_t *string);
void Melder_wcsReduceBackslashSequences_inline (const wchar_t *string);




void Melder_textEncoding_prefs (void);
void Melder_setInputEncoding (enum kMelder_textInputEncoding encoding);
int Melder_getInputEncoding (void);
void Melder_setOutputEncoding (enum kMelder_textOutputEncoding encoding);
int Melder_getOutputEncoding (void);
# 280 "../sys/melder.h"
typedef unsigned short MelderUtf16;
typedef unsigned int MelderUtf32;

_Bool Melder_isValidAscii (const wchar_t *string);
_Bool Melder_strIsValidUtf8 (const char *string);
_Bool Melder_isEncodable (const wchar_t *string, int outputEncoding);
extern wchar_t Melder_decodeMacRoman [256];
extern wchar_t Melder_decodeWindowsLatin1 [256];

long Melder_killReturns_inlineW (wchar_t *text);

int Melder_8bitToWcs_inline (const char *string, wchar_t *wcs, int inputEncoding);
wchar_t * Melder_8bitToWcs (const char *string, int inputEncoding);
wchar_t * Melder_utf8ToWcs (const char *string);
int Melder_wcsTo8bit_inline (const wchar_t *wcs, char *string, int outputEncoding);
char * Melder_wcsTo8bit (const wchar_t *string, int outputEncoding);

char * Melder_wcsToUtf8 (const wchar_t *string);
void Melder_wcsToUtf8_inline (const wchar_t *wcs, char *utf8);
void Melder_wcsTo8bitFileRepresentation_inline (const wchar_t *wcs, char *utf8);
void Melder_8bitFileRepresentationToWcs_inline (const char *utf8, wchar_t *wcs);
wchar_t * Melder_peekUtf8ToWcs (const char *string);
char * Melder_peekWcsToUtf8 (const wchar_t *string);
const MelderUtf16 * Melder_peekWcsToUtf16 (const wchar_t *string);
const void * Melder_peekWcsToCfstring (const wchar_t *string);
void Melder_fwriteWcsAsUtf8 (const wchar_t *ptr, size_t n, FILE *f);
# 318 "../sys/melder.h"
void _Melder_free (void **pointer);







double Melder_allocationCount (void);






double Melder_deallocationCount (void);





double Melder_allocationSize (void);
# 354 "../sys/melder.h"
struct FLAC__StreamDecoder;
struct FLAC__StreamEncoder;

typedef struct {
 FILE *filePointer;
 wchar_t path [260];
 _Bool openForReading, openForWriting, verbose, requiresCRLF;
 unsigned long outputEncoding;
 int indent;
 struct FLAC__StreamEncoder *flacEncoder;
} structMelderFile, *MelderFile;
typedef struct {
 wchar_t path [260];
} structMelderDir, *MelderDir;





typedef struct {
 unsigned long length;
 unsigned long bufferSize;
 wchar_t *string;
} MelderString;
typedef struct {
 unsigned long length;
 unsigned long bufferSize;
 MelderUtf16 *string;
} MelderString16;

void MelderString_free (MelderString *me);
void MelderString16_free (MelderString16 *me);
void MelderString_empty (MelderString *me);
void MelderString16_empty (MelderString16 *me);
_Bool MelderString_copy (MelderString *me, const wchar_t *source);
_Bool MelderString_ncopy (MelderString *me, const wchar_t *source, unsigned long n);
_Bool MelderString_append (MelderString *me, const wchar_t *source);
_Bool MelderString_append1 (MelderString *me, const wchar_t *s1);
_Bool MelderString_append2 (MelderString *me, const wchar_t *s1, const wchar_t *s2);
_Bool MelderString_append3 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
_Bool MelderString_append4 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
_Bool MelderString_append5 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5);
_Bool MelderString_append6 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6);
_Bool MelderString_append7 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
_Bool MelderString_append8 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
_Bool MelderString_append9 (MelderString *me, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4,
 const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
_Bool MelderString_appendCharacter (MelderString *me, wchar_t character);
_Bool MelderString16_appendCharacter (MelderString16 *me, wchar_t character);
_Bool MelderString_get (MelderString *me, wchar_t *destination);
double MelderString_allocationCount (void);
double MelderString_deallocationCount (void);
double MelderString_allocationSize (void);
double MelderString_deallocationSize (void);

typedef struct structMelderReadText *MelderReadText;
MelderReadText MelderReadText_createFromFile (MelderFile file);
MelderReadText MelderReadText_createFromString (const wchar_t *string);
_Bool MelderReadText_isValid (MelderReadText text);
wchar_t MelderReadText_getChar (MelderReadText text);
wchar_t * MelderReadText_readLine (MelderReadText text);
long MelderReadText_getNumberOfLines (MelderReadText me);
const wchar_t * MelderReadText_getLineNumber (MelderReadText text);
void MelderReadText_delete (MelderReadText text);

const wchar_t * Melder_wcscat2 (const wchar_t *s1, const wchar_t *s2);
const wchar_t * Melder_wcscat3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
const wchar_t * Melder_wcscat4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
const wchar_t * Melder_wcscat5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
const wchar_t * Melder_wcscat6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
const wchar_t * Melder_wcscat7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
const wchar_t * Melder_wcscat8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
const wchar_t * Melder_wcscat9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);



int Melder_numberMatchesCriterion (double value, int which_kMelder_number, double criterion);
int Melder_stringMatchesCriterion (const wchar_t *value, int which_kMelder_string, const wchar_t *criterion);
# 449 "../sys/melder.h"
long Melder_countTokens (const wchar_t *string);
wchar_t *Melder_firstToken (const wchar_t *string);
wchar_t *Melder_nextToken (void);
wchar_t ** Melder_getTokens (const wchar_t *string, long *n);
void Melder_freeTokens (wchar_t ***tokens);
long Melder_searchToken (const wchar_t *string, wchar_t **tokens, long n);
# 467 "../sys/melder.h"
void Melder_casual (const char *format, ...);
void Melder_casual1 (const wchar_t *s1);
void Melder_casual2 (const wchar_t *s1, const wchar_t *s2);
void Melder_casual3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void Melder_casual4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void Melder_casual5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void Melder_casual6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void Melder_casual7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void Melder_casual8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void Melder_casual9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
# 486 "../sys/melder.h"
void MelderInfo_open (void);
void MelderInfo_write1 (const wchar_t *s1);
void MelderInfo_write2 (const wchar_t *s1, const wchar_t *s2);
void MelderInfo_write3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void MelderInfo_write4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void MelderInfo_write5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void MelderInfo_write6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void MelderInfo_write7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void MelderInfo_write8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void MelderInfo_write9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void MelderInfo_writeLine1 (const wchar_t *s1);
void MelderInfo_writeLine2 (const wchar_t *s1, const wchar_t *s2);
void MelderInfo_writeLine3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void MelderInfo_writeLine4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void MelderInfo_writeLine5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void MelderInfo_writeLine6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void MelderInfo_writeLine7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void MelderInfo_writeLine8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void MelderInfo_writeLine9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void MelderInfo_close (void);

void Melder_information1 (const wchar_t *s1);
void Melder_information2 (const wchar_t *s1, const wchar_t *s2);
void Melder_information3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void Melder_information4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void Melder_information5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void Melder_information6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void Melder_information7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void Melder_information8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void Melder_information9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);

void Melder_informationReal (double value, const wchar_t *units);

void Melder_divertInfo (MelderString *buffer);

void Melder_print (const wchar_t *s);


void Melder_clearInfo (void);
wchar_t * Melder_getInfo (void);
void Melder_help (const wchar_t *query);

void Melder_search (void);


void Melder_warning1 (const wchar_t *s1);
void Melder_warning2 (const wchar_t *s1, const wchar_t *s2);
void Melder_warning3 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void Melder_warning4 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void Melder_warning5 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void Melder_warning6 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void Melder_warning7 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void Melder_warning8 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void Melder_warning9 (const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void Melder_warningOff (void);
void Melder_warningOn (void);

void Melder_beep (void);

extern int Melder_debug;



int Melder_progress1 (double progress, const wchar_t *s1);
int Melder_progress2 (double progress, const wchar_t *s1, const wchar_t *s2);
int Melder_progress3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
int Melder_progress4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
int Melder_progress5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
int Melder_progress6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
int Melder_progress7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
int Melder_progress8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
int Melder_progress9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void Melder_progressOff (void);
void Melder_progressOn (void);
# 586 "../sys/melder.h"
void * Melder_monitor1 (double progress, const wchar_t *s1);
void * Melder_monitor2 (double progress, const wchar_t *s1, const wchar_t *s2);
void * Melder_monitor3 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void * Melder_monitor4 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void * Melder_monitor5 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void * Melder_monitor6 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void * Melder_monitor7 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void * Melder_monitor8 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void * Melder_monitor9 (double progress, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
# 629 "../sys/melder.h"
_Bool Melder_pause (const wchar_t *message);
# 654 "../sys/melder.h"
int Melder_publish (void *anything);

int Melder_record (double duration);
int Melder_recordFromFile (MelderFile fs);
void Melder_play (void);
void Melder_playReverse (void);
int Melder_publishPlayed (void);



extern unsigned long Melder_systemVersion;






extern char Melder_buffer1 [30001], Melder_buffer2 [30001];
# 690 "../sys/melder.h"
void MelderGui_create ( void *appContext, void *parent);






extern int Melder_batch;
extern int Melder_backgrounding;
extern _Bool Melder_consoleIsAnsi;

 extern void *Melder_appContext;
 extern void *Melder_topShell;
# 711 "../sys/melder.h"
void Melder_setCasualProc (void (*casualProc) (wchar_t *message));
void Melder_setProgressProc (int (*progressProc) (double progress, wchar_t *message));
void Melder_setMonitorProc (void * (*monitorProc) (double progress, wchar_t *message));
void Melder_setPauseProc (_Bool (*pauseProc) (const wchar_t *message));
void Melder_setInformationProc (void (*informationProc) (wchar_t *message));
void Melder_setHelpProc (void (*help) (const wchar_t *query));
void Melder_setSearchProc (void (*search) (void));
void Melder_setWarningProc (void (*warningProc) (wchar_t *message));
void Melder_setErrorProc (void (*errorProc) (wchar_t *message));
void Melder_setFatalProc (void (*fatalProc) (wchar_t *message));
void Melder_setPublishProc (int (*publish) (void *));
void Melder_setRecordProc (int (*record) (double));
void Melder_setRecordFromFileProc (int (*recordFromFile) (MelderFile));
void Melder_setPlayProc (void (*play) (void));
void Melder_setPlayReverseProc (void (*playReverse) (void));
void Melder_setPublishPlayedProc (int (*publishPlayed) (void));
# 736 "../sys/melder.h"
wchar_t * MelderFile_name (MelderFile file);
wchar_t * MelderDir_name (MelderDir dir);
int Melder_pathToDir (const wchar_t *path, MelderDir dir);
int Melder_pathToFile (const wchar_t *path, MelderFile file);
int Melder_relativePathToFile (const wchar_t *path, MelderFile file);
wchar_t * Melder_dirToPath (MelderDir dir);

wchar_t * Melder_fileToPath (MelderFile file);
void MelderFile_copy (MelderFile file, MelderFile copy);
void MelderDir_copy (MelderDir dir, MelderDir copy);
int MelderFile_equal (MelderFile file1, MelderFile file2);
int MelderDir_equal (MelderDir dir1, MelderDir dir2);
void MelderFile_setToNull (MelderFile file);
int MelderFile_isNull (MelderFile file);
void MelderDir_setToNull (MelderDir dir);
int MelderDir_isNull (MelderDir dir);
void MelderDir_getFile (MelderDir parent, const wchar_t *fileName, MelderFile file);
void MelderDir_relativePathToFile (MelderDir dir, const wchar_t *path, MelderFile file);
void MelderFile_getParentDir (MelderFile file, MelderDir parent);
void MelderDir_getParentDir (MelderDir file, MelderDir parent);
int MelderDir_isDesktop (MelderDir dir);
int MelderDir_getSubdir (MelderDir parent, const wchar_t *subdirName, MelderDir subdir);
void Melder_rememberShellDirectory (void);
wchar_t * Melder_getShellDirectory (void);
void Melder_getHomeDir (MelderDir homeDir);
void Melder_getPrefDir (MelderDir prefDir);
void Melder_getTempDir (MelderDir tempDir);

int MelderFile_exists (MelderFile file);
int MelderFile_readable (MelderFile file);
long MelderFile_length (MelderFile file);
int MelderFile_delete (MelderFile file);


FILE * Melder_fopen (MelderFile file, const char *type);






int Melder_fclose (MelderFile file, FILE *stream);
void Melder_files_cleanUp (void);


void MelderFile_open (MelderFile file);
void MelderFile_append (MelderFile file);
void MelderFile_create (MelderFile file, const wchar_t *macType, const wchar_t *macCreator, const wchar_t *winExtension);
void * MelderFile_read (MelderFile file, long nbytes);
char * MelderFile_readLine (MelderFile file);
void MelderFile_writeCharacter (MelderFile file, wchar_t kar);
void MelderFile_write1 (MelderFile file, const wchar_t *s1);
void MelderFile_write2 (MelderFile file, const wchar_t *s1, const wchar_t *s2);
void MelderFile_write3 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void MelderFile_write4 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void MelderFile_write5 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void MelderFile_write6 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void MelderFile_write7 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void MelderFile_write8 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void MelderFile_write9 (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void MelderFile_rewind (MelderFile file);
void MelderFile_seek (MelderFile file, long position, int direction);
long MelderFile_tell (MelderFile file);
void MelderFile_close (MelderFile file);





wchar_t * MelderFile_readText (MelderFile file);
int MelderFile_writeText (MelderFile file, const wchar_t *text);
int MelderFile_appendText (MelderFile file, const wchar_t *text);

int Melder_createDirectory (MelderDir parent, const wchar_t *subdirName, int mode);

void Melder_getDefaultDir (MelderDir dir);
void Melder_setDefaultDir (MelderDir dir);
void MelderFile_setDefaultDir (MelderFile file);





wchar_t * Melder_peekExpandBackslashes (const wchar_t *message);
wchar_t * MelderFile_messageName (MelderFile file);

double Melder_stopwatch (void);

long Melder_killReturns_inline (char *text);
# 839 "../sys/melder.h"
void MelderAudio_setInputUsesPortAudio (_Bool inputUsesPortAudio);
_Bool MelderAudio_getInputUsesPortAudio (void);







void MelderAudio_setOutputUsesPortAudio (_Bool outputUsesPortAudio);
_Bool MelderAudio_getOutputUsesPortAudio (void);




void MelderAudio_setOutputSilenceBefore (double silenceBefore);
double MelderAudio_getOutputSilenceBefore (void);
# 864 "../sys/melder.h"
void MelderAudio_setOutputSilenceAfter (double silenceAfter);
double MelderAudio_getOutputSilenceAfter (void);
void MelderAudio_setOutputUsesBlocking (_Bool outputUsesBlocking);
_Bool MelderAudio_getOutputUsesBlocking (void);
void MelderAudio_setUseInternalSpeaker (_Bool useInternalSpeaker);
_Bool MelderAudio_getUseInternalSpeaker (void);
void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity);
enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity (void);
long MelderAudio_getOutputBestSampleRate (long fsamp);

extern int MelderAudio_isPlaying;
int MelderAudio_play16 (const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels,
 int (*playCallback) (void *playClosure, long numberOfSamplesPlayed), void *playClosure);
int MelderAudio_stopPlaying (_Bool explicit);


long MelderAudio_getSamplesPlayed (void);
_Bool MelderAudio_stopWasExplicit (void);

void Melder_audio_prefs (void);
# 897 "../sys/melder.h"
wchar_t * Melder_audioFileTypeString (int audioFileType);
wchar_t * Melder_macAudioFileType (int audioFileType);
wchar_t * Melder_winAudioFileExtension (int audioFileType);
# 917 "../sys/melder.h"
int Melder_defaultAudioFileEncoding16 (int audioFileType);
int MelderFile_writeAudioFileHeader16 (MelderFile file, int audioFileType, long sampleRate, long numberOfSamples, int numberOfChannels);
int MelderFile_writeAudioFile16 (MelderFile file, int audioFileType, const short *buffer, long sampleRate, long numberOfSamples, int numberOfChannels);

int MelderFile_checkSoundFile (MelderFile file, int *numberOfChannels, int *encoding,
 double *sampleRate, long *startOfData, long *numberOfSamples);




int Melder_bytesPerSamplePoint (int encoding);
int Melder_readAudioToFloat (FILE *f, int numberOfChannels, int encoding,
 double *leftBuffer, double *rightBuffer, long numberOfSamples);




int Melder_readAudioToShort (FILE *f, int numberOfChannels, int encoding, short *buffer, long numberOfSamples);



int MelderFile_writeFloatToAudio (MelderFile file, int encoding, const double *left, long nleft, const double *right, long nright, int warnIfClipped);
int MelderFile_writeShortToAudio (MelderFile file, int numberOfChannels, int encoding, const short *buffer, long numberOfSamples);

void Melder_audioTrigger (void);
# 951 "../sys/melder.h"
const wchar_t * MelderQuantity_getText (int quantity);
const wchar_t * MelderQuantity_getWithUnitText (int quantity);
const wchar_t * MelderQuantity_getLongUnitText (int quantity);
const wchar_t * MelderQuantity_getShortUnitText (int quantity);



wchar_t * Melder_getenv (const wchar_t *variableName);
int Melder_system (const wchar_t *command);
double Melder_clock (void);
# 40 "../sys/Thing.h" 2

# 1 "../sys/enum.h" 1
# 85 "../sys/enum.h"
typedef struct enum_ANY { int _length; const wchar_t *_type, *zero, *_end; int _trailer; } enum_ANY;

typedef signed char enum1;
typedef signed short enum2;
# 100 "../sys/enum.h"
int enum_search (void *enumerated, const wchar_t *string);
# 42 "../sys/Thing.h" 2

# 1 "../sys/oo.h" 1
# 44 "../sys/Thing.h" 2

# 1 "../sys/abcio.h" 1
# 33 "../sys/abcio.h"
# 1 "../sys/complex.h" 1
# 39 "../sys/complex.h"
typedef struct fcomplex { float re, im; } fcomplex;
typedef struct dcomplex { double re, im; } dcomplex;







fcomplex fcomplex_add (fcomplex a, fcomplex b);
dcomplex dcomplex_add (dcomplex a, dcomplex b);

fcomplex fcomplex_sub (fcomplex a, fcomplex b);
dcomplex dcomplex_sub (dcomplex a, dcomplex b);

fcomplex fcomplex_mul (fcomplex a, fcomplex b);
dcomplex dcomplex_mul (dcomplex a, dcomplex b);

fcomplex fcomplex_create (float re, float im);
dcomplex dcomplex_create (double re, double im);

fcomplex fcomplex_conjugate (fcomplex z);
dcomplex dcomplex_conjugate (dcomplex z);

fcomplex fcomplex_div (fcomplex a, fcomplex b);
dcomplex dcomplex_div (dcomplex a, dcomplex b);

float fcomplex_abs (fcomplex z);
double dcomplex_abs (dcomplex z);

fcomplex fcomplex_sqrt (fcomplex z);
dcomplex dcomplex_sqrt (dcomplex z);

fcomplex fcomplex_rmul (float x, fcomplex a);
dcomplex dcomplex_rmul (double x, dcomplex a);

fcomplex fcomplex_exp (fcomplex z);
dcomplex dcomplex_exp (dcomplex z);
# 34 "../sys/abcio.h" 2







int texgeti1 (MelderReadText text);
int texgeti2 (MelderReadText text);
long texgeti4 (MelderReadText text);
unsigned int texgetu1 (MelderReadText text);
unsigned int texgetu2 (MelderReadText text);
unsigned long texgetu4 (MelderReadText text);
double texgetr4 (MelderReadText text);
double texgetr8 (MelderReadText text);
double texgetr10 (MelderReadText text);
fcomplex texgetc8 (MelderReadText text);
dcomplex texgetc16 (MelderReadText text);
char texgetc1 (MelderReadText text);
short texgete1 (MelderReadText text, void *enumerated);
short texgete2 (MelderReadText text, void *enumerated);
short texgeteb (MelderReadText text);
short texgeteq (MelderReadText text);
short texgetex (MelderReadText text);
char *texgets2 (MelderReadText text);
char *texgets4 (MelderReadText text);
wchar_t *texgetw2 (MelderReadText text);
wchar_t *texgetw4 (MelderReadText text);

void texindent (MelderFile file);
void texexdent (MelderFile file);
void texresetindent (MelderFile file);
void texputintro (MelderFile file, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);

void texputi1 (MelderFile file, int i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputi2 (MelderFile file, int i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputi4 (MelderFile file, long i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputu1 (MelderFile file, unsigned int u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputu2 (MelderFile file, unsigned int u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputu4 (MelderFile file, unsigned long u, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputr4 (MelderFile file, double x, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputr8 (MelderFile file, double x, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputc8 (MelderFile file, fcomplex z, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputc16 (MelderFile file, dcomplex z, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputc1 (MelderFile file, int i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texpute1 (MelderFile file, int i, void *enumerated, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texpute2 (MelderFile file, int i, void *enumerated, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputeb (MelderFile file, _Bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputeq (MelderFile file, _Bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputex (MelderFile file, _Bool i, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputs1 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputs2 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputs4 (MelderFile file, const char *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputw2 (MelderFile file, const wchar_t *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void texputw4 (MelderFile file, const wchar_t *s, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
# 97 "../sys/abcio.h"
unsigned int bingetu1 (FILE *f); void binputu1 (unsigned int i, FILE *f);
unsigned int bingetu2 (FILE *f); void binputu2 (unsigned int i, FILE *f);
unsigned long bingetu4 (FILE *f); void binputu4 (unsigned long i, FILE *f);

int bingeti1 (FILE *f); void binputi1 (int i, FILE *f);
int bingeti2 (FILE *f); void binputi2 (int i, FILE *f);
long bingeti3 (FILE *f); void binputi3 (long i, FILE *f);
long bingeti4 (FILE *f); void binputi4 (long i, FILE *f);






int bingeti2LE (FILE *f); void binputi2LE (int i, FILE *f);
long bingeti3LE (FILE *f); void binputi3LE (long i, FILE *f);
long bingeti4LE (FILE *f); void binputi4LE (long i, FILE *f);
unsigned int bingetu2LE (FILE *f); void binputu2LE (unsigned int i, FILE *f);
unsigned long bingetu4LE (FILE *f); void binputu4LE (unsigned long i, FILE *f);






unsigned int bingetb1 (FILE *f); void binputb1 (unsigned int value, FILE *f);
unsigned int bingetb2 (FILE *f); void binputb2 (unsigned int value, FILE *f);
unsigned int bingetb3 (FILE *f); void binputb3 (unsigned int value, FILE *f);
unsigned int bingetb4 (FILE *f); void binputb4 (unsigned int value, FILE *f);
unsigned int bingetb5 (FILE *f); void binputb5 (unsigned int value, FILE *f);
unsigned int bingetb6 (FILE *f); void binputb6 (unsigned int value, FILE *f);
unsigned int bingetb7 (FILE *f); void binputb7 (unsigned int value, FILE *f);
void bingetb (FILE *f); void binputb (FILE *f);
# 146 "../sys/abcio.h"
int bingete1 (FILE *f, void *enumerated);
int bingete2 (FILE *f, void *enumerated);




void binpute1 (int value, FILE *f, void *enumerated);
void binpute2 (int value, FILE *f, void *enumerated);




double bingetr4 (FILE *f); void binputr4 (double x, FILE *f);
# 167 "../sys/abcio.h"
double bingetr4LE (FILE *f); void binputr4LE (double x, FILE *f);

double bingetr8 (FILE *f); void binputr8 (double x, FILE *f);
# 179 "../sys/abcio.h"
double bingetr10 (FILE *f); void binputr10 (double x, FILE *f);
# 191 "../sys/abcio.h"
fcomplex bingetc8 (FILE *f);
dcomplex bingetc16 (FILE *f);
void binputc8 (fcomplex z, FILE *f);
void binputc16 (dcomplex z, FILE *f);



char * bingets1 (FILE *f); void binputs1 (const char *s, FILE *f);
char * bingets2 (FILE *f); void binputs2 (const char *s, FILE *f);
char * bingets4 (FILE *f); void binputs4 (const char *s, FILE *f);







wchar_t * bingetw1 (FILE *f); void binputw1 (const wchar_t *s, FILE *f);
wchar_t * bingetw2 (FILE *f); void binputw2 (const wchar_t *s, FILE *f);
wchar_t * bingetw4 (FILE *f); void binputw4 (const wchar_t *s, FILE *f);



typedef struct { unsigned char *ptr, *base, *max; } CACHE;

CACHE * memopen (size_t nbytes);



int memclose (CACHE *f);
size_t memread (void *ptr, size_t size, size_t nmemb, CACHE *m);
size_t memwrite (const void *ptr, size_t size, size_t nmemb, CACHE *m);
int memseek (CACHE *f, long offset, int whence);
long memtell (CACHE *f);
void memrewind (CACHE *me);

void memprint1 (CACHE *me, const char *s1);
void memprint2 (CACHE *me, const char *s1, const char *s2);
void memprint3 (CACHE *me, const char *s1, const char *s2, const char *s3);
void memprint4 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4);
void memprint5 (CACHE *me, const char *s1, const char *s2, const char *s3, const char *s4, const char *s5);
# 240 "../sys/abcio.h"
unsigned int cacgetb1 (CACHE *f); void cacputb1 (unsigned int value, CACHE *f);
unsigned int cacgetb2 (CACHE *f); void cacputb2 (unsigned int value, CACHE *f);
unsigned int cacgetb3 (CACHE *f); void cacputb3 (unsigned int value, CACHE *f);
unsigned int cacgetb4 (CACHE *f); void cacputb4 (unsigned int value, CACHE *f);
unsigned int cacgetb5 (CACHE *f); void cacputb5 (unsigned int value, CACHE *f);
unsigned int cacgetb6 (CACHE *f); void cacputb6 (unsigned int value, CACHE *f);
unsigned int cacgetb7 (CACHE *f); void cacputb7 (unsigned int value, CACHE *f);
void cacgetb (CACHE *f); void cacputb (CACHE *f);
unsigned int cacgetu1 (CACHE *f); void cacputu1 (unsigned int u, CACHE *f);
unsigned int cacgetu2 (CACHE *f); void cacputu2 (unsigned int i, CACHE *f);
unsigned long cacgetu4 (CACHE *f); void cacputu4 (unsigned long i, CACHE *f);
int cacgete1 (CACHE *f, void *enumerated); void cacpute1 (int value, CACHE *f, void *enumerated);
int cacgete2 (CACHE *f, void *enumerated); void cacpute2 (int value, CACHE *f, void *enumerated);






int cacgeti1 (CACHE *f); void cacputi1 (int i, CACHE *f);
int cacgeti2 (CACHE *f); void cacputi2 (int i, CACHE *f);
long cacgeti4 (CACHE *f); void cacputi4 (long i, CACHE *f);
int cacgeti2LE (CACHE *f); void cacputi2LE (int i, CACHE *f);
double cacgetr4 (CACHE *f); void cacputr4 (double x, CACHE *f);
double cacgetr8 (CACHE *f); void cacputr8 (double x, CACHE *f);
double cacgetr10 (CACHE *f); void cacputr10 (double x, CACHE *f);
fcomplex cacgetc8 (CACHE *f);
dcomplex cacgetc16 (CACHE *f);
void cacputc8 (fcomplex z, CACHE *f);
void cacputc16 (dcomplex z, CACHE *f);


long cacgeti4LE (CACHE *f); void cacputi4LE (long i, CACHE *f);
unsigned int cacgetu2LE (CACHE *f); void cacputu2LE (unsigned int i, CACHE *f);
unsigned long cacgetu4LE (CACHE *f); void cacputu4LE (unsigned long i, CACHE *f);
char * cacgets1 (CACHE *f); void cacputs1 (const char *s, CACHE *f);
char * cacgets2 (CACHE *f); void cacputs2 (const char *s, CACHE *f);
char * cacgets4 (CACHE *f); void cacputs4 (const char *s, CACHE *f);
# 46 "../sys/Thing.h" 2
# 1 "../sys/lispio.h" 1
# 33 "../sys/lispio.h"
typedef struct Lispio { long length; const char *string; } Lispio;



typedef struct LispioSeq { long length; const char *string; } LispioSeq;




int Lispio_openList (const Lispio *me, LispioSeq *thee);
# 52 "../sys/lispio.h"
int Lispio_openListFromFile (LispioSeq *me, FILE *f, char *buffer, long maxLength);
# 89 "../sys/lispio.h"
char * Lispio_string (const Lispio *me);







int Lispio_equal (const Lispio *me, const Lispio *thee);




int Lispio_strequ (const Lispio *me, const char *string);




int Lispio_read (LispioSeq *me, Lispio *first);
# 117 "../sys/lispio.h"
long Lispio_count (const LispioSeq *me);

int Lispio_isInteger (const Lispio *me);

int Lispio_integer (const Lispio *me, long *value);
# 47 "../sys/Thing.h" 2



typedef void *Any;
# 81 "../sys/Thing.h"
wchar_t * Thing_className (Any void_me);


int Thing_member (Any void_me, void *klas);






int Thing_subclass (void *klas, void *ancestor);






void Thing_info (Any void_me);
void Thing_infoWithId (Any void_me, unsigned long id);
# 111 "../sys/Thing.h"
Any Thing_new (void *klas);
# 121 "../sys/Thing.h"
void Thing_recognizeClassesByName (void *readableClass, ...);
# 139 "../sys/Thing.h"
void Thing_recognizeClassByOtherName (void *readableClass, const wchar_t *otherName);

Any Thing_newFromClassNameA (const char *className);
Any Thing_newFromClassName (const wchar_t *className);
# 154 "../sys/Thing.h"
void *Thing_classFromClassName (const wchar_t *className);
# 168 "../sys/Thing.h"
wchar_t * Thing_getName (Any void_me);

wchar_t * Thing_messageName (Any void_me);

void Thing_setName (Any void_me, const wchar_t *name);







void Thing_overrideClass (Any void_me, void *klas);
# 200 "../sys/Thing.h"
void Thing_swap (Any void_me, Any void_thee);
# 272 "../sys/Thing.h"
typedef struct structThing *Thing; typedef struct structThing_Table *Thing_Table; struct structThing_Table { void (* _initialize) (void *table); wchar_t *_className; Thing_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); }; struct structThing { Thing_Table methods; wchar_t *name; }; extern struct structThing_Table theStructThing; extern Thing_Table classThing;
# 310 "../sys/Thing.h"
void _Thing_forget (Thing *me);

void * _Thing_check (Any void_me, void *table, const char *fileName, int line);




long Thing_getTotalNumberOfThings (void);


extern long Thing_version;
# 30 "../sys/Data.h" 2


Any Data_copy (Any void_me);







_Bool Data_equal (Any data1, Any data2);
# 49 "../sys/Data.h"
_Bool Data_canWriteAsEncoding (Any void_me, int outputEncoding);






_Bool Data_canWriteText (Any void_me);






int Data_createTextFile (Any void_me, MelderFile file, _Bool verbose);

int Data_writeText (Any void_me, MelderFile openFile);
# 78 "../sys/Data.h"
int Data_writeToTextFile (Any void_me, MelderFile file);
# 89 "../sys/Data.h"
int Data_writeToShortTextFile (Any void_me, MelderFile file);
# 100 "../sys/Data.h"
_Bool Data_canWriteBinary (Any void_me);






int Data_writeBinary (Any void_me, FILE *f);
# 122 "../sys/Data.h"
int Data_writeToBinaryFile (Any void_me, MelderFile file);
# 132 "../sys/Data.h"
_Bool Data_canWriteLisp (Any void_me);






int Data_writeLisp (Any void_me, FILE *f);
# 152 "../sys/Data.h"
int Data_writeLispToConsole (Any void_me);
# 163 "../sys/Data.h"
int Data_writeToLispFile (Any void_me, MelderFile file);
# 179 "../sys/Data.h"
_Bool Data_canReadText (Any void_me);







int Data_readText (Any void_me, MelderReadText text);
# 202 "../sys/Data.h"
Any Data_readFromTextFile (MelderFile file);
# 218 "../sys/Data.h"
_Bool Data_canReadBinary (Any void_me);







int Data_readBinary (Any void_me, FILE *f);
# 241 "../sys/Data.h"
Any Data_readFromBinaryFile (MelderFile file);
# 257 "../sys/Data.h"
_Bool Data_canReadLisp (Any void_me);







int Data_readLisp (Any void_me, FILE *f);
# 280 "../sys/Data.h"
Any Data_readFromLispFile (MelderFile fs);
# 296 "../sys/Data.h"
typedef struct structData_Description {
 const wchar_t *name;
 int type;
 int offset;
 int size;
 const wchar_t *tagName;
 void *tagType;
 int rank;
 const wchar_t *min1, *max1;
 const wchar_t *min2, *max2;
} *Data_Description;
# 348 "../sys/Data.h"
typedef struct structData *Data; typedef struct structData_Table *Data_Table; struct structData_Table { void (* _initialize) (void *table); wchar_t *_className; Thing_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; struct structData { Data_Table methods; wchar_t *name; }; extern struct structData_Table theStructData; extern Data_Table classData;
# 384 "../sys/Data.h"
int Data_Description_countMembers (Data_Description structDescription);


Data_Description Data_Description_findMatch (Data_Description structDescription, const wchar_t *member);



Data_Description Data_Description_findNumberUse (Data_Description structDescription, const wchar_t *string);




long Data_Description_integer (void *structAddress, Data_Description description);


int Data_Description_evaluateInteger (void *structAddress, Data_Description structDescription,
 const wchar_t *formula, long *result);







void Data_recognizeFileType (Any (*recognizer) (int nread, const char *header, MelderFile fs));
# 454 "../sys/Data.h"
Any Data_readFromFile (MelderFile file);
# 470 "../sys/Data.h"
extern structMelderDir Data_directoryBeingRead;
# 28 "../sys/Simple.h" 2


# 1 "../sys/Simple_def.h" 1
# 25 "../sys/Simple_def.h"
typedef struct structSimpleInt_Table *SimpleInt_Table; typedef struct structSimpleInt { SimpleInt_Table methods; wchar_t *name;
 int number;
} *SimpleInt;



typedef struct structSimpleShort_Table *SimpleShort_Table; typedef struct structSimpleShort { SimpleShort_Table methods; wchar_t *name;
 short number;
} *SimpleShort;



typedef struct structSimpleLong_Table *SimpleLong_Table; typedef struct structSimpleLong { SimpleLong_Table methods; wchar_t *name;
 long number;
} *SimpleLong;



typedef struct structSimpleDouble_Table *SimpleDouble_Table; typedef struct structSimpleDouble { SimpleDouble_Table methods; wchar_t *name;
 double number;
} *SimpleDouble;



typedef struct structSimpleString_Table *SimpleString_Table; typedef struct structSimpleString { SimpleString_Table methods; wchar_t *name;
 wchar_t *string;
} *SimpleString;
# 31 "../sys/Simple.h" 2


struct structSimpleInt_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structSimpleInt_Table theStructSimpleInt; extern SimpleInt_Table classSimpleInt;


struct structSimpleShort_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structSimpleShort_Table theStructSimpleShort; extern SimpleShort_Table classSimpleShort;


struct structSimpleLong_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structSimpleLong_Table theStructSimpleLong; extern SimpleLong_Table classSimpleLong;


struct structSimpleDouble_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structSimpleDouble_Table theStructSimpleDouble; extern SimpleDouble_Table classSimpleDouble;


struct structSimpleString_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structSimpleString_Table theStructSimpleString; extern SimpleString_Table classSimpleString;

SimpleInt SimpleInt_create (int number);
SimpleShort SimpleShort_create (short number);
SimpleLong SimpleLong_create (long number);
SimpleDouble SimpleDouble_create (double number);
SimpleString SimpleString_create (const wchar_t *string);
# 54 "../sys/Collection.h" 2
# 62 "../sys/Collection.h"
typedef struct structCollection *Collection; typedef struct structCollection_Table *Collection_Table; struct structCollection_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); }; struct structCollection { Collection_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structCollection_Table theStructCollection; extern Collection_Table classCollection;
# 76 "../sys/Collection.h"
int Collection_init (Any void_me, void *itemClass, long initialCapacity);
Any Collection_create (void *itemClass, long initialCapacity);
# 94 "../sys/Collection.h"
int Collection_addItem (Any void_me, Any item);
# 108 "../sys/Collection.h"
void Collection_removeItem (Any void_me, long position);
# 119 "../sys/Collection.h"
void Collection_undangleItem (Any void_me, Any item);
# 130 "../sys/Collection.h"
Any Collection_subtractItem (Any void_me, long position);
# 143 "../sys/Collection.h"
void Collection_removeAllItems (Any void_me);
# 152 "../sys/Collection.h"
void Collection_shrinkToFit (Any void_me);







Any Collections_merge (Any void_me, Any void_thee);
# 171 "../sys/Collection.h"
int _Collection_insertItem (Any void_me, Any item, long position);
# 185 "../sys/Collection.h"
typedef struct structOrdered *Ordered; typedef struct structOrdered_Table *Ordered_Table; struct structOrdered_Table { void (* _initialize) (void *table); wchar_t *_className; Collection_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); }; struct structOrdered { Ordered_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structOrdered_Table theStructOrdered; extern Ordered_Table classOrdered;

Any Ordered_create (void);
int Ordered_init (Any void_me, void *itemClass, long initialCapacity);





int Ordered_addItemPos (Any void_me, Any data, long position);
# 208 "../sys/Collection.h"
typedef struct structSorted *Sorted; typedef struct structSorted_Table *Sorted_Table; struct structSorted_Table { void (* _initialize) (void *table); wchar_t *_className; Collection_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSorted { Sorted_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSorted_Table theStructSorted; extern Sorted_Table classSorted;

int Sorted_init (Any void_me, void *itemClass, long initialCapacity);
# 226 "../sys/Collection.h"
int Sorted_addItem_unsorted (Any void_me, Any data);






void Sorted_sort (Any void_me);







typedef struct structSortedSet *SortedSet; typedef struct structSortedSet_Table *SortedSet_Table; struct structSortedSet_Table { void (* _initialize) (void *table); wchar_t *_className; Sorted_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSet { SortedSet_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSet_Table theStructSortedSet; extern SortedSet_Table classSortedSet;

int SortedSet_init (Any void_me, void *itemClass, long initialCapacity);







int SortedSet_hasItem (Any void_me, Any item);





typedef struct structSortedSetOfInt *SortedSetOfInt; typedef struct structSortedSetOfInt_Table *SortedSetOfInt_Table; struct structSortedSetOfInt_Table { void (* _initialize) (void *table); wchar_t *_className; SortedSet_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSetOfInt { SortedSetOfInt_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSetOfInt_Table theStructSortedSetOfInt; extern SortedSetOfInt_Table classSortedSetOfInt;

int SortedSetOfInt_init (Any void_me);
SortedSetOfInt SortedSetOfInt_create (void);





typedef struct structSortedSetOfShort *SortedSetOfShort; typedef struct structSortedSetOfShort_Table *SortedSetOfShort_Table; struct structSortedSetOfShort_Table { void (* _initialize) (void *table); wchar_t *_className; SortedSet_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSetOfShort { SortedSetOfShort_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSetOfShort_Table theStructSortedSetOfShort; extern SortedSetOfShort_Table classSortedSetOfShort;

int SortedSetOfShort_init (Any void_me);
SortedSetOfShort SortedSetOfShort_create (void);





typedef struct structSortedSetOfLong *SortedSetOfLong; typedef struct structSortedSetOfLong_Table *SortedSetOfLong_Table; struct structSortedSetOfLong_Table { void (* _initialize) (void *table); wchar_t *_className; SortedSet_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSetOfLong { SortedSetOfLong_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSetOfLong_Table theStructSortedSetOfLong; extern SortedSetOfLong_Table classSortedSetOfLong;

int SortedSetOfLong_init (Any void_me);
SortedSetOfLong SortedSetOfLong_create (void);





typedef struct structSortedSetOfDouble *SortedSetOfDouble; typedef struct structSortedSetOfDouble_Table *SortedSetOfDouble_Table; struct structSortedSetOfDouble_Table { void (* _initialize) (void *table); wchar_t *_className; SortedSet_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSetOfDouble { SortedSetOfDouble_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSetOfDouble_Table theStructSortedSetOfDouble; extern SortedSetOfDouble_Table classSortedSetOfDouble;

int SortedSetOfDouble_init (Any void_me);
SortedSetOfDouble SortedSetOfDouble_create (void);





typedef struct structSortedSetOfString *SortedSetOfString; typedef struct structSortedSetOfString_Table *SortedSetOfString_Table; struct structSortedSetOfString_Table { void (* _initialize) (void *table); wchar_t *_className; SortedSet_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structSortedSetOfString { SortedSetOfString_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structSortedSetOfString_Table theStructSortedSetOfString; extern SortedSetOfString_Table classSortedSetOfString;

int SortedSetOfString_init (Any void_me);
SortedSetOfString SortedSetOfString_create (void);
long SortedSetOfString_lookUp (SortedSetOfString me, const wchar_t *string);
int SortedSetOfString_add (SortedSetOfString me, const wchar_t *string);






typedef struct structCyclic *Cyclic; typedef struct structCyclic_Table *Cyclic_Table; struct structCyclic_Table { void (* _initialize) (void *table); wchar_t *_className; Collection_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); long (*position) (Any void_me, Any data); int (*compare) (Any void_me, Any void_thee); }; struct structCyclic { Cyclic_Table methods; wchar_t *name; void *itemClass; long _capacity, size; Any *item; }; extern struct structCyclic_Table theStructCyclic; extern Cyclic_Table classCyclic;

int Cyclic_init (Any void_me, void *itemClass, long initialCapacity);

void Cyclic_unicize (Any void_me);
# 28 "KlattGrid.h" 2
# 42 "KlattGrid.h"
# 1 "KlattTable.h" 1
# 35 "KlattTable.h"
typedef struct structKlattTable *KlattTable; typedef struct structKlattTable_Table *KlattTable_Table; struct structKlattTable_Table { void (* _initialize) (void *table); wchar_t *_className; Table_Table _parent; long _size; Table_methods }; struct structKlattTable { KlattTable_Table methods; Table_members }; extern struct structKlattTable_Table theStructKlattTable; extern KlattTable_Table classKlattTable;

KlattTable KlattTable_create (double frameDuration, double totalDuration);
# 100 "KlattTable.h"
Sound KlattTable_to_Sound (KlattTable me, double samplingFrequency, int synthesisModel, int numberOfFormants, double frameDuration, int voicing,
double flutter, int outputType);

Sound KlattTable_and_Sound_to_Sound (KlattTable me, Sound thee);

KlattTable KlattTable_createExample (void);
# 43 "KlattGrid.h" 2


# 1 "../sys/Graphics.h" 1
# 30 "../sys/Graphics.h"
# 1 "../sys/Graphics_enums.h" 1
# 24 "../sys/Graphics_enums.h"
enum kGraphics_font { kGraphics_font_MIN = 0,
 kGraphics_font_HELVETICA = 0,
 kGraphics_font_TIMES = 1,
 kGraphics_font_COURIER = 2,
 kGraphics_font_PALATINO = 3,
kGraphics_font_MAX = 3, kGraphics_font_DEFAULT = kGraphics_font_TIMES }; const wchar_t * kGraphics_font_getText (int value); int kGraphics_font_getValue (const wchar_t *text);

enum kGraphics_horizontalAlignment { kGraphics_horizontalAlignment_MIN = 0,
 kGraphics_horizontalAlignment_LEFT = 0,
 kGraphics_horizontalAlignment_CENTRE = 1,

 kGraphics_horizontalAlignment_RIGHT = 2,




kGraphics_horizontalAlignment_MAX = 2, kGraphics_horizontalAlignment_DEFAULT = kGraphics_horizontalAlignment_CENTRE }; const wchar_t * kGraphics_horizontalAlignment_getText (int value); int kGraphics_horizontalAlignment_getValue (const wchar_t *text);

enum kGraphicsPostscript_spots { kGraphicsPostscript_spots_MIN = 0,
 kGraphicsPostscript_spots_FINE = 0,
 kGraphicsPostscript_spots_PHOTOCOPYABLE = 1,



kGraphicsPostscript_spots_MAX = 1, kGraphicsPostscript_spots_DEFAULT = kGraphicsPostscript_spots_FINE }; const wchar_t * kGraphicsPostscript_spots_getText (int value); int kGraphicsPostscript_spots_getValue (const wchar_t *text);

enum kGraphicsPostscript_paperSize { kGraphicsPostscript_paperSize_MIN = 0,
 kGraphicsPostscript_paperSize_A4 = 0,
 kGraphicsPostscript_paperSize_A3 = 1,
 kGraphicsPostscript_paperSize_US_LETTER = 2,




kGraphicsPostscript_paperSize_MAX = 2, kGraphicsPostscript_paperSize_DEFAULT = kGraphicsPostscript_paperSize_A4 }; const wchar_t * kGraphicsPostscript_paperSize_getText (int value); int kGraphicsPostscript_paperSize_getValue (const wchar_t *text);

enum kGraphicsPostscript_orientation { kGraphicsPostscript_orientation_MIN = 0,
 kGraphicsPostscript_orientation_PORTRAIT = 0,
 kGraphicsPostscript_orientation_LANDSCAPE = 1,



kGraphicsPostscript_orientation_MAX = 1, kGraphicsPostscript_orientation_DEFAULT = kGraphicsPostscript_orientation_PORTRAIT }; const wchar_t * kGraphicsPostscript_orientation_getText (int value); int kGraphicsPostscript_orientation_getValue (const wchar_t *text);

enum kGraphicsPostscript_fontChoiceStrategy { kGraphicsPostscript_fontChoiceStrategy_MIN = 0,
 kGraphicsPostscript_fontChoiceStrategy_AUTOMATIC = 0,
 kGraphicsPostscript_fontChoiceStrategy_LINOTYPE = 1,
 kGraphicsPostscript_fontChoiceStrategy_MONOTYPE = 2,
 kGraphicsPostscript_fontChoiceStrategy_PS_MONOTYPE = 3,





kGraphicsPostscript_fontChoiceStrategy_MAX = 3, kGraphicsPostscript_fontChoiceStrategy_DEFAULT = kGraphicsPostscript_fontChoiceStrategy_AUTOMATIC }; const wchar_t * kGraphicsPostscript_fontChoiceStrategy_getText (int value); int kGraphicsPostscript_fontChoiceStrategy_getValue (const wchar_t *text);
# 31 "../sys/Graphics.h" 2

typedef struct structGraphics *Graphics;
Graphics Graphics_create (int resolution);
Graphics Graphics_create_postscriptjob (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
 enum kGraphicsPostscript_paperSize paperSize, enum kGraphicsPostscript_orientation rotation, double magnification);
Graphics Graphics_create_epsfile (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
 double xmin, double xmax, double ymin, double ymax, _Bool includeFonts, _Bool useSilipaPS);
Graphics Graphics_create_postscriptprinter (void);
Graphics Graphics_create_screenPrinter (void *display, unsigned long window);
Graphics Graphics_create_screen (void *display, unsigned long window, int resolution);



Graphics Graphics_create_xmdrawingarea ( void *drawingArea);

int Graphics_getResolution (Any void_me);

void Graphics_setWsViewport (Any void_me, short x1DC, short x2DC, short y1DC, short y2DC);
void Graphics_setWsWindow (Any void_me, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
void Graphics_inqWsViewport (Any void_me, short *x1DC, short *x2DC, short *y1DC, short *y2DC);
void Graphics_inqWsWindow (Any void_me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_clearWs (Any void_me);
void Graphics_flushWs (Any void_me);
void Graphics_updateWs (Any void_me);
void Graphics_DCtoWC (Any void_me, short xDC, short yDC, double *xWC, double *yWC);
void Graphics_WCtoDC (Any void_me, double xWC, double yWC, short *xDC, short *yDC);

typedef struct { double x1NDC, x2NDC, y1NDC, y2NDC; } Graphics_Viewport;
void Graphics_setViewport (Any void_me, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
Graphics_Viewport Graphics_insetViewport (Any void_me, double x1rel, double x2rel, double y1rel, double y2rel);
void Graphics_resetViewport (Any void_me, Graphics_Viewport viewport);
void Graphics_setWindow (Any void_me, double x1, double x2, double y1, double y2);

void Graphics_polyline (Any void_me, long numberOfPoints, double *x, double *y);
void Graphics_text (Any void_me, double x, double y, const wchar_t *text);
void Graphics_text1 (Any void_me, double x, double y, const wchar_t *s1);
void Graphics_text2 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2);
void Graphics_text3 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void Graphics_text4 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void Graphics_text5 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void Graphics_text6 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void Graphics_text7 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void Graphics_text8 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void Graphics_text9 (Any void_me, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void Graphics_textRect (Any void_me, double x1, double x2, double y1, double y2, const wchar_t *text);
double Graphics_textWidth (Any void_me, const wchar_t *text);
double Graphics_textWidth_ps (Any void_me, const wchar_t *text, _Bool useSilipaPS);
double Graphics_textWidth_ps_mm (Any void_me, const wchar_t *text, _Bool useSilipaPS);
void Graphics_fillArea (Any void_me, long numberOfPoints, double *x, double *y);
void Graphics_cellArray (Any void_me, double **z, long ix1, long ix2, double x1, double x2,
 long iy1, long iy2, double y1, double y2, double minimum, double maximum);
void Graphics_cellArray8 (Any void_me, unsigned char **z, long ix1, long ix2, double x1, double x2,
 long iy1, long iy2, double y1, double y2, unsigned char minimum, unsigned char maximum);
void Graphics_image (Any void_me, double **z, long ix1, long ix2, double x1, double x2,
 long iy1, long iy2, double y1, double y2, double minimum, double maximum);
void Graphics_image8 (Any void_me, unsigned char **z, long ix1, long ix2, double x1, double x2,
 long iy1, long iy2, double y1, double y2, unsigned char minimum, unsigned char maximum);
void Graphics_line (Any void_me, double x1, double y1, double x2, double y2);
void Graphics_rectangle (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_fillRectangle (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_roundedRectangle (Any void_me, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_fillRoundedRectangle (Any void_me, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_function (Any void_me, double y [], long ix1, long ix2, double x1, double x2);
void Graphics_function16 (Any void_me, short y [], int stagger, long ix1, long ix2, double x1, double x2);

void Graphics_circle (Any void_me, double x, double y, double r);
void Graphics_fillCircle (Any void_me, double x, double y, double r);
void Graphics_circle_mm (Any void_me, double x, double y, double d);
void Graphics_fillCircle_mm (Any void_me, double x, double y, double d);
void Graphics_rectangle_mm (Any void_me, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_fillRectangle_mm (Any void_me, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_arc (Any void_me, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_fillArc (Any void_me, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_ellipse (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_fillEllipse (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_arrow (Any void_me, double x1, double y1, double x2, double y2);
void Graphics_doubleArrow (Any void_me, double x1, double y1, double x2, double y2);
void Graphics_arcArrow (Any void_me, double x, double y, double r, double fromAngle, double toAngle, int arrowAtStart, int arrowAtEnd);
void Graphics_mark (Any void_me, double x, double y, double size_mm, const wchar_t *markString);
void Graphics_button (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_innerRectangle (Any void_me, double x1, double x2, double y1, double y2);

void Graphics_xorOn (Any void_me, int colour);
void Graphics_xorOff (Any void_me);
void Graphics_highlight (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_unhighlight (Any void_me, double x1, double x2, double y1, double y2);
void Graphics_highlight2 (Any void_me, double x1, double x2, double y1, double y2,
 double innerX1, double innerX2, double innerY1, double innerY2);
void Graphics_unhighlight2 (Any void_me, double x1, double x2, double y1, double y2,
 double innerX1, double innerX2, double innerY1, double innerY2);
# 130 "../sys/Graphics.h"
void Graphics_setTextAlignment (Any void_me, int horizontal, int vertical);
void Graphics_setFont (Any void_me, enum kGraphics_font font);
void Graphics_setFontSize (Any void_me, int height);
void Graphics_setFontStyle (Any void_me, int style);
void Graphics_setItalic (Any void_me, _Bool onoff);
void Graphics_setBold (Any void_me, _Bool onoff);
void Graphics_setCode (Any void_me, _Bool onoff);





void Graphics_setTextRotation (Any void_me, double angle);
void Graphics_setTextRotation_vector (Any void_me, double dx, double dy);
void Graphics_setWrapWidth (Any void_me, double wrapWidth);
void Graphics_setSecondIndent (Any void_me, double indent);
double Graphics_inqTextX (Any void_me);
double Graphics_inqTextY (Any void_me);
typedef struct { double x1, x2, y1, y2; wchar_t *name; } Graphics_Link;
int Graphics_getLinks (Graphics_Link **plinks);
void Graphics_setNumberSignIsBold (Any void_me, _Bool isBold);
void Graphics_setPercentSignIsItalic (Any void_me, _Bool isItalic);
void Graphics_setCircumflexIsSuperscript (Any void_me, _Bool isSuperscript);
void Graphics_setUnderscoreIsSubscript (Any void_me, _Bool isSubscript);
void Graphics_setDollarSignIsCode (Any void_me, _Bool isCode);
void Graphics_setAtSignIsLink (Any void_me, _Bool isLink);

void Graphics_setLineType (Any void_me, int lineType);



void Graphics_setLineWidth (Any void_me, double lineWidth);
void Graphics_setArrowSize (Any void_me, double arrorSize);

void Graphics_setColour (Any void_me, int colour);
# 182 "../sys/Graphics.h"
void Graphics_setGrey (Any void_me, double grey);


void Graphics_inqViewport (Any void_me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_inqWindow (Any void_me, double *x1WC, double *x2WC, double *y1WC, double *y2WC);
int Graphics_inqFont (Any void_me);
int Graphics_inqFontSize (Any void_me);
int Graphics_inqFontStyle (Any void_me);
int Graphics_inqLineType (Any void_me);
double Graphics_inqLineWidth (Any void_me);
double Graphics_inqArrowSize (Any void_me);
int Graphics_inqColour (Any void_me);

void Graphics_contour (Any void_me, double **z,
 long ix1, long ix2, double x1WC, double x2WC, long iy1, long iy2, double y1WC, double y2WC, double height);
void Graphics_altitude (Any void_me, double **z,
 long ix1, long ix2, double x1, double x2, long iy1, long iy2, double y1, double y2, int numberOfBorders, double borders []);
void Graphics_grey (Any void_me, double **z,
 long ix1, long ix2, double x1, double x2, long iy1, long iy2, double y1, double y2, int numberOfBorders, double borders []);

void Graphics_surface (Any void_me, double **z, long ix1, long ix2, double x1, double x2,
 long iy1, long iy2, double y1, double y2, double minimum, double maximum, double elevation, double azimuth);

void Graphics_setInner (Any void_me);
void Graphics_unsetInner (Any void_me);
void Graphics_drawInnerBox (Any void_me);
void Graphics_textLeft (Any void_me, _Bool far, const wchar_t *text);
void Graphics_textRight (Any void_me, _Bool far, const wchar_t *text);
void Graphics_textBottom (Any void_me, _Bool far, const wchar_t *text);
void Graphics_textTop (Any void_me, _Bool far, const wchar_t *text);
void Graphics_marksLeft (Any void_me, int numberOfMarks, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksRight (Any void_me, int numberOfMarks, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksBottom (Any void_me, int numberOfMarks, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksTop (Any void_me, int numberOfMarks, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksLeftLogarithmic (Any void_me, int numberOfMarksPerDecade, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksRightLogarithmic (Any void_me, int numberOfMarksPerDecade, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksBottomLogarithmic (Any void_me, int numberOfMarksPerDecade, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksTopLogarithmic (Any void_me, int numberOfMarksPerDecade, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_markLeft (Any void_me, double yWC, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markRight (Any void_me, double yWC, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markBottom (Any void_me, double xWC, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markTop (Any void_me, double xWC, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markLeftLogarithmic (Any void_me, double y , _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markRightLogarithmic (Any void_me, double y, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markBottomLogarithmic (Any void_me, double x, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_markTopLogarithmic (Any void_me, double x, _Bool hasNumber, _Bool hasTick, _Bool hasDottedLine, const wchar_t *text);
void Graphics_marksLeftEvery (Any void_me, double units, double distance, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksRightEvery (Any void_me, double units, double distance, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksBottomEvery (Any void_me, double units, double distance, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);
void Graphics_marksTopEvery (Any void_me, double units, double distance, _Bool haveNumbers, _Bool haveTicks, _Bool haveDottedLines);

void *Graphics_x_getCR (Any void_me);
void Graphics_x_setCR (Any void_me, void *cr);
void *Graphics_x_getGC (Any void_me);

int Graphics_startRecording (Any void_me);
int Graphics_stopRecording (Any void_me);
void Graphics_play (Graphics from, Graphics to);
int Graphics_writeRecordings (Any void_me, FILE *f);
int Graphics_readRecordings (Any void_me, FILE *f);



void Graphics_markGroup (Any void_me);
void Graphics_undoGroup (Any void_me);

double Graphics_dxMMtoWC (Any void_me, double dx_mm);
double Graphics_dyMMtoWC (Any void_me, double dy_mm);
double Graphics_distanceWCtoMM (Any void_me, double x1WC, double y1WC, double x2WC, double y2WC);
double Graphics_dxWCtoMM (Any void_me, double dxWC);
double Graphics_dyWCtoMM (Any void_me, double dyWC);

int Graphics_mouseStillDown (Any void_me);
void Graphics_waitMouseUp (Any void_me);
void Graphics_getMouseLocation (Any void_me, double *xWC, double *yWC);

void Graphics_nextSheetOfPaper (Any void_me);
# 46 "KlattGrid.h" 2


# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
typedef struct structPhonationPoint_Table *PhonationPoint_Table; typedef struct structPhonationPoint { PhonationPoint_Table methods; wchar_t *name;

 double time;
 double period;
 double openPhase;
 double collisionPhase;
 double te;
 double power1;
 double power2;
 double pulseScale;

} *PhonationPoint;



typedef struct structPhonationTier_Table *PhonationTier_Table; typedef struct structPhonationTier { PhonationTier_Table methods; Function_members

 SortedSetOfDouble points;

} *PhonationTier;



typedef struct structPhonationGrid_Table *PhonationGrid_Table; typedef struct structPhonationGrid { PhonationGrid_Table methods; Function_members

 PitchTier pitch;
 RealTier flutter;
 IntensityTier voicingAmplitude;
 RealTier doublePulsing;
 RealTier openPhase;
 RealTier collisionPhase;
 RealTier power1;
 RealTier power2;
 IntensityTier spectralTilt;
 IntensityTier aspirationAmplitude;
 IntensityTier breathinessAmplitude;

} *PhonationGrid;



typedef struct structVocalTractGrid_Table *VocalTractGrid_Table; typedef struct structVocalTractGrid { VocalTractGrid_Table methods; Function_members

 FormantGrid formants;
 FormantGrid nasal_formants;
 FormantGrid nasal_antiformants;

 Ordered formants_amplitudes;
 Ordered nasal_formants_amplitudes;

} *VocalTractGrid;



typedef struct structCouplingGrid_Table *CouplingGrid_Table; typedef struct structCouplingGrid { CouplingGrid_Table methods; Function_members

 FormantGrid tracheal_formants;
 FormantGrid tracheal_antiformants;
 Ordered tracheal_formants_amplitudes;
 FormantGrid delta_formants;

  PhonationTier glottis;


} *CouplingGrid;



typedef struct structFricationGrid_Table *FricationGrid_Table; typedef struct structFricationGrid { FricationGrid_Table methods; Function_members

 IntensityTier noise_amplitude;
 FormantGrid formants;
 Ordered formants_amplitudes;
 IntensityTier bypass;

} *FricationGrid;



typedef struct structKlattGrid_Table *KlattGrid_Table; typedef struct structKlattGrid { KlattGrid_Table methods; Function_members

 PhonationGrid phonation;
 VocalTractGrid vocalTract;
 CouplingGrid coupling;
 FricationGrid frication;
 IntensityTier gain;

} *KlattGrid;
# 49 "KlattGrid.h" 2


struct structPhonationPoint_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); }; extern struct structPhonationPoint_Table theStructPhonationPoint; extern PhonationPoint_Table classPhonationPoint;


struct structPhonationTier_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structPhonationTier_Table theStructPhonationTier; extern PhonationTier_Table classPhonationTier;


struct structPhonationGrid_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structPhonationGrid_Table theStructPhonationGrid; extern PhonationGrid_Table classPhonationGrid;


struct structVocalTractGrid_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structVocalTractGrid_Table theStructVocalTractGrid; extern VocalTractGrid_Table classVocalTractGrid;


struct structCouplingGrid_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structCouplingGrid_Table theStructCouplingGrid; extern CouplingGrid_Table classCouplingGrid;


struct structFricationGrid_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structFricationGrid_Table theStructFricationGrid; extern FricationGrid_Table classFricationGrid;


struct structKlattGrid_Table { void (* _initialize) (void *table); wchar_t *_className; Function_Table _parent; long _size; Function_methods }; extern struct structKlattGrid_Table theStructKlattGrid; extern KlattGrid_Table classKlattGrid;

typedef struct synthesisParams {
 double samplingFrequency;
 double maximumPeriod;
 int voicing, aspiration, spectralTilt;
 int filterModel;
 int sourceIsFlowDerivative;
 long startFormant, endFormant;
 long startNasalFormant, endNasalFormant;
 long startTrachealFormant, endTrachealFormant;
 long startNasalAntiFormant, endNasalAntiFormant;
 long startTrachealAntiFormant, endTrachealAntiFormant;
 long startFricationFormant, endFricationFormant;
 double openglottis_fadeFraction;
 int fricationBypass;
 int klatt80;
 KlattGrid *kg;
} *synthesisParams;



PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
 double power1, double power2, double pulseScale);

PhonationTier PhonationTier_create (double tmin, double tmax);



PhonationGrid PhonationGrid_create (double tmin, double tmax);

void PhonationGrid_draw (PhonationGrid me, Graphics g);

double PhonationGrid_getMaximumPeriod (PhonationGrid me);

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me, synthesisParams p);



VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
 long numberOfNasalFormants, long numberOfNasalAntiFormants);

void VocalTractGrid_draw (VocalTractGrid me, Graphics g, int filterModel);



CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants);

double CouplingGrid_getDeltaFormantAtTime (CouplingGrid me, long iformant, double t);
double CouplingGrid_getDeltaBandwidthAtTime (CouplingGrid me, long iformant, double t);



int FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee, double fadeFraction);



int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant);
int Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant);
Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign);



FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants);

void FricationGrid_draw (FricationGrid me, Graphics g);

Sound FricationGrid_to_Sound (FricationGrid me, synthesisParams p);

Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee, synthesisParams params);



Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling, synthesisParams p);



KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
 long numberOfNasalFormants, long numberOfNasalAntiFormants,
 long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
 long numberOfFricationFormants, long numberOfDeltaFormants);

KlattGrid KlattGrid_createExample (void);

KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration);

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel);
void klattGrid_drawPhonation (KlattGrid me, Graphics g);
void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea);
# 177 "KlattGrid.h"
double KlattGrid_getPitchAtTime (KlattGrid me, double t); int KlattGrid_addPitchPoint (KlattGrid me, double t, double value); void KlattGrid_removePitchPointsBetween (KlattGrid me, double t1, double t2); PitchTier KlattGrid_extractPitchTier (KlattGrid me); int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee);
double KlattGrid_getFlutterAtTime (KlattGrid me, double t); int KlattGrid_addFlutterPoint (KlattGrid me, double t, double value); void KlattGrid_removeFlutterPointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractFlutterTier (KlattGrid me); int KlattGrid_replaceFlutterTier (KlattGrid me, RealTier thee);
double KlattGrid_getDoublePulsingAtTime (KlattGrid me, double t); int KlattGrid_addDoublePulsingPoint (KlattGrid me, double t, double value); void KlattGrid_removeDoublePulsingPointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractDoublePulsingTier (KlattGrid me); int KlattGrid_replaceDoublePulsingTier (KlattGrid me, RealTier thee);
double KlattGrid_getOpenPhaseAtTime (KlattGrid me, double t); int KlattGrid_addOpenPhasePoint (KlattGrid me, double t, double value); void KlattGrid_removeOpenPhasePointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractOpenPhaseTier (KlattGrid me); int KlattGrid_replaceOpenPhaseTier (KlattGrid me, RealTier thee);
double KlattGrid_getCollisionPhaseAtTime (KlattGrid me, double t); int KlattGrid_addCollisionPhasePoint (KlattGrid me, double t, double value); void KlattGrid_removeCollisionPhasePointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractCollisionPhaseTier (KlattGrid me); int KlattGrid_replaceCollisionPhaseTier (KlattGrid me, RealTier thee);
double KlattGrid_getSpectralTiltAtTime (KlattGrid me, double t); int KlattGrid_addSpectralTiltPoint (KlattGrid me, double t, double value); void KlattGrid_removeSpectralTiltPointsBetween (KlattGrid me, double t1, double t2); IntensityTier KlattGrid_extractSpectralTiltTier (KlattGrid me); int KlattGrid_replaceSpectralTiltTier (KlattGrid me, IntensityTier thee);
double KlattGrid_getPower1AtTime (KlattGrid me, double t); int KlattGrid_addPower1Point (KlattGrid me, double t, double value); void KlattGrid_removePower1PointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractPower1Tier (KlattGrid me); int KlattGrid_replacePower1Tier (KlattGrid me, RealTier thee);
double KlattGrid_getPower2AtTime (KlattGrid me, double t); int KlattGrid_addPower2Point (KlattGrid me, double t, double value); void KlattGrid_removePower2PointsBetween (KlattGrid me, double t1, double t2); RealTier KlattGrid_extractPower2Tier (KlattGrid me); int KlattGrid_replacePower2Tier (KlattGrid me, RealTier thee);
double KlattGrid_getVoicingAmplitudeAtTime (KlattGrid me, double t); int KlattGrid_addVoicingAmplitudePoint (KlattGrid me, double t, double value); void KlattGrid_removeVoicingAmplitudePointsBetween (KlattGrid me, double t1, double t2); IntensityTier KlattGrid_extractVoicingAmplitudeTier (KlattGrid me); int KlattGrid_replaceVoicingAmplitudeTier (KlattGrid me, IntensityTier thee);
double KlattGrid_getAspirationAmplitudeAtTime (KlattGrid me, double t); int KlattGrid_addAspirationAmplitudePoint (KlattGrid me, double t, double value); void KlattGrid_removeAspirationAmplitudePointsBetween (KlattGrid me, double t1, double t2); IntensityTier KlattGrid_extractAspirationAmplitudeTier (KlattGrid me); int KlattGrid_replaceAspirationAmplitudeTier (KlattGrid me, IntensityTier thee);
double KlattGrid_getBreathinessAmplitudeAtTime (KlattGrid me, double t); int KlattGrid_addBreathinessAmplitudePoint (KlattGrid me, double t, double value); void KlattGrid_removeBreathinessAmplitudePointsBetween (KlattGrid me, double t1, double t2); IntensityTier KlattGrid_extractBreathinessAmplitudeTier (KlattGrid me); int KlattGrid_replaceBreathinessAmplitudeTier (KlattGrid me, IntensityTier thee);
# 198 "KlattGrid.h"
double KlattGrid_getFormantAtTime (KlattGrid me, int formantType, long iformant, double t); int KlattGrid_addFormantPoint (KlattGrid me, int formantType, long iformant, double t, double value); void KlattGrid_removeFormantPointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2); double KlattGrid_getDeltaFormantAtTime (KlattGrid me, long iformant, double t); int KlattGrid_addDeltaFormantPoint (KlattGrid me, long iformant, double t, double value); void KlattGrid_removeDeltaFormantPointsBetween (KlattGrid me, long iformant, double t1, double t2);
double KlattGrid_getBandwidthAtTime (KlattGrid me, int formantType, long iformant, double t); int KlattGrid_addBandwidthPoint (KlattGrid me, int formantType, long iformant, double t, double value); void KlattGrid_removeBandwidthPointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2); double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, long iformant, double t); int KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long iformant, double t, double value); void KlattGrid_removeDeltaBandwidthPointsBetween (KlattGrid me, long iformant, double t1, double t2);

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType);
int KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee);

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me);
int KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee);

FormantGrid KlattGrid_to_FormantGrid_openPhases (KlattGrid me, double fadeFraction);

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t);
int KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value);
void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2);
IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant);
int KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee);


double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t);
int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me);
int KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee);

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t);
int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value);
void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2);
IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me);
int KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee);



Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int filterType);

int KlattGrid_play (KlattGrid me);

Sound KlattGrid_to_Sound (KlattGrid me, synthesisParams params);

int KlattGrid_synthesize (KlattGrid me, double t1, double t2, double samplingFrequency, double maximumPeriod);





Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, synthesisParams params);
Sound Sound_KlattGrid_filter_laryngial_cascade (Sound me, KlattGrid thee, synthesisParams params);
Sound Sounds_KlattGrid_filter_allSources (Sound glottal, Sound frication, KlattGrid me, synthesisParams params);

Sound Sounds_KlattGrid_filter_oneSource (Sound frication, KlattGrid me, int sourceType, synthesisParams params);


Sound Sound_KlattGrid_filter (Sound me, KlattGrid thee, int type, synthesisParams params);



int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee);
# 24 "KlattGrid.c" 2

# 1 "Resonator.h" 1
# 41 "Resonator.h"
typedef struct structFilter *Filter; typedef struct structFilter_Table *Filter_Table; struct structFilter_Table { void (* _initialize) (void *table); wchar_t *_className; Data_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); double (*getOutput) (Any void_me, double input); void (*setFB) (Any void_me, double f, double b); void (*resetMemory)(Any void_me); }; struct structFilter { Filter_Table methods; wchar_t *name; double dT; double a, b, c; double p1, p2; }; extern struct structFilter_Table theStructFilter; extern Filter_Table classFilter;



typedef struct structResonator *Resonator; typedef struct structResonator_Table *Resonator_Table; struct structResonator_Table { void (* _initialize) (void *table); wchar_t *_className; Filter_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); double (*getOutput) (Any void_me, double input); void (*setFB) (Any void_me, double f, double b); void (*resetMemory)(Any void_me); }; struct structResonator { Resonator_Table methods; wchar_t *name; double dT; double a, b, c; double p1, p2; }; extern struct structResonator_Table theStructResonator; extern Resonator_Table classResonator;



typedef struct structAntiResonator *AntiResonator; typedef struct structAntiResonator_Table *AntiResonator_Table; struct structAntiResonator_Table { void (* _initialize) (void *table); wchar_t *_className; Filter_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); double (*getOutput) (Any void_me, double input); void (*setFB) (Any void_me, double f, double b); void (*resetMemory)(Any void_me); }; struct structAntiResonator { AntiResonator_Table methods; wchar_t *name; double dT; double a, b, c; double p1, p2; }; extern struct structAntiResonator_Table theStructAntiResonator; extern AntiResonator_Table classAntiResonator;






typedef struct structConstantGainResonator *ConstantGainResonator; typedef struct structConstantGainResonator_Table *ConstantGainResonator_Table; struct structConstantGainResonator_Table { void (* _initialize) (void *table); wchar_t *_className; Filter_Table _parent; long _size; long version; void (*destroy) (Any void_me); void (*info) (Any void_me); void (*nameChanged) (Any void_me); struct structData_Description *description; int (*copy) (Any data_from, Any data_to); _Bool (*equal) (Any data1, Any data2); _Bool (*canWriteAsEncoding) (Any void_me, int outputEncoding); int (*writeText) (Any void_me, MelderFile openFile); int (*readText) (Any void_me, MelderReadText text); int (*writeBinary) (Any void_me, FILE *f); int (*readBinary) (Any void_me, FILE *f); int (*writeCache) (Any void_me, CACHE *f); int (*readCache) (Any void_me, CACHE *f); int (*writeLisp) (Any void_me, FILE *f); int (*readLisp) (Any void_me, FILE *f); double (*getNrow) (Any void_me); double (*getNcol) (Any void_me); double (*getXmin) (Any void_me); double (*getXmax) (Any void_me); double (*getYmin) (Any void_me); double (*getYmax) (Any void_me); double (*getNx) (Any void_me); double (*getNy) (Any void_me); double (*getDx) (Any void_me); double (*getDy) (Any void_me); double (*getX) (Any void_me, long ix); double (*getY) (Any void_me, long iy); wchar_t * (*getRowStr) (Any void_me, long irow); wchar_t * (*getColStr) (Any void_me, long icol); double (*getCell) (Any void_me); wchar_t * (*getCellStr) (Any void_me); double (*getVector) (Any void_me, long irow, long icol); wchar_t * (*getVectorStr) (Any void_me, long icol); double (*getMatrix) (Any void_me, long irow, long icol); wchar_t * (*getMatrixStr) (Any void_me, long irow, long icol); double (*getFunction0) (Any void_me); double (*getFunction1) (Any void_me, long irow, double x); double (*getFunction2) (Any void_me, double x, double y); double (*getRowIndex) (Any void_me, const wchar_t *rowLabel); double (*getColumnIndex) (Any void_me, const wchar_t *columnLabel); double (*getOutput) (Any void_me, double input); void (*setFB) (Any void_me, double f, double b); void (*resetMemory)(Any void_me); }; struct structConstantGainResonator { ConstantGainResonator_Table methods; wchar_t *name; double dT; double a, b, c; double p1, p2; double d; double p3, p4; }; extern struct structConstantGainResonator_Table theStructConstantGainResonator; extern ConstantGainResonator_Table classConstantGainResonator;


Resonator Resonator_create (double dT);

ConstantGainResonator ConstantGainResonator_create (double dT);

AntiResonator AntiResonator_create (double dT);


void Filter_setFB (Any void_me, double f, double b);

double Filter_getOutput (Any void_me, double input);

void Filter_resetMemory (Any void_me);

Sound Sound_filterByResonator (Sound me, double f, double b, int anti, int constantGain);
# 26 "KlattGrid.c" 2





# 1 "../sys/oo_DESTROY.h" 1
# 28 "../sys/oo_DESTROY.h"
# 1 "../sys/oo_undef.h" 1
# 29 "../sys/oo_DESTROY.h" 2
# 32 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static void classPhonationPoint_destroy (Any void_me) { PhonationPoint me = (PhonationPoint) void_me;










classPhonationPoint -> _parent -> destroy (me); }



static void classPhonationTier_destroy (Any void_me) { PhonationTier me = (PhonationTier) void_me;

 _Thing_forget ((Thing *) & (me -> points));

classPhonationTier -> _parent -> destroy (me); }



static void classPhonationGrid_destroy (Any void_me) { PhonationGrid me = (PhonationGrid) void_me;

 _Thing_forget ((Thing *) & (me -> pitch));
 _Thing_forget ((Thing *) & (me -> flutter));
 _Thing_forget ((Thing *) & (me -> voicingAmplitude));
 _Thing_forget ((Thing *) & (me -> doublePulsing));
 _Thing_forget ((Thing *) & (me -> openPhase));
 _Thing_forget ((Thing *) & (me -> collisionPhase));
 _Thing_forget ((Thing *) & (me -> power1));
 _Thing_forget ((Thing *) & (me -> power2));
 _Thing_forget ((Thing *) & (me -> spectralTilt));
 _Thing_forget ((Thing *) & (me -> aspirationAmplitude));
 _Thing_forget ((Thing *) & (me -> breathinessAmplitude));

classPhonationGrid -> _parent -> destroy (me); }



static void classVocalTractGrid_destroy (Any void_me) { VocalTractGrid me = (VocalTractGrid) void_me;

 _Thing_forget ((Thing *) & (me -> formants));
 _Thing_forget ((Thing *) & (me -> nasal_formants));
 _Thing_forget ((Thing *) & (me -> nasal_antiformants));

 _Thing_forget ((Thing *) & (me -> formants_amplitudes));
 _Thing_forget ((Thing *) & (me -> nasal_formants_amplitudes));

classVocalTractGrid -> _parent -> destroy (me); }



static void classCouplingGrid_destroy (Any void_me) { CouplingGrid me = (CouplingGrid) void_me;

 _Thing_forget ((Thing *) & (me -> tracheal_formants));
 _Thing_forget ((Thing *) & (me -> tracheal_antiformants));
 _Thing_forget ((Thing *) & (me -> tracheal_formants_amplitudes));
 _Thing_forget ((Thing *) & (me -> delta_formants));

  _Thing_forget ((Thing *) & (me -> glottis));


classCouplingGrid -> _parent -> destroy (me); }



static void classFricationGrid_destroy (Any void_me) { FricationGrid me = (FricationGrid) void_me;

 _Thing_forget ((Thing *) & (me -> noise_amplitude));
 _Thing_forget ((Thing *) & (me -> formants));
 _Thing_forget ((Thing *) & (me -> formants_amplitudes));
 _Thing_forget ((Thing *) & (me -> bypass));

classFricationGrid -> _parent -> destroy (me); }



static void classKlattGrid_destroy (Any void_me) { KlattGrid me = (KlattGrid) void_me;

 _Thing_forget ((Thing *) & (me -> phonation));
 _Thing_forget ((Thing *) & (me -> vocalTract));
 _Thing_forget ((Thing *) & (me -> coupling));
 _Thing_forget ((Thing *) & (me -> frication));
 _Thing_forget ((Thing *) & (me -> gain));

classKlattGrid -> _parent -> destroy (me); }
# 33 "KlattGrid.c" 2
# 1 "../sys/oo_COPY.h" 1
# 28 "../sys/oo_COPY.h"
# 1 "../sys/oo_undef.h" 1
# 29 "../sys/oo_COPY.h" 2
# 34 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static int classPhonationPoint_copy (Any void_me, Any void_thee) { PhonationPoint me = (PhonationPoint) void_me; PhonationPoint thee = (PhonationPoint) void_thee; if (! classPhonationPoint -> _parent -> copy (me, thee)) return 0;

 thee -> time = me -> time;
 thee -> period = me -> period;
 thee -> openPhase = me -> openPhase;
 thee -> collisionPhase = me -> collisionPhase;
 thee -> te = me -> te;
 thee -> power1 = me -> power1;
 thee -> power2 = me -> power2;
 thee -> pulseScale = me -> pulseScale;

return 1; }



static int classPhonationTier_copy (Any void_me, Any void_thee) { PhonationTier me = (PhonationTier) void_me; PhonationTier thee = (PhonationTier) void_thee; if (! classPhonationTier -> _parent -> copy (me, thee)) return 0;

 if (me -> points && ! (thee -> points = Data_copy (me -> points))) return 0;

return 1; }



static int classPhonationGrid_copy (Any void_me, Any void_thee) { PhonationGrid me = (PhonationGrid) void_me; PhonationGrid thee = (PhonationGrid) void_thee; if (! classPhonationGrid -> _parent -> copy (me, thee)) return 0;

 if (me -> pitch && ! (thee -> pitch = Data_copy (me -> pitch))) return 0;
 if (me -> flutter && ! (thee -> flutter = Data_copy (me -> flutter))) return 0;
 if (me -> voicingAmplitude && ! (thee -> voicingAmplitude = Data_copy (me -> voicingAmplitude))) return 0;
 if (me -> doublePulsing && ! (thee -> doublePulsing = Data_copy (me -> doublePulsing))) return 0;
 if (me -> openPhase && ! (thee -> openPhase = Data_copy (me -> openPhase))) return 0;
 if (me -> collisionPhase && ! (thee -> collisionPhase = Data_copy (me -> collisionPhase))) return 0;
 if (me -> power1 && ! (thee -> power1 = Data_copy (me -> power1))) return 0;
 if (me -> power2 && ! (thee -> power2 = Data_copy (me -> power2))) return 0;
 if (me -> spectralTilt && ! (thee -> spectralTilt = Data_copy (me -> spectralTilt))) return 0;
 if (me -> aspirationAmplitude && ! (thee -> aspirationAmplitude = Data_copy (me -> aspirationAmplitude))) return 0;
 if (me -> breathinessAmplitude && ! (thee -> breathinessAmplitude = Data_copy (me -> breathinessAmplitude))) return 0;

return 1; }



static int classVocalTractGrid_copy (Any void_me, Any void_thee) { VocalTractGrid me = (VocalTractGrid) void_me; VocalTractGrid thee = (VocalTractGrid) void_thee; if (! classVocalTractGrid -> _parent -> copy (me, thee)) return 0;

 if (me -> formants && ! (thee -> formants = Data_copy (me -> formants))) return 0;
 if (me -> nasal_formants && ! (thee -> nasal_formants = Data_copy (me -> nasal_formants))) return 0;
 if (me -> nasal_antiformants && ! (thee -> nasal_antiformants = Data_copy (me -> nasal_antiformants))) return 0;

 if (me -> formants_amplitudes && ! (thee -> formants_amplitudes = Data_copy (me -> formants_amplitudes))) return 0;
 if (me -> nasal_formants_amplitudes && ! (thee -> nasal_formants_amplitudes = Data_copy (me -> nasal_formants_amplitudes))) return 0;

return 1; }



static int classCouplingGrid_copy (Any void_me, Any void_thee) { CouplingGrid me = (CouplingGrid) void_me; CouplingGrid thee = (CouplingGrid) void_thee; if (! classCouplingGrid -> _parent -> copy (me, thee)) return 0;

 if (me -> tracheal_formants && ! (thee -> tracheal_formants = Data_copy (me -> tracheal_formants))) return 0;
 if (me -> tracheal_antiformants && ! (thee -> tracheal_antiformants = Data_copy (me -> tracheal_antiformants))) return 0;
 if (me -> tracheal_formants_amplitudes && ! (thee -> tracheal_formants_amplitudes = Data_copy (me -> tracheal_formants_amplitudes))) return 0;
 if (me -> delta_formants && ! (thee -> delta_formants = Data_copy (me -> delta_formants))) return 0;

  if (me -> glottis && ! (thee -> glottis = Data_copy (me -> glottis))) return 0;


return 1; }



static int classFricationGrid_copy (Any void_me, Any void_thee) { FricationGrid me = (FricationGrid) void_me; FricationGrid thee = (FricationGrid) void_thee; if (! classFricationGrid -> _parent -> copy (me, thee)) return 0;

 if (me -> noise_amplitude && ! (thee -> noise_amplitude = Data_copy (me -> noise_amplitude))) return 0;
 if (me -> formants && ! (thee -> formants = Data_copy (me -> formants))) return 0;
 if (me -> formants_amplitudes && ! (thee -> formants_amplitudes = Data_copy (me -> formants_amplitudes))) return 0;
 if (me -> bypass && ! (thee -> bypass = Data_copy (me -> bypass))) return 0;

return 1; }



static int classKlattGrid_copy (Any void_me, Any void_thee) { KlattGrid me = (KlattGrid) void_me; KlattGrid thee = (KlattGrid) void_thee; if (! classKlattGrid -> _parent -> copy (me, thee)) return 0;

 if (me -> phonation && ! (thee -> phonation = Data_copy (me -> phonation))) return 0;
 if (me -> vocalTract && ! (thee -> vocalTract = Data_copy (me -> vocalTract))) return 0;
 if (me -> coupling && ! (thee -> coupling = Data_copy (me -> coupling))) return 0;
 if (me -> frication && ! (thee -> frication = Data_copy (me -> frication))) return 0;
 if (me -> gain && ! (thee -> gain = Data_copy (me -> gain))) return 0;

return 1; }
# 35 "KlattGrid.c" 2
# 1 "../sys/oo_EQUAL.h" 1
# 29 "../sys/oo_EQUAL.h"
# 1 "../sys/oo_undef.h" 1
# 30 "../sys/oo_EQUAL.h" 2
# 36 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static _Bool classPhonationPoint_equal (Any void_me, Any void_thee) { PhonationPoint me = (PhonationPoint) void_me; PhonationPoint thee = (PhonationPoint) void_thee; if (! classPhonationPoint -> _parent -> equal (me, thee)) return 0;

 if (me -> time != thee -> time) return 0;
 if (me -> period != thee -> period) return 0;
 if (me -> openPhase != thee -> openPhase) return 0;
 if (me -> collisionPhase != thee -> collisionPhase) return 0;
 if (me -> te != thee -> te) return 0;
 if (me -> power1 != thee -> power1) return 0;
 if (me -> power2 != thee -> power2) return 0;
 if (me -> pulseScale != thee -> pulseScale) return 0;

return 1; }



static _Bool classPhonationTier_equal (Any void_me, Any void_thee) { PhonationTier me = (PhonationTier) void_me; PhonationTier thee = (PhonationTier) void_thee; if (! classPhonationTier -> _parent -> equal (me, thee)) return 0;

 if (! me -> points != ! thee -> points || (me -> points && ! Data_equal (me -> points, thee -> points))) return 0;

return 1; }



static _Bool classPhonationGrid_equal (Any void_me, Any void_thee) { PhonationGrid me = (PhonationGrid) void_me; PhonationGrid thee = (PhonationGrid) void_thee; if (! classPhonationGrid -> _parent -> equal (me, thee)) return 0;

 if (! me -> pitch != ! thee -> pitch || (me -> pitch && ! Data_equal (me -> pitch, thee -> pitch))) return 0;
 if (! me -> flutter != ! thee -> flutter || (me -> flutter && ! Data_equal (me -> flutter, thee -> flutter))) return 0;
 if (! me -> voicingAmplitude != ! thee -> voicingAmplitude || (me -> voicingAmplitude && ! Data_equal (me -> voicingAmplitude, thee -> voicingAmplitude))) return 0;
 if (! me -> doublePulsing != ! thee -> doublePulsing || (me -> doublePulsing && ! Data_equal (me -> doublePulsing, thee -> doublePulsing))) return 0;
 if (! me -> openPhase != ! thee -> openPhase || (me -> openPhase && ! Data_equal (me -> openPhase, thee -> openPhase))) return 0;
 if (! me -> collisionPhase != ! thee -> collisionPhase || (me -> collisionPhase && ! Data_equal (me -> collisionPhase, thee -> collisionPhase))) return 0;
 if (! me -> power1 != ! thee -> power1 || (me -> power1 && ! Data_equal (me -> power1, thee -> power1))) return 0;
 if (! me -> power2 != ! thee -> power2 || (me -> power2 && ! Data_equal (me -> power2, thee -> power2))) return 0;
 if (! me -> spectralTilt != ! thee -> spectralTilt || (me -> spectralTilt && ! Data_equal (me -> spectralTilt, thee -> spectralTilt))) return 0;
 if (! me -> aspirationAmplitude != ! thee -> aspirationAmplitude || (me -> aspirationAmplitude && ! Data_equal (me -> aspirationAmplitude, thee -> aspirationAmplitude))) return 0;
 if (! me -> breathinessAmplitude != ! thee -> breathinessAmplitude || (me -> breathinessAmplitude && ! Data_equal (me -> breathinessAmplitude, thee -> breathinessAmplitude))) return 0;

return 1; }



static _Bool classVocalTractGrid_equal (Any void_me, Any void_thee) { VocalTractGrid me = (VocalTractGrid) void_me; VocalTractGrid thee = (VocalTractGrid) void_thee; if (! classVocalTractGrid -> _parent -> equal (me, thee)) return 0;

 if (! me -> formants != ! thee -> formants || (me -> formants && ! Data_equal (me -> formants, thee -> formants))) return 0;
 if (! me -> nasal_formants != ! thee -> nasal_formants || (me -> nasal_formants && ! Data_equal (me -> nasal_formants, thee -> nasal_formants))) return 0;
 if (! me -> nasal_antiformants != ! thee -> nasal_antiformants || (me -> nasal_antiformants && ! Data_equal (me -> nasal_antiformants, thee -> nasal_antiformants))) return 0;

 if (! me -> formants_amplitudes != ! thee -> formants_amplitudes || (me -> formants_amplitudes && ! Data_equal (me -> formants_amplitudes, thee -> formants_amplitudes))) return 0;
 if (! me -> nasal_formants_amplitudes != ! thee -> nasal_formants_amplitudes || (me -> nasal_formants_amplitudes && ! Data_equal (me -> nasal_formants_amplitudes, thee -> nasal_formants_amplitudes))) return 0;

return 1; }



static _Bool classCouplingGrid_equal (Any void_me, Any void_thee) { CouplingGrid me = (CouplingGrid) void_me; CouplingGrid thee = (CouplingGrid) void_thee; if (! classCouplingGrid -> _parent -> equal (me, thee)) return 0;

 if (! me -> tracheal_formants != ! thee -> tracheal_formants || (me -> tracheal_formants && ! Data_equal (me -> tracheal_formants, thee -> tracheal_formants))) return 0;
 if (! me -> tracheal_antiformants != ! thee -> tracheal_antiformants || (me -> tracheal_antiformants && ! Data_equal (me -> tracheal_antiformants, thee -> tracheal_antiformants))) return 0;
 if (! me -> tracheal_formants_amplitudes != ! thee -> tracheal_formants_amplitudes || (me -> tracheal_formants_amplitudes && ! Data_equal (me -> tracheal_formants_amplitudes, thee -> tracheal_formants_amplitudes))) return 0;
 if (! me -> delta_formants != ! thee -> delta_formants || (me -> delta_formants && ! Data_equal (me -> delta_formants, thee -> delta_formants))) return 0;

  if (! me -> glottis != ! thee -> glottis || (me -> glottis && ! Data_equal (me -> glottis, thee -> glottis))) return 0;


return 1; }



static _Bool classFricationGrid_equal (Any void_me, Any void_thee) { FricationGrid me = (FricationGrid) void_me; FricationGrid thee = (FricationGrid) void_thee; if (! classFricationGrid -> _parent -> equal (me, thee)) return 0;

 if (! me -> noise_amplitude != ! thee -> noise_amplitude || (me -> noise_amplitude && ! Data_equal (me -> noise_amplitude, thee -> noise_amplitude))) return 0;
 if (! me -> formants != ! thee -> formants || (me -> formants && ! Data_equal (me -> formants, thee -> formants))) return 0;
 if (! me -> formants_amplitudes != ! thee -> formants_amplitudes || (me -> formants_amplitudes && ! Data_equal (me -> formants_amplitudes, thee -> formants_amplitudes))) return 0;
 if (! me -> bypass != ! thee -> bypass || (me -> bypass && ! Data_equal (me -> bypass, thee -> bypass))) return 0;

return 1; }



static _Bool classKlattGrid_equal (Any void_me, Any void_thee) { KlattGrid me = (KlattGrid) void_me; KlattGrid thee = (KlattGrid) void_thee; if (! classKlattGrid -> _parent -> equal (me, thee)) return 0;

 if (! me -> phonation != ! thee -> phonation || (me -> phonation && ! Data_equal (me -> phonation, thee -> phonation))) return 0;
 if (! me -> vocalTract != ! thee -> vocalTract || (me -> vocalTract && ! Data_equal (me -> vocalTract, thee -> vocalTract))) return 0;
 if (! me -> coupling != ! thee -> coupling || (me -> coupling && ! Data_equal (me -> coupling, thee -> coupling))) return 0;
 if (! me -> frication != ! thee -> frication || (me -> frication && ! Data_equal (me -> frication, thee -> frication))) return 0;
 if (! me -> gain != ! thee -> gain || (me -> gain && ! Data_equal (me -> gain, thee -> gain))) return 0;

return 1; }
# 37 "KlattGrid.c" 2
# 1 "../sys/oo_CAN_WRITE_AS_ENCODING.h" 1
# 27 "../sys/oo_CAN_WRITE_AS_ENCODING.h"
# 1 "../sys/oo_undef.h" 1
# 28 "../sys/oo_CAN_WRITE_AS_ENCODING.h" 2
# 38 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static _Bool classPhonationPoint_canWriteAsEncoding (Any void_me, int encoding) { PhonationPoint me = (PhonationPoint) void_me; if (! classPhonationPoint -> _parent -> canWriteAsEncoding (me, encoding)) return 0;










return 1; }



static _Bool classPhonationTier_canWriteAsEncoding (Any void_me, int encoding) { PhonationTier me = (PhonationTier) void_me; if (! classPhonationTier -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> points && ! Data_canWriteAsEncoding (me -> points, encoding)) return 0;

return 1; }



static _Bool classPhonationGrid_canWriteAsEncoding (Any void_me, int encoding) { PhonationGrid me = (PhonationGrid) void_me; if (! classPhonationGrid -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> pitch && ! Data_canWriteAsEncoding (me -> pitch, encoding)) return 0;
 if (me -> flutter && ! Data_canWriteAsEncoding (me -> flutter, encoding)) return 0;
 if (me -> voicingAmplitude && ! Data_canWriteAsEncoding (me -> voicingAmplitude, encoding)) return 0;
 if (me -> doublePulsing && ! Data_canWriteAsEncoding (me -> doublePulsing, encoding)) return 0;
 if (me -> openPhase && ! Data_canWriteAsEncoding (me -> openPhase, encoding)) return 0;
 if (me -> collisionPhase && ! Data_canWriteAsEncoding (me -> collisionPhase, encoding)) return 0;
 if (me -> power1 && ! Data_canWriteAsEncoding (me -> power1, encoding)) return 0;
 if (me -> power2 && ! Data_canWriteAsEncoding (me -> power2, encoding)) return 0;
 if (me -> spectralTilt && ! Data_canWriteAsEncoding (me -> spectralTilt, encoding)) return 0;
 if (me -> aspirationAmplitude && ! Data_canWriteAsEncoding (me -> aspirationAmplitude, encoding)) return 0;
 if (me -> breathinessAmplitude && ! Data_canWriteAsEncoding (me -> breathinessAmplitude, encoding)) return 0;

return 1; }



static _Bool classVocalTractGrid_canWriteAsEncoding (Any void_me, int encoding) { VocalTractGrid me = (VocalTractGrid) void_me; if (! classVocalTractGrid -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> formants && ! Data_canWriteAsEncoding (me -> formants, encoding)) return 0;
 if (me -> nasal_formants && ! Data_canWriteAsEncoding (me -> nasal_formants, encoding)) return 0;
 if (me -> nasal_antiformants && ! Data_canWriteAsEncoding (me -> nasal_antiformants, encoding)) return 0;

 if (me -> formants_amplitudes && ! Data_canWriteAsEncoding (me -> formants_amplitudes, encoding)) return 0;
 if (me -> nasal_formants_amplitudes && ! Data_canWriteAsEncoding (me -> nasal_formants_amplitudes, encoding)) return 0;

return 1; }



static _Bool classCouplingGrid_canWriteAsEncoding (Any void_me, int encoding) { CouplingGrid me = (CouplingGrid) void_me; if (! classCouplingGrid -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> tracheal_formants && ! Data_canWriteAsEncoding (me -> tracheal_formants, encoding)) return 0;
 if (me -> tracheal_antiformants && ! Data_canWriteAsEncoding (me -> tracheal_antiformants, encoding)) return 0;
 if (me -> tracheal_formants_amplitudes && ! Data_canWriteAsEncoding (me -> tracheal_formants_amplitudes, encoding)) return 0;
 if (me -> delta_formants && ! Data_canWriteAsEncoding (me -> delta_formants, encoding)) return 0;

  if (me -> glottis && ! Data_canWriteAsEncoding (me -> glottis, encoding)) return 0;


return 1; }



static _Bool classFricationGrid_canWriteAsEncoding (Any void_me, int encoding) { FricationGrid me = (FricationGrid) void_me; if (! classFricationGrid -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> noise_amplitude && ! Data_canWriteAsEncoding (me -> noise_amplitude, encoding)) return 0;
 if (me -> formants && ! Data_canWriteAsEncoding (me -> formants, encoding)) return 0;
 if (me -> formants_amplitudes && ! Data_canWriteAsEncoding (me -> formants_amplitudes, encoding)) return 0;
 if (me -> bypass && ! Data_canWriteAsEncoding (me -> bypass, encoding)) return 0;

return 1; }



static _Bool classKlattGrid_canWriteAsEncoding (Any void_me, int encoding) { KlattGrid me = (KlattGrid) void_me; if (! classKlattGrid -> _parent -> canWriteAsEncoding (me, encoding)) return 0;

 if (me -> phonation && ! Data_canWriteAsEncoding (me -> phonation, encoding)) return 0;
 if (me -> vocalTract && ! Data_canWriteAsEncoding (me -> vocalTract, encoding)) return 0;
 if (me -> coupling && ! Data_canWriteAsEncoding (me -> coupling, encoding)) return 0;
 if (me -> frication && ! Data_canWriteAsEncoding (me -> frication, encoding)) return 0;
 if (me -> gain && ! Data_canWriteAsEncoding (me -> gain, encoding)) return 0;

return 1; }
# 39 "KlattGrid.c" 2
# 1 "../sys/oo_WRITE_TEXT.h" 1
# 29 "../sys/oo_WRITE_TEXT.h"
# 1 "../sys/oo_undef.h" 1
# 30 "../sys/oo_WRITE_TEXT.h" 2
# 40 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static int classPhonationPoint_writeText (Any void_me, MelderFile file) { PhonationPoint me = (PhonationPoint) void_me; if (! classPhonationPoint -> _parent -> writeText (me, file)) return 0;

 texputr8 (file, me -> time, L"" "time", 0,0,0,0,0);
 texputr8 (file, me -> period, L"" "period", 0,0,0,0,0);
 texputr8 (file, me -> openPhase, L"" "openPhase", 0,0,0,0,0);
 texputr8 (file, me -> collisionPhase, L"" "collisionPhase", 0,0,0,0,0);
 texputr8 (file, me -> te, L"" "te", 0,0,0,0,0);
 texputr8 (file, me -> power1, L"" "power1", 0,0,0,0,0);
 texputr8 (file, me -> power2, L"" "power2", 0,0,0,0,0);
 texputr8 (file, me -> pulseScale, L"" "pulseScale", 0,0,0,0,0);

return 1; }



static int classPhonationTier_writeText (Any void_me, MelderFile file) { PhonationTier me = (PhonationTier) void_me; if (! classPhonationTier -> _parent -> writeText (me, file)) return 0;

 texputi4 (file, me -> points ? me -> points -> size : 0, L"" "points" ": size", 0,0,0,0,0); if (me -> points) { for (long i = 1; i <= me -> points -> size; i ++) { PhonationPoint data = me -> points -> item [i]; texputintro (file, L"" "points" " [", Melder_integer (i), L"]:", 0,0,0); if (! classPhonationPoint -> writeText (data, file)) return 0; texexdent (file); } }

return 1; }



static int classPhonationGrid_writeText (Any void_me, MelderFile file) { PhonationGrid me = (PhonationGrid) void_me; if (! classPhonationGrid -> _parent -> writeText (me, file)) return 0;

 texputex (file, me -> pitch != ((void *)0), L"" "pitch", 0,0,0,0,0); if (me -> pitch && ! Data_writeText (me -> pitch, file)) return 0;
 texputex (file, me -> flutter != ((void *)0), L"" "flutter", 0,0,0,0,0); if (me -> flutter && ! Data_writeText (me -> flutter, file)) return 0;
 texputex (file, me -> voicingAmplitude != ((void *)0), L"" "voicingAmplitude", 0,0,0,0,0); if (me -> voicingAmplitude && ! Data_writeText (me -> voicingAmplitude, file)) return 0;
 texputex (file, me -> doublePulsing != ((void *)0), L"" "doublePulsing", 0,0,0,0,0); if (me -> doublePulsing && ! Data_writeText (me -> doublePulsing, file)) return 0;
 texputex (file, me -> openPhase != ((void *)0), L"" "openPhase", 0,0,0,0,0); if (me -> openPhase && ! Data_writeText (me -> openPhase, file)) return 0;
 texputex (file, me -> collisionPhase != ((void *)0), L"" "collisionPhase", 0,0,0,0,0); if (me -> collisionPhase && ! Data_writeText (me -> collisionPhase, file)) return 0;
 texputex (file, me -> power1 != ((void *)0), L"" "power1", 0,0,0,0,0); if (me -> power1 && ! Data_writeText (me -> power1, file)) return 0;
 texputex (file, me -> power2 != ((void *)0), L"" "power2", 0,0,0,0,0); if (me -> power2 && ! Data_writeText (me -> power2, file)) return 0;
 texputex (file, me -> spectralTilt != ((void *)0), L"" "spectralTilt", 0,0,0,0,0); if (me -> spectralTilt && ! Data_writeText (me -> spectralTilt, file)) return 0;
 texputex (file, me -> aspirationAmplitude != ((void *)0), L"" "aspirationAmplitude", 0,0,0,0,0); if (me -> aspirationAmplitude && ! Data_writeText (me -> aspirationAmplitude, file)) return 0;
 texputex (file, me -> breathinessAmplitude != ((void *)0), L"" "breathinessAmplitude", 0,0,0,0,0); if (me -> breathinessAmplitude && ! Data_writeText (me -> breathinessAmplitude, file)) return 0;

return 1; }



static int classVocalTractGrid_writeText (Any void_me, MelderFile file) { VocalTractGrid me = (VocalTractGrid) void_me; if (! classVocalTractGrid -> _parent -> writeText (me, file)) return 0;

 texputex (file, me -> formants != ((void *)0), L"" "formants", 0,0,0,0,0); if (me -> formants && ! Data_writeText (me -> formants, file)) return 0;
 texputex (file, me -> nasal_formants != ((void *)0), L"" "nasal_formants", 0,0,0,0,0); if (me -> nasal_formants && ! Data_writeText (me -> nasal_formants, file)) return 0;
 texputex (file, me -> nasal_antiformants != ((void *)0), L"" "nasal_antiformants", 0,0,0,0,0); if (me -> nasal_antiformants && ! Data_writeText (me -> nasal_antiformants, file)) return 0;

 texputi4 (file, me -> formants_amplitudes ? me -> formants_amplitudes -> size : 0, L"" "formants_amplitudes" ": size", 0,0,0,0,0); if (me -> formants_amplitudes) { for (long i = 1; i <= me -> formants_amplitudes -> size; i ++) { IntensityTier data = me -> formants_amplitudes -> item [i]; texputintro (file, L"" "formants_amplitudes" " [", Melder_integer (i), L"]:", 0,0,0); if (! classIntensityTier -> writeText (data, file)) return 0; texexdent (file); } }
 texputi4 (file, me -> nasal_formants_amplitudes ? me -> nasal_formants_amplitudes -> size : 0, L"" "nasal_formants_amplitudes" ": size", 0,0,0,0,0); if (me -> nasal_formants_amplitudes) { for (long i = 1; i <= me -> nasal_formants_amplitudes -> size; i ++) { IntensityTier data = me -> nasal_formants_amplitudes -> item [i]; texputintro (file, L"" "nasal_formants_amplitudes" " [", Melder_integer (i), L"]:", 0,0,0); if (! classIntensityTier -> writeText (data, file)) return 0; texexdent (file); } }

return 1; }



static int classCouplingGrid_writeText (Any void_me, MelderFile file) { CouplingGrid me = (CouplingGrid) void_me; if (! classCouplingGrid -> _parent -> writeText (me, file)) return 0;

 texputex (file, me -> tracheal_formants != ((void *)0), L"" "tracheal_formants", 0,0,0,0,0); if (me -> tracheal_formants && ! Data_writeText (me -> tracheal_formants, file)) return 0;
 texputex (file, me -> tracheal_antiformants != ((void *)0), L"" "tracheal_antiformants", 0,0,0,0,0); if (me -> tracheal_antiformants && ! Data_writeText (me -> tracheal_antiformants, file)) return 0;
 texputi4 (file, me -> tracheal_formants_amplitudes ? me -> tracheal_formants_amplitudes -> size : 0, L"" "tracheal_formants_amplitudes" ": size", 0,0,0,0,0); if (me -> tracheal_formants_amplitudes) { for (long i = 1; i <= me -> tracheal_formants_amplitudes -> size; i ++) { IntensityTier data = me -> tracheal_formants_amplitudes -> item [i]; texputintro (file, L"" "tracheal_formants_amplitudes" " [", Melder_integer (i), L"]:", 0,0,0); if (! classIntensityTier -> writeText (data, file)) return 0; texexdent (file); } }
 texputex (file, me -> delta_formants != ((void *)0), L"" "delta_formants", 0,0,0,0,0); if (me -> delta_formants && ! Data_writeText (me -> delta_formants, file)) return 0;




return 1; }



static int classFricationGrid_writeText (Any void_me, MelderFile file) { FricationGrid me = (FricationGrid) void_me; if (! classFricationGrid -> _parent -> writeText (me, file)) return 0;

 texputex (file, me -> noise_amplitude != ((void *)0), L"" "noise_amplitude", 0,0,0,0,0); if (me -> noise_amplitude && ! Data_writeText (me -> noise_amplitude, file)) return 0;
 texputex (file, me -> formants != ((void *)0), L"" "formants", 0,0,0,0,0); if (me -> formants && ! Data_writeText (me -> formants, file)) return 0;
 texputi4 (file, me -> formants_amplitudes ? me -> formants_amplitudes -> size : 0, L"" "formants_amplitudes" ": size", 0,0,0,0,0); if (me -> formants_amplitudes) { for (long i = 1; i <= me -> formants_amplitudes -> size; i ++) { RealTier data = me -> formants_amplitudes -> item [i]; texputintro (file, L"" "formants_amplitudes" " [", Melder_integer (i), L"]:", 0,0,0); if (! classRealTier -> writeText (data, file)) return 0; texexdent (file); } }
 texputex (file, me -> bypass != ((void *)0), L"" "bypass", 0,0,0,0,0); if (me -> bypass && ! Data_writeText (me -> bypass, file)) return 0;

return 1; }



static int classKlattGrid_writeText (Any void_me, MelderFile file) { KlattGrid me = (KlattGrid) void_me; if (! classKlattGrid -> _parent -> writeText (me, file)) return 0;

 texputex (file, me -> phonation != ((void *)0), L"" "phonation", 0,0,0,0,0); if (me -> phonation && ! Data_writeText (me -> phonation, file)) return 0;
 texputex (file, me -> vocalTract != ((void *)0), L"" "vocalTract", 0,0,0,0,0); if (me -> vocalTract && ! Data_writeText (me -> vocalTract, file)) return 0;
 texputex (file, me -> coupling != ((void *)0), L"" "coupling", 0,0,0,0,0); if (me -> coupling && ! Data_writeText (me -> coupling, file)) return 0;
 texputex (file, me -> frication != ((void *)0), L"" "frication", 0,0,0,0,0); if (me -> frication && ! Data_writeText (me -> frication, file)) return 0;
 texputex (file, me -> gain != ((void *)0), L"" "gain", 0,0,0,0,0); if (me -> gain && ! Data_writeText (me -> gain, file)) return 0;

return 1; }
# 41 "KlattGrid.c" 2
# 1 "../sys/oo_WRITE_BINARY.h" 1
# 28 "../sys/oo_WRITE_BINARY.h"
# 1 "../sys/oo_undef.h" 1
# 29 "../sys/oo_WRITE_BINARY.h" 2
# 42 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static int classPhonationPoint_writeBinary (Any void_me, FILE *f) { PhonationPoint me = (PhonationPoint) void_me; if (! classPhonationPoint -> _parent -> writeBinary (me, f)) return 0;

 binputr8 (me -> time, f);
 binputr8 (me -> period, f);
 binputr8 (me -> openPhase, f);
 binputr8 (me -> collisionPhase, f);
 binputr8 (me -> te, f);
 binputr8 (me -> power1, f);
 binputr8 (me -> power2, f);
 binputr8 (me -> pulseScale, f);

return 1; }



static int classPhonationTier_writeBinary (Any void_me, FILE *f) { PhonationTier me = (PhonationTier) void_me; if (! classPhonationTier -> _parent -> writeBinary (me, f)) return 0;

 binputi4 (me -> points ? me -> points -> size : 0, f); if (me -> points) { long i; for (i = 1; i <= me -> points -> size; i ++) { PhonationPoint data = me -> points -> item [i]; if (! classPhonationPoint -> writeBinary (data, f)) return 0; } }

return 1; }



static int classPhonationGrid_writeBinary (Any void_me, FILE *f) { PhonationGrid me = (PhonationGrid) void_me; if (! classPhonationGrid -> _parent -> writeBinary (me, f)) return 0;

 binputi1 (me -> pitch != ((void *)0), f); if (me -> pitch && ! Data_writeBinary (me -> pitch, f)) return 0;
 binputi1 (me -> flutter != ((void *)0), f); if (me -> flutter && ! Data_writeBinary (me -> flutter, f)) return 0;
 binputi1 (me -> voicingAmplitude != ((void *)0), f); if (me -> voicingAmplitude && ! Data_writeBinary (me -> voicingAmplitude, f)) return 0;
 binputi1 (me -> doublePulsing != ((void *)0), f); if (me -> doublePulsing && ! Data_writeBinary (me -> doublePulsing, f)) return 0;
 binputi1 (me -> openPhase != ((void *)0), f); if (me -> openPhase && ! Data_writeBinary (me -> openPhase, f)) return 0;
 binputi1 (me -> collisionPhase != ((void *)0), f); if (me -> collisionPhase && ! Data_writeBinary (me -> collisionPhase, f)) return 0;
 binputi1 (me -> power1 != ((void *)0), f); if (me -> power1 && ! Data_writeBinary (me -> power1, f)) return 0;
 binputi1 (me -> power2 != ((void *)0), f); if (me -> power2 && ! Data_writeBinary (me -> power2, f)) return 0;
 binputi1 (me -> spectralTilt != ((void *)0), f); if (me -> spectralTilt && ! Data_writeBinary (me -> spectralTilt, f)) return 0;
 binputi1 (me -> aspirationAmplitude != ((void *)0), f); if (me -> aspirationAmplitude && ! Data_writeBinary (me -> aspirationAmplitude, f)) return 0;
 binputi1 (me -> breathinessAmplitude != ((void *)0), f); if (me -> breathinessAmplitude && ! Data_writeBinary (me -> breathinessAmplitude, f)) return 0;

return 1; }



static int classVocalTractGrid_writeBinary (Any void_me, FILE *f) { VocalTractGrid me = (VocalTractGrid) void_me; if (! classVocalTractGrid -> _parent -> writeBinary (me, f)) return 0;

 binputi1 (me -> formants != ((void *)0), f); if (me -> formants && ! Data_writeBinary (me -> formants, f)) return 0;
 binputi1 (me -> nasal_formants != ((void *)0), f); if (me -> nasal_formants && ! Data_writeBinary (me -> nasal_formants, f)) return 0;
 binputi1 (me -> nasal_antiformants != ((void *)0), f); if (me -> nasal_antiformants && ! Data_writeBinary (me -> nasal_antiformants, f)) return 0;

 binputi4 (me -> formants_amplitudes ? me -> formants_amplitudes -> size : 0, f); if (me -> formants_amplitudes) { long i; for (i = 1; i <= me -> formants_amplitudes -> size; i ++) { IntensityTier data = me -> formants_amplitudes -> item [i]; if (! classIntensityTier -> writeBinary (data, f)) return 0; } }
 binputi4 (me -> nasal_formants_amplitudes ? me -> nasal_formants_amplitudes -> size : 0, f); if (me -> nasal_formants_amplitudes) { long i; for (i = 1; i <= me -> nasal_formants_amplitudes -> size; i ++) { IntensityTier data = me -> nasal_formants_amplitudes -> item [i]; if (! classIntensityTier -> writeBinary (data, f)) return 0; } }

return 1; }



static int classCouplingGrid_writeBinary (Any void_me, FILE *f) { CouplingGrid me = (CouplingGrid) void_me; if (! classCouplingGrid -> _parent -> writeBinary (me, f)) return 0;

 binputi1 (me -> tracheal_formants != ((void *)0), f); if (me -> tracheal_formants && ! Data_writeBinary (me -> tracheal_formants, f)) return 0;
 binputi1 (me -> tracheal_antiformants != ((void *)0), f); if (me -> tracheal_antiformants && ! Data_writeBinary (me -> tracheal_antiformants, f)) return 0;
 binputi4 (me -> tracheal_formants_amplitudes ? me -> tracheal_formants_amplitudes -> size : 0, f); if (me -> tracheal_formants_amplitudes) { long i; for (i = 1; i <= me -> tracheal_formants_amplitudes -> size; i ++) { IntensityTier data = me -> tracheal_formants_amplitudes -> item [i]; if (! classIntensityTier -> writeBinary (data, f)) return 0; } }
 binputi1 (me -> delta_formants != ((void *)0), f); if (me -> delta_formants && ! Data_writeBinary (me -> delta_formants, f)) return 0;




return 1; }



static int classFricationGrid_writeBinary (Any void_me, FILE *f) { FricationGrid me = (FricationGrid) void_me; if (! classFricationGrid -> _parent -> writeBinary (me, f)) return 0;

 binputi1 (me -> noise_amplitude != ((void *)0), f); if (me -> noise_amplitude && ! Data_writeBinary (me -> noise_amplitude, f)) return 0;
 binputi1 (me -> formants != ((void *)0), f); if (me -> formants && ! Data_writeBinary (me -> formants, f)) return 0;
 binputi4 (me -> formants_amplitudes ? me -> formants_amplitudes -> size : 0, f); if (me -> formants_amplitudes) { long i; for (i = 1; i <= me -> formants_amplitudes -> size; i ++) { RealTier data = me -> formants_amplitudes -> item [i]; if (! classRealTier -> writeBinary (data, f)) return 0; } }
 binputi1 (me -> bypass != ((void *)0), f); if (me -> bypass && ! Data_writeBinary (me -> bypass, f)) return 0;

return 1; }



static int classKlattGrid_writeBinary (Any void_me, FILE *f) { KlattGrid me = (KlattGrid) void_me; if (! classKlattGrid -> _parent -> writeBinary (me, f)) return 0;

 binputi1 (me -> phonation != ((void *)0), f); if (me -> phonation && ! Data_writeBinary (me -> phonation, f)) return 0;
 binputi1 (me -> vocalTract != ((void *)0), f); if (me -> vocalTract && ! Data_writeBinary (me -> vocalTract, f)) return 0;
 binputi1 (me -> coupling != ((void *)0), f); if (me -> coupling && ! Data_writeBinary (me -> coupling, f)) return 0;
 binputi1 (me -> frication != ((void *)0), f); if (me -> frication && ! Data_writeBinary (me -> frication, f)) return 0;
 binputi1 (me -> gain != ((void *)0), f); if (me -> gain && ! Data_writeBinary (me -> gain, f)) return 0;

return 1; }
# 43 "KlattGrid.c" 2
# 1 "../sys/oo_READ_TEXT.h" 1
# 35 "../sys/oo_READ_TEXT.h"
# 1 "../sys/oo_undef.h" 1
# 36 "../sys/oo_READ_TEXT.h" 2
# 44 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static int classPhonationPoint_readText (Any void_me, MelderReadText text) { PhonationPoint me = (PhonationPoint) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationPoint -> _parent -> readText (me, text)) return 0;

 me -> time = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "time", L"\".");
 me -> period = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "period", L"\".");
 me -> openPhase = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "openPhase", L"\".");
 me -> collisionPhase = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "collisionPhase", L"\".");
 me -> te = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "te", L"\".");
 me -> power1 = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "power1", L"\".");
 me -> power2 = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "power2", L"\".");
 me -> pulseScale = texgetr8 (text); if (Melder_hasError ()) return Melder_error3 (L"Trying to read \"", L"" "pulseScale", L"\".");

return 1; }



static int classPhonationTier_readText (Any void_me, MelderReadText text) { PhonationTier me = (PhonationTier) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationTier -> _parent -> readText (me, text)) return 0;

 { long n = texgeti4 (text); if ((me -> points = SortedSetOfDouble_create ()) == ((void *)0)) return 0; for (long i = 1; i <= n; i ++) { long saveVersion = Thing_version; PhonationPoint item = (PhonationPoint) Thing_new ((void *) classPhonationPoint); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readText (item, text)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> points, item)) return 0; } }

return 1; }



static int classPhonationGrid_readText (Any void_me, MelderReadText text) { PhonationGrid me = (PhonationGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationGrid -> _parent -> readText (me, text)) return 0;

 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> pitch = (PitchTier) Thing_new ((void *) classPitchTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> pitch, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> flutter = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> flutter, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> voicingAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> voicingAmplitude, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> doublePulsing = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> doublePulsing, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> openPhase = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> openPhase, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> collisionPhase = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> collisionPhase, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> power1 = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> power1, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> power2 = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> power2, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> spectralTilt = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> spectralTilt, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> aspirationAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> aspirationAmplitude, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> breathinessAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> breathinessAmplitude, text)) return 0; Thing_version = saveVersion; }

return 1; }



static int classVocalTractGrid_readText (Any void_me, MelderReadText text) { VocalTractGrid me = (VocalTractGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classVocalTractGrid -> _parent -> readText (me, text)) return 0;

 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> formants, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> nasal_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> nasal_formants, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> nasal_antiformants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> nasal_antiformants, text)) return 0; Thing_version = saveVersion; }

 { long n = texgeti4 (text); if ((me -> formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (long i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readText (item, text)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> formants_amplitudes, item)) return 0; } }
 { long n = texgeti4 (text); if ((me -> nasal_formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (long i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readText (item, text)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> nasal_formants_amplitudes, item)) return 0; } }

return 1; }



static int classCouplingGrid_readText (Any void_me, MelderReadText text) { CouplingGrid me = (CouplingGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classCouplingGrid -> _parent -> readText (me, text)) return 0;

 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> tracheal_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> tracheal_formants, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> tracheal_antiformants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> tracheal_antiformants, text)) return 0; Thing_version = saveVersion; }
 { long n = texgeti4 (text); if ((me -> tracheal_formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (long i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readText (item, text)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> tracheal_formants_amplitudes, item)) return 0; } }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> delta_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> delta_formants, text)) return 0; Thing_version = saveVersion; }




return 1; }



static int classFricationGrid_readText (Any void_me, MelderReadText text) { FricationGrid me = (FricationGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classFricationGrid -> _parent -> readText (me, text)) return 0;

 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> noise_amplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> noise_amplitude, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> formants, text)) return 0; Thing_version = saveVersion; }
 { long n = texgeti4 (text); if ((me -> formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (long i = 1; i <= n; i ++) { long saveVersion = Thing_version; RealTier item = (RealTier) Thing_new ((void *) classRealTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readText (item, text)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> formants_amplitudes, item)) return 0; } }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> bypass = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> bypass, text)) return 0; Thing_version = saveVersion; }

return 1; }



static int classKlattGrid_readText (Any void_me, MelderReadText text) { KlattGrid me = (KlattGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classKlattGrid -> _parent -> readText (me, text)) return 0;

 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> phonation = (PhonationGrid) Thing_new ((void *) classPhonationGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> phonation, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> vocalTract = (VocalTractGrid) Thing_new ((void *) classVocalTractGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> vocalTract, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> coupling = (CouplingGrid) Thing_new ((void *) classCouplingGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> coupling, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> frication = (FricationGrid) Thing_new ((void *) classFricationGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> frication, text)) return 0; Thing_version = saveVersion; }
 if (texgetex (text) == 1) { long saveVersion = Thing_version; if ((me -> gain = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readText (me -> gain, text)) return 0; Thing_version = saveVersion; }

return 1; }
# 45 "KlattGrid.c" 2
# 1 "../sys/oo_READ_BINARY.h" 1
# 29 "../sys/oo_READ_BINARY.h"
# 1 "../sys/oo_undef.h" 1
# 30 "../sys/oo_READ_BINARY.h" 2
# 46 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static int classPhonationPoint_readBinary (Any void_me, FILE *f) { PhonationPoint me = (PhonationPoint) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationPoint -> _parent -> readBinary (me, f)) return 0;

 me -> time = bingetr8 (f);
 me -> period = bingetr8 (f);
 me -> openPhase = bingetr8 (f);
 me -> collisionPhase = bingetr8 (f);
 me -> te = bingetr8 (f);
 me -> power1 = bingetr8 (f);
 me -> power2 = bingetr8 (f);
 me -> pulseScale = bingetr8 (f);

return 1; }



static int classPhonationTier_readBinary (Any void_me, FILE *f) { PhonationTier me = (PhonationTier) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationTier -> _parent -> readBinary (me, f)) return 0;

 { long n = bingeti4 (f), i; if ((me -> points = SortedSetOfDouble_create ()) == ((void *)0)) return 0; for (i = 1; i <= n; i ++) { long saveVersion = Thing_version; PhonationPoint item = (PhonationPoint) Thing_new ((void *) classPhonationPoint); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readBinary (item, f)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> points, item)) return 0; } }

return 1; }



static int classPhonationGrid_readBinary (Any void_me, FILE *f) { PhonationGrid me = (PhonationGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classPhonationGrid -> _parent -> readBinary (me, f)) return 0;

 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> pitch = (PitchTier) Thing_new ((void *) classPitchTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> pitch, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> flutter = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> flutter, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> voicingAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> voicingAmplitude, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> doublePulsing = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> doublePulsing, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> openPhase = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> openPhase, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> collisionPhase = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> collisionPhase, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> power1 = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> power1, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> power2 = (RealTier) Thing_new ((void *) classRealTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> power2, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> spectralTilt = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> spectralTilt, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> aspirationAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> aspirationAmplitude, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> breathinessAmplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> breathinessAmplitude, f)) return 0; Thing_version = saveVersion; }

return 1; }



static int classVocalTractGrid_readBinary (Any void_me, FILE *f) { VocalTractGrid me = (VocalTractGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classVocalTractGrid -> _parent -> readBinary (me, f)) return 0;

 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> formants, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> nasal_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> nasal_formants, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> nasal_antiformants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> nasal_antiformants, f)) return 0; Thing_version = saveVersion; }

 { long n = bingeti4 (f), i; if ((me -> formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readBinary (item, f)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> formants_amplitudes, item)) return 0; } }
 { long n = bingeti4 (f), i; if ((me -> nasal_formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readBinary (item, f)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> nasal_formants_amplitudes, item)) return 0; } }

return 1; }



static int classCouplingGrid_readBinary (Any void_me, FILE *f) { CouplingGrid me = (CouplingGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classCouplingGrid -> _parent -> readBinary (me, f)) return 0;

 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> tracheal_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> tracheal_formants, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> tracheal_antiformants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> tracheal_antiformants, f)) return 0; Thing_version = saveVersion; }
 { long n = bingeti4 (f), i; if ((me -> tracheal_formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (i = 1; i <= n; i ++) { long saveVersion = Thing_version; IntensityTier item = (IntensityTier) Thing_new ((void *) classIntensityTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readBinary (item, f)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> tracheal_formants_amplitudes, item)) return 0; } }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> delta_formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> delta_formants, f)) return 0; Thing_version = saveVersion; }




return 1; }



static int classFricationGrid_readBinary (Any void_me, FILE *f) { FricationGrid me = (FricationGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classFricationGrid -> _parent -> readBinary (me, f)) return 0;

 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> noise_amplitude = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> noise_amplitude, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> formants = (FormantGrid) Thing_new ((void *) classFormantGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> formants, f)) return 0; Thing_version = saveVersion; }
 { long n = bingeti4 (f), i; if ((me -> formants_amplitudes = Ordered_create ()) == ((void *)0)) return 0; for (i = 1; i <= n; i ++) { long saveVersion = Thing_version; RealTier item = (RealTier) Thing_new ((void *) classRealTier); if (item == ((void *)0)) return 0; Thing_version = 0; if (! item -> methods -> readBinary (item, f)) return 0; Thing_version = saveVersion; if (! Collection_addItem (me -> formants_amplitudes, item)) return 0; } }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> bypass = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> bypass, f)) return 0; Thing_version = saveVersion; }

return 1; }



static int classKlattGrid_readBinary (Any void_me, FILE *f) { KlattGrid me = (KlattGrid) void_me; int localVersion = Thing_version; (void) localVersion; if (localVersion > me -> methods -> version) return Melder_error ("The format of this file is too new. Download a newer version of Praat."); if (! classKlattGrid -> _parent -> readBinary (me, f)) return 0;

 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> phonation = (PhonationGrid) Thing_new ((void *) classPhonationGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> phonation, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> vocalTract = (VocalTractGrid) Thing_new ((void *) classVocalTractGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> vocalTract, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> coupling = (CouplingGrid) Thing_new ((void *) classCouplingGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> coupling, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> frication = (FricationGrid) Thing_new ((void *) classFricationGrid)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> frication, f)) return 0; Thing_version = saveVersion; }
 if (bingeti1 (f)) { long saveVersion = Thing_version; if ((me -> gain = (IntensityTier) Thing_new ((void *) classIntensityTier)) == ((void *)0)) return 0; Thing_version = 0; if (! Data_readBinary (me -> gain, f)) return 0; Thing_version = saveVersion; }

return 1; }
# 47 "KlattGrid.c" 2
# 1 "../sys/oo_DESCRIPTION.h" 1
# 30 "../sys/oo_DESCRIPTION.h"
# 1 "../sys/oo_undef.h" 1
# 31 "../sys/oo_DESCRIPTION.h" 2
# 48 "KlattGrid.c" 2
# 1 "KlattGrid_def.h" 1
# 25 "KlattGrid_def.h"
static struct structData_Description classPhonationPoint_description [] = { { L"" "PhonationPoint", 28, 0, sizeof (struct structPhonationPoint), L"" "PhonationPoint", & theStructData. description },

 { L"" "time", 11, (char *) & ((PhonationPoint) 0) -> time - (char *) 0, sizeof (double) },
 { L"" "period", 11, (char *) & ((PhonationPoint) 0) -> period - (char *) 0, sizeof (double) },
 { L"" "openPhase", 11, (char *) & ((PhonationPoint) 0) -> openPhase - (char *) 0, sizeof (double) },
 { L"" "collisionPhase", 11, (char *) & ((PhonationPoint) 0) -> collisionPhase - (char *) 0, sizeof (double) },
 { L"" "te", 11, (char *) & ((PhonationPoint) 0) -> te - (char *) 0, sizeof (double) },
 { L"" "power1", 11, (char *) & ((PhonationPoint) 0) -> power1 - (char *) 0, sizeof (double) },
 { L"" "power2", 11, (char *) & ((PhonationPoint) 0) -> power2 - (char *) 0, sizeof (double) },
 { L"" "pulseScale", 11, (char *) & ((PhonationPoint) 0) -> pulseScale - (char *) 0, sizeof (double) },

{ 0 } };



static struct structData_Description classPhonationTier_description [] = { { L"" "PhonationTier", 28, 0, sizeof (struct structPhonationTier), L"" "PhonationTier", & theStructFunction. description },

 { L"" "points", 27, (char *) & ((PhonationTier) 0) -> points - (char *) 0, sizeof (struct structPhonationPoint), L"" "SortedSetOfDouble", & theStructSortedSetOfDouble, 0, (const wchar_t *) & theStructPhonationPoint },

{ 0 } };



static struct structData_Description classPhonationGrid_description [] = { { L"" "PhonationGrid", 28, 0, sizeof (struct structPhonationGrid), L"" "PhonationGrid", & theStructFunction. description },

 { L"" "pitch", 26, (char *) & ((PhonationGrid) 0) -> pitch - (char *) 0, sizeof (PitchTier), L"" "PitchTier", & theStructPitchTier },
 { L"" "flutter", 26, (char *) & ((PhonationGrid) 0) -> flutter - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "voicingAmplitude", 26, (char *) & ((PhonationGrid) 0) -> voicingAmplitude - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },
 { L"" "doublePulsing", 26, (char *) & ((PhonationGrid) 0) -> doublePulsing - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "openPhase", 26, (char *) & ((PhonationGrid) 0) -> openPhase - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "collisionPhase", 26, (char *) & ((PhonationGrid) 0) -> collisionPhase - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "power1", 26, (char *) & ((PhonationGrid) 0) -> power1 - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "power2", 26, (char *) & ((PhonationGrid) 0) -> power2 - (char *) 0, sizeof (RealTier), L"" "RealTier", & theStructRealTier },
 { L"" "spectralTilt", 26, (char *) & ((PhonationGrid) 0) -> spectralTilt - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },
 { L"" "aspirationAmplitude", 26, (char *) & ((PhonationGrid) 0) -> aspirationAmplitude - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },
 { L"" "breathinessAmplitude", 26, (char *) & ((PhonationGrid) 0) -> breathinessAmplitude - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },

{ 0 } };



static struct structData_Description classVocalTractGrid_description [] = { { L"" "VocalTractGrid", 28, 0, sizeof (struct structVocalTractGrid), L"" "VocalTractGrid", & theStructFunction. description },

 { L"" "formants", 26, (char *) & ((VocalTractGrid) 0) -> formants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },
 { L"" "nasal_formants", 26, (char *) & ((VocalTractGrid) 0) -> nasal_formants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },
 { L"" "nasal_antiformants", 26, (char *) & ((VocalTractGrid) 0) -> nasal_antiformants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },

 { L"" "formants_amplitudes", 27, (char *) & ((VocalTractGrid) 0) -> formants_amplitudes - (char *) 0, sizeof (struct structIntensityTier), L"" "Ordered", & theStructOrdered, 0, (const wchar_t *) & theStructIntensityTier },
 { L"" "nasal_formants_amplitudes", 27, (char *) & ((VocalTractGrid) 0) -> nasal_formants_amplitudes - (char *) 0, sizeof (struct structIntensityTier), L"" "Ordered", & theStructOrdered, 0, (const wchar_t *) & theStructIntensityTier },

{ 0 } };



static struct structData_Description classCouplingGrid_description [] = { { L"" "CouplingGrid", 28, 0, sizeof (struct structCouplingGrid), L"" "CouplingGrid", & theStructFunction. description },

 { L"" "tracheal_formants", 26, (char *) & ((CouplingGrid) 0) -> tracheal_formants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },
 { L"" "tracheal_antiformants", 26, (char *) & ((CouplingGrid) 0) -> tracheal_antiformants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },
 { L"" "tracheal_formants_amplitudes", 27, (char *) & ((CouplingGrid) 0) -> tracheal_formants_amplitudes - (char *) 0, sizeof (struct structIntensityTier), L"" "Ordered", & theStructOrdered, 0, (const wchar_t *) & theStructIntensityTier },
 { L"" "delta_formants", 26, (char *) & ((CouplingGrid) 0) -> delta_formants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },

  { L"" "glottis", 26, (char *) & ((CouplingGrid) 0) -> glottis - (char *) 0, sizeof (PhonationTier), L"" "PhonationTier", & theStructPhonationTier },


{ 0 } };



static struct structData_Description classFricationGrid_description [] = { { L"" "FricationGrid", 28, 0, sizeof (struct structFricationGrid), L"" "FricationGrid", & theStructFunction. description },

 { L"" "noise_amplitude", 26, (char *) & ((FricationGrid) 0) -> noise_amplitude - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },
 { L"" "formants", 26, (char *) & ((FricationGrid) 0) -> formants - (char *) 0, sizeof (FormantGrid), L"" "FormantGrid", & theStructFormantGrid },
 { L"" "formants_amplitudes", 27, (char *) & ((FricationGrid) 0) -> formants_amplitudes - (char *) 0, sizeof (struct structRealTier), L"" "Ordered", & theStructOrdered, 0, (const wchar_t *) & theStructRealTier },
 { L"" "bypass", 26, (char *) & ((FricationGrid) 0) -> bypass - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },

{ 0 } };



static struct structData_Description classKlattGrid_description [] = { { L"" "KlattGrid", 28, 0, sizeof (struct structKlattGrid), L"" "KlattGrid", & theStructFunction. description },

 { L"" "phonation", 26, (char *) & ((KlattGrid) 0) -> phonation - (char *) 0, sizeof (PhonationGrid), L"" "PhonationGrid", & theStructPhonationGrid },
 { L"" "vocalTract", 26, (char *) & ((KlattGrid) 0) -> vocalTract - (char *) 0, sizeof (VocalTractGrid), L"" "VocalTractGrid", & theStructVocalTractGrid },
 { L"" "coupling", 26, (char *) & ((KlattGrid) 0) -> coupling - (char *) 0, sizeof (CouplingGrid), L"" "CouplingGrid", & theStructCouplingGrid },
 { L"" "frication", 26, (char *) & ((KlattGrid) 0) -> frication - (char *) 0, sizeof (FricationGrid), L"" "FricationGrid", & theStructFricationGrid },
 { L"" "gain", 26, (char *) & ((KlattGrid) 0) -> gain - (char *) 0, sizeof (IntensityTier), L"" "IntensityTier", & theStructIntensityTier },

{ 0 } };
# 49 "KlattGrid.c" 2



PointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod);

int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, Any void_thee, long iformant, int antiformant);

Sound KlattGrid_to_Sound_aspiration (KlattGrid me, synthesisParams p);

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling, synthesisParams params);






static double NUMinterpolateLinear (double x1, double y1, double x2, double y2, double x)
{
 if (y1 == y2) return y1;
 if (x1 == x2) return NUMundefined;
 return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

static void rel_to_abs (double *w, double *ws, long n, double d)
{
 long i; double sum;

 for (sum = 0, i = 1; i <= n; i++)
 {
  sum += w[i];
 }
 if (sum != 0)
 {
  double dw = d / sum;
  for (sum = 0, i = 1; i <= n; i++)
  {
   w[i] *= dw;
   sum += w[i];
   ws[i] = sum;
  }
 }
}

static RealTier RealTier_copyInterval (Any void_me, double t1, double t2, int interpolateBorders)
{
 RealTier me = (RealTier) void_me;
 long i = 1, i1, npoints = me -> points -> size;
 RealPoint p1;

 if (t1 >= t2 || t1 < me -> xmin || t2 > me -> xmax) return ((void *)0);

 RealTier thee = RealTier_create (t1, t2);
 if (thee == ((void *)0)) return ((void *)0);

 while (i <= npoints && (p1 = me -> points -> item[i], p1 -> time <= t1)) { i++; }
 i1 = i;

 while (i <= npoints && (p1 = me -> points -> item[i], p1 -> time <= t2))
 {
  if (! RealTier_addPoint (thee, p1 -> time, p1 -> value)) goto end;
  i++;
 }
 if (interpolateBorders)
 {
  RealPoint p2;
  double value;
  if (i1 > 1 && i > i1)
  {
   p1 = me -> points -> item[i1 - 1]; p2 = me -> points -> item[i1];
   value = NUMinterpolateLinear (p1 -> time, p1 -> value, p2 -> time, p2 -> value, t1);
   RealTier_addPoint (thee, t1, value);
  }
  if (i <= npoints)
  {
   p1 = me -> points -> item[i - 1]; p2 = me -> points -> item[i];
   value = NUMinterpolateLinear (p1 -> time, p1 -> value, p2 -> time, p2 -> value, t2);
   RealTier_addPoint (thee, t2, value);
  }

 }
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}

static double PointProcess_getPeriodAtIndex (PointProcess me, long it, double maximumPeriod)
{
 double period = NUMundefined;
 if (it >= 2)
 {
  period = me -> t[it] - me -> t[it - 1];
  if (period > maximumPeriod) { period = NUMundefined; }
 }
 if (period == NUMundefined)
 {
  if (it < me -> nt)
  {
   period = me -> t[it + 1] - me -> t[it];
   if (period > maximumPeriod) { period = NUMundefined; }
  }
 }

 return period;
}
# 163 "KlattGrid.c"
static RealTier RealTier_updateWithDelta (RealTier me, RealTier delta, PhonationTier glottis, double openglottis_fadeFraction)
{
 long myindex = 1;
 RealPoint mypoint = me -> points -> item [myindex];
 long numberOfValues = me -> points -> size;
 double mytime = mypoint -> time;
 double myvalue = mypoint -> value;
 double lasttime = me -> xmin - 0.001;
 RealTier thee = RealTier_create (me -> xmin, me -> xmax);
 if (thee == ((void *)0)) return ((void *)0);

 if (openglottis_fadeFraction <= 0) openglottis_fadeFraction = 0.0001;
 if (openglottis_fadeFraction >= 0.5) openglottis_fadeFraction = 0.4999;

 for (long ipoint = 1; ipoint <= glottis -> points -> size; ipoint++)
 {
  PhonationPoint point = glottis -> points -> item [ipoint];
  double t4 = point -> time;
  double openDuration = point -> te ;
  double t1 = t4 - openDuration;
  double t2 = t1 + openglottis_fadeFraction * openDuration;
  double t3 = t4 - openglottis_fadeFraction * openDuration;


  while (mytime > lasttime && mytime < t1)
  {
   RealTier_addPoint (thee, mytime, myvalue); lasttime=mytime; myindex++; if (myindex <= numberOfValues) { mypoint = me -> points -> item [myindex]; mytime = mypoint -> time; myvalue = mypoint -> value; } else mytime = me -> xmax;
  }

  if (t2 > t1)
  {

   double myvalue1 = RealTier_getValueAtTime (me, t1);
   RealTier_addPoint (thee, t1, myvalue1);

   while (mytime > lasttime && mytime < t2)
   {
    double dvalue = RealTier_getValueAtTime (delta, mytime);
    if (dvalue != NUMundefined)
    {
     double fraction = (mytime - t1) / (openglottis_fadeFraction * openDuration);
     myvalue += dvalue * fraction;
    }
    RealTier_addPoint (thee, mytime, myvalue); lasttime=mytime; myindex++; if (myindex <= numberOfValues) { mypoint = me -> points -> item [myindex]; mytime = mypoint -> time; myvalue = mypoint -> value; } else mytime = me -> xmax;
   }
  }

  double myvalue2 = RealTier_getValueAtTime (me, t2);
  double dvalue = RealTier_getValueAtTime (delta, t2);
  if (dvalue != NUMundefined) myvalue2 += dvalue;
  RealTier_addPoint (thee, t2, myvalue2);



  while (mytime > lasttime && mytime < t3)
  {
   dvalue = RealTier_getValueAtTime (delta, mytime);
   if (dvalue != NUMundefined) myvalue += dvalue;
   RealTier_addPoint (thee, mytime, myvalue); lasttime=mytime; myindex++; if (myindex <= numberOfValues) { mypoint = me -> points -> item [myindex]; mytime = mypoint -> time; myvalue = mypoint -> value; } else mytime = me -> xmax;
  }



  double myvalue3 = RealTier_getValueAtTime (me, t3);
  dvalue = RealTier_getValueAtTime (delta, t3);
  if (dvalue != NUMundefined) myvalue3 += dvalue;
  RealTier_addPoint (thee, t3, myvalue3);

  if (t4 > t3)
  {

   while (mytime > lasttime && mytime < t4)
   {
    dvalue = RealTier_getValueAtTime (delta, mytime);
    if (dvalue != NUMundefined)
    {
     double fraction = 1 - (mytime - t3) / (openglottis_fadeFraction * openDuration);
     myvalue += dvalue * fraction;
    }
    RealTier_addPoint (thee, mytime, myvalue); lasttime=mytime; myindex++; if (myindex <= numberOfValues) { mypoint = me -> points -> item [myindex]; mytime = mypoint -> time; myvalue = mypoint -> value; } else mytime = me -> xmax;
   }


   double myvalue4 = RealTier_getValueAtTime (me, t4);
   RealTier_addPoint (thee, t4, myvalue4);
  }
 }
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}

static void check_formants (long numberOfFormants, long *ifb, long *ife)
{
 if (numberOfFormants <= 0 || *ifb > numberOfFormants || *ife < *ifb || *ife < 1)
 {
  *ife = 0;
  return;
 }
 if (*ifb <= 1) *ifb = 1;
 if (*ife > numberOfFormants) *ife = numberOfFormants;
 return;
}

static void synthesisParams_setCommonDefaults (synthesisParams p)
{
 p -> samplingFrequency = 44100;
}

static void synthesisParams_setDefaultsFricationGrid (synthesisParams p, FricationGrid thee)
{
 p -> endFricationFormant = thee -> formants -> formants -> size;
 p -> startFricationFormant = 2;
 p -> fricationBypass = 1;
}

static void synthesisParams_setDefaultsCouplingGrid (synthesisParams p, CouplingGrid thee)
{
 p -> openglottis_fadeFraction = 0.1;
 p -> endTrachealFormant = thee -> tracheal_formants -> formants -> size;
 p -> startTrachealFormant = 1;
 p -> endTrachealAntiFormant = thee -> tracheal_antiformants -> formants -> size;
 p -> startTrachealAntiFormant = 1;
}

static void synthesisParams_setDefaultsPhonationGrid (synthesisParams p)
{
 p -> maximumPeriod = 0;
 p -> voicing = p -> aspiration = p -> spectralTilt = 1;
}

static void synthesisParams_setDefaultsVocalTractGrid (synthesisParams p, VocalTractGrid thee)
{
 p -> filterModel = 0;
 p -> endFormant = thee -> formants -> formants -> size;
 p -> startFormant = 1;
 p -> endNasalFormant = thee -> nasal_formants -> formants -> size;
 p -> startNasalFormant = 1;
 p -> endNasalAntiFormant = thee -> nasal_antiformants -> formants -> size;
 p -> startNasalAntiFormant = 1;
}

static void synthesisParams_setDefault (synthesisParams p, KlattGrid thee)
{
 synthesisParams_setCommonDefaults (p);
 synthesisParams_setDefaultsPhonationGrid (p);

 synthesisParams_setDefaultsVocalTractGrid (p, thee -> vocalTract);

 synthesisParams_setDefaultsFricationGrid (p, thee -> frication);

 synthesisParams_setDefaultsCouplingGrid (p, thee -> coupling);
 p -> kg = &thee;
}

static struct synthesisParams synthesisParams_createDefault (KlattGrid me)
{
 struct synthesisParams p;
 synthesisParams_setDefault (&p, me);
 return p;
}

static Sound Sound_createEmptyMono (double xmin, double xmax, double samplingFrequency)
{
 long nt = ceil ((xmax - xmin) * samplingFrequency);
 double dt = 1.0 / samplingFrequency;
 double tmid = (xmin + xmax) / 2;
 double t1 = tmid - 0.5 * (nt - 1) * dt;

 return Sound_create (1, xmin, xmax, nt, dt, t1);
}

static void _Sounds_add_inline (Sound me, Sound thee)
{
 for (long i = 1; i <= me -> nx; i++)
 {
  me -> z[1][i] += thee -> z[1][i];
 }
}

static void _Sounds_addDifferentiated_inline (Sound me, Sound thee)
{
 double pval = 0, dx = me -> dx;
 for (long i = 1; i <= me -> nx; i++)
 {
  double val = thee -> z[1][i];
  me -> z[1][i] += (val - pval) / dx;
  pval = val;
 }
}

typedef struct connections { long numberOfConnections; double *x, *y;} *connections;

typedef struct drawingRelations {
 long nx;
 double dy;
 double *xw;
 connections in, out;
} *drawingRelations;

static void connections_free (connections me)
{
 if (me == ((void *)0)) return;
 NUMdvector_free (me -> x, 1);
 NUMdvector_free (me -> y, 1);
 _Melder_free ((void **) & (me));
}

static connections connections_create (long numberOfConnections)
{
 connections me = (connections) _Melder_malloc (sizeof (struct connections));
 if (me == ((void *)0)) return ((void *)0);
 me -> numberOfConnections = numberOfConnections;
 me -> x = NUMdvector (1, numberOfConnections);
 if (me -> x == ((void *)0)) goto end;
 me -> y = NUMdvector (1, numberOfConnections);
end:
 if (Melder_hasError ()) connections_free (me);
 return me;
}

static connections connections_createSimple (double x, double y)
{
 connections me = connections_create (1);
 if (me == ((void *)0)) return ((void *)0);
 me -> x[1] = x;
 me -> y[1] = y;
 return me;
}



static void NUMcircle_radial_intersection_sq (double x, double y, double r, double xp, double yp, double *xi, double *yi)
{
 double dx = xp - x, dy = yp - y;
 double d = sqrt (dx * dx + dy * dy);
 if (d > 0)
 {
  *xi = x + dx * r / d;
  *yi = y + dy * r / d;
 }
 else { *xi = *yi = NUMundefined; }
}

static void summer_draw (Graphics g, double x, double y, double r, int alternating)
{
 Graphics_setLineWidth (g, 2);
 Graphics_circle (g, x, y, r);
 double dy = 3 * r / 4;

 if (alternating) y += r / 4;
 Graphics_line (g, x, y + r / 2, x, y - r / 2);
 Graphics_line (g, x - r / 2, y, x + r / 2, y);
 if (alternating) Graphics_line (g, x - r / 2, y - dy , x + r / 2, y - dy);
}

static void _summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, int alternating, double horizontalFraction)
{
 summer_draw (g, x, y, r, alternating);

 for (long i = 1; i <= thee -> numberOfConnections; i++)
 {
  double xto, yto, xp = thee -> x[i], yp = thee -> y[i];
  if (horizontalFraction > 0)
  {
   double dx = x - xp;
   if (dx > 0)
   {
    xp += horizontalFraction * dx;
    Graphics_line (g, thee -> x[i], yp, xp, yp);
   }
  }
  NUMcircle_radial_intersection_sq (x, y, r, xp, yp, &xto, &yto);
  if (xto == NUMundefined || yto == NUMundefined) continue;
  if (arrow) Graphics_arrow (g, xp, yp, xto, yto);
  else Graphics_line (g, xp, yp, xto, yto);
 }
}

static void summer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction)
{
 _summer_drawConnections (g, x, y, r, thee, arrow, 0, horizontalFraction);
}

static void alternatingSummer_drawConnections (Graphics g, double x, double y, double r, connections thee, int arrow, double horizontalFraction)
{
 _summer_drawConnections (g, x, y, r, thee, arrow, 1, horizontalFraction);
}

static void draw_oneSection (Graphics g, double xmin, double xmax, double ymin, double ymax, wchar_t *line1, wchar_t *line2, wchar_t *line3)
{
 long numberOfTextLines = 0;
 Graphics_rectangle (g, xmin, xmax, ymin, ymax);
 if (line1 != ((void *)0)) numberOfTextLines++;
 if (line2 != ((void *)0)) numberOfTextLines++;
 if (line3 != ((void *)0)) numberOfTextLines++;
 double dy = (ymax - ymin) / (numberOfTextLines + 1);
 double x = (xmax + xmin) / 2;
 double y = ymax - dy;
 if (line1 != ((void *)0))
 {
  Graphics_text1 (g, x, y, line1);
  y -= dy;
 }
 if (line2 != ((void *)0))
 {
  Graphics_text1 (g, x, y, line2);
  y -= dy;
 }
 if (line3 != ((void *)0))
 {
  Graphics_text1 (g, x, y, line3);
 }
}
# 484 "KlattGrid.c"
static void _PhonationPoint_initialize (void *table); struct structPhonationPoint_Table theStructPhonationPoint = { _PhonationPoint_initialize, L"" "PhonationPoint", & theStructData, sizeof (struct structPhonationPoint) }; PhonationPoint_Table classPhonationPoint = & theStructPhonationPoint; static void _PhonationPoint_initialize (void *table) { PhonationPoint_Table us = (PhonationPoint_Table) table; if (! classData -> destroy) classData -> _initialize (classData); classData -> _initialize (us);
{
 us -> destroy = classPhonationPoint_destroy;
 us -> copy = classPhonationPoint_copy;
 us -> equal = classPhonationPoint_equal;
 us -> canWriteAsEncoding = classPhonationPoint_canWriteAsEncoding;
 us -> writeText = classPhonationPoint_writeText;
 us -> readText = classPhonationPoint_readText;
 us -> writeBinary = classPhonationPoint_writeBinary;
 us -> readBinary = classPhonationPoint_readBinary;
 us -> description = classPhonationPoint_description;
 }
}

PhonationPoint PhonationPoint_create (double time, double period, double openPhase, double collisionPhase, double te,
 double power1, double power2, double pulseScale)
{
 PhonationPoint me = (PhonationPoint) Thing_new ((void *) classPhonationPoint);
 if (me == ((void *)0)) return ((void *)0);
 me -> time = time; me -> period = period;
 me -> openPhase = openPhase;
 me -> collisionPhase = collisionPhase; me -> te = te;
 me -> power1 = power1; me -> power2 = power2;
 me -> pulseScale = pulseScale;
 return me;
}

static void _PhonationTier_initialize (void *table); struct structPhonationTier_Table theStructPhonationTier = { _PhonationTier_initialize, L"" "PhonationTier", & theStructFunction, sizeof (struct structPhonationTier) }; PhonationTier_Table classPhonationTier = & theStructPhonationTier; static void _PhonationTier_initialize (void *table) { PhonationTier_Table us = (PhonationTier_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classPhonationTier_destroy;
 us -> copy = classPhonationTier_copy;
 us -> equal = classPhonationTier_equal;
 us -> canWriteAsEncoding = classPhonationTier_canWriteAsEncoding;
 us -> writeText = classPhonationTier_writeText;
 us -> readText = classPhonationTier_readText;
 us -> writeBinary = classPhonationTier_writeBinary;
 us -> readBinary = classPhonationTier_readBinary;
 us -> description = classPhonationTier_description;
 }
}

PhonationTier PhonationTier_create (double tmin, double tmax)
{
 PhonationTier me = (PhonationTier) Thing_new ((void *) classPhonationTier);
 if (me == ((void *)0) || ! Function_init (me, tmin, tmax) ||
  ((me -> points = SortedSetOfDouble_create ()) == ((void *)0))) _Thing_forget ((Thing *) & (me));
 return me;
}




static void _PhonationGrid_initialize (void *table); struct structPhonationGrid_Table theStructPhonationGrid = { _PhonationGrid_initialize, L"" "PhonationGrid", & theStructFunction, sizeof (struct structPhonationGrid) }; PhonationGrid_Table classPhonationGrid = & theStructPhonationGrid; static void _PhonationGrid_initialize (void *table) { PhonationGrid_Table us = (PhonationGrid_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classPhonationGrid_destroy;
 us -> copy = classPhonationGrid_copy;
 us -> equal = classPhonationGrid_equal;
 us -> canWriteAsEncoding = classPhonationGrid_canWriteAsEncoding;
 us -> writeText = classPhonationGrid_writeText;
 us -> readText = classPhonationGrid_readText;
 us -> writeBinary = classPhonationGrid_writeBinary;
 us -> readBinary = classPhonationGrid_readBinary;
 us -> description = classPhonationGrid_description;
 }
}

PhonationGrid PhonationGrid_create (double tmin, double tmax)
{
 PhonationGrid me = (PhonationGrid) Thing_new ((void *) classPhonationGrid);

 if (me == ((void *)0) || ! Function_init (me, tmin, tmax)) goto end;

 if (((me -> pitch = PitchTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> voicingAmplitude = IntensityTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> openPhase = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> collisionPhase = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> power1 = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> power2 = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> flutter = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> doublePulsing = RealTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> spectralTilt = IntensityTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> aspirationAmplitude = IntensityTier_create (tmin, tmax)) == ((void *)0))) goto end;
 me -> breathinessAmplitude = IntensityTier_create (tmin, tmax);
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}

static void PhonationGrid_draw_inside (PhonationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout)
{

 (void) me;
 double xw[6] = { 0, 1, 0.5, 1, 0.5, 0.5 }, xws[6];
 double x1, y1, x2, y2, xs, ys, ymid, r;
 int arrow = 1;

 connections thee = connections_create (2);
 if (thee == ((void *)0)) return;

 rel_to_abs (xw, xws, 5, xmax - xmin);

 dy = (ymax - ymin) / (1 + (dy < 0 ? 0 : dy) + 1);

 x1 = xmin; x2 = x1 + xw[1];
 y2 = ymax; y1 = y2 - dy;
 draw_oneSection (g, x1, x2, y1, y2, ((void *)0), L"Voicing", ((void *)0));

 x1 = x2; x2 = x1 + xw[2]; ymid = (y1 + y2) / 2;
 Graphics_line (g, x1, ymid, x2, ymid);

 x1 = x2; x2 = x1 + xw[3];
 draw_oneSection (g, x1, x2, y1, y2, ((void *)0), L"Tilt", ((void *)0));

 thee -> x[1] = x2; thee -> y[1] = ymid;

 y2 = y1 - 0.5 * dy; y1 = y2 - dy; ymid = (y1 + y2) / 2;
 x2 = xmin + xws[3]; x1 = x2 - 1.5 * xw[3];
 draw_oneSection (g, x1, x2, y1, y2, ((void *)0), L"Aspiration", ((void *)0));

 thee -> x[2] = x2; thee -> y[2] = ymid;

 r = xw[5] / 2;
 xs = xmax - r; ys = (ymax + ymin) / 2;

 summer_drawConnections (g, xs, ys, r, thee, arrow, 0.4);
 connections_free (thee);

 if (yout != ((void *)0)) *yout = ys;
}

void PhonationGrid_draw (PhonationGrid me, Graphics g)
{
 double xmin = 0, xmax2 = 0.9, xmax = 1, ymin = 0, ymax = 1, dy = 0.5, yout;

 Graphics_setInner (g);
 Graphics_setWindow (g, xmin, xmax, ymin, ymax);

 PhonationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);
 Graphics_arrow (g, xmax2, yout, xmax, yout);
 Graphics_unsetInner (g);
}

double PhonationGrid_getMaximumPeriod (PhonationGrid me)
{
 double minimumPitch = RealTier_getMinimumValue (me -> pitch);
 return 2 / ((minimumPitch == NUMundefined || minimumPitch == 0) ? (me -> xmax - me -> xmin) : minimumPitch);
}

PointProcess PitchTier_to_PointProcess_flutter (PitchTier pitch, RealTier flutter, double maximumPeriod)
{
 PointProcess thee = PitchTier_to_PointProcess (pitch);
 if (thee == ((void *)0)) return ((void *)0);
 double tsum = 0;
 for (long it = 2; it <= thee -> nt; it++)
 {
  double t = thee -> t[it - 1];
  double period = thee -> t[it] - thee -> t[it - 1];
  if (period < maximumPeriod && flutter -> points -> size > 0)
  {
   double fltr = RealTier_getValueAtTime (flutter, t);
   if (fltr != NUMundefined)
   {

    double newPeriod = period / (1 + (fltr / 50) * (sin (2*NUMpi*12.7*t) + sin (2*NUMpi*7.1*t) + sin (2*NUMpi*4.7*t)));
    tsum += newPeriod - period;
   }
  }
  thee -> t[it] += tsum;
 }
 return thee;
}

static Sound PhonationGrid_to_Sound_aspiration (PhonationGrid me, synthesisParams p)
{
 double lastval = 0;

 Sound thee = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
 if (thee == ((void *)0)) return ((void *)0);



 if (me -> aspirationAmplitude -> points -> size > 0)
 {
  for (long i = 1; i <= thee -> nx; i++)
  {
   double t = thee -> x1 + (i - 1) * thee -> dx;
   double val = NUMrandomUniform (-1, 1);
   double a = (pow (10.0, RealTier_getValueAtTime (me -> aspirationAmplitude, t) / 20.0) * 2.0e-5);
   if (a != NUMundefined)
   {
    thee -> z[1][i] = lastval = val + 0.75 * lastval;
    lastval = (val += 0.75 * lastval);
    thee -> z[1][i] = val * a;
   }
  }
 }
 return thee;
}

static void Sound_PhonationGrid_spectralTilt_inline (Sound thee, PhonationGrid me)
{
 if (me -> spectralTilt -> points -> size > 0)
 {
# 697 "KlattGrid.c"
  double cosf = cos (2 * NUMpi * 3000 * thee -> dx), ynm1 = 0;

  for (long i = 1; i <= thee -> nx; i++)
  {
   double t = thee -> x1 + (i - 1) * thee -> dx;
   double tilt_db = RealTier_getValueAtTime (me -> spectralTilt, t);

   if (tilt_db > 0)
   {
    double d = pow (10, -tilt_db / 10);
    double q = (1 - d * cosf) / (1 - d);
    double b = q - sqrt (q * q - 1);
    double a = 1 - b;
    thee -> z[1][i] = a * thee -> z[1][i] + b * ynm1;
    ynm1 = thee -> z[1][i];
   }
  }
 }
}

struct nrfunction_struct { double n; double m; double a;};
static void nrfunction (double x, double *fx, double *dfx, void *closure)
{
 struct nrfunction_struct *nrfs = (struct nrfunction_struct *) closure;
 double mplusax = nrfs -> m + nrfs -> a * x;
 double mminn = nrfs -> m - nrfs -> n;
 *fx = pow (x, mminn) - (nrfs -> n + nrfs -> a * x) / mplusax;
 *dfx = mminn * pow (x,mminn-1) - nrfs -> a * mminn / (mplusax * mplusax);
}

static double get_collisionPoint_x (double n, double m, double collisionPhase)
{
 double y = NUMundefined;
# 742 "KlattGrid.c"
 if (collisionPhase <= 0) return 1;
 double a = 1 / collisionPhase;
 if (m - n == 1)
 {
  double b = m - a;
  double c = - n, y1, y2;
  int nroots = gsl_poly_solve_quadratic (a, b, c, &y1, &y2);
  if (nroots == 2) y = y2;
  else if (nroots == 1) y = y1;
 }
 else
 {

  struct nrfunction_struct nrfs = {n, m, a};
  double root, xmaxFlow = pow (n / m, 1.0 / (m - n));
  if (NUMnrbis (&nrfunction, xmaxFlow, 1, &nrfs, &root)) y = root;
 }
 return y;
}

PhonationTier PhonationGrid_to_PhonationTier (PhonationGrid me, synthesisParams p)
{
 long diplophonicPulseIndex = 0;
 double openPhase_default = 0.7, power1_default = 2;

 if (me -> pitch -> points -> size == 0) return Melder_errorp1 (L"Pitch tier is empty.");

 if (p -> maximumPeriod == 0) p -> maximumPeriod = PhonationGrid_getMaximumPeriod (me);

 PointProcess point = PitchTier_to_PointProcess_flutter (me -> pitch, me -> flutter, p -> maximumPeriod);
 if (point == ((void *)0)) goto end;

 PhonationTier thee = PhonationTier_create (me -> xmin, me -> xmax);
 if (thee == ((void *)0)) return ((void *)0);
# 786 "KlattGrid.c"
 for (long it = 1; it <= point -> nt; it++)
 {
  double te, t = point -> t[it];
  double period = NUMundefined;
  double pulseDelay = 0;
  double pulseScale = 1;

  period = PointProcess_getPeriodAtIndex (point, it, p -> maximumPeriod);
  if (period == NUMundefined)
  {
   period = 0.5 * p -> maximumPeriod;
  }




  double periodStart = t - period;
  double collisionPhase, power1, power2, re;

  if (p -> klatt80)
  {
   collisionPhase = 0;
   power1 = 2;
   power2 = 3;
   re = 1;
  }
  else
  {
   collisionPhase = RealTier_getValueAtTime (me -> collisionPhase, periodStart);
   if (collisionPhase == NUMundefined) collisionPhase = 0;
   power1 = RealTier_getValueAtTime (me -> power1, periodStart);
   if (power1 == NUMundefined) power1 = power1_default;
   power2 = RealTier_getValueAtTime (me -> power2, periodStart);
   if (power2 == NUMundefined) power2 = power1_default + 1;

   re = get_collisionPoint_x (power1, power2, collisionPhase);
   if (re == NUMundefined)
   {
    Melder_warning9 (L"Illegal collision point at t = ", Melder_double (t), L" (power1=", Melder_double(power1), L", power2=", Melder_double(power2), L"colPhase=", Melder_double(collisionPhase), L")");
   }
  }

  double openPhase = RealTier_getValueAtTime (me -> openPhase, periodStart);
  if (openPhase == NUMundefined) openPhase = openPhase_default;

  te = re * period * openPhase;
# 840 "KlattGrid.c"
  double doublePulsing = RealTier_getValueAtTime (me -> doublePulsing, periodStart);
  if (doublePulsing == NUMundefined) doublePulsing = 0;

  if (doublePulsing > 0)
  {
   diplophonicPulseIndex++;
   if (diplophonicPulseIndex % 2 == 1)
   {
    double nextPeriod = PointProcess_getPeriodAtIndex (point, it+1, p -> maximumPeriod);
    if (nextPeriod == NUMundefined) nextPeriod = period;
    double openPhase2 = openPhase_default;
    if (me -> openPhase -> points -> size > 0) openPhase2 = RealTier_getValueAtTime (me -> openPhase, t);
    double maxDelay = period * openPhase2;
    pulseDelay = maxDelay * doublePulsing;
    pulseScale *= (1 - doublePulsing);
   }
  }
  else
  {
   diplophonicPulseIndex = 0;
  }

  t += pulseDelay;
  PhonationPoint phonationPoint = PhonationPoint_create (t, period, openPhase, collisionPhase, te, power1, power2, pulseScale);
  if (phonationPoint == ((void *)0) || ! AnyTier_addPoint (thee, phonationPoint)) goto end;
 }

end:
 _Thing_forget ((Thing *) & (point));
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}


static Sound PhonationGrid_to_Sound_voiced (PhonationGrid me, synthesisParams p, PhonationTier *phonTier)
{
 Sound thee = ((void *)0), breathy = ((void *)0);
 long i;
 double lastVal = NUMundefined;

 if (me -> voicingAmplitude -> points -> size == 0) return Melder_errorp1 (L"Amplitude of voicing tier is empty.");

 PhonationTier phTier = PhonationGrid_to_PhonationTier (me, p);
 if (phTier == ((void *)0)) return ((void *)0);

 thee = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
 if (thee == ((void *)0)) goto end;

 if (me -> breathinessAmplitude -> points -> size > 0)
 {
  breathy = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
  if (breathy == ((void *)0)) goto end;
 }
# 902 "KlattGrid.c"
 double *sound = thee -> z[1];
 for (long it = 1; it <= phTier -> points -> size; it++)
 {
  PhonationPoint point = phTier -> points ->item[it];
  double t = point -> time;
  double te = point -> te;
  double period = point -> period;
  double openPhase = point -> openPhase;
  double collisionPhase = point -> collisionPhase;
  double pulseScale = point -> pulseScale;
  double power1 = point -> power1, power2 = point -> power2;
  double phase;
  double flow;
# 924 "KlattGrid.c"
  double amplitude = pulseScale / (pow (power1 / power2, 1 / (power2 / power1 - 1)) * (1 - power1 / power2));



  long midSample = Sampled_xToLowIndex (thee, t), beginSample;
  beginSample = midSample - floor (te / thee -> dx);
  if (beginSample < 1) beginSample = 0;
  for (i = beginSample; i <= midSample; i++)
  {
   double tsamp = thee -> x1 + (i - 1) * thee -> dx;
   phase = (tsamp - (t - te)) / (period * openPhase);
   if (phase > 0.0)
   {
    flow = amplitude * (pow (phase, power1) - pow (phase, power2));
    if (i == 0) { lastVal = flow; continue; }
    sound[i] += flow;


    if (breathy)
    {
     double val = flow * NUMrandomUniform (-1, 1);
     double a = RealTier_getValueAtTime (me -> breathinessAmplitude, t);
     breathy -> z[1][i] += val * (pow (10.0, a / 20.0) * 2.0e-5);
    }
   }
  }



  phase = te / (period * openPhase);



  flow = amplitude * (pow (phase, power1) - pow (phase, power2));



  if (flow > 0.0)
  {
   double ta = collisionPhase * (period * openPhase);
   double factorPerSample = exp (- thee -> dx / ta);
   double value = flow * exp (-(thee -> x1 + midSample * thee -> dx - t) / ta);
   long endSample = midSample + floor (20 * ta / thee -> dx);
   if (endSample > thee -> nx) endSample = thee -> nx;
   for (i = midSample + 1; i <= endSample; i++)
   {
    sound[i] += value;
    value *= factorPerSample;
   }
  }
 }


 if (p -> sourceIsFlowDerivative)
 {
  double extremum = Vector_getAbsoluteExtremum (thee, 0, 0, Vector_VALUE_INTERPOLATION_CUBIC);
  if (! NUMdefined (lastVal)) lastVal = 0;
  for (i = 1; i <= thee -> nx; i++)
  {
   double val = thee -> z[1][i];
   thee -> z[1][i] -= lastVal;
   lastVal = val;
  }
  Vector_scale (thee, extremum);
 }

 for (i = 1; i <= thee -> nx; i++)
 {
  double t = thee -> x1 + (i - 1) * thee -> dx;
  thee -> z[1][i] *= (pow (10.0, RealTier_getValueAtTime (me -> voicingAmplitude, t) / 20.0) * 2.0e-5);
  if (breathy) thee -> z[1][i] += breathy -> z[1][i];
 }

end:
 _Thing_forget ((Thing *) & (breathy));
 if (Melder_hasError ()) { _Thing_forget ((Thing *) & (thee)); _Thing_forget ((Thing *) & (phTier)); return ((void *)0); }
 if (phonTier == ((void *)0)) _Thing_forget ((Thing *) & (phTier));
 else *phonTier = phTier;
 return thee;
}

static Sound PhonationGrid_to_Sound (PhonationGrid me, synthesisParams p, CouplingGrid him)
{
 Sound thee = ((void *)0);
 if (p -> voicing)
 {
  _Thing_forget ((Thing *) & (him -> glottis));
  thee = PhonationGrid_to_Sound_voiced (me, p, &(him -> glottis));
  if (thee == ((void *)0)) return ((void *)0);
  if (p -> spectralTilt) Sound_PhonationGrid_spectralTilt_inline (thee, me);
 }
 if (p -> aspiration)
 {
  Sound aspiration = PhonationGrid_to_Sound_aspiration (me, p);
  if (aspiration == ((void *)0)) goto end;
  if (thee == ((void *)0)) thee = aspiration;
  else
  {
   _Sounds_add_inline (thee, aspiration);
   _Thing_forget ((Thing *) & (aspiration));
  }
 }
 if (thee == ((void *)0)) thee = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}

static Ordered formantsAmplitudes_create (double tmin, double tmax, long numberOfFormants)
{
 Ordered me = Ordered_create ();
 if (me == ((void *)0)) return ((void *)0);
 for (long i = 1; i <= numberOfFormants; i++)
 {
  IntensityTier a = IntensityTier_create (tmin, tmax);
  if (a == ((void *)0) || ! Collection_addItem (me, a)) goto end;
 }
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}



static void _VocalTractGrid_initialize (void *table); struct structVocalTractGrid_Table theStructVocalTractGrid = { _VocalTractGrid_initialize, L"" "VocalTractGrid", & theStructFunction, sizeof (struct structVocalTractGrid) }; VocalTractGrid_Table classVocalTractGrid = & theStructVocalTractGrid; static void _VocalTractGrid_initialize (void *table) { VocalTractGrid_Table us = (VocalTractGrid_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classVocalTractGrid_destroy;
 us -> copy = classVocalTractGrid_copy;
 us -> equal = classVocalTractGrid_equal;
 us -> canWriteAsEncoding = classVocalTractGrid_canWriteAsEncoding;
 us -> writeText = classVocalTractGrid_writeText;
 us -> readText = classVocalTractGrid_readText;
 us -> writeBinary = classVocalTractGrid_writeBinary;
 us -> readBinary = classVocalTractGrid_readBinary;
 us -> description = classVocalTractGrid_description;
 }
}

VocalTractGrid VocalTractGrid_create (double tmin, double tmax, long numberOfFormants,
 long numberOfNasalFormants, long numberOfNasalAntiFormants)
{
 VocalTractGrid me = (VocalTractGrid) Thing_new ((void *) classVocalTractGrid);

 if (me == ((void *)0) || ! Function_init (me, tmin, tmax)) goto end;

 if (((me -> formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants)) == ((void *)0)) ||
  ((me -> nasal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalFormants)) == ((void *)0)) ||
  ((me -> nasal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfNasalAntiFormants)) == ((void *)0))) goto end;

 if ((me -> formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants)) == ((void *)0)) goto end;

 me -> nasal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfNasalFormants);

end:

 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}

static void VocalTractGrid_CouplingGrid_drawCascade_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double *yin, double *yout)
{
 long numberOfNormalFormants = me -> formants -> formants -> size;
 long numberOfNasalFormants = me -> nasal_formants -> formants -> size;
 long numberOfNasalAntiFormants = me -> nasal_antiformants -> formants -> size;
 long numberOfTrachealFormants = thee != ((void *)0) ? thee -> tracheal_formants -> formants -> size : 0;
 long numberOfTrachealAntiFormants = thee != ((void *)0) ? thee -> tracheal_antiformants -> formants -> size : 0;
  double x1, y1 = ymin, x2, y2 = ymax, dx, ddx = 0.2, ymid = (y1 + y2) / 2;
  wchar_t *text[6] = { 0, L"NF", L"NAF", L"TF", L"TAF", L""};
  long nf[6] = {0, numberOfNasalFormants, numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfNormalFormants};
 long numberOfFilters, numberOfXSections = 5, nsx = 0, isection, i;
 MelderString ff = { 0 }, fb = { 0 };

 numberOfFilters = numberOfNasalFormants + numberOfNasalAntiFormants + numberOfTrachealFormants + numberOfTrachealAntiFormants + numberOfNormalFormants;

 if (numberOfFilters == 0)
 {
  x2 = xmax;
  Graphics_line (g, xmin, ymid, x2, ymid);
  goto end;
 }

 for (isection = 1; isection <= numberOfXSections; isection++) if (nf[isection] > 0) nsx++;
 dx = (xmax - xmin) / (numberOfFilters + (nsx - 1) * ddx);

 x1 = xmin;
 for (isection = 1; isection <= numberOfXSections; isection++)
 {
  long numberOfFormants = nf[isection];

  if (numberOfFormants == 0) continue;

  x2 = x1 + dx;
  for (i = 1; i <= numberOfFormants; i++)
  {
   MelderString_append2 (&ff, L"F", Melder_integer (i));
   MelderString_append2 (&fb, L"B", Melder_integer (i));
   draw_oneSection (g, x1, x2, y1, y2, text[isection], ff.string, fb.string);
   if (i < numberOfFormants) { x1 = x2; x2 = x1 + dx; }
   MelderString_empty (&ff); MelderString_empty (&fb);
  }

  if (isection < numberOfXSections)
  {
   x1 = x2; x2 = x1 + ddx * dx;
   Graphics_line (g, x1, ymid, x2, ymid);
   x1 = x2;
  }
 }
end:
 if (yin != ((void *)0)) *yin = ymid;
 if (yout != ((void *)0)) *yout = ymid;

 MelderString_free (&ff); MelderString_free (&fb);
}

static void VocalTractGrid_CouplingGrid_drawParallel_inline (VocalTractGrid me, CouplingGrid thee, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout)
{


 double xw[9] = { 0, 0.3, 0.2, 1, 0.5, 0.5, 1, 0.5, 0.5 }, xws[9];
 long i, isection, numberOfXSections = 8, ic = 0, numberOfYSections = 4;
 long numberOfNasalFormants = me -> nasal_formants -> formants -> size;
 long numberOfNormalFormants = me -> formants -> formants -> size;
 long numberOfTrachealFormants = thee != ((void *)0) ? thee -> tracheal_formants -> formants -> size : 0;
 long numberOfFormants = numberOfNasalFormants + numberOfNormalFormants + numberOfTrachealFormants;
 long numberOfUpperPartFormants = numberOfNasalFormants + (numberOfNormalFormants > 0 ? 1 : 0);
 long numberOfLowerPartFormants = numberOfFormants - numberOfUpperPartFormants;
 double ddy = dy < 0 ? 0 : dy, x1, y1, x2, y2, x3, r, ymid;
  wchar_t *text[5] = { 0, L"Nasal", L"", L"", L"Tracheal"};
  long nffrom[5] = {0, 1, 1, 2, 1};
  long nfto[5] = {0, numberOfNasalFormants, (numberOfNormalFormants > 0 ? 1 : 0), numberOfNormalFormants, numberOfTrachealFormants};
 MelderString fba = { 0 };

 rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

 if (numberOfFormants == 0)
 {
  y1 = y2 = (ymin + ymax) / 2;
  Graphics_line (g, xmin, y1, xmax, y1);
  goto end;
 }

 dy = (ymax - ymin) / (numberOfFormants * (1 + ddy) - ddy);

 connections local_in = connections_create (numberOfFormants);
 if (local_in == ((void *)0)) return;
 connections local_out = connections_create (numberOfFormants);
 if (local_out == ((void *)0)) goto end;


 x1 = xmin + xws[5]; x2 = x1 + xw[6]; y2 = ymax;
 x3 = xmin + xws[4];
 for (isection = 1; isection <= numberOfYSections; isection++)
 {
  long ifrom = nffrom[isection], ito = nfto[isection];
  if (ito < ifrom) continue;
  for (i = ifrom; i <= ito; i++)
  {
   y1 = y2 - dy; ymid = (y1 + y2) / 2;
   const wchar_t *fi = Melder_integer (i);
   MelderString_append6 (&fba, L"A", fi, L" F", fi, L" B", fi);
   draw_oneSection (g, x1, x2, y1, y2, text[isection], fba.string, ((void *)0));
   Graphics_line (g, x3, ymid, x1, ymid);
   ic++;
   local_in -> x[ic] = x3; local_out -> x[ic] = x2;
   local_in -> y[ic] = local_out -> y[ic] = ymid;
   y2 = y1 - 0.5 * dy;
   MelderString_empty (&fba);
  }
 }

 ic = 0;
 x1 = local_in -> y[1];
 if (numberOfUpperPartFormants > 0)
 {
  x1 = local_in -> x[numberOfUpperPartFormants]; y1 = local_in -> y[numberOfUpperPartFormants];
  if (numberOfUpperPartFormants > 1) Graphics_line (g, x1, y1, local_in -> x[1], local_in -> y[1]);
  x2 = xmin;
  if (numberOfLowerPartFormants > 0) { x2 += xw[1]; }
  Graphics_line (g, x1, y1, x2, y1);
 }
 if (numberOfLowerPartFormants > 0)
 {
  long ifrom = numberOfUpperPartFormants + 1;
  x1 = local_in -> x[ifrom]; y1 = local_in -> y[ifrom];
  if (numberOfLowerPartFormants > 1) Graphics_line (g, x1, y1, local_in -> x[numberOfFormants], local_in -> y[numberOfFormants]);
  x2 = xmin + xws[3];
  Graphics_line (g, x1, y1, x2, y1);
  x1 = xmin + xws[2];
  draw_oneSection (g, x1, x2, y1 + 0.5 * dy, y1 - 0.5 * dy, L"Pre-emphasis", ((void *)0), ((void *)0));
  x2 = x1;
  if (numberOfUpperPartFormants > 0)
  {
   x2 = xmin + xw[1]; y2 = y1;
   Graphics_line (g, x1, y1, x2, y1);
   y1 += (1 + ddy) * dy;
   Graphics_line (g, x2, y2, x2, y1);
   y1 -= 0.5 * (1 + ddy) * dy;
  }
  Graphics_line (g, xmin, y1, x2, y1);
 }

 r = xw[8] / 2;
 x2 = xmax - r; y2 = (ymin + ymax) / 2;

 alternatingSummer_drawConnections (g, x2, y2, r, local_out, 1, 0.4);

end:

 connections_free (local_out); connections_free (local_in);

 if (yin != ((void *)0)) *yin = y1;
 if (yout != ((void *)0)) *yout = y2;

 MelderString_free (&fba);
}

static void VocalTractGrid_CouplingGrid_draw_inside (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel, double xmin, double xmax, double ymin, double ymax, double dy, double *yin, double *yout)
{
 filterModel == 0 ? VocalTractGrid_CouplingGrid_drawCascade_inline (me, thee, g, xmin, xmax, ymin, ymax, yin, yout) :
  VocalTractGrid_CouplingGrid_drawParallel_inline (me, thee, g, xmin, xmax, ymin, ymax, dy, yin, yout);
}

static void VocalTractGrid_CouplingGrid_draw (VocalTractGrid me, CouplingGrid thee, Graphics g, int filterModel)
{
 double xmin = 0, xmin1 = 0.05, xmax1 = 0.95, xmax = 1, ymin = 0, ymax = 1, dy = 0.5, yin, yout;

 Graphics_setInner (g);
 Graphics_setWindow (g, xmin, xmax, ymin, ymax);
 Graphics_setTextAlignment (g, kGraphics_horizontalAlignment_CENTRE, 1);
 Graphics_setLineWidth (g, 2);
 VocalTractGrid_CouplingGrid_draw_inside (me, thee, g, filterModel, xmin1, xmax1, ymin, ymax, dy, &yin, &yout);
 Graphics_line (g, xmin, yin, xmin1, yin);
 Graphics_arrow (g, xmax1, yout, xmax, yout);
 Graphics_unsetInner (g);
}

static Sound Sound_VocalTractGrid_CouplingGrid_filter_cascade (Sound me, VocalTractGrid thee, CouplingGrid coupling, synthesisParams params)
{
 struct synthesisParams p; if (params == ((void *)0)) { synthesisParams_setDefaultsVocalTractGrid (&p, thee); synthesisParams_setDefaultsCouplingGrid (&p, coupling);} else p = *params;
 FormantGrid formants = ((void *)0);
 FormantGrid normal_formants = thee -> formants;
 FormantGrid nasal_formants = thee -> nasal_formants;
 FormantGrid nasal_antiformants = thee -> nasal_antiformants;
 FormantGrid tracheal_formants = coupling -> tracheal_formants;
 FormantGrid tracheal_antiformants = coupling -> tracheal_antiformants;

 long iformant; int antiformants = 0;
 long numberOfFormants = p.klatt80 ? ((5) < (normal_formants -> formants -> size) ? (5) : (normal_formants -> formants -> size)) : normal_formants -> formants -> size;
 long numberOfTrachealFormants = p.klatt80 ? ((1) < (tracheal_formants -> formants -> size) ? (1) : (tracheal_formants -> formants -> size)) : tracheal_formants -> formants -> size;
 long numberOfTrachealAntiFormants = p.klatt80 ? ((1) < (tracheal_antiformants -> formants -> size) ? (1) : (tracheal_antiformants -> formants -> size)) : tracheal_antiformants -> formants -> size;
 long numberOfNasalFormants = p.klatt80 ? ((1) < (nasal_formants -> formants -> size) ? (1) : (nasal_formants -> formants -> size)) : nasal_formants -> formants -> size;
 long numberOfNasalAntiFormants = p.klatt80 ? ((1) < (nasal_antiformants -> formants -> size) ? (1) : (nasal_antiformants -> formants -> size)) : nasal_antiformants -> formants -> size;
 check_formants (numberOfFormants, & p.startFormant, & p.endFormant);
 check_formants (numberOfNasalFormants, &p.startNasalFormant, & p.endNasalFormant);
 check_formants (numberOfTrachealFormants, & p.startTrachealFormant, & p.endTrachealFormant);
 check_formants (numberOfNasalAntiFormants, & p.startNasalAntiFormant, & p.endNasalAntiFormant);
 check_formants (numberOfTrachealAntiFormants, & p.startTrachealAntiFormant, & p.endTrachealAntiFormant);

 Sound him = Data_copy (me);
 if (him == ((void *)0)) return ((void *)0);

 if (coupling -> glottis && coupling -> glottis -> points -> size > 0)
 {
  formants = Data_copy (thee -> formants);
  if (formants == ((void *)0) || ! FormantGrid_CouplingGrid_updateOpenPhases (formants, coupling, p.openglottis_fadeFraction)) goto end;
 }

 if (p.endNasalFormant > 0)
 {
  antiformants = 0;
  for (iformant = p.startNasalFormant; iformant <= p.endNasalFormant; iformant++)
  {
   _Sound_FormantGrid_filterWithOneFormant_inline (him, thee -> nasal_formants, iformant, antiformants);
  }
 }

 if (p.endNasalAntiFormant > 0)
 {
  antiformants = 1;
  for (iformant = p.startNasalAntiFormant; iformant <= p.endNasalAntiFormant; iformant++)
  {
   _Sound_FormantGrid_filterWithOneFormant_inline (him, thee -> nasal_antiformants, iformant, antiformants);
  }
 }

 if (p.endTrachealFormant > 0)
 {
  antiformants = 0;
  for (iformant = p.startTrachealFormant; iformant <= p.endTrachealFormant; iformant++)
  {
   _Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_formants, iformant, antiformants);
  }
 }

 if (p.endTrachealAntiFormant > 0)
 {
  antiformants = 1;
  for (iformant = p.startTrachealAntiFormant; iformant <= p.endTrachealAntiFormant; iformant++)
  {
   _Sound_FormantGrid_filterWithOneFormant_inline (him, tracheal_antiformants, iformant, antiformants);
  }
 }

 if (p.endFormant > 0)
 {
  antiformants = 0;
  if (formants == ((void *)0)) formants = thee -> formants;
  for (iformant = p.startFormant; iformant <= p.endFormant; iformant++)
  {
   _Sound_FormantGrid_filterWithOneFormant_inline (him, formants, iformant, antiformants);
  }
 }
 if (coupling -> glottis && coupling -> glottis -> points -> size > 0) _Thing_forget ((Thing *) & (formants));
end:
 return him;
}

Sound Sound_VocalTractGrid_CouplingGrid_filter_parallel (Sound me, VocalTractGrid thee, CouplingGrid coupling, synthesisParams params)
{
 Sound f1 = ((void *)0), vocalTract = ((void *)0), trachea = ((void *)0), nasal = ((void *)0), him = ((void *)0);
 struct synthesisParams p; if (params == ((void *)0)) { synthesisParams_setDefaultsVocalTractGrid (&p, thee); synthesisParams_setDefaultsCouplingGrid (&p, coupling);} else p = *params;
 FormantGrid formants = thee -> formants;
 int alternatingSign = 0;
 long numberOfFormants = p.klatt80 ? ((4) < (thee -> formants -> formants -> size) ? (4) : (thee -> formants -> formants -> size)) : thee -> formants -> formants -> size;
 long numberOfNasalFormants = p.klatt80 ? ((1) < (thee -> nasal_formants -> formants -> size) ? (1) : (thee -> nasal_formants -> formants -> size)) : thee -> nasal_formants -> formants -> size;
 long numberOfTrachealFormants = p.klatt80 ? ((1) < (coupling -> tracheal_formants -> formants -> size) ? (1) : (coupling -> tracheal_formants -> formants -> size)) : coupling ->tracheal_formants -> formants -> size;

 check_formants (numberOfFormants, &(p.startFormant), &(p.endFormant));
 check_formants (numberOfNasalFormants, &(p.startNasalFormant), &(p.endNasalFormant));
 check_formants (numberOfTrachealFormants, &(p.startTrachealFormant), &(p.endTrachealFormant));

 if (coupling -> glottis)
 {
  formants = Data_copy (thee -> formants);
  if (formants == ((void *)0) || ! FormantGrid_CouplingGrid_updateOpenPhases (formants, coupling, p.openglottis_fadeFraction)) goto end;
 }

 if (p.endFormant > 0)
 {
  if (p.startFormant == 1)
  {
   him = Data_copy (me);
   if (him == ((void *)0)) return ((void *)0);
   if (formants -> formants -> size > 0 &&
    ! Sound_FormantGrid_Intensities_filterWithOneFormant_inline (him, formants, thee -> formants_amplitudes, 1)) goto end;
  }
 }

 if (p.endNasalFormant > 0)
 {
  alternatingSign = 0;
  nasal = Sound_FormantGrid_Intensities_filter (me, thee -> nasal_formants, thee -> nasal_formants_amplitudes, p.startNasalFormant, p.endNasalFormant, alternatingSign);
  if (nasal == ((void *)0)) goto end;

  if (him == ((void *)0))
  {
   him = Data_copy (nasal);
   if (him == ((void *)0)) goto end;
  }
  else _Sounds_add_inline (him, nasal);
 }
# 1397 "KlattGrid.c"
 if (p.endFormant >= 2)
 {
  long startFormant2 = p.startFormant > 2 ? p.startFormant : 2;
  alternatingSign = startFormant2 % 2 == 0 ? -1 : 1;
  if (startFormant2 <= formants -> formants -> size)
  {
   vocalTract = Sound_FormantGrid_Intensities_filter (me, formants, thee -> formants_amplitudes, startFormant2, p.endFormant, alternatingSign);
   if (vocalTract == ((void *)0)) goto end;

   if (him == ((void *)0))
   {
    him = Data_copy (vocalTract);
    if (him == ((void *)0)) goto end;
   }
   else _Sounds_addDifferentiated_inline (him, vocalTract);
  }
 }

 if (p.endTrachealFormant > 0)
 {
  alternatingSign = 0;
  trachea = Sound_FormantGrid_Intensities_filter (me, coupling -> tracheal_formants, coupling -> tracheal_formants_amplitudes,
    p.startTrachealFormant, p.endTrachealFormant, alternatingSign);
  if (trachea == ((void *)0)) goto end;

  if (him == ((void *)0))
  {
   him = Data_copy (trachea);
   if (him == ((void *)0)) goto end;
  }
  else _Sounds_addDifferentiated_inline (him, trachea);
 }

 if (him == ((void *)0)) him = Data_copy (me);

end:
 _Thing_forget ((Thing *) & (vocalTract)); _Thing_forget ((Thing *) & (f1)); _Thing_forget ((Thing *) & (trachea)); _Thing_forget ((Thing *) & (nasal));
 if (coupling -> glottis) _Thing_forget ((Thing *) & (formants));
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (him));
 return him;
}

Sound Sound_VocalTractGrid_CouplingGrid_filter (Sound me, VocalTractGrid thee, CouplingGrid coupling, synthesisParams p)
{
 return p -> filterModel == 0 ? Sound_VocalTractGrid_CouplingGrid_filter_cascade (me, thee, coupling, p) :
  Sound_VocalTractGrid_CouplingGrid_filter_parallel (me, thee, coupling, p);
}



static void _CouplingGrid_initialize (void *table); struct structCouplingGrid_Table theStructCouplingGrid = { _CouplingGrid_initialize, L"" "CouplingGrid", & theStructFunction, sizeof (struct structCouplingGrid) }; CouplingGrid_Table classCouplingGrid = & theStructCouplingGrid; static void _CouplingGrid_initialize (void *table) { CouplingGrid_Table us = (CouplingGrid_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classCouplingGrid_destroy;
 us -> copy = classCouplingGrid_copy;
 us -> equal = classCouplingGrid_equal;
 us -> canWriteAsEncoding = classCouplingGrid_canWriteAsEncoding;
 us -> writeText = classCouplingGrid_writeText;
 us -> readText = classCouplingGrid_readText;
 us -> writeBinary = classCouplingGrid_writeBinary;
 us -> readBinary = classCouplingGrid_readBinary;
 us -> description = classCouplingGrid_description;
 }
}

CouplingGrid CouplingGrid_create (double tmin, double tmax, long numberOfTrachealFormants, long numberOfTrachealAntiFormants, long numberOfDeltaFormants)
{
 CouplingGrid me = (CouplingGrid) Thing_new ((void *) classCouplingGrid);
 if (me == ((void *)0) || ! Function_init (me, tmin, tmax)) goto end;
 if (((me -> tracheal_formants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealFormants)) == ((void *)0)) ||
  ((me -> tracheal_antiformants = FormantGrid_createEmpty (tmin, tmax, numberOfTrachealAntiFormants)) == ((void *)0)) ||
  ((me -> tracheal_formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfTrachealFormants)) == ((void *)0)) ||
  ((me -> delta_formants = FormantGrid_createEmpty (tmin, tmax, numberOfDeltaFormants)) == ((void *)0))) goto end;

 me -> glottis = PhonationTier_create (tmin, tmax);
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}



int FormantGrid_CouplingGrid_updateOpenPhases (FormantGrid me, CouplingGrid thee, double fadeFraction)
{
 RealTier delta;
 for (long itier = 1; itier <= thee -> delta_formants -> formants -> size; itier++)
 {
  delta = thee -> delta_formants -> formants -> item[itier];
  if (itier <= me -> formants -> size)
  {
   if (delta -> points -> size > 0)
   {
    RealTier rt = RealTier_updateWithDelta (me -> formants -> item[itier], delta, thee -> glottis, fadeFraction);
    if (rt == ((void *)0)) goto end;
    _Thing_forget ((Thing *) & (me -> formants -> item[itier]));
    me -> formants -> item[itier] = rt;
   }
  }
  delta = thee -> delta_formants -> bandwidths -> item[itier];
  if (itier <= me -> bandwidths -> size)
  {
   if (delta -> points -> size > 0)
   {
    RealTier rt = RealTier_updateWithDelta (me -> bandwidths -> item[itier], delta, thee -> glottis, fadeFraction);
    if (rt == ((void *)0)) goto end;
    _Thing_forget ((Thing *) & (me -> bandwidths -> item[itier]));
    me -> bandwidths -> item[itier] = rt;
   }
  }
 }
end:
 return ! Melder_hasError ();
}



int _Sound_FormantGrid_filterWithOneFormant_inline (Sound me, Any void_thee, long iformant, int antiformant)
{
 FormantGrid thee = (FormantGrid) void_thee;
 if (iformant < 1 || iformant > thee -> formants -> size)
 {
  Melder_warning3 (L"Formant ", Melder_integer (iformant), L" does not exist.");
  return 1;
 }

 RealTier ftier = thee -> formants -> item[iformant];
 RealTier btier = thee -> bandwidths -> item[iformant];
 if (ftier -> points -> size == 0 || btier -> points -> size == 0)
 {

  return 1;
 }

 double nyquist = 0.5 / me -> dx;
 Filter r = antiformant != 0 ? (Filter) AntiResonator_create (me -> dx) : (Filter) Resonator_create (me -> dx);
 if (r == ((void *)0)) return 0;

 for (long is = 1; is <= me -> nx; is++)
 {
  double t = me -> x1 + (is - 1) * me -> dx;
  double f = RealTier_getValueAtTime (ftier, t);
  double b = RealTier_getValueAtTime (btier, t);
  if (f <= nyquist && b != NUMundefined) Filter_setFB (r, f, b);
  me -> z[1][is] = Filter_getOutput (r, me -> z[1][is]);
 }

 _Thing_forget ((Thing *) & (r));
 return 1;
}

int Sound_FormantGrid_filterWithOneAntiFormant_inline (Sound me, FormantGrid thee, long iformant)
{
 return _Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 1);
}

int Sound_FormantGrid_filterWithOneFormant_inline (Sound me, FormantGrid thee, long iformant)
{
 return _Sound_FormantGrid_filterWithOneFormant_inline (me, thee, iformant, 0);
}

int Sound_FormantGrid_Intensities_filterWithOneFormant_inline (Sound me, FormantGrid thee, Ordered amplitudes, long iformant)
{
 if (iformant < 1 || iformant > thee -> formants -> size) return Melder_error3 (L"Formant ", Melder_integer (iformant), L" not defined. \nThis formant will not be used.");
 double nyquist = 0.5 / me -> dx;

 RealTier ftier = thee -> formants -> item[iformant];
 RealTier btier = thee -> bandwidths -> item[iformant];
 RealTier atier = amplitudes -> item[iformant];

 if (ftier -> points -> size == 0 || btier -> points -> size == 0 || atier -> points -> size == 0) return Melder_error3 (L"Frequencies or bandwidths or amplitudes of formant ", Melder_integer (iformant), L" not defined.\nThis formant will not be used.");

 Resonator r = Resonator_create (me -> dx);
 if (r == ((void *)0)) return 0;

 for (long is = 1; is <= me -> nx; is++)
 {
  double t = me -> x1 + (is - 1) * me -> dx;
  double f = RealTier_getValueAtTime (ftier, t);
  double b = RealTier_getValueAtTime (btier, t);
  if (f <= nyquist && b != NUMundefined)
  {
   Filter_setFB (r, f, b);
   if (atier -> points -> size > 0)
   {
    double a = RealTier_getValueAtTime (atier, t);
    if (a != NUMundefined) r -> a *= (pow (10.0, a / 20.0) * 2.0e-5);
   }
  }
  me -> z[1][is] = Filter_getOutput (r, me -> z[1][is]);
 }

 _Thing_forget ((Thing *) & (r));
 return 1;
}

Sound Sound_FormantGrid_Intensities_filter (Sound me, FormantGrid thee, Ordered amplitudes, long iformantb, long iformante, int alternatingSign)
{
 if (iformantb > iformante) { iformantb = 1; iformante = thee -> formants -> size; }
 if (iformantb < 1 || iformantb > thee -> formants -> size ||
  iformante < 1 || iformante > thee -> formants -> size) return Melder_errorp1 (L"No such formant number.");

 Sound him = Sound_create (me -> ny, me -> xmin, me -> xmax, me -> nx, me -> dx, me -> x1);
 if (him == ((void *)0)) return ((void *)0);

 for (long iformant = iformantb; iformant <= iformante; iformant++)
 {
  Sound tmp = Data_copy (me);
  if (tmp == ((void *)0)) goto end;
  if (Sound_FormantGrid_Intensities_filterWithOneFormant_inline (tmp, thee, amplitudes, iformant))
  {
   for (long is = 1; is <= me -> nx; is++)
   {
    him -> z[1][is] += alternatingSign >= 0 ? tmp -> z[1][is] : - tmp -> z[1][is];
   }
   _Thing_forget ((Thing *) & (tmp));
   if (alternatingSign != 0) alternatingSign = -alternatingSign;
  }
  else Melder_clearError ();
 }
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (him));
 return him;
}



static void _FricationGrid_initialize (void *table); struct structFricationGrid_Table theStructFricationGrid = { _FricationGrid_initialize, L"" "FricationGrid", & theStructFunction, sizeof (struct structFricationGrid) }; FricationGrid_Table classFricationGrid = & theStructFricationGrid; static void _FricationGrid_initialize (void *table) { FricationGrid_Table us = (FricationGrid_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classFricationGrid_destroy;
 us -> copy = classFricationGrid_copy;
 us -> equal = classFricationGrid_equal;
 us -> canWriteAsEncoding = classFricationGrid_canWriteAsEncoding;
 us -> writeText = classFricationGrid_writeText;
 us -> readText = classFricationGrid_readText;
 us -> writeBinary = classFricationGrid_writeBinary;
 us -> readBinary = classFricationGrid_readBinary;
 us -> description = classFricationGrid_description;
 }
}

FricationGrid FricationGrid_create (double tmin, double tmax, long numberOfFormants)
{
 FricationGrid me = (FricationGrid) Thing_new ((void *) classFricationGrid);

 if (me == ((void *)0) || ! Function_init (me, tmin, tmax)) goto end;
 if (((me -> noise_amplitude = IntensityTier_create (tmin, tmax)) == ((void *)0)) ||
  ((me -> formants = FormantGrid_createEmpty (tmin, tmax, numberOfFormants)) == ((void *)0)) ||
  ((me -> bypass = IntensityTier_create (tmin, tmax)) == ((void *)0))) goto end;

 me -> formants_amplitudes = formantsAmplitudes_create (tmin, tmax, numberOfFormants);

end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}

static void FricationGrid_draw_inside (FricationGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, double dy, double *yout)
{
 long numberOfXSections = 5;
 long numberOfFormants = me -> formants -> formants -> size;
 long numberOfParts = numberOfFormants + (numberOfFormants > 1 ? 0 : 1) ;

 double xw[6] = { 0, 1.5, 1, 1, 1, 0.5 }, xws[6];
 double r, x1, y1, x2, y2, x3, xs, ys, ymid = (ymin + ymax) / 2;

 rel_to_abs (xw, xws, numberOfXSections, xmax - xmin);

 dy = dy < 0 ? 0 : dy;
 dy = (ymax - ymin) / (numberOfParts * (1 + dy) - dy);

 connections cp = connections_create (numberOfParts);
 if (cp == ((void *)0)) return;


 x1 = xmin; x2 = x1 + xw[1]; y1 = ymid - 0.5 * dy; y2 = y1 + dy;
 draw_oneSection (g, x1, x2, y1, y2, L"Frication", L"noise", L"generator");


 x1 = x2; x2 = x1 + xw[2] / 2;
 Graphics_line (g, x1, ymid, x2, ymid);
 Graphics_line (g, x2, ymax - dy / 2, x2, ymin + dy / 2);
 x3 = x2;

 x1 = xmin + xws[2]; x2 = x1 + xw[3]; y2 = ymax;
 MelderString fba = { 0 };
 for (long i = 1; i <= numberOfParts; i++)
 {
  const wchar_t *fi = Melder_integer (i+1);
  y1 = y2 - dy;
  if (i < numberOfParts) { MelderString_append6 (&fba, L"A", fi, L" F", fi, L" B", fi); }
  else { MelderString_append1 (&fba, L"Bypass"); }
  draw_oneSection (g, x1, x2, y1, y2, ((void *)0), fba.string, ((void *)0));
  double ymidi = (y1 + y2) / 2;
  Graphics_line (g, x3, ymidi, x1, ymidi);
  cp -> x[i] = x2; cp -> y[i] = ymidi;
  y2 = y1 - 0.5 * dy;
  MelderString_empty (&fba);
 }

 r = xw[5] / 2;
 xs = xmax - r; ys = ymid;

 if (numberOfParts > 1) alternatingSummer_drawConnections (g, xs, ys, r, cp, 1, 0.4);
 else Graphics_line (g, cp -> x[1], cp -> y[1], xs + r, ys);

 connections_free (cp);

 if (yout != ((void *)0)) *yout = ys;
 MelderString_free (&fba);
}

void FricationGrid_draw (FricationGrid me, Graphics g)
{
 double xmin = 0, xmax = 1, xmax2 = 0.9, ymin = 0, ymax = 1, dy = 0.5, yout;

 Graphics_setInner (g);
 Graphics_setWindow (g, xmin, xmax, ymin, ymax);
 Graphics_setTextAlignment (g, kGraphics_horizontalAlignment_CENTRE, 1);
 Graphics_setLineWidth (g, 2);

 FricationGrid_draw_inside (me, g, xmin, xmax2, ymin, ymax, dy, &yout);

 Graphics_arrow (g, xmax2, yout, xmax, yout);
 Graphics_unsetInner (g);
}

Sound FricationGrid_to_Sound (FricationGrid me, synthesisParams p)
{
 Sound thee = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
 if (thee == ((void *)0)) return ((void *)0);

 double lastval = 0;
 for (long i = 1; i <= thee -> nx; i++)
 {
  double t = thee -> x1 + (i - 1) * thee -> dx;
  double val = NUMrandomUniform (-1, 1);
  double a = 0;
  if (me -> noise_amplitude -> points -> size > 0)
  {
   double dba = RealTier_getValueAtTime (me -> noise_amplitude, t);
   a = dba == NUMundefined ? 0 : (pow (10.0, dba / 20.0) * 2.0e-5);
  }
  lastval = (val += 0.75 * lastval);
  thee -> z[1][i] = val * a;
 }

 Sound him = Sound_FricationGrid_filter (thee, me, p);
 _Thing_forget ((Thing *) & (thee));
 return him;
}



Sound Sound_FricationGrid_filter (Sound me, FricationGrid thee, synthesisParams params)
{
 Sound him = ((void *)0);
 struct synthesisParams p; if (params == ((void *)0)) synthesisParams_setDefaultsFricationGrid (&p, thee); else p = *params;
 long numberOfFricationFormants = p.klatt80 ? ((6) < (thee -> formants -> formants -> size) ? (6) : (thee -> formants -> formants -> size)) : thee -> formants -> formants -> size;

 check_formants (numberOfFricationFormants, &(p.startFricationFormant), &(p.endFricationFormant));

 if (p.endFricationFormant > 1)
 {
  long startFricationFormant2 = p.startFricationFormant > 2 ? p.startFricationFormant : 2;
  int alternatingSign = startFricationFormant2 % 2 == 0 ? 1 : -1;
  him = Sound_FormantGrid_Intensities_filter (me, thee -> formants, thee -> formants_amplitudes, startFricationFormant2, p.endFricationFormant, alternatingSign);
  if (him == ((void *)0)) return ((void *)0);
 }

 if (him == ((void *)0)) him = Data_copy (me);
 if (him == ((void *)0)) return ((void *)0);

 if (p.fricationBypass)
 {
  for (long is = 1; is <= him -> nx; is++)
  {
   double t = him -> x1 + (is - 1) * him -> dx;
   double ab = 0;
   if (thee -> bypass -> points -> size > 0)
   {
    double val = RealTier_getValueAtTime (thee -> bypass, t);
    ab = val == NUMundefined ? 0 : (pow (10.0, val / 20.0) * 2.0e-5);
   }
   him -> z[1][is] += me -> z[1][is] * ab;
  }
 }
 return him;
}



static void _KlattGrid_initialize (void *table); struct structKlattGrid_Table theStructKlattGrid = { _KlattGrid_initialize, L"" "KlattGrid", & theStructFunction, sizeof (struct structKlattGrid) }; KlattGrid_Table classKlattGrid = & theStructKlattGrid; static void _KlattGrid_initialize (void *table) { KlattGrid_Table us = (KlattGrid_Table) table; if (! classFunction -> destroy) classFunction -> _initialize (classFunction); classFunction -> _initialize (us);
{
 us -> destroy = classKlattGrid_destroy;
 us -> copy = classKlattGrid_copy;
 us -> equal = classKlattGrid_equal;
 us -> canWriteAsEncoding = classKlattGrid_canWriteAsEncoding;
 us -> writeText = classKlattGrid_writeText;
 us -> readText = classKlattGrid_readText;
 us -> writeBinary = classKlattGrid_writeBinary;
 us -> readBinary = classKlattGrid_readBinary;
 us -> description = classKlattGrid_description;
 }
}

KlattGrid KlattGrid_create (double tmin, double tmax, long numberOfFormants,
 long numberOfNasalFormants, long numberOfNasalAntiFormants,
 long numberOfTrachealFormants, long numberOfTrachealAntiFormants,
 long numberOfFricationFormants, long numberOfDeltaFormants)
{
 KlattGrid me = (KlattGrid) Thing_new ((void *) classKlattGrid);

 if (me == ((void *)0) || ! Function_init (me, tmin, tmax)) goto end;

 if ((me -> phonation = PhonationGrid_create (tmin, tmax)) == ((void *)0)) goto end;

 if ((me -> vocalTract = VocalTractGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants, numberOfNasalAntiFormants)) == ((void *)0)) goto end;

 if ((me -> coupling = CouplingGrid_create (tmin, tmax, numberOfTrachealFormants, numberOfTrachealAntiFormants, numberOfDeltaFormants)) == ((void *)0)) goto end;

 if ((me -> frication = FricationGrid_create (tmin, tmax, numberOfFricationFormants)) == ((void *)0)) goto end;

 if ((me -> gain = IntensityTier_create (tmin, tmax)) == ((void *)0)) goto end;

end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (me));
 return me;
}

KlattGrid KlattGrid_createExample (void)
{
 KlattTable thee = KlattTable_createExample ();
 if (thee == ((void *)0)) return ((void *)0);

 KlattGrid me = KlattTable_to_KlattGrid (thee, 0.005);
 _Thing_forget ((Thing *) & (thee));
 return me;
}



static void _KlattGrid_queryParallelSplit (KlattGrid me, double dy, double *y, double *y1)
{
 long ny = me -> vocalTract -> nasal_formants -> formants -> size + me -> vocalTract -> formants -> formants -> size + me -> coupling -> tracheal_formants -> formants -> size;
 long n1 = me -> vocalTract -> nasal_formants -> formants -> size + (me -> vocalTract -> formants -> formants -> size > 0 ? 1 : 0);

 long n2 = ny - n1;
 if (ny == 0) { *y = 0; *y1 = 0; return; }

 *y = ny + (ny - 1) * dy;

 if (n1 == 0) { *y1 = 0.5; }
 else if (n2 == 0) { *y1 = *y - 0.5; }
 else { *y1 = n1 + (n1 - 1) * dy + 0.5 * dy; }
 return;
}

static void getYpositions (double h1, double h2, double h3, double h4, double h5, double fractionOverlap, double *dy, double *ymin1, double *ymax1, double *ymin2, double *ymax2, double *ymin3, double *ymax3)
{
# 1881 "KlattGrid.c"
 double h;
 double h13 = h1 > h3 ? h1 : h3;
 if (h2 >= h4)
 {
  h = h13 + h2 + h5;
 }
 else
 {
  double maximumOverlap3 = fractionOverlap * h5;
  if (maximumOverlap3 < (h1 + h2)) maximumOverlap3 = 0;
  else if (maximumOverlap3 > (h4 - h2)) maximumOverlap3 = h4 - h2;
  h = h13 + h4 + h5 - maximumOverlap3;
 }
 *dy = 1 / (1.1 * h);
 *ymin1 = 1 - (h13 + h2) * *dy; *ymax1 = *ymin1 + (h1 + h2) * *dy;
 *ymin2 = 1 - (h13 + h4) * *dy; *ymax2 = *ymin2 + (h3 + h4) * *dy;
 *ymin3 = 0; *ymax3 = h5 * *dy;
}

void KlattGrid_drawVocalTract (KlattGrid me, Graphics g, int filterModel, int withTrachea)
{
 VocalTractGrid_CouplingGrid_draw (me -> vocalTract, withTrachea ? me -> coupling : ((void *)0), g, filterModel);
}

void KlattGrid_draw (KlattGrid me, Graphics g, int filterModel)
{
  double xs1, xs2, ys1, ys2, xf1, xf2, yf1, yf2;
  double xp1, xp2, yp1, yp2, xc1, xc2, yc1, yc2;
  double dy, r, xs, ys;
  double xmin = 0, xmax2 = 0.90, xmax3 = 0.95, xmax = 1, ymin = 0, ymax = 1;
 double xws[6];
 double height_phonation = 0.3;
 double dy_phonation = 0.5, dy_vocalTract_p = 0.5, dy_frication = 0.5;

 connections tf = connections_create (2);
 if (tf == ((void *)0)) return;

 Graphics_setInner (g);

 Graphics_setWindow (g, xmin, xmax, ymin, ymax);
 Graphics_setTextAlignment (g, kGraphics_horizontalAlignment_CENTRE, 1);
 Graphics_setLineWidth (g, 2);

 long nff = me -> frication -> formants -> formants -> size - 1 + 1;
 double yh_frication = nff > 0 ? nff + (nff - 1) * dy_frication : 1;
 double yh_phonation = 1 + dy_phonation + 1;
 double yout_phonation, yout_frication;
 dy = height_phonation / yh_phonation;

 if (filterModel == 0)
 {


  double xw[6] = {0, 1, 0.125, 3, 0.25, 0.125 };
  double yin_vocalTract_c, yout_vocalTract_c;

  rel_to_abs (xw, xws, 5, xmax2 - xmin);



  height_phonation = yh_phonation / (yh_phonation + yh_frication);
  if (height_phonation < 0.3) height_phonation = 0.3;
  dy = height_phonation / yh_phonation;

  xs1 = xmin; xs2 = xs1 + xw[1]; ys2 = ymax; ys1 = ys2 - height_phonation;
  PhonationGrid_draw_inside (me -> phonation, g, xs1, xs2, ys1, ys2, dy_phonation, &yout_phonation);



  xc1 = xmin + xws[2]; xc2 = xc1 + xw[3];
  yc2 = yout_phonation + dy / 2; yc1 = yc2 - dy;
  VocalTractGrid_CouplingGrid_drawCascade_inline (me -> vocalTract, me -> coupling, g, xc1, xc2, yc1, yc2, &yin_vocalTract_c, &yout_vocalTract_c);

  tf -> x[1] = xc2; tf -> y[1] = yout_vocalTract_c;

  Graphics_line (g, xs2, yout_phonation, xc1, yin_vocalTract_c);

  xf1 = xmin + xws[2]; xf2 = xf1 + xw[3]; yf2 = ymax - height_phonation; yf1 = 0;

  FricationGrid_draw_inside (me -> frication, g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
 }
 else
 {


  double yf_parallel, yh_parallel, ytrans_phonation, ytrans_parallel, yh_overlap = 0.3, yin_vocalTract_p, yout_vocalTract_p;
  double xw[6] = { 0, 1, 0.125, 3, 0.25, 0.125 };

  rel_to_abs (xw, xws, 5, xmax2 - xmin);





  _KlattGrid_queryParallelSplit (me, dy_vocalTract_p, &yh_parallel, &yf_parallel);
  if (yh_parallel == 0) { yh_parallel = yh_phonation; yf_parallel = yh_parallel / 2; yh_overlap = -0.1; }

  height_phonation = yh_phonation / (yh_phonation + yh_frication);
  if (height_phonation < 0.3) height_phonation = 0.3;
  double yunit = (ymax - ymin) / (yh_parallel + (1 - yh_overlap) * yh_frication);

  double ycs = ymax - 0.5 * height_phonation;
  double ycp = ymax - yf_parallel * yunit;
  ytrans_phonation = ycs > ycp ? ycp - ycs : 0;
  ytrans_parallel = ycp > ycs ? ycs - ycp : 0;


  xs1 = xmin; xs2 = xs1 + xw[1];

  double h1 = yh_phonation / 2, h2 = h1, h3 = yf_parallel, h4 = yh_parallel - h3, h5 = yh_frication;
  getYpositions (h1, h2, h3, h4, h5, yh_overlap, &dy, &ys1, &ys2, &yp1, &yp2, &yf1, &yf2);

  PhonationGrid_draw_inside (me -> phonation, g, xs1, xs2, ys1, ys2, dy_phonation, &yout_phonation);

  xp1 = xmin + xws[2]; xp2 = xp1 + xw[3];
  VocalTractGrid_CouplingGrid_drawParallel_inline (me -> vocalTract, me -> coupling, g, xp1, xp2, yp1, yp2, dy_vocalTract_p, &yin_vocalTract_p, &yout_vocalTract_p);

  tf -> x[1] = xp2; tf -> y[1] = yout_vocalTract_p;

  Graphics_line (g, xs2, yout_phonation, xp1, yin_vocalTract_p);

  xf1 = xmin ; xf2 = xf1 + 0.45 * (xw[2] + xws[3]);

  FricationGrid_draw_inside (me -> frication, g, xf1, xf2, yf1, yf2, dy_frication, &yout_frication);
 }

 tf -> x[2] = xf2; tf -> y[2] = yout_frication;
 r = (xmax3 - xmax2) / 2; xs = xmax2 + r / 2; ys = (ymax - ymin) / 2;

 summer_drawConnections (g, xs, ys, r, tf, 1, 0.6);

 Graphics_arrow (g, xs + r, ys, xmax, ys);

 Graphics_unsetInner (g);
 connections_free (tf);
}
# 2040 "KlattGrid.c"
double KlattGrid_getPitchAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> pitch, t); } int KlattGrid_addPitchPoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> pitch, t, value);} void KlattGrid_removePitchPointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> pitch, t1, t2); } PitchTier KlattGrid_extractPitchTier (KlattGrid me) { return Data_copy (me -> phonation -> pitch); } int KlattGrid_replacePitchTier (KlattGrid me, PitchTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> pitch)); me -> phonation -> pitch = any; return 1; }
double KlattGrid_getVoicingAmplitudeAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> voicingAmplitude, t); } int KlattGrid_addVoicingAmplitudePoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> voicingAmplitude, t, value);} void KlattGrid_removeVoicingAmplitudePointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> voicingAmplitude, t1, t2); } IntensityTier KlattGrid_extractVoicingAmplitudeTier (KlattGrid me) { return Data_copy (me -> phonation -> voicingAmplitude); } int KlattGrid_replaceVoicingAmplitudeTier (KlattGrid me, IntensityTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> voicingAmplitude)); me -> phonation -> voicingAmplitude = any; return 1; }
double KlattGrid_getFlutterAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> flutter, t); } int KlattGrid_addFlutterPoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> flutter, t, value);} void KlattGrid_removeFlutterPointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> flutter, t1, t2); } RealTier KlattGrid_extractFlutterTier (KlattGrid me) { return Data_copy (me -> phonation -> flutter); } int KlattGrid_replaceFlutterTier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> flutter)); me -> phonation -> flutter = any; return 1; }
double KlattGrid_getPower1AtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> power1, t); } int KlattGrid_addPower1Point (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> power1, t, value);} void KlattGrid_removePower1PointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> power1, t1, t2); } RealTier KlattGrid_extractPower1Tier (KlattGrid me) { return Data_copy (me -> phonation -> power1); } int KlattGrid_replacePower1Tier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> power1)); me -> phonation -> power1 = any; return 1; }
double KlattGrid_getPower2AtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> power2, t); } int KlattGrid_addPower2Point (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> power2, t, value);} void KlattGrid_removePower2PointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> power2, t1, t2); } RealTier KlattGrid_extractPower2Tier (KlattGrid me) { return Data_copy (me -> phonation -> power2); } int KlattGrid_replacePower2Tier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> power2)); me -> phonation -> power2 = any; return 1; }
double KlattGrid_getOpenPhaseAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> openPhase, t); } int KlattGrid_addOpenPhasePoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> openPhase, t, value);} void KlattGrid_removeOpenPhasePointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> openPhase, t1, t2); } RealTier KlattGrid_extractOpenPhaseTier (KlattGrid me) { return Data_copy (me -> phonation -> openPhase); } int KlattGrid_replaceOpenPhaseTier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> openPhase)); me -> phonation -> openPhase = any; return 1; }
double KlattGrid_getCollisionPhaseAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> collisionPhase, t); } int KlattGrid_addCollisionPhasePoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> collisionPhase, t, value);} void KlattGrid_removeCollisionPhasePointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> collisionPhase, t1, t2); } RealTier KlattGrid_extractCollisionPhaseTier (KlattGrid me) { return Data_copy (me -> phonation -> collisionPhase); } int KlattGrid_replaceCollisionPhaseTier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> collisionPhase)); me -> phonation -> collisionPhase = any; return 1; }
double KlattGrid_getDoublePulsingAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> doublePulsing, t); } int KlattGrid_addDoublePulsingPoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> doublePulsing, t, value);} void KlattGrid_removeDoublePulsingPointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> doublePulsing, t1, t2); } RealTier KlattGrid_extractDoublePulsingTier (KlattGrid me) { return Data_copy (me -> phonation -> doublePulsing); } int KlattGrid_replaceDoublePulsingTier (KlattGrid me, RealTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> doublePulsing)); me -> phonation -> doublePulsing = any; return 1; }
double KlattGrid_getSpectralTiltAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> spectralTilt, t); } int KlattGrid_addSpectralTiltPoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> spectralTilt, t, value);} void KlattGrid_removeSpectralTiltPointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> spectralTilt, t1, t2); } IntensityTier KlattGrid_extractSpectralTiltTier (KlattGrid me) { return Data_copy (me -> phonation -> spectralTilt); } int KlattGrid_replaceSpectralTiltTier (KlattGrid me, IntensityTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> spectralTilt)); me -> phonation -> spectralTilt = any; return 1; }
double KlattGrid_getAspirationAmplitudeAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> aspirationAmplitude, t); } int KlattGrid_addAspirationAmplitudePoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> aspirationAmplitude, t, value);} void KlattGrid_removeAspirationAmplitudePointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> aspirationAmplitude, t1, t2); } IntensityTier KlattGrid_extractAspirationAmplitudeTier (KlattGrid me) { return Data_copy (me -> phonation -> aspirationAmplitude); } int KlattGrid_replaceAspirationAmplitudeTier (KlattGrid me, IntensityTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> aspirationAmplitude)); me -> phonation -> aspirationAmplitude = any; return 1; }
double KlattGrid_getBreathinessAmplitudeAtTime (KlattGrid me, double t) { return RealTier_getValueAtTime (me -> phonation -> breathinessAmplitude, t); } int KlattGrid_addBreathinessAmplitudePoint (KlattGrid me, double t, double value) { return RealTier_addPoint (me -> phonation -> breathinessAmplitude, t, value);} void KlattGrid_removeBreathinessAmplitudePointsBetween (KlattGrid me, double t1, double t2) { AnyTier_removePointsBetween (me -> phonation -> breathinessAmplitude, t1, t2); } IntensityTier KlattGrid_extractBreathinessAmplitudeTier (KlattGrid me) { return Data_copy (me -> phonation -> breathinessAmplitude); } int KlattGrid_replaceBreathinessAmplitudeTier (KlattGrid me, IntensityTier thee) { if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal"); Any any = Data_copy (thee); if (any == ((void *)0)) return 0; _Thing_forget ((Thing *) & (me -> phonation -> breathinessAmplitude)); me -> phonation -> breathinessAmplitude = any; return 1; }

static Any _KlattGrid_getAddressOfFormantGrid (KlattGrid me, int formantType)
{
 return formantType == 1 ? &(me -> vocalTract -> formants) :
  formantType == 2 ? &(me -> vocalTract -> nasal_formants) :
  formantType == 3 ? &(me -> frication -> formants) :
  formantType == 4 ? &(me -> coupling -> tracheal_formants) :
  formantType == 5 ? &(me -> vocalTract -> nasal_antiformants) :
  formantType == 6 ? &(me -> coupling -> tracheal_antiformants) :
  formantType == 7 ? &(me -> coupling -> delta_formants) : ((void *)0);
}

static Any _KlattGrid_getAddressOfAmplitudes (KlattGrid me, int formantType)
{
 return formantType == 1 ? &(me -> vocalTract -> formants_amplitudes) :
  formantType == 2 ? &(me -> vocalTract -> nasal_formants_amplitudes) :
  formantType == 3 ? &(me -> frication -> formants_amplitudes) :
  formantType == 4 ? &(me -> coupling -> tracheal_formants_amplitudes) : ((void *)0);
}
# 2089 "KlattGrid.c"
double KlattGrid_getFormantAtTime (KlattGrid me, int formantType, long iformant, double t) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); return FormantGrid_getFormantAtTime (*fg, iformant, t); } int KlattGrid_addFormantPoint (KlattGrid me, int formantType, long iformant, double t, double value) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); return FormantGrid_addFormantPoint (*fg, iformant, t, value); } void KlattGrid_removeFormantPointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); FormantGrid_removeFormantPointsBetween (*fg, iformant, t1, t2); }
double KlattGrid_getBandwidthAtTime (KlattGrid me, int formantType, long iformant, double t) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); return FormantGrid_getBandwidthAtTime (*fg, iformant, t); } int KlattGrid_addBandwidthPoint (KlattGrid me, int formantType, long iformant, double t, double value) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); return FormantGrid_addBandwidthPoint (*fg, iformant, t, value); } void KlattGrid_removeBandwidthPointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2) { FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType); FormantGrid_removeBandwidthPointsBetween (*fg, iformant, t1, t2); }

double KlattGrid_getAmplitudeAtTime (KlattGrid me, int formantType, long iformant, double t)
{
 Ordered *ordered = _KlattGrid_getAddressOfAmplitudes (me, formantType);
 if (iformant < 0 || iformant > (*ordered) -> size) return NUMundefined;
 return RealTier_getValueAtTime ((*ordered) -> item[iformant], t);
}
int KlattGrid_addAmplitudePoint (KlattGrid me, int formantType, long iformant, double t, double value)
{
 Ordered *ordered = _KlattGrid_getAddressOfAmplitudes (me, formantType);
 if (iformant < 0 || iformant > (*ordered) -> size) return Melder_error1 (L"Formant does not exist.");
 return RealTier_addPoint ((*ordered) -> item[iformant], t, value);
}
void KlattGrid_removeAmplitudePointsBetween (KlattGrid me, int formantType, long iformant, double t1, double t2)
{
 Ordered *ordered = _KlattGrid_getAddressOfAmplitudes (me, formantType);
 if (iformant < 0 || iformant > (*ordered) ->size) return;
 AnyTier_removePointsBetween ((*ordered) -> item[iformant], t1, t2);
}
IntensityTier KlattGrid_extractAmplitudeTier (KlattGrid me, int formantType, long iformant)
{
 Ordered *ordered = _KlattGrid_getAddressOfAmplitudes (me, formantType);
 if (iformant < 0 || iformant > (*ordered) ->size) return Melder_errorp1 (L"Formant does not exist.");
 return Data_copy ((*ordered) -> item[iformant]);
}

int KlattGrid_replaceAmplitudeTier (KlattGrid me, int formantType, long iformant, IntensityTier thee)
{
 if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal");
 Ordered *ordered = _KlattGrid_getAddressOfAmplitudes (me, formantType);
 if (iformant < 0 || iformant > (*ordered) -> size) return Melder_error1 (L"Formant does not exist.");
 IntensityTier any = Data_copy (thee);
 if (any == ((void *)0)) return 0;
 _Thing_forget ((Thing *) & ((*ordered) -> item[iformant]));
 (*ordered) -> item[iformant] = any;
 return 1;
}

FormantGrid KlattGrid_extractFormantGrid (KlattGrid me, int formantType)
{
 FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType);
 return Data_copy (*fg);
}

int KlattGrid_replaceFormantGrid (KlattGrid me, int formantType, FormantGrid thee)
{
 if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal");
 FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, formantType);
 Any any = Data_copy (thee);
 if (any == ((void *)0)) return 0;
 _Thing_forget ((Thing *) & (*fg));
 *fg = any;
 return 1;
}

double KlattGrid_getDeltaFormantAtTime (KlattGrid me, long iformant, double t)
 { return FormantGrid_getFormantAtTime (me -> coupling -> delta_formants, iformant, t); }
int KlattGrid_addDeltaFormantPoint (KlattGrid me, long iformant, double t, double value)
 { return FormantGrid_addFormantPoint (me -> coupling -> delta_formants, iformant, t, value); }
void KlattGrid_removeDeltaFormantPointsBetween (KlattGrid me, long iformant, double t1, double t2)
 { FormantGrid_removeFormantPointsBetween (me -> coupling -> delta_formants, iformant, t1, t2); }
double KlattGrid_getDeltaBandwidthAtTime (KlattGrid me, long iformant, double t)
 { return FormantGrid_getBandwidthAtTime (me -> coupling -> delta_formants, iformant, t); }
int KlattGrid_addDeltaBandwidthPoint (KlattGrid me, long iformant, double t, double value)
 { return FormantGrid_addBandwidthPoint (me -> coupling -> delta_formants, iformant, t, value); }
void KlattGrid_removeDeltaBandwidthPointsBetween (KlattGrid me, long iformant, double t1, double t2)
 { FormantGrid_removeBandwidthPointsBetween (me -> coupling -> delta_formants, iformant, t1, t2); }

FormantGrid KlattGrid_extractDeltaFormantGrid (KlattGrid me)
{
 FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, 7);
 return Data_copy (*fg);
}

int KlattGrid_replaceDeltaFormantGrid (KlattGrid me, FormantGrid thee)
{
 if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal");
 FormantGrid *fg = _KlattGrid_getAddressOfFormantGrid (me, 7);
 Any any = Data_copy (thee);
 if (any == ((void *)0)) return 0;
 _Thing_forget ((Thing *) & (*fg));
 *fg = any;
 return 1;
}

FormantGrid KlattGrid_to_FormantGrid_openPhases (KlattGrid me, double fadeFraction)
{
 if (me -> vocalTract -> formants -> formants -> size == 0 && me -> vocalTract -> formants -> bandwidths -> size == 0)
  return Melder_errorp1 (L"Formant grid is empty.");

 FormantGrid thee = Data_copy (me -> vocalTract -> formants);
 if (thee == ((void *)0)) return ((void *)0);

 if (me -> coupling -> glottis == ((void *)0) || me -> coupling -> glottis -> points -> size == 0)
 {
  struct synthesisParams p = synthesisParams_createDefault (me);
  PhonationTier phTier = PhonationGrid_to_PhonationTier (me -> phonation, &p);
  if (phTier == ((void *)0)) goto end;
  _Thing_forget ((Thing *) & (me -> coupling -> glottis));
  me -> coupling -> glottis = phTier;
 }
 FormantGrid_CouplingGrid_updateOpenPhases (thee, me -> coupling, fadeFraction);
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}

double KlattGrid_getFricationAmplitudeAtTime (KlattGrid me, double t)
{
 return RealTier_getValueAtTime (me -> frication -> noise_amplitude, t);
}

int KlattGrid_addFricationAmplitudePoint (KlattGrid me, double t, double value)
{
 return RealTier_addPoint (me -> frication -> noise_amplitude, t, value);
}

void KlattGrid_removeFricationAmplitudePointsBetween (KlattGrid me, double t1, double t2)
{
 AnyTier_removePointsBetween (me -> frication -> noise_amplitude, t1, t2);
}

IntensityTier KlattGrid_extractFricationAmplitudeTier (KlattGrid me)
{
 return Data_copy (me -> frication -> noise_amplitude);
}

int KlattGrid_replaceFricationAmplitudeTier (KlattGrid me, IntensityTier thee)
{
 if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal");
 Any any = Data_copy (thee);
 if (any == ((void *)0)) return 0;
 _Thing_forget ((Thing *) & (me -> frication -> noise_amplitude));
 me -> frication -> noise_amplitude = any;
 return 1;
}

double KlattGrid_getFricationBypassAtTime (KlattGrid me, double t)
{
 return RealTier_getValueAtTime (me -> frication -> bypass, t);
}

int KlattGrid_addFricationBypassPoint (KlattGrid me, double t, double value)
{
 return RealTier_addPoint (me -> frication -> bypass, t, value);
}

void KlattGrid_removeFricationBypassPointsBetween (KlattGrid me, double t1, double t2)
{
 AnyTier_removePointsBetween (me -> frication -> bypass, t1, t2);
}

IntensityTier KlattGrid_extractFricationBypassTier (KlattGrid me)
{
 return Data_copy (me -> frication -> bypass);
}

int KlattGrid_replaceFricationBypassTier (KlattGrid me, IntensityTier thee)
{
 if (me -> xmin != thee -> xmin || me -> xmax != thee -> xmax) return Melder_error1 (L"Domains must be equal");
 Any any = Data_copy (thee);
 if (any == ((void *)0)) return 0;
 _Thing_forget ((Thing *) & (me -> frication -> bypass));
 me -> frication -> bypass = any;
 return 1;
}


Sound KlattGrid_to_Sound_aspiration (KlattGrid me, synthesisParams p)
{
 return PhonationGrid_to_Sound_aspiration (me -> phonation, p);
}

Sound KlattGrid_to_Sound (KlattGrid me, synthesisParams p)
{
 Sound thee = ((void *)0), frication = ((void *)0);

 if (p -> aspiration || p -> voicing)
 {
  if (p -> maximumPeriod == 0) p -> maximumPeriod = PhonationGrid_getMaximumPeriod (me -> phonation);
  Sound source = PhonationGrid_to_Sound (me -> phonation, p, me -> coupling);
  if (source == ((void *)0)) goto end;

  thee = Sound_VocalTractGrid_CouplingGrid_filter (source, me -> vocalTract, me -> coupling, p);

  _Thing_forget ((Thing *) & (source));

  if (thee == ((void *)0)) goto end;
 }

 if (p -> endFricationFormant > 0 || p -> fricationBypass)
 {
  frication = FricationGrid_to_Sound (me -> frication, p);
  if (frication == ((void *)0)) goto end;

  if (thee != ((void *)0))
  {
   _Sounds_add_inline (thee, frication);
   _Thing_forget ((Thing *) & (frication));
  }
  else thee = frication;
 }
 if (thee == ((void *)0)) thee = Sound_createEmptyMono (me -> xmin, me -> xmax, p -> samplingFrequency);
end:
 if (Melder_hasError ()) _Thing_forget ((Thing *) & (thee));
 return thee;
}

int KlattGrid_play (KlattGrid me)
{
 struct synthesisParams p = synthesisParams_createDefault (me);
 Sound thee = KlattGrid_to_Sound (me, &p);
 if (thee == ((void *)0)) return 0;
 Vector_scale (thee, 0.99);
 (void) Sound_play (thee, ((void *)0), ((void *)0));
 _Thing_forget ((Thing *) & (thee));
 return 1;
}

Sound KlattGrid_to_Sound_simple (KlattGrid me, double samplingFrequency, int parallel)
{
 struct synthesisParams p;
 synthesisParams_setDefault (&p, me);
 p.filterModel = parallel ? 1 : 0;
 p.samplingFrequency = samplingFrequency;
 return KlattGrid_to_Sound (me, &p);
}



Sound Sound_KlattGrid_filter_frication (Sound me, KlattGrid thee, synthesisParams params)
{
 return Sound_FricationGrid_filter (me, thee -> frication, params);
}

Sound Sound_KlattGrid_filter_laryngial_cascade (Sound me, KlattGrid thee, synthesisParams params)
{
 return Sound_VocalTractGrid_CouplingGrid_filter (me, thee -> vocalTract, thee -> coupling, params);
}





KlattGrid KlattTable_to_KlattGrid (KlattTable me, double frameDuration)
{
 Table kt = (Table) me;

 long nrows = me -> rows -> size;
 double tmin = 0, tmax = nrows * frameDuration;
 double dBNul = -300;
 double dB_offset = -20 * log10 (2.0e-5) - 87;
 double dB_offset_voicing = 20 * log10 (320000/32767);
 double dB_offset_noise = -20 * log10 (32.767 / 8.192);

 double ap[7] = {0, 0.4, 0.15, 0.06, 0.04, 0.022, 0.03 };
 long numberOfFormants = 6;
 long numberOfNasalFormants = 1;
 long numberOfNasalAntiFormants = numberOfNasalFormants;
 long numberOfTrachealFormants = 0;
 long numberOfTrachealAntiFormants = numberOfTrachealFormants;
 long numberOfFricationFormants = 6;
 long numberOfDeltaFormants = 1;

 KlattGrid thee = KlattGrid_create (tmin, tmax, numberOfFormants, numberOfNasalFormants,
  numberOfNasalAntiFormants, numberOfTrachealFormants, numberOfTrachealAntiFormants,
  numberOfFricationFormants, numberOfDeltaFormants);
 if (thee == ((void *)0)) return ((void *)0);
 for (long irow = 1; irow <= nrows; irow++)
 {
  double t = (irow - 1) * frameDuration;
  double val, f0, fk;
  long icol = 1;

  f0 = val = Table_getNumericValue (kt, irow, icol) / 10;
  RealTier_addPoint (thee -> phonation -> pitch, t, f0);
  icol++; val = Table_getNumericValue (kt, irow, icol);

  val -= 7;
  if (val < 13) val = dBNul;

  for (long kf = 1; kf <= 6; kf++)
  {
   icol++; fk = val = Table_getNumericValue (kt, irow, icol);
   RealTier_addPoint (thee -> vocalTract -> formants -> formants -> item[kf], t, val);
   RealTier_addPoint (thee -> frication -> formants -> formants -> item[kf], t, val);
   icol++; val = Table_getNumericValue (kt, irow, icol);
   if (val <= 0) val = fk / 10;
   RealTier_addPoint (thee -> vocalTract -> formants -> bandwidths -> item[kf], t, val);
  }
  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> vocalTract -> nasal_antiformants -> formants -> item[1], t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> vocalTract -> nasal_antiformants -> bandwidths -> item[1], t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> vocalTract -> nasal_formants -> formants -> item[1], t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> vocalTract -> nasal_formants -> bandwidths -> item[1], t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
  RealTier_addPoint (thee -> phonation -> aspirationAmplitude, t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  double openPhase = f0 > 0 ? (val / 16000) * f0 : 0.7;
  RealTier_addPoint (thee -> phonation -> openPhase, t, openPhase);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  if (val < 13) val = dBNul; else val += 20 * log10 (0.1) + dB_offset_noise;
  RealTier_addPoint (thee -> phonation -> breathinessAmplitude, t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> phonation -> spectralTilt, t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  if (val < 13) val = dBNul; else val += 20 * log10 (0.25) + dB_offset_noise;
  RealTier_addPoint (thee -> frication -> noise_amplitude, t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);

  for (long kf = 1; kf <= 6; kf++)
  {
   icol++; val = Table_getNumericValue (kt, irow, icol);
   if (val < 13) val = dBNul; else val += 20 * log10 (ap[kf]) + dB_offset;
   RealTier_addPoint (thee -> vocalTract -> formants_amplitudes -> item[kf], t, val);
   RealTier_addPoint (thee -> frication -> formants_amplitudes -> item[kf], t, val);
   icol++; val = Table_getNumericValue (kt, irow, icol);
   RealTier_addPoint (thee -> frication -> formants -> bandwidths -> item[kf], t, val);
  }

  icol++; val = Table_getNumericValue (kt, irow, icol);
  if (val < 13) val = dBNul; else val += 20 * log10 (0.6) + dB_offset;
  RealTier_addPoint (thee -> vocalTract -> nasal_formants_amplitudes -> item[1], t, val);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  if (val < 13) val = dBNul; else val += 20 * log10 (0.05) + dB_offset_noise;
  RealTier_addPoint (thee -> frication -> bypass, t, val);

  icol++; val = Table_getNumericValue (kt, irow, icol);
  RealTier_addPoint (thee -> phonation -> voicingAmplitude, t, val + dB_offset_voicing);
  icol++; val = Table_getNumericValue (kt, irow, icol);
  val -= 3; if (val <= 0) val = 57;
  RealTier_addPoint (thee -> gain, t, val + dB_offset);
 }




 if (Melder_hasError()) _Thing_forget ((Thing *) & (thee));
 return thee;
}
