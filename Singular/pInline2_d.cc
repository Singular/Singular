 


 










 


 









 






 
 
 










 


 

 
 
 
 
 

 
 
 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 

 
 
 
 
 

 

 

 

 
 
 

 

 
 
 

 

 

 


 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 

 

 

 

 

 

 


 


 
 

 
 


 


 
  

 


 



 


 


 

 
 

 
 

 


 




 


 


 

  
    
  


 




 

 


 

 


 
























 


 

































 
 

 


extern int pShortOut;






 

 



 






 


 


 













 






 



 













































































 









 

 

 


 

 


 









 
 

 
 
 













 










extern "C" 
{

extern void dReportError(const char* fmt, ...);



}


















 
 









 

















 






 





















 















































 













 





 



 


 


 






 







































 








 








 

 




















 




















 









 







 






 





 








 

 








 


































 








 





 


 



 




 



 









 





 

 



 

 






























typedef unsigned int size_t;



















 




 



























 































 





 















extern "C" 
{ 

 
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





 



 





 

extern int __ctype_get_mb_cur_max  (void)  ;



 
extern double atof  (__const char *__nptr)  ;

 
extern int atoi  (__const char *__nptr)  ;

 
extern long int atol  (__const char *__nptr)  ;



 
extern long long int atoll  (__const char *__nptr)  ;



 
extern double strtod  (__const char *__nptr, char **__endptr)  ;




 
extern long int strtol  (__const char *__nptr, char **__endptr,
			     int __base)  ;

 
extern unsigned long int strtoul  (__const char *__nptr,
				       char **__endptr, int __base)  ;



 
extern long long int strtoq  (__const char *__nptr, char **__endptr,
				  int __base)  ;

 
extern unsigned long long int strtouq  (__const char *__nptr,
					    char **__endptr, int __base)  ;




 

 
extern long long int strtoll  (__const char *__nptr, char **__endptr,
				   int __base)  ;

 
extern unsigned long long int strtoull  (__const char *__nptr,
					     char **__endptr, int __base)  ;





 


extern double __strtod_internal  (__const char *__nptr,
				      char **__endptr, int __group)  ;

extern float __strtof_internal  (__const char *__nptr, char **__endptr,
				     int __group)  ;

extern long double  __strtold_internal  (__const char *__nptr,
						char **__endptr, int __group)  ;

extern long int __strtol_internal  (__const char *__nptr, char **__endptr,
					int __base, int __group)  ;

extern unsigned long int __strtoul_internal  (__const char *__nptr,
						  char **__endptr, int __base,
						  int __group)  ;


extern long long int __strtoq_internal  (__const char *__nptr,
					     char **__endptr, int __base,
					     int __group)  ;

extern unsigned long long int __strtouq_internal  (__const char *__nptr,
						       char **__endptr,
						       int __base,
						       int __group)  ;




 

extern __inline double strtod (__const char *__nptr, char **__endptr)

{ return __strtod_internal (__nptr, __endptr, 0);
 
}
extern __inline long int strtol (__const char *__nptr,
				 char **__endptr, int __base)

{ return __strtol_internal (__nptr, __endptr, __base, 0);
 
}
extern __inline unsigned long int strtoul (__const char *__nptr,
					   char **__endptr, int __base)

{ return __strtoul_internal (__nptr, __endptr, __base, 0);
 
}




extern __inline long long int strtoq (__const char *__nptr, char **__endptr,
				      int __base)

{ return __strtoq_internal (__nptr, __endptr, __base, 0);
 
}
extern __inline unsigned long long int strtouq (__const char *__nptr,
						char **__endptr, int __base)

{ return __strtouq_internal (__nptr, __endptr, __base, 0);
 
}



extern __inline long long int strtoll (__const char *__nptr, char **__endptr,
				       int __base)

{ return __strtoq_internal (__nptr, __endptr, __base, 0);
 
}
extern __inline unsigned long long int strtoull (__const char *__nptr,
						 char **__endptr, int __base)

{ return __strtouq_internal (__nptr, __endptr, __base, 0);
 
}


extern __inline double atof (__const char *__nptr)

{ return strtod (__nptr, (char **) ((void *)0) );
 
}
extern __inline int atoi (__const char *__nptr)

{ return (int) strtol (__nptr, (char **) ((void *)0) , 10);
 
}
extern __inline long int atol (__const char *__nptr)

{ return strtol (__nptr, (char **) ((void *)0) , 10);
 
}


extern __inline long long int atoll (__const char *__nptr)

{ return strtoll (__nptr, (char **) ((void *)0) , 10);
 
}





 


extern char *l64a  (long int __n)  ;


 
extern long int a64l  (__const char *__s)  ;



 

















 








extern "C" 
{ 

 






















 
typedef unsigned char __u_char;

typedef unsigned short __u_short;

typedef unsigned int __u_int;

typedef unsigned long __u_long;


typedef unsigned long long int __u_quad_t;

typedef long long int __quad_t;


typedef __quad_t *__qaddr_t;


typedef __u_quad_t __dev_t;
		 
typedef __u_int __uid_t;
		 
typedef __u_int __gid_t;
		 
typedef __u_long __ino_t;
		 
typedef __u_int __mode_t;
		 
typedef __u_int __nlink_t;
 		 
typedef long int __off_t;
		 
typedef __quad_t __loff_t;
		 
typedef int __pid_t;
			 
typedef int __ssize_t;
			 

typedef struct
  
{
    int __val[2];

  
} __fsid_t;
				 

 
typedef int __daddr_t;
			 
typedef char *__caddr_t;

typedef long int __time_t;

typedef long int __swblk_t;
		 

typedef long int __clock_t;


 
typedef unsigned long int __fd_mask;


 


 




 
typedef struct
  
{
     
    __fd_mask fds_bits[1024  / (8 * sizeof (__fd_mask)) ];

  
} __fd_set;



typedef int __key_t;


typedef unsigned short int __ipc_pid_t;






typedef __u_char u_char;

typedef __u_short u_short;

typedef __u_int u_int;

typedef __u_long u_long;

typedef __quad_t quad_t;

typedef __u_quad_t u_quad_t;

typedef __fsid_t fsid_t;



typedef __dev_t dev_t;

typedef __gid_t gid_t;

typedef __ino_t ino_t;

typedef __mode_t mode_t;

typedef __nlink_t nlink_t;

typedef __off_t off_t;

typedef __loff_t loff_t;

typedef __pid_t pid_t;

typedef __uid_t uid_t;



typedef __ssize_t ssize_t;





typedef __daddr_t daddr_t;

typedef __caddr_t caddr_t;




typedef __key_t key_t;




 

















 






















 
typedef __time_t time_t;

























 





 


 



 




 



 









 





 

 



 

 







 




 



 





 













 
typedef unsigned long int ulong;

typedef unsigned short int ushort;

typedef unsigned int uint;



 



 



typedef int int8_t __attribute__ ((__mode__ (  __QI__ ))) ;

typedef unsigned int u_int8_t __attribute__ ((__mode__ (  __QI__ ))) ;

typedef int int16_t __attribute__ ((__mode__ (  __HI__ ))) ;

typedef unsigned int u_int16_t __attribute__ ((__mode__ (  __HI__ ))) ;

typedef int int32_t __attribute__ ((__mode__ (  __SI__ ))) ;

typedef unsigned int u_int32_t __attribute__ ((__mode__ (  __SI__ ))) ;

typedef int int64_t __attribute__ ((__mode__ (  __DI__ ))) ;

typedef unsigned int u_int64_t __attribute__ ((__mode__ (  __DI__ ))) ;


typedef int register_t __attribute__ ((__mode__ (__word__)));



 






 
 





















 









 
 














 
 


















 






 


 
 
































 

 

















 
























 

struct timespec
  
{
    long int tv_sec;
		 
    long int tv_nsec;
		 
  
};















extern "C" 
{ 

 



struct timeval;


typedef __fd_mask fd_mask;


 
typedef __fd_set fd_set;


 



 




 






 




extern int __select  (int __nfds, __fd_set *__readfds,
			  __fd_set *__writefds, __fd_set *__exceptfds,
			  struct timeval *__timeout)  ;

extern int select  (int __nfds, __fd_set *__readfds,
			__fd_set *__writefds, __fd_set *__exceptfds,
			struct timeval *__timeout)  ;


 


extern int __pselect  (int __nfds, __fd_set *__readfds,
			   __fd_set *__writefds, __fd_set *__exceptfds,
			   struct timespec *__timeout)  ;

extern int pselect  (int __nfds, __fd_set *__readfds,
			 __fd_set *__writefds, __fd_set *__exceptfds,
			 struct timespec *__timeout)  ;




} 







} 




 



 
extern int32_t __random  (void)  ;

extern int32_t random  (void)  ;


 
extern void __srandom  (unsigned int __seed)  ;

extern void srandom  (unsigned int __seed)  ;


 



extern void *  __initstate  (unsigned int __seed, void *  __statebuf,
				 size_t __statelen)  ;

extern void *  initstate  (unsigned int __seed, void *  __statebuf,
			       size_t __statelen)  ;


 

extern void *  __setstate  (void *  __statebuf)  ;

extern void *  setstate  (void *  __statebuf)  ;




 



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


extern int __random_r  (struct random_data *__buf, int32_t *__result)  ;

extern int random_r  (struct random_data *__buf, int32_t *__result)  ;


extern int __srandom_r  (unsigned int __seed, struct random_data *__buf)  ;

extern int srandom_r  (unsigned int __seed, struct random_data *__buf)  ;


extern int __initstate_r  (unsigned int __seed, void *  __statebuf,
			       size_t __statelen, struct random_data *__buf)  ;

extern int initstate_r  (unsigned int __seed, void *  __statebuf,
			     size_t __statelen, struct random_data *__buf)  ;


extern int __setstate_r  (void *  __statebuf, struct random_data *__buf)  ;

extern int setstate_r  (void *  __statebuf, struct random_data *__buf)  ;





 
extern int rand  (void)  ;

 
extern void srand  (unsigned int __seed)  ;



 
extern int __rand_r  (unsigned int *__seed)  ;

extern int rand_r  (unsigned int *__seed)  ;





 

 
extern double drand48  (void)  ;

extern double erand48  (unsigned short int __xsubi[3])  ;


 
extern long lrand48  (void)  ;

extern long nrand48  (unsigned short int __xsubi[3])  ;


 
extern long mrand48  (void)  ;

extern long jrand48  (unsigned short int __xsubi[3])  ;


 
extern void srand48  (long __seedval)  ;

extern unsigned short int *seed48  (unsigned short int __seed16v[3])  ;

extern void lcong48  (unsigned short int __param[7])  ;


 
struct drand48_data
  
{
    unsigned short int x[3];
	 
    unsigned short int a[3];
	 
    unsigned short int c;
	 
    unsigned short int old_x[3];
  
    int init;
			 
  
};



 
extern int drand48_r  (struct drand48_data *__buffer, double *__result)  ;

extern int erand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__buffer, double *__result)  ;


 
extern int lrand48_r  (struct drand48_data *__buffer, long *__result)  ;

