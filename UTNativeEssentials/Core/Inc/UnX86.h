/*=============================================================================
	UnX86.h: Unreal X86 specific architecture code.

	Revision history:
		* Created by Fernando Velazquez (Higor)
=============================================================================*/

//
// Unreal X86 requires SSE2 instruction set.
//
#ifndef USES_SSE_INTRINSICS
# define USES_SSE_INTRINSICS 1
#endif
#include <xmmintrin.h>
#include <emmintrin.h>
#if _MSC_VER
# include <intrin.h>
#else
# include <x86intrin.h>
#endif
#include <math.h>



//
// Round a floating point number to an integer using SSE intrinsics
// Note that (int+.5) is rounded to (int+1).
//
#define DEFINED_appRound 1
inline INT appRound(FLOAT Value)
{
	return _mm_cvtss_si32(_mm_load_ss(&Value));
}
inline INT appRound(DOUBLE Value)
{
	return _mm_cvtsd_si32(_mm_load_sd(&Value));
}


#define DEFINED_appFloor 1
inline INT appFloor(FLOAT F)
{
	return appRound(F-0.5f);
}
inline INT appFloor(DOUBLE D)
{
	return appRound(D-0.5);
}

//
// Visual Studio doesn't have built-in intrinsics operators
//
#ifdef _MSC_VER

inline __m128 operator+( __m128 a, __m128 b)   { return _mm_add_ps( a, b); }
inline __m128 operator-( __m128 a, __m128 b)   { return _mm_sub_ps( a, b); }
inline __m128 operator*( __m128 a, __m128 b)   { return _mm_mul_ps( a, b); }
inline __m128 operator/( __m128 a, __m128 b)   { return _mm_div_ps( a, b); }

inline __m128& operator+=( __m128& a, __m128 b)   { return (a = a+b); }
inline __m128& operator-=( __m128& a, __m128 b)   { return (a = a-b); }
inline __m128& operator*=( __m128& a, __m128 b)   { return (a = a*b); }
inline __m128& operator/=( __m128& a, __m128 b)   { return (a = a/b); }

#endif

#if !_MSC_VER

#include <cpuid.h>

inline void cpuid(int regs[4], int function_id)
{
	__cpuid(function_id, regs[0], regs[1], regs[2], regs[3]);
}

inline void cpuidex(int regs[4], int function_id, int subfunction_id)
{
	__cpuid_count(function_id, subfunction_id, regs[0], regs[1], regs[2], regs[3]);
}

#else

inline void cpuid(int regs[4], int function_id)
{
	__cpuid(regs, function_id);
}

inline void cpuidex(int regs[4], int function_id, int subfunction_id)
{
	__cpuidex(regs, function_id, subfunction_id);
}

#endif

class FString;
extern CORE_API void appGetCPUInfoX86(FString& Vendor, FString& Brand, FString& Features);
