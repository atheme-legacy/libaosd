#include <sys/time.h>

/* We copy these macros here because they might not be portable.
 * Besides, it's not very convenient to pass pointers each time.
 */
#ifdef timerisset
#undef timerisset
#endif

#ifdef timerclear
#undef timerclear
#endif

#ifdef timercmp
#undef timercmp
#endif

#ifdef timeradd
#undef timeradd
#endif

#ifdef timersub
#undef timersub
#endif

#ifdef timerset
#undef timerset
#endif

# define timerisset(tv)	((tv).tv_sec || (tv).tv_usec)
# define timerclear(tv)	((tv).tv_sec = (tv).tv_usec = 0)
# define timercmp(a, b, CMP) 						      \
  (((a).tv_sec == (b).tv_sec) ? 					      \
   ((a).tv_usec CMP (b).tv_usec) : 					      \
   ((a).tv_sec CMP (b).tv_sec))
# define timeradd(a, b, result)						      \
  do {									      \
    (result).tv_sec = (a).tv_sec + (b).tv_sec;				      \
    (result).tv_usec = (a).tv_usec + (b).tv_usec;			      \
    if ((result).tv_usec >= 1000000)					      \
      {									      \
	++(result).tv_sec;						      \
	(result).tv_usec -= 1000000;					      \
      }									      \
  } while (0)
# define timersub(a, b, result)						      \
  do {									      \
    (result).tv_sec = (a).tv_sec - (b).tv_sec;				      \
    (result).tv_usec = (a).tv_usec - (b).tv_usec;			      \
    if ((result).tv_usec < 0) {						      \
      --(result).tv_sec;						      \
      (result).tv_usec += 1000000;					      \
    }									      \
  } while (0)
#define timerset(tv, msec)						      \
  do {									      \
    (tv).tv_sec = (msec) / 1000;					      \
    (tv).tv_usec = ((msec) % 1000) * 1000;				      \
  } while (0)