extern int nrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__buffer, long *__result)  ;


 
extern int mrand48_r  (struct drand48_data *__buffer, long *__result)  ;

extern int jrand48_r  (unsigned short int __xsubi[3],
			   struct drand48_data *__buffer, long *__result)  ;


 
extern int srand48_r  (long __seedval, struct drand48_data *__buffer)  ;

extern int seed48_r  (unsigned short int __seed16v[3],
			  struct drand48_data *__buffer)  ;

extern int lcong48_r  (unsigned short int __param[7],
			   struct drand48_data *__buffer)  ;



 
extern int __drand48_iterate  (unsigned short int __xsubi[3],
				   struct drand48_data *__buffer)  ;




 
extern void *  malloc  (size_t __size)  ;

 

extern void *  realloc  (void *  __ptr, size_t __size)  ;

 
extern void *  calloc  (size_t __nmemb, size_t __size)  ;

 
extern void free  (void *  __ptr)  ;



 
extern void cfree  (void *  __ptr)  ;




 




























 





 


 



 




 



 









 





 

 



 

 







 




 



 





 












extern "C" 
{ 

 



 
extern void *  __alloca  (size_t __size)  ;

extern void *  alloca  (size_t __size)  ;








} 






 
extern void *  valloc  (size_t __size)  ;




 
extern void abort  (void)   __attribute__ ((__noreturn__));



 
extern int atexit  (void (*__func) (void))  ;



 

extern int __on_exit  (void (*__func) (int __status, void *  __arg),
			   void *  __arg)  ;

extern int on_exit  (void (*__func) (int __status, void *  __arg),
			 void *  __arg)  ;



 


extern void exit  (int __status)   __attribute__ ((__noreturn__));



 
extern char *getenv  (__const char *__name)  ;


 

extern char *__secure_getenv  (__const char *__name)  ;



 
 

extern int putenv  (__const char *__string)  ;




 

extern int setenv  (__const char *__name, __const char *__value,
			int __replace)  ;


 
extern void unsetenv  (__const char *__name)  ;




 


extern int __clearenv  (void)  ;

extern int clearenv  (void)  ;





 



extern char *mktemp  (char *__template)  ;


 




extern int mkstemp  (char *__template)  ;




 
extern int system  (__const char *__command)  ;






 





extern char *realpath  (__const char *__name, char *__resolved)  ;




 


typedef int (*__compar_fn_t)  (__const void * , __const void * )  ;





 

extern void *  bsearch  (__const void *  __key, __const void *  __base,
			     size_t __nmemb, size_t __size,
			     __compar_fn_t __compar)  ;


 

extern void qsort  (void *  __base, size_t __nmemb, size_t __size,
			__compar_fn_t __compar)  ;



 
extern int abs  (int __x)   __attribute__ ((__const__));

extern long int labs  (long int __x)   __attribute__ ((__const__));




 

 
extern div_t div  (int __numer, int __denom)   __attribute__ ((__const__));

extern ldiv_t ldiv  (long int __numer, long int __denom)   __attribute__ ((__const__));





 


 


extern char *ecvt  (double __value, int __ndigit, int *__decpt,
			int *__sign)  ;


 


extern char *fcvt  (double __value, int __ndigit, int *__decpt,
			int *__sign)  ;


 


extern char *gcvt  (double __value, int __ndigit, char *__buf)  ;


 
extern char *qecvt  (long double  __value, int __ndigit, int *__decpt,
			 int *__sign)  ;

extern char *qfcvt  (long double  __value, int __ndigit, int *__decpt,
			 int *__sign)  ;

extern char *qgcvt  (long double  __value, int __ndigit, char *__buf)  ;




 

extern int ecvt_r  (double __value, int __ndigit, int *__decpt,
			int *__sign, char *__buf, size_t __len)  ;

extern int fcvt_r  (double __value, int __ndigit, int *__decpt,
			int *__sign, char *__buf, size_t __len)  ;


extern int qecvt_r  (long double  __value, int __ndigit, int *__decpt,
			 int *__sign, char *__buf, size_t __len)  ;

extern int qfcvt_r  (long double  __value, int __ndigit, int *__decpt,
			 int *__sign, char *__buf, size_t __len)  ;





 

extern int mblen  (__const char *__s, size_t __n)  ;

 

extern int mbtowc  (wchar_t *__pwc, __const char *__s, size_t __n)  ;

 

extern int wctomb  (char *__s, wchar_t __wchar)  ;



 
extern size_t mbstowcs  (wchar_t *__pwcs, __const char *__s, size_t __n)  ;

 
extern size_t wcstombs  (char *__s, __const wchar_t *__pwcs, size_t __n)  ;




 



extern int rpmatch  (__const char *__response)  ;











} 



 



















 







 























 





 





 










 





 


 



 




 



 









 





 

 



 

 







 




 



 





 















 




typedef unsigned int wint_t;










typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));

typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));

typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));

typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));





 










 




 


























 

 




















 




typedef void *__gnuc_va_list;




 



























 























 
























 



















struct _IO_jump_t;
  struct _IO_FILE;


 

typedef void _IO_lock_t;




 

struct _IO_marker 
{
  struct _IO_marker *_next;

  struct _IO_FILE *_sbuf;

   

   
  int _pos;



};


struct _IO_FILE 
{
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

  int _blksize;

  __off_t   _offset;



   
  unsigned short _cur_column;

  char _unused;

  char _shortbuf[1];


   

  _IO_lock_t *_lock;


};




struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_stdin_, _IO_stdout_, _IO_stderr_;






 
typedef struct

{
  __ssize_t   (*read)  (struct _IO_FILE *, void *, __ssize_t  )  ;

  __ssize_t   (*write)  (struct _IO_FILE *, const void *, __ssize_t  )  ;

  __off_t   (*seek)  (struct _IO_FILE *, __off_t  , int)  ;

  int (*close)  (struct _IO_FILE *)  ;


} _IO_cookie_io_functions_t;


 
struct _IO_cookie_file 
{
  struct _IO_FILE file;

  const void *vtable;

  void *cookie;

  _IO_cookie_io_functions_t io_functions;


};




