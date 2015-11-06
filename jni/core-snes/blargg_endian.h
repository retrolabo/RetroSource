/* CPU Byte Order Utilities */

/* snes_spc 0.9.0 */
#ifndef BLARGG_ENDIAN
#define BLARGG_ENDIAN

/* Uncomment to enable platform-specific (and possibly non-portable) optimizations */
#define BLARGG_NONPORTABLE 1

/* PS3 - if SNC compiler is used - enable platform-specific optimizations  */
#ifdef __SNC__
#include <ppu_intrinsics.h>
#define BLARGG_NONPORTABLE 1
#endif

/* BLARGG_COMPILER_HAS_BOOL: If 0, provides bool support for old compiler. If 1, */
/* compiler is assumed to support bool. If undefined, availability is determined. */
#ifndef BLARGG_COMPILER_HAS_BOOL
	#if defined (__MWERKS__)
		#if !__option(bool)
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (_MSC_VER)
		#if _MSC_VER < 1100
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (__GNUC__)
		/* supports bool */
	#elif __cplusplus < 199711
		#define BLARGG_COMPILER_HAS_BOOL 0
	#endif
#endif

#if defined (BLARGG_COMPILER_HAS_BOOL) && !BLARGG_COMPILER_HAS_BOOL
	typedef int bool;
	const bool true  = 1;
	const bool false = 0;
#endif

#if UINT_MAX < 0xFFFFFFFF || ULONG_MAX == 0xFFFFFFFF
	typedef unsigned long blargg_ulong;
#else
	typedef unsigned blargg_ulong;
#endif

/* HAVE_STDINT_H: If defined, use <stdint.h> for int8_t etc. */
#if defined (HAVE_STDINT_H)
	#include <stdint.h>
/* HAVE_INTTYPES_H: If defined, use <stdint.h> for int8_t etc. */
#elif defined (HAVE_INTTYPES_H)
	#include <inttypes.h>
#endif

/* BLARGG_CPU_CISC: Defined if CPU has very few general-purpose registers (< 16) */
#if defined (_M_IX86) || defined (_M_IA64) || defined (__i486__) || \
		defined (__x86_64__) || defined (__ia64__) || defined (__i386__) || defined(ANDROID_X86)
	#define BLARGG_CPU_X86 1
	#define BLARGG_CPU_CISC 1
#endif

#if defined (__powerpc__) || defined (__ppc__) || defined (__POWERPC__) || defined (__powerc)
	#define BLARGG_CPU_POWERPC 1
	#define BLARGG_CPU_RISC 1
#endif

/* BLARGG_BIG_ENDIAN, BLARGG_LITTLE_ENDIAN: Determined automatically, otherwise only */
/* one may be #defined to 1. Only needed if something actually depends on byte order. */
#if !defined (BLARGG_BIG_ENDIAN) && !defined (BLARGG_LITTLE_ENDIAN)
#ifdef __GLIBC__
	/* GCC handles this for us */
	#include <endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define BLARGG_LITTLE_ENDIAN 1
	#elif __BYTE_ORDER == __BIG_ENDIAN
		#define BLARGG_BIG_ENDIAN 1
	#endif
#else

#if defined (LSB_FIRST) || defined (__LITTLE_ENDIAN__) || BLARGG_CPU_X86 ||  \
                defined(ANDROID_X86) || defined(ANDROID_MIPS) || \
   defined(__BLACKBERRY_QNX__) || (defined (LITTLE_ENDIAN) && LITTLE_ENDIAN+0 != 1234)
	#define BLARGG_LITTLE_ENDIAN 1
#endif

#if defined (MSB_FIRST)     || defined (__BIG_ENDIAN__) || defined (WORDS_BIGENDIAN) || \
	defined (__sparc__)     ||  BLARGG_CPU_POWERPC || \
	(defined (BIG_ENDIAN) && BIG_ENDIAN+0 != 4321)
	#define BLARGG_BIG_ENDIAN 1
#elif !defined(__mips__) || !defined(ANDROID_MIPS)
	/* No endian specified; assume little-endian, since it's most common */
	#define BLARGG_LITTLE_ENDIAN 1
#endif
#endif
#endif

#if BLARGG_LITTLE_ENDIAN && BLARGG_BIG_ENDIAN
	#undef BLARGG_LITTLE_ENDIAN
	#undef BLARGG_BIG_ENDIAN
