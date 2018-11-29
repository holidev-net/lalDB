
#if !defined(_LALDB_INTERNAL_H)
#define _LALDB_INTERNAL_H

#if defined(_MSC_VER)  // MSVC
# define DECORATED_STRUCT(name, body) struct name body // Avoid __attibute__
# pragma warning(disable : 4200) //disable an useless warning for all char[] at end of struct
#else // GCC's compilers
# define DECORATED_STRUCT(name, body) struct name body __attribute__((ms_struct)) // use microsoft's struct for file compatibity
#endif

#endif // _LALDB_INTERNAL_H
