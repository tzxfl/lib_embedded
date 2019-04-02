#ifndef LPI_TYPES_H
#define LPI_TYPES_H

typedef unsigned char   u8;
typedef signed char     s8;

typedef unsigned short  u16;
typedef signed short    s16;

typedef unsigned int    uint;
typedef signed int      sint;

typedef unsigned long   u32;
typedef signed long     s32;

typedef unsigned long long  u64;
typedef signed long long    s64;

typedef float           fp32;
typedef double          fp64;

typedef volatile unsigned char vu8;
typedef volatile signed char    vs8;

typedef volatile unsigned short vu16;
typedef volatile signed short   vs16;

typedef volatile unsigned long  vu32;
typedef volatile signed long    vs32;

typedef unsigned char const uc8; /** read only **/
typedef unsigned short const uc16;
typedef unsigned long const uc32;

#define U8_MAX     ((u8)255)
#define S8_MAX     ((s8)127)
#define S8_MIN     ((s8)-128)
#define U16_MAX    ((u16)65535u)
#define S16_MAX    ((s16)32767)
#define S16_MIN    ((s16)-32768)
#define U32_MAX    ((u32)4294967295uL)
#define S32_MAX    ((s32)2147483647)
#define S32_MIN    ((s32)2147483648uL)

typedef enum {FALSE = 0,TRUE = !FALSE}bool;

#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#define MIN(a,b)    ((a) < (b) ? (a) : (b))

#define ARRAY_LEN(array)    (sizeof(array) / sizeof(array[0]))

/** ¶¨Òå¾ä±ú **/
#define DECLAR_HANDLE(name) struct name##__{int unused;};typedef struct name##__ *name

#endif // LPI_TYPES_H