#endif

#if BLARGG_NONPORTABLE
	/* Optimized implementation if byte order is known */
	#if BLARGG_LITTLE_ENDIAN
		#define GET_LE16( addr )        (*(uint16_t*) (addr))
		#define GET_LE32( addr )        (*(uint32_t*) (addr))
		#define SET_LE16( addr, data )  (void) (*(uint16_t*) (addr) = (data))
		#define SET_LE32( addr, data )  (void) (*(uint32_t*) (addr) = (data))
	#elif BLARGG_BIG_ENDIAN
		#if BLARGG_CPU_POWERPC
			/* PowerPC has special byte-reversed instructions */
			#if defined (__SNC__)
				#define GET_LE16( addr )        (__builtin_lhbrx(addr, 0))
				#define GET_LE32( addr )        (__builtin_lwbrx(addr, 0))
				#define SET_LE16( addr, in )    (__builtin_sthbrx(in, addr, 0))
				#define SET_LE32( addr, in )    (__builtin_stwbrx(in, addr, 0))
			#elif defined (_XBOX360)
				#include <PPCIntrinsics.h>
				#define GET_LE16( addr )        (__loadshortbytereverse(0, addr))
				#define GET_LE32( addr )        (__loadwordbytereverse(0, addr))
				#define SET_LE16( addr, in )    (__storeshortbytereverse(in, 0, addr))
				#define SET_LE32( addr, in )    (__storewordbytereverse(in, 0, addr))
			#elif defined (__MWERKS__)
				#define GET_LE16( addr )        (__lhbrx( addr, 0 ))
				#define GET_LE32( addr )        (__lwbrx( addr, 0 ))
				#define SET_LE16( addr, in )    (__sthbrx( in, addr, 0 ))
				#define SET_LE32( addr, in )    (__stwbrx( in, addr, 0 ))
			#elif defined (__GNUC__)
				#define GET_LE16( addr )        ({unsigned ppc_lhbrx_; asm( "lhbrx %0,0,%1" : "=r" (ppc_lhbrx_) : "r" (addr), "0" (ppc_lhbrx_) ); ppc_lhbrx_;})
				#define GET_LE32( addr )        ({unsigned ppc_lwbrx_; asm( "lwbrx %0,0,%1" : "=r" (ppc_lwbrx_) : "r" (addr), "0" (ppc_lwbrx_) ); ppc_lwbrx_;})
				#define SET_LE16( addr, in )    ({asm( "sthbrx %0,0,%1" : : "r" (in), "r" (addr) );})
				#define SET_LE32( addr, in )    ({asm( "stwbrx %0,0,%1" : : "r" (in), "r" (addr) );})
			#endif
		#endif
	#endif
#else
static INLINE unsigned get_le16( void const* p )
{
	return (unsigned) ((unsigned char const*) p) [1] << 8 | (unsigned) ((unsigned char const*) p) [0];
}

static INLINE blargg_ulong get_le32( void const* p )
{
	return (blargg_ulong) ((unsigned char const*) p) [3] << 24 |
		(blargg_ulong) ((unsigned char const*) p) [2] << 16 |
		(blargg_ulong) ((unsigned char const*) p) [1] << 8 |
		(blargg_ulong) ((unsigned char const*) p) [0];
}

static INLINE void set_le16( void* p, unsigned n )
{
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [0] = (unsigned char) n;
}

static INLINE void set_le32( void* p, blargg_ulong n )
{
	((unsigned char*) p) [0] = (unsigned char) n;
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [2] = (unsigned char) (n >> 16);
	((unsigned char*) p) [3] = (unsigned char) (n >> 24);
}
#endif

#ifndef GET_LE16
	#define GET_LE16( addr )        get_le16( addr )
	#define SET_LE16( addr, data )  set_le16( addr, data )
#endif

#ifndef GET_LE32
	#define GET_LE32( addr )        get_le32( addr )
	#define SET_LE32( addr, data )  set_le32( addr, data )
#endif

#define GET_LE16SA( addr )      ((int16_t) GET_LE16( addr ))
#define GET_LE16A( addr )       GET_LE16( addr )
#define SET_LE16A( addr, data ) SET_LE16( addr, data )

#endif