extern "C" 
{


extern int __underflow  (_IO_FILE*)  ;

extern int __uflow  (_IO_FILE*)  ;

extern int __overflow  (_IO_FILE*, int)  ;










extern int _IO_getc  (_IO_FILE *__fp)  ;

extern int _IO_putc  (int __c, _IO_FILE *__fp)  ;

extern int _IO_feof  (_IO_FILE *__fp)  ;

extern int _IO_ferror  (_IO_FILE *__fp)  ;


extern int _IO_peekc_locked  (_IO_FILE *__fp)  ;


 


extern void _IO_flockfile  (_IO_FILE *)  ;

extern void _IO_funlockfile  (_IO_FILE *)  ;

extern int _IO_ftrylockfile  (_IO_FILE *)  ;










 










extern int _IO_vfscanf  (_IO_FILE*, const char*, __gnuc_va_list , int*)  ;

extern int _IO_vfprintf  (_IO_FILE*, const char*, __gnuc_va_list )  ;

extern __ssize_t   _IO_padn  (_IO_FILE *, int, __ssize_t  )  ;

extern size_t   _IO_sgetn  (_IO_FILE *, void*, size_t  )  ;


extern __off_t   _IO_seekoff  (_IO_FILE*, __off_t  , int, int)  ;

extern __off_t   _IO_seekpos  (_IO_FILE*, __off_t  , int)  ;


extern void _IO_free_backup_area  (_IO_FILE*)  ;




}




















  


typedef struct _IO_FILE FILE;

typedef __off_t   fpos_t;






















 



 
extern FILE *stdin, *stdout, *stderr;
  






extern "C" 
{




extern void clearerr  (FILE*)  ;

extern int fclose  (FILE*)  ;

extern int feof  (FILE*)  ;

extern int ferror  (FILE*)  ;

extern int fflush  (FILE*)  ;

extern int fgetc  (FILE *)  ;

extern int fgetpos  (FILE* fp, fpos_t *__pos)  ;

extern char* fgets  (char*, int, FILE*)  ;

extern FILE* fopen  (__const char*, __const char*)  ;

extern FILE* fopencookie  (void *__cookie, __const char *__mode,
			       _IO_cookie_io_functions_t __io_functions)  ;

extern int fprintf  (FILE*, __const char* __format, ...)  ;

extern int fputc  (int, FILE*)  ;

extern int fputs  (__const char *__str, FILE *__fp)  ;

extern size_t fread  (void*, size_t, size_t, FILE*)  ;

extern FILE* freopen  (__const char*, __const char*, FILE*)  ;

extern int fscanf  (FILE *__fp, __const char* __format, ...)  ;

extern int fseek  (FILE* __fp, long int __offset, int __whence)  ;

extern int fsetpos  (FILE* __fp, __const fpos_t *__pos)  ;

extern long int ftell  (FILE* __fp)  ;

extern size_t fwrite  (__const void*, size_t, size_t, FILE*)  ;

extern int getc  (FILE *)  ;

extern int getchar  (void)  ;

extern char* gets  (char*)  ;

extern void perror  (__const char *)  ;

extern int printf  (__const char* __format, ...)  ;

extern int putc  (int, FILE *)  ;

extern int putchar  (int)  ;

extern int puts  (__const char *__str)  ;

extern int remove  (__const char*)  ;

extern int rename  (__const char* __old, __const char* __new)  ;

extern void rewind  (FILE*)  ;

extern int scanf  (__const char* format, ...)  ;

extern void setbuf  (FILE*, char*)  ;

extern void setlinebuf  (FILE*)  ;

extern void setbuffer  (FILE*, char*, int)  ;

extern int setvbuf  (FILE*, char*, int __mode, size_t __size)  ;

extern int sprintf  (char*, __const char* __format, ...)  ;

extern int sscanf  (__const char* string, __const char* __format, ...)  ;

extern FILE* tmpfile  (void)  ;

extern char* tmpnam  (char*)  ;


extern char* tmpnam_r  (char*)  ;



extern char *tempnam  (__const char *__dir, __const char *__pfx)  ;


extern char *__stdio_gen_tempname  (char *__buf, size_t bufsize,
					__const char *dir, __const char *pfx,
					int dir_search, size_t *lenptr,
					FILE **streamptr)  ;

extern int ungetc  (int c, FILE* fp)  ;

extern int vfprintf  (FILE *fp, char __const *fmt0, __gnuc_va_list )  ;

extern int vprintf  (char __const *fmt, __gnuc_va_list )  ;

extern int vsprintf  (char* string, __const char* format, __gnuc_va_list )  ;

extern void __libc_fatal  (__const char *__message)  
     __attribute__ ((__noreturn__));



extern int dprintf  (int, __const char *, ...)  ;

extern int vdprintf  (int, __const char *, __gnuc_va_list )  ;

extern int vfscanf  (FILE*, __const char *, __gnuc_va_list )  ;

extern int __vfscanf  (FILE*, __const char *, __gnuc_va_list )  ;

extern int vscanf  (__const char *, __gnuc_va_list )  ;

extern int vsscanf  (__const char *, __const char *, __gnuc_va_list )  ;

extern int __vsscanf  (__const char *, __const char *, __gnuc_va_list )  ;






extern FILE *fdopen  (int, __const char *)  ;

extern int fileno  (FILE*)  ;

extern FILE* popen  (__const char*, __const char*)  ;

extern int pclose  (FILE*)  ;





 
extern char *ctermid  (char *__buf)  ;


 
extern char *cuserid  (char * __buf)  ;




extern int snprintf  (char *, size_t, __const char *, ...)  ;

extern int __snprintf  (char *, size_t, __const char *, ...)  ;

extern int vsnprintf  (char *, size_t, __const char *, __gnuc_va_list )  ;

extern int __vsnprintf  (char *, size_t, __const char *, __gnuc_va_list )  ;





extern int __underflow  (struct _IO_FILE*)  ;

extern int __overflow  (struct _IO_FILE*, int)  ;



extern int sys_nerr;

extern const char *const sys_errlist[];




 

extern void clearerr_locked  (FILE *)  ;

extern void clearerr_unlocked  (FILE *)  ;

extern int feof_locked  (FILE *)  ;

extern int feof_unlocked  (FILE *)  ;

extern int ferror_locked  (FILE*)  ;

extern int ferror_unlocked  (FILE*)  ;

extern int fileno_locked  (FILE *)  ;

extern int fileno_unlocked  (FILE *)  ;

extern int fclose_unlocked  (FILE *)  ;

extern int fflush_locked  (FILE *)  ;

extern int fflush_unlocked  (FILE *)  ;

extern size_t fread_unlocked  (void *, size_t, size_t, FILE *)  ;

extern size_t fwrite_unlocked  (const void *, size_t, size_t, FILE *)  ;


extern int fputc_locked  (int, FILE*)  ;

extern int fputc_unlocked  (int, FILE*)  ;

extern int getc_locked  (FILE *)  ;

extern int getchar_locked  (void)  ;

extern int putc_locked  (int, FILE *)  ;

extern int putchar_locked  (int)  ;










 
extern void flockfile  (FILE *)  ;

extern void funlockfile  (FILE *)  ;

extern int ftrylockfile  (FILE *)  ;


extern int getc_unlocked  (FILE *)  ;

extern int getchar_unlocked  (void)  ;

extern int putc_unlocked  (int, FILE *)  ;

extern int putchar_unlocked  (int)  ;


















}








extern "C" 
{



 
 
 
 









 


 




 



 



 



 



 

 

 



 

 

 

 

 



 

 

 



 

 

 



 



 



 



 

 

 




 



 


 



 

 


 



 
 


 
 


 
 

 

 

 

 

 



 

 

 



 



 



 

 

 







 
 











 






 



























 




 




 







 
































 
 









 




enum omError_e

{
  omError_NoError = 0,
  omError_Unknown,
  omError_InternalBug,
  omError_MemoryCorrupted,
  omError_NullAddr,
  omError_InvalidRangeAddr,
  omError_FalseAddr,
  omError_FalseAddrOrMemoryCorrupted,
  omError_WrongSize,
  omError_FreedAddr,
  omError_FreedAddrOrMemoryCorrupted,
  omError_WrongBin,
  omError_UnknownBin,
  omError_NotBinAddr,
  omError_UnalignedAddr,
  omError_NullSizeAlloc,
  omError_ListCycleError,
  omError_SortedListError,
  omError_KeptAddrListCorrupted,
  omError_FreePattern,
  omError_BackPattern,
  omError_FrontPattern,
  omError_NotString,
  omError_MaxError

};

typedef enum omError_e omError_t;


 
extern omError_t om_ErrorStatus;

 
extern omError_t om_InternalErrorStatus;

 
const char* omError2String(omError_t error);

 
const char* omError2Serror(omError_t error);

extern omError_t omReportError(omError_t error, omError_t report_error,     void* r ,  
                               const char* fmt, ...);





 
 









struct omBin_s;

typedef struct omBin_s      omBin_t;

typedef omBin_t*            omBin;


struct omBinPage_s;

typedef struct omBinPage_s  omBinPage_t;

typedef omBinPage_t*        omBinPage;


struct omBinPageRegion_s;

typedef struct omBinPageRegion_s omBinPageRegion_t;

typedef omBinPageRegion_t* omBinPageRegion;


struct omOpts_s;

typedef struct omOpts_s omOpts_t;


struct omInfo_s;

typedef struct omInfo_s omInfo_t;


struct omSpecBin_s;

typedef struct omSpecBin_s omSpecBin_t;

typedef omSpecBin_t*        omSpecBin;


struct omRetInfo_s;

typedef struct omRetInfo_s omRetInfo_t;

typedef omRetInfo_t*        omRetInfo;





 
 





















 

















































































 










                                                        

























 
 


 





omError_t omTestAddrBin(void* addr, omBin bin, int check_level);

omError_t omTestBinAddr(void* addr, int check_level);

omError_t omTestAddrSize(void* addr, size_t size, int check_level);

omError_t omTestAddr(void* addr, int check_level);

omError_t omtestAddrSize(void* addr, size_t size, int check_level);

omError_t omtestAddr(void* addr, int check_level);

omError_t omTestAddrAlignedBin(void* addr, omBin bin, int check_level);

omError_t omTestAddrAlignedSize(void* addr, size_t size, int check_level);

omError_t omTestAddrAligned(void* addr, int check_level);

omError_t omtestAddrAlignedSize(void* addr, size_t size, int check_level);

omError_t omtestAddrAligned(void* addr, int check_level);

omError_t omTestBin(omBin bin, int check_level);

omError_t omTestMemory(int check_level);








 
 










static inline  omBin omGetBinOfPage(omBinPage page);

static inline  int _omIsBinPageAddr(void* addr);


static inline  void* _omAllocBin(omBin bin);

static inline  void* _omAlloc0Bin(omBin bin);

static inline  void* _omReallocBin(void* addr, omBin old_bin, omBin new_bin);

static inline  void* _omRealloc0Bin(void* addr, omBin old_bin, omBin new_bin);


static inline  void* _omAlloc(size_t size);

static inline  void* _omAlloc0(size_t size);

static inline  void* _omReallocSize(void* addr, size_t old_size, size_t new_size);

static inline  void* _omRealloc0Size(void* addr, size_t old_size, size_t new_size);

static inline  void* _omRealloc(void* addr, size_t size);

static inline  void* _omRealloc0(void* addr, size_t size);


static inline  void* _omalloc(size_t size);

static inline  void* _omalloc0(size_t size);

static inline  void* _omreallocSize(void* addr, size_t old_size, size_t new_size);

static inline  void* _omrealloc0Size(void* addr, size_t old_size, size_t new_size);

static inline  void* _omrealloc(void* addr, size_t size);

static inline  void* _omrealloc0(void* addr, size_t size);


static inline  char* _omStrDup(const char* s);

static inline  void* _omMemDup(void* addr);








 
 













omBin _omGetSpecBin(size_t size, int align, int track);




void  _omUnGetSpecBin(omBin *bin, int force);

size_t omGetUsedBinBytes();


unsigned long omGetNewStickyBinTag(omBin bin);

void omSetStickyBinTag(omBin bin, unsigned long sticky);

void omUnSetStickyBinTag(omBin bin, unsigned long sticky);

void omDeleteStickyBinTag(omBin bin, unsigned long sticky);


unsigned long omGetNewStickyAllBinTag();

void omSetStickyAllBinTag(unsigned long sticky);

void omUnSetStickyAllBinTag(unsigned long sticky);

void omDeleteStickyAllBinTag(unsigned long sticky);


void omPrintBinStats(FILE* fd);






 
 


































 
 












 




 
int _omListLength(void* list, int next);

 
void* _omListLast(void* list, int next);

 


void* _omListHasCycle(void* list, int next);

 

void* _omIsOnList(void* list, int next, void* addr);

 
void* _omRemoveFromList(void* list, int next, void* addr);

 


 
void* _omFindInList(void* list, int next, int long_field, 
                    unsigned long what);

 


 
void* _omFindInSortedList(void* list, int next, int long_field, 
                          unsigned long what);

 
void* _omRemoveFromSortedList(void* list,int next,int long_field,
                                  void* addr);

 
void* _omInsertInSortedList(void* list, int next, int long_field, 
                            void* addr);


 
omError_t _omCheckList(void* list, int next, int level, omError_t report,     void* r );

 
omError_t _omCheckSortedList(void* list, int next, int long_field, int level, omError_t report,     void* r );



 


















 




















 
 









 
 
char* omFindExec(const char* prog, char* exec);




 
 










void omInitGetBackTrace();

int omGetBackTrace(void** bt, int start, int max);





 
 










struct omRetInfo_s

{
  void* addr;

  char  func[100];
  
  char  file[100];
  
  int   line;


};


 









int omPrintRetInfo(omRetInfo info, int max, FILE *fd, const char* fmt);

 

int omBackTrace_2_RetInfo(void** bt, omRetInfo info, int max);


 
void omInitRet_2_Info(const char* argv0);


int omPrintBackTrace(void** bt, int max, FILE* fd);

int omPrintCurrentBackTrace(FILE* fd);


int omFilterRetInfo_i(omRetInfo info, int max, int i);










 
 









struct omInfo_s

{
  long MaxBytesSystem;
       
  long CurrentBytesSystem;
   
  long MaxBytesSbrk;
         
  long CurrentBytesSbrk;
     
  long MaxBytesMmap;
         
  long CurrentBytesMmap;
     
  long UsedBytes;
             
  long AvailBytes;
            
  long UsedBytesMalloc;
       
  long AvailBytesMalloc;
      
  long MaxBytesFromMalloc;
       
  long CurrentBytesFromMalloc;
   
  long MaxBytesFromValloc;
       
  long CurrentBytesFromValloc;
   
  long UsedBytesFromValloc;
  
  long AvailBytesFromValloc;
 
  long MaxPages;
             
  long UsedPages;
            
  long AvailPages;
           
  long MaxRegionsAlloc;
      
  long CurrentRegionsAlloc;
  

};


 
extern struct omInfo_s omGetInfo();

 
 
extern struct omInfo_s om_Info;

 
extern void omUpdateInfo();

 
extern void omInitInfo();

extern void omPrintStats(FILE* fd);

extern void omPrintInfo(FILE* fd);





 
 









 
struct omOpts_s

{
  int MinTrack;

  int MinCheck;

  int Keep;

  int HowToReportErrors;

  int MarkAsStatic;

  unsigned int PagesPerRegion;

  void (*OutOfMemoryFunc)();

  void (*MemoryLowFunc)();


};

extern omOpts_t om_Opts;





 
 









 















 





 

















extern unsigned long om_MaxBinPageIndex;

extern unsigned long om_MinBinPageIndex;

extern unsigned long *om_BinPageIndicies;

















 
 










size_t omSizeOfAddr(void* addr);

size_t omSizeWOfAddr(void* addr);


size_t omSizeOfLargeAddr(void* addr);



void* omAllocFromSystem(size_t size);

void* omReallocFromSystem(void* addr, size_t newsize);

void  omFreeToSystem(void* addr);


void* omReallocSizeFromSystem(void* addr, size_t oldsize, size_t newsize);

void  omFreeSizeToSystem(void* addr, size_t size);



void* _omVallocFromSystem(size_t size, int fail);

void omVfreeToSystem(void* page, size_t size);








void* omRealloc0Large(void* old_addr, size_t new_size);

void* omAlloc0Large(size_t size);




 








 
 










 




 
struct omBinPage_s

{
  long          used_blocks;
     
  void*         current;
         
  omBinPage     next;
            
  omBinPage     prev;

  void*         bin_sticky;
      

  omBinPageRegion region;
        

};


 
 



 

struct omBin_s

{
  omBinPage     current_page;
    
  omBinPage     last_page;
       
  omBin         next;
            
  size_t        sizeW;
           
  long          max_blocks;
      

  unsigned long sticky;
          

};


struct omSpecBin_s

{
  omSpecBin        next;
        
  omBin            bin;
         
  long             max_blocks;
  
  long             ref;
         

};


extern  omSpecBin om_SpecBin;

extern  omBinPage_t om_ZeroPage[];

extern  omBin       om_Size2Bin[];


 














extern omBin_t om_StaticBin[];

extern omBin om_Size2Bin[];



 











 




extern void* omAllocBinFromFullPage(omBin bin);

extern void  omFreeToPageFault(omBinPage page, void* addr);


 
 





 
 












 
 










      




void* omDoRealloc(void* old_addr, size_t new_size, int flags);





















 
 



























 


typedef unsigned short omTrackFlags_t;


void* _omDebugAlloc(void* size_bin, omTrackFlags_t flags, char check, char track     );

void* _omDebugRealloc(void* old_addr, void* old_size_bin, void* new_size_bin,
                      omTrackFlags_t old_flags, omTrackFlags_t new_flags, char check, char track     );

void  _omDebugFree(void* addr, void* size_bin, omTrackFlags_t flags, char check     );

void* _omDebugMemDup(void* addr, omTrackFlags_t flags, char check, char track     );

char* _omDebugStrDup(const char* addr, char track     );


omError_t _omDebugBin(omBin bin, char check     );

omError_t _omDebugMemory(char check     );

omError_t _omDebugAddr(void* addr, void* bin_size, omTrackFlags_t flags, char check     );


void omFreeKeptAddr();

void omPrintUsedAddrs(FILE* fd);

void omPrintUsedTrackAddrs(FILE* fd);


void omMarkAsStaticAddr(void* addr);

void omMarkMemoryAsStatic();

void omUnMarkAsStaticAddr(void* addr);

void omUnMarkMemoryAsStatic();






void omSetCustomOfTrackAddr(void* addr, void* value);

void* omGetCustomOfTrackAddr(void* addr);









 
 












static inline  omBin omGetBinOfPage(omBinPage page)

{
  unsigned long sticky = (((unsigned long) (( page )->bin_sticky)) & (4 -1)) ;

  omBin bin = ((omBin) ( ((unsigned long) (( page )->bin_sticky)) & ~(4  - 1))) ;

  
  while (bin->sticky != sticky && bin->next != ((void *)0) ) 
  
{
    bin = bin->next;

  
}
  return bin;


}

static inline  int _omIsBinPageAddr(void* addr)

{
  unsigned long index = (((unsigned long)  addr ) >> (5  + 12 )) ;

  if (index >= om_MinBinPageIndex && index <= om_MaxBinPageIndex)
  
{
    unsigned long shift = ((((unsigned long)  addr ) & ((4096  << 5 )  -1)) >> 12 ) ;

    return om_BinPageIndicies[index - om_MinBinPageIndex] & (1 << shift);

  
}
  return 0;


}

static inline  void* _omAllocBin(omBin bin)

{
  void* addr;

  do 
{ register omBinPage __om_page = (  bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
     addr   = (  void*  )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else   addr  = ( void* ) omAllocBinFromFullPage(  bin );
 
} while (0) ;

  return addr;


}
static inline  void* _omAlloc0Bin(omBin bin)

{
  void* addr;

  do 
{ do 
{ register omBinPage __om_page = (    bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       addr    = (   void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     addr   = (  void*  ) omAllocBinFromFullPage(    bin  );
 
} while (0) ;
 do 
{ long* _p1 = (long*) (   addr  );
 unsigned long _l =   (  bin )->sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 
} while (0) ;

  return addr;


}
static inline  void* _omReallocBin(void* addr, omBin old_bin, omBin new_bin)

{
  void* new_addr;
 
  do 
{ if (  old_bin  !=   new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(   addr   )  && ! (( ((omBinPage) ((long) (    addr    ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   old_bin ->sizeW : omSizeWOfAddr( addr ));
 do 
{ register omBinPage __om_page = (    new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       new_addr    = (    void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     new_addr   = (   void*  ) omAllocBinFromFullPage(    new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  new_bin ->sizeW > old_sizeW ? old_sizeW :   new_bin ->sizeW) ;
 long* _s1 = (long*) (   new_addr  );
 const long* _s2 = (long*) (   addr  );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (  addr  );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{   new_addr  = (  void* )  addr ;
 
} 
} while (0) ;

  return new_addr;


}
static inline  void* _omRealloc0Bin(void* addr, omBin old_bin, omBin new_bin)

{
  void* new_addr;
 
  do 
{ if (  old_bin  !=   new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(   addr   )  && ! (( ((omBinPage) ((long) (    addr    ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   old_bin ->sizeW : omSizeWOfAddr( addr ));
 do 
{ register omBinPage __om_page = (    new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       new_addr    = (    void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     new_addr   = (   void*  ) omAllocBinFromFullPage(    new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  new_bin ->sizeW > old_sizeW ? old_sizeW :   new_bin ->sizeW) ;
 long* _s1 = (long*) (   new_addr  );
 const long* _s2 = (long*) (   addr  );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 if (  new_bin ->sizeW > old_sizeW) do 
{ long* _p1 = (long*) ( (void**)  new_addr  + old_sizeW );
 unsigned long _l =     new_bin ->sizeW - old_sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (  addr  );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{   new_addr  = (  void* )  addr ;
 
} 
} while (0) ;

  return new_addr;


}
  

static inline  void* _omAlloc(size_t size)

{
  void* addr;

  do 
{ size_t __size =   size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       addr    = (   void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     addr   = (  void*  ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   addr  = ( void* ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

  return addr;


}
static inline  void* _omAlloc0(size_t size)

{
  void* addr;

  do 
{ size_t __size =   size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ do 
{ register omBinPage __om_page = (    __om_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
         addr     = (    void*    )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else       addr    = (   void*   ) omAllocBinFromFullPage(    __om_bin  );
 
} while (0) ;
 do 
{ long* _p1 = (long*) (     addr   );
 unsigned long _l =   (  __om_bin )->sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 
} while (0) ;
 
} else 
{   addr  = ( void* ) omAlloc0Large(__size);
 
} 
} while (0) ;

  return addr;


}
static inline  void* _omReallocSize(void* addr, size_t old_size, size_t new_size)

{
  void* new_addr;

  do 
{ size_t __new_size =     new_size  ;
 if (__new_size <= 1016  &&     old_size   <= 1016 ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*    ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*   )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*  ) omDoRealloc(  addr  , __new_size,   0 );
 
} 
} while (0)  ;

  return new_addr;


}
static inline  void* _omRealloc0Size(void* addr, size_t old_size, size_t new_size)

{
  void* new_addr;

  do 
{ size_t __new_size =     new_size  ;
 if (__new_size <= 1016  &&     old_size   <= 1016 ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*    ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 if (  __new_bin ->sizeW > old_sizeW) do 
{ long* _p1 = (long*) ( (void**)      new_addr    + old_sizeW );
 unsigned long _l =     __new_bin ->sizeW - old_sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*   )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*  ) omDoRealloc(  addr  , __new_size,   1 );
 
} 
} while (0)  ;

  return new_addr;


}
static inline  void* _omRealloc(void* addr, size_t size)

{
  void* new_addr;

  do 
{ size_t __new_size =     size  ;
 if (__new_size <= 1016  && _omIsBinPageAddr(   addr   ) ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*    ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*   )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*  ) omDoRealloc(  addr  , __new_size,   0 );
 
} 
} while (0)  ;

  return new_addr;


}
static inline  void* _omRealloc0(void* addr, size_t size)

{
  void* new_addr;

  do 
{ size_t __new_size =     size  ;
 if (__new_size <= 1016  && _omIsBinPageAddr(   addr   ) ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*    ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 if (  __new_bin ->sizeW > old_sizeW) do 
{ long* _p1 = (long*) ( (void**)      new_addr    + old_sizeW );
 unsigned long _l =     __new_bin ->sizeW - old_sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*   )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*  ) omDoRealloc(  addr  , __new_size,   1 );
 
} 
} while (0)  ;

  return new_addr;


}



static inline  char* _omStrDup(const char* s)

{
  char* r;

  int i=0;

  
  while (s[i]) i++;

  i++;

  do 
{ size_t __size =   i ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       r    = (   char*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     r   = (  char*  ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   r  = ( char* ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

  memcpy(r, s, i);

  return r;


}

static inline  void* _omMemDup(void* addr)

{
  void* r;

  if ((_omIsBinPageAddr(  addr  )  && ! (( ((omBinPage) ((long) (   addr   ) & ~(4096  -1)))  )->used_blocks < 0)  ) )
  
{
    omBin bin = ((omBin) ( ((unsigned long) (( ((omBinPage) ((void*) ((long) (  addr  ) & ~(4096  -1))) ) )->bin_sticky)) & ~(4  - 1)))  ;

    do 
{ register omBinPage __om_page = (  bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
     r   = (  void*  )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else   r  = ( void* ) omAllocBinFromFullPage(  bin );
 
} while (0) ;

    do 
{ long _i =   bin->sizeW ;
 long* _s1 = (long*) ( r );
 const long* _s2 = (long*) (  addr );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;

  
}
  else
  
{
    size_t size = omSizeWOfAddr(addr);

    do 
{ size_t __size =   size << 2  ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       r    = (   void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     r   = (  void*  ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   r  = ( void* ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

    do 
{ long _i =   size ;
 long* _s1 = (long*) ( r );
 const long* _s2 = (long*) (  addr );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;

  
}
  return r;


}

static inline  void* _omalloc(size_t size)

{
  void* addr;

  if (! size) size = 1;

  do 
{ size_t __size =  size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       addr    = (   void*   )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     addr   = (  void*  ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   addr  = ( void* ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

  return addr;


}

static inline  void* _omalloc0(size_t size)

{
  void* addr;

  if (! size) size = 1;

  do 
{ size_t __size =   size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ do 
{ register omBinPage __om_page = (    __om_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
        addr     = (    void*    )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else      addr    = (   void*   ) omAllocBinFromFullPage(    __om_bin  );
 
} while (0) ;
 do 
{ long* _p1 = (long*) (    addr   );
 unsigned long _l =   (  __om_bin )->sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 
} while (0) ;
 
} else 
{  addr  = ( void* ) omAlloc0Large(__size);
 
} 
} while (0) ;

  return addr;


}

static inline  void* _omreallocSize(void* addr, size_t old_size, size_t new_size)

{
  void* new_addr;

  
  if (!new_size) new_size = 1;

  if (addr != ((void *)0) )
  
{
    do 
{ size_t __new_size =     new_size  ;
 if (__new_size <= 1016  &&     old_size   <= 1016 ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*      )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*     ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*    )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*   ) omDoRealloc(  addr  , __new_size,   0 );
 
} 
} while (0)  ;

  
}
  else
  
{
    do 
{ size_t __size =   new_size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       new_addr    = (   void*    )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     new_addr   = (  void*   ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   new_addr  = ( void*  ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

  
}
  return new_addr;


}
  
static inline  void* _omrealloc0Size(void* addr, size_t old_size, size_t new_size)

{
  void* new_addr;

  
  if (!new_size) new_size = 1;

  if (addr != ((void *)0)  && old_size > 0)
  
{
    do 
{ size_t __new_size =     new_size  ;
 if (__new_size <= 1016  &&     old_size   <= 1016 ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*      )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*     ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 if (  __new_bin ->sizeW > old_sizeW) do 
{ long* _p1 = (long*) ( (void**)      new_addr    + old_sizeW );
 unsigned long _l =     __new_bin ->sizeW - old_sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*    )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*   ) omDoRealloc(  addr  , __new_size,   1 );
 
} 
} while (0)  ;

  
}
  else
  
{
    do 
{ size_t __size =   new_size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ do 
{ register omBinPage __om_page = (    __om_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
         new_addr     = (    void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else       new_addr    = (   void*    ) omAllocBinFromFullPage(    __om_bin  );
 
} while (0) ;
 do 
{ long* _p1 = (long*) (     new_addr   );
 unsigned long _l =   (  __om_bin )->sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 
} while (0) ;
 
} else 
{   new_addr  = ( void*  ) omAlloc0Large(__size);
 
} 
} while (0) ;

  
}
  return new_addr;


}
  
static inline  void* _omrealloc(void* addr, size_t size)

{
  void* new_addr;

  
  if (!size) size = 1;

  if (addr != ((void *)0) )
  
{
    do 
{ size_t __new_size =     size  ;
 if (__new_size <= 1016  && _omIsBinPageAddr(   addr   ) ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*      )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*     ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*    )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*   ) omDoRealloc(  addr  , __new_size,   0 );
 
} 
} while (0)  ;

  
}
  else
  
{
    do 
{ size_t __size =   size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ register omBinPage __om_page = (  __om_bin )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
       new_addr    = (   void*    )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else     new_addr   = (  void*   ) omAllocBinFromFullPage(  __om_bin );
 
} while (0) ;
 
} else 
{   new_addr  = ( void*  ) omAllocFromSystem(((((unsigned long)   __size  ) + 3 ) & (~3 )) ) ;
 
} 
} while(0) ;

  
}
  return new_addr;


}
  
static inline  void* _omrealloc0(void* addr, size_t size)

{
  void* new_addr;

  
  if (!size) size = 1;

  if (addr != ((void *)0) )
  
{
    do 
{ size_t __new_size =     size  ;
 if (__new_size <= 1016  && _omIsBinPageAddr(   addr   ) ) 
{ omBin __old_bin = omGetBinOfPage(((omBinPage) ((long) (    addr    ) & ~(4096  -1))) ) ;
 omBin __new_bin =   om_Size2Bin[(( __new_size ) -1)>> 2 ] ;
   do 
{ if (  __old_bin  !=   __new_bin ) 
{ size_t old_sizeW = ((_omIsBinPageAddr(     addr     )  && ! (( ((omBinPage) ((long) (      addr      ) & ~(4096  -1)))  )->used_blocks < 0)  )  ?   __old_bin ->sizeW : omSizeWOfAddr(   addr   ));
 do 
{ register omBinPage __om_page = (    __new_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
           new_addr      = (        void*      )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else         new_addr     = (       void*     ) omAllocBinFromFullPage(    __new_bin  );
 
} while (0) ;
 do 
{ long _i =   (  __new_bin ->sizeW > old_sizeW ? old_sizeW :   __new_bin ->sizeW) ;
 long* _s1 = (long*) (       new_addr    );
 const long* _s2 = (long*) (     addr    );
 for (;
;
) 
{ *_s1 = *_s2;
 _i--;
 if (_i == 0) break;
 _s1++;
 _s2++;
 
} 
} while(0) ;
 if (  __new_bin ->sizeW > old_sizeW) do 
{ long* _p1 = (long*) ( (void**)      new_addr    + old_sizeW );
 unsigned long _l =     __new_bin ->sizeW - old_sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 do 
{ register void* __om_addr = (void*) (    addr    );
 register omBinPage __om_page = ((omBinPage) ((long) ( __om_addr ) & ~(4096  -1))) ;
 do 
{ if ((  __om_page )->used_blocks > 0) 
{ *((void**) ( __om_addr )) = (  __om_page )->current;
 ((  __om_page )->used_blocks)--;
 (  __om_page )->current = ( __om_addr );
 
} else 
{ omFreeToPageFault(  __om_page ,  __om_addr );
 
} 
} while (0) ;
 
} while (0) ;
 
} else 
{       new_addr    = (      void*    )    addr   ;
 
} 
} while (0) ;
 
} else 
{     new_addr   = (    void*   ) omDoRealloc(  addr  , __new_size,   1 );
 
} 
} while (0)  ;

  
}
  else
  
{
    do 
{ size_t __size =   size ;
 if (__size <= 1016 ) 
{ omBin __om_bin = om_Size2Bin[(( __size ) -1)>> 2 ] ;
 do 
{ do 
{ register omBinPage __om_page = (    __om_bin  )->current_page;
 if (__om_page->current != ((void *)0) ) do 
{ ((  __om_page )->used_blocks)++;
         new_addr     = (    void*     )((  __om_page )->current);
 (  __om_page )->current =  *((void**) (  __om_page )->current);
 
} while (0) ;
 else       new_addr    = (   void*    ) omAllocBinFromFullPage(    __om_bin  );
 
} while (0) ;
 do 
{ long* _p1 = (long*) (     new_addr   );
 unsigned long _l =   (  __om_bin )->sizeW ;
 unsigned long _w =   0 ;
 while(_l) 
{ *_p1++ = _w;
 _l--;
 
} 
} while(0) ;
 
} while (0) ;
 
} else 
{   new_addr  = ( void*  ) omAlloc0Large(__size);
 
} 
} while (0) ;

  
}
  return new_addr;


}







}






 


 
 



 
typedef unsigned char  uchar ;

typedef unsigned short CARDINAL;

typedef short BOOLEAN;


typedef void * Sy_reference;




 
 



typedef long Exponent_t;

typedef unsigned long Order_t;


enum tHomog

{
   isNotHomog = 0 ,
   isHomog    = 1 ,
   testHomog

};

enum noeof_t

{
  noeof_brace = 1,
  noeof_asstring,
  noeof_block,
  noeof_bracket,
  noeof_comment,
  noeof_procname,
  noeof_string

};


 

class ip_smatrix;

class idrec;

class intvec;

class sleftv;

class slists;

class sattr;

class skStrategy;

class ssyStrategy;

class procinfo;

class namerec;

class kBucket;



struct  sip_sring;

struct  sip_sideal;

struct  sip_link;

struct  spolynom;

struct _ssubexpr;

struct _sssym;

struct snumber;

struct sip_command;

struct sip_package;

struct s_si_link_extension;

struct reca;

typedef struct _ssubexpr   sSubexpr;

typedef struct _sssym      ssym;

typedef struct spolyrec    polyrec;

typedef struct snumber     rnumber;

typedef struct sip_sideal  ip_sideal;

typedef struct sip_smap    ip_smap;

typedef struct sip_sring   ip_sring;

typedef struct sip_link    ip_link;

typedef struct sip_command ip_command;

typedef struct sip_package ip_package;


 
typedef char *              char_ptr;

typedef int  *              int_ptr;

typedef short *             short_ptr;

typedef void *              void_ptr;

typedef ip_sring *         ring;

typedef int                idtyp;

typedef rnumber *          number;

typedef polyrec *          poly;

typedef poly *             polyset;

typedef ip_sideal *        ideal;

typedef ip_smap *          map;

typedef ideal *            resolvente;

typedef union uutypes      utypes;

typedef ip_command *       command;

typedef struct s_si_link_extension *si_link_extension;

typedef struct reca *      alg;



typedef idrec *            idhdl;

typedef ip_smatrix *       matrix;

typedef ip_link *          si_link;

typedef sleftv *           leftv;

typedef slists *           lists;

typedef sSubexpr *         Subexpr;

typedef sattr *            attr;

typedef skStrategy *       kStrategy;

typedef ip_package *       package;

typedef ssyStrategy *      syStrategy;

typedef procinfo *         procinfov;

typedef namerec *          namehdl;

typedef kBucket*           kBucket_pt;

typedef struct p_Procs_s p_Procs_s;


 
typedef struct sindlist indlist;

typedef indlist * indset;

struct sindlist

{
  indset nx;

  intvec * set;


};


 
struct snaIdeal

{
  int anz;

  alg *liste;


};

typedef struct snaIdeal * naIdeal;



 
typedef struct smprec sm_prec;

typedef sm_prec * smpoly;

struct smprec

{
  smpoly n;
             
  int pos;
              
  int e;
                
  poly m;
               
  float f;
              

};


struct _scmdnames

{
  char *name;

  short alias;

  short tokval;

  short toktype;


};

typedef struct _scmdnames cmdnames;


typedef enum

{
  ro_dp,  
  ro_wp,  
  ro_cp,  
  ro_syzcomp,  
  ro_syz,  
  ro_none

}
ro_typ;


 
struct sro_dp

{
  short place;
   
  short start;
   
  short end;


};

typedef struct sro_dp sro_dp;


 
struct sro_wp

{
  short place;
   
  short start;
   
  short end;

  int *weights;
  

};

typedef struct sro_wp sro_wp;


 
struct sro_cp

{
  short place;
   
  short start;
   
  short end;


};

typedef struct sro_cp sro_cp;


 
struct sro_syzcomp

{
  short place;
   
  long *ShiftedComponents;
  
  int* Components;


  long length;



};

typedef struct sro_syzcomp sro_syzcomp;


 
struct sro_syz

{
  short place;
        
  int limit;
          
  int* syz_index;
     
  int  curr_index;
    

};


typedef struct sro_syz sro_syz;




struct sro_ord

{
  ro_typ  ord_typ;

  union
  
{
     sro_dp dp;

     sro_wp wp;

     sro_cp cp;

     sro_syzcomp syzcomp;

     sro_syz syz;

  
} data;


};


struct sip_sring

{
  idhdl      idroot;
  
  int*       order;
   
  int*       block0;
  
  int*       block1;
  
  char**     parameter;
  
  number     minpoly;

  int**      wvhdl;
   
  char **    names;
   

   
  long      *ordsgn;
   
                        

   
  sro_ord *  typ;
    

  ideal      qideal;
  

  unsigned long bitmask;


  int      *VarOffset;

   

   



  struct omBin_s*   PolyBin;
  
  short      ch;
      
  short      ch_flags;
  

  short      N;
       

  short      P;
       
  short      OrdSgn;
  
   
  short      pVarLowIndex;
   
  short      pVarHighIndex;
  
                             
   
   
  
  short      pCompHighIndex;
  
                              
  short      pCompLSize;
  

  short      pCompIndex;
  
  short      pOrdIndex;
  

  short      ExpESize;
  
  short      ExpLSize;
  
  short      OrdSize;
  

  p_Procs_s* p_Procs;

  short      ref;
  

};


struct sip_sideal

{
  poly*  m;

  long rank;

  int nrows;

  int ncols;

  
  inline int& idelems(void) 
{ return ncols;
 
}

};


struct sip_smap

{
  poly *m;

  char *preimage;

  int nrows;

  int ncols;


};



 
typedef number (*numberfunc)(number a,number b);


typedef void    (*pSetmProc)(poly p);

typedef int     (*pLDegProc)(poly p, int *length);

typedef int     (*pFDegProc)(poly p);

typedef int     (*pCompProc)(poly p1, poly p2);


extern ring      currRing;


 



 


extern "C" 
{

void  m2_end(short i);



}


 


int   inits(void);


extern int siSeed;

int siRand();




 
extern unsigned int  test;

 

extern "C" unsigned int  verbose;


 


extern int    yydebug;






class libstack;

typedef libstack *  libstackv;




extern struct omBin_s* MP_INT_bin;

extern struct omBin_s* char_ptr_bin;

extern struct omBin_s* ideal_bin;

extern struct omBin_s* int_bin;

extern struct omBin_s* poly_bin;

extern struct omBin_s* void_ptr_bin;

extern struct omBin_s* indlist_bin;

extern struct omBin_s* naIdeal_bin;

extern struct omBin_s* snaIdeal_bin;

extern struct omBin_s* sm_prec_bin;

extern struct omBin_s* smprec_bin;

extern struct omBin_s* sip_sideal_bin;

extern struct omBin_s* sip_smap_bin;

extern struct omBin_s* sip_sring_bin;

extern struct omBin_s* ip_sideal_bin;

extern struct omBin_s* ip_smap_bin;

extern struct omBin_s* ip_sring_bin;







 


 

 












 





extern void p_Setm(poly p, ring r);



 








union s_exp

{
   unsigned long  l[(10)  +1];


};


typedef s_exp  monomial;

typedef Exponent_t* Exponent_pt;

struct  spolyrec

{
  poly      next;
  
  number    coef;
  
  monomial  exp;
   

};


 




 






 





 






 







  int p_IsFromRing(poly p, ring r)

{
  void* custom = ((_omIsBinPageAddr(   p   )  && (( ((omBinPage) ((long) (    p    ) & ~(4096  -1)))  )->used_blocks < 0)  )  ? omGetCustomOfTrackAddr( p ) : ((void *)0) ) ;

  if (custom != ((void *)0) )
  
{
    return custom == r;

  
}
  else
  
{
    if (_omIsBinPageAddr( p )  && omSizeWOfAddr(p) == r->PolyBin->sizeW)
      return 1;

  
}
  return 0;


}













 







 









 
extern int pVariables;

 
extern int pMonomSize;

 
extern int pMonomSizeW;

 
 
 
 
extern int *pVarOffset;

 
 
 
extern omBin currPolyBin;



 




 
 








inline  BOOLEAN    __pDivisibleBy(poly a, poly b)  

{
  int i=pVariables;
  

  for (;
;
)
  
{
    if (p_GetExp(a,i,currRing) > p_GetExp(b,i,currRing))
      return 0 ;

    i--;

    if (i==0)
      return 1 ;

  
}

}


inline BOOLEAN _pDivisibleBy(poly a, poly b)


{
  if ((a!= ((void *)0) )&&(((( a )->exp.l[ currRing ->pCompIndex]) ==0) || ((( a )->exp.l[ currRing ->pCompIndex])  == (( b )->exp.l[ currRing ->pCompIndex]) )))
  
{
    return __pDivisibleBy(a,b);

  
}
  return 0 ;


}


inline BOOLEAN _pDivisibleBy1(poly a, poly b)


{
  if ((( a )->exp.l[ currRing ->pCompIndex])  == 0 || (( a )->exp.l[ currRing ->pCompIndex])  == (( b )->exp.l[  currRing ->pCompIndex]) )
    return __pDivisibleBy(a,b);

  return 0 ;


}







BOOLEAN mmDBEqual(poly p, poly q, char* file, int line);



inline  BOOLEAN    __pEqual(poly p1, poly p2)  

{
  const unsigned long *s1 = (unsigned long*) &(p1->exp.l[0]);

  const unsigned long *s2 = (unsigned long*) &(p2->exp.l[0]);

  const unsigned long* const lb = s1 + currRing->ExpLSize;


  for(;
;
)
  
{
    if (*s1 != *s2) return 0 ;

    s1++;

    if (s1 == lb) return 1 ;

    s2++;

  
}

}

 





 
 
 






BOOLEAN pDBShortDivisibleBy(poly p1, unsigned long sev_1,
                            poly p2, unsigned long not_sev_2,
                            char* f, int l);




 







inline  int    _pExpQuerSum(poly p)  

{
  int s = 0;

  int i = pVariables;

  for (;
;
)
  
{
    s += p_GetExp(p, i,currRing);

    i--;

    if (i==0) return s;

  
}

}


 




 


 









 


















  
 















 














 













 













 













 













 











  
 













 












 












 











 











 












 












 













 











 











 












 








































 






 










 


 
 




 
 
 










 
extern short fftable[];

 

 
extern numberfunc nMult, nSub ,nAdd ,nDiv, nIntDiv, nIntMod, nExactDiv;

extern void    (*nNew)(number * a);

extern number  (*nInit)(int i);

extern number  (*nPar)(int i);

extern int     (*nParDeg)(number n);

extern int     (*nSize)(number n);

extern int     (*nInt)(number &n);

extern number  (*nNeg)(number a);

extern number  (*nInvers)(number a);

extern number  (*nCopy)(number a);

extern void    (*nWrite)(number &a);

extern char *  (*nRead)(char * s, number * a);

extern void    (*nNormalize)(number &a);

extern BOOLEAN (*nGreater)(number a,number b),
               (*nEqual)(number a,number b),
               (*nIsZero)(number a),
               (*nIsOne)(number a),
               (*nIsMOne)(number a),
               (*nGreaterZero)(number a);

extern void    (*nPower)(number a, int i, number * result);

extern number  (*nGetDenom)(number &n);

extern numberfunc nGcd, nLcm;


extern number nNULL;
  


extern BOOLEAN (*nDBTest)(number a, char *f, int l);


extern void    (*nDBDelete)(number * a,char *f, int l);




extern BOOLEAN (*nSetMap)(ring r);

extern number  (*nMap)(number from);

extern char *  (*nName)(number n);


void nDummy1(number* d);

void nDummy2(number &d);

number ndGcd(number a, number b);

number ndCopy(number a);



void nDBDummy1(number* d,char *f, int l);


int  nGetChar();

void nSetChar(ring r, BOOLEAN complete);









  number p_SetCoeff(poly p, number n, ring r)

{
  ((void)0) ;

  nDBDelete(  &(p->coef)  ,"pInline2.h",36)  ;

  (p)->coef=n;

  return n;


}

 
  Order_t p_GetOrder(poly p, ring r)

{
  ((void)0) ;

  return ((p)->exp.l[r->pOrdIndex]);


}

 
   unsigned long p_SetComp(poly p, unsigned long c, ring r)

{
  ((void)0) ;

  ((void)0) ;

  (( p )->exp.l[ r ->pCompIndex])  = c;

  return c;


}
  unsigned long p_IncrComp(poly p, ring r)

{
  ((void)0) ;

  ((void)0) ;

  return ++((( p )->exp.l[ r ->pCompIndex]) );


}
  unsigned long p_DecrComp(poly p, ring r)

{
  ((void)0) ;

  ((void)0) ;

  ((void)0) ;

  return --((( p )->exp.l[ r ->pCompIndex]) );


}
  unsigned long p_AddComp(poly p, unsigned long v, ring r)

{
  ((void)0) ;

  ((void)0) ;

  return (( p )->exp.l[ r ->pCompIndex])  += v;


}
  unsigned long p_SubComp(poly p, unsigned long v, ring r)

{
  ((void)0) ;

  ((void)0) ;

  ((void)0) ;

  return (( p )->exp.l[ r ->pCompIndex])  -= v;


}

 
  Exponent_t p_GetExp(poly p, int v, ring r)

{
  ((void)0) ;

  ((void)0) ;

  return (p->exp.l[(r->VarOffset[v] & 0xffffff)] >> (r->VarOffset[v] >> 24))
          & r->bitmask;


}
  Exponent_t p_SetExp(poly p, int v, int e, ring r)

{
  ((void)0) ;

  ((void)0) ;

  ((void)0) ;

  ((void)0) ;


   
  register int shift = r->VarOffset[v] >> 24;

  unsigned long ee = ((unsigned long)e) << shift  ;

   
  register int offset = (r->VarOffset[v] & 0xffffff);

  p->exp.l[offset]  &=
    ~( r->bitmask << shift );

   
  p->exp.l[ offset ] |= ee;

  return e;


}

 
   Exponent_t p_IncrExp(poly p, int v, ring r)

{
  ((void)0) ;

  Exponent_t e = p_GetExp(p,v,r);

  e++;

  return p_SetExp(p,v,e,r);


}
   Exponent_t p_DecrExp(poly p, int v, ring r)

{
  ((void)0) ;

  Exponent_t e = p_GetExp(p,v,r);

  ((void)0) ;

  e--;

  return p_SetExp(p,v,e,r);


}
   Exponent_t p_AddExp(poly p, int v, Exponent_t ee, ring r)

{
  ((void)0) ;

  Exponent_t e = p_GetExp(p,v,r);

  e += ee;

  return p_SetExp(p,v,e,r);


}
   Exponent_t p_SubExp(poly p, int v, Exponent_t ee, ring r)

{
  ((void)0) ;

  Exponent_t e = p_GetExp(p,v,r);

  ((void)0) ;

  e -= ee;

  return p_SetExp(p,v,e,r);


}
   Exponent_t p_MultExp(poly p, int v, Exponent_t ee, ring r)

{
  ((void)0) ;

  Exponent_t e = p_GetExp(p,v,r);

  e *= ee;

  return p_SetExp(p,v,e,r);


}

  Exponent_t p_GetExpSum(poly p1, poly p2, int i, ring r)

{
  ((void)0) ;

  ((void)0) ;

  return p_GetExp(p1,i,r) + p_GetExp(p2,i,r);


}
  Exponent_t p_GetExpDiff(poly p1, poly p2, int i, ring r)

{
  return p_GetExp(p1,i,r) - p_GetExp(p2,i,r);


}

  void p_GetExpV(poly p, Exponent_t *ev, ring r)

{
  ((void)0) ;

  for (int j = r->N;
 j;
 j--)
      ev[j] = p_GetExp(p, j, r);


  ev[0] = (( p )->exp.l[  r ->pCompIndex]) ;


}
  void p_SetExpV(poly p, Exponent_t *ev, ring r)

{
  ((void)0) ;

  for (int j = r->N;
 j;
 j--)
      p_SetExp(p, j, ev[j], r);


  p_SetComp(p, ev[0],r);

  p_Setm(p, r);


}

 




  poly p_New(ring r)

{
  ((void)0) ;

  poly p;

    p =( poly )_omDebugAlloc(  r->PolyBin ,1 ,1 ,3      ) ;

  ((void)0) ;

  return p;


}

  void p_Delete1(poly *p, ring r)

{
  ((void)0) ;

  do 
{ if (! (  r != ((void *)0)   )) 
{ dReportError("assume violation at %s:%l condition:%s",     "pInline2.h"  ,    195  , "r != ((void *)0)" ) ;
 
} 
} while (0)  ;

  poly h = *p;

  if (h != ((void *)0) )
  
{
    nDBDelete(  & (( h )->coef)   ,"pInline2.h",199)  ;

    *p = (( h )->next) ;

    _omDebugFree( h ,((void *)0) ,256 ,1      ) ;

  
}


}
  void p_Free(poly p, ring r)

{
  ((void)0) ;

  _omDebugFree( p ,((void *)0) ,256 ,1      ) ;


}
  poly p_FreeAndNext(poly p, ring r)

{
  ((void)0) ;

  poly pnext = (( p )->next) ;

  _omDebugFree( p ,((void *)0) ,256 ,1      ) ;

  return pnext;


}
  void p_LmDelete(poly p, ring r)

{
  ((void)0) ;

  nDBDelete(  & (( p )->coef)   ,"pInline2.h",220)  ;

  _omDebugFree( p ,((void *)0) ,256 ,1      ) ;


}
  poly p_LmDeleteAndNext(poly p, ring r)

{
  ((void)0) ;

  poly pnext = (( p )->next) ;

  nDBDelete(  & (( p )->coef)   ,"pInline2.h",227)  ;

  _omDebugFree( p ,((void *)0) ,256 ,1      ) ;

  return pnext;


}






