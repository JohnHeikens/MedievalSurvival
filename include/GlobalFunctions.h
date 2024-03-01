#pragma once
//defines

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


//https://stackoverflow.com/questions/2324658/how-to-determine-the-version-of-the-c-standard-used-by-the-compiler
namespace cPlusPlusVersions
{
	//https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
	constexpr int currentVersion = __cplusplus;//does not work until you define /Zc:__cplusplus
	constexpr int pre98 = 1;
	constexpr int cpp98 = 199711L;
	constexpr int cpp11 = 201103L;
	constexpr int cpp14 = 201402L;
	constexpr int cpp17 = 201703L;
}

//not changing types, but make it easier to type every time.
typedef double fp;//a floating point precision-value, precision can be fp(4 bytes), fp(8 bytes, fp 4 = 8), or long fp(16 bytes).
typedef long long ll;
typedef unsigned long long ull;
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef wchar_t letter;//unicode charachter
//virtual keycode
typedef byte vk;

typedef size_t fsize_t;//fast size type, will result in sse instructions and loop unrolling
typedef const fsize_t cfsize_t;//fast size type, will result in sse instructions and loop unrolling

typedef const ll cll;
typedef const ull cull;
typedef const int cint;
typedef const byte cbyte;
typedef const uint cuint;
typedef const bool cbool;
typedef const int cint;
typedef const ushort cushort;
typedef const sbyte csbyte;
//const floating point
typedef const fp cfp;
typedef const letter cletter;
//const virtual keycode
typedef const vk cvk;
typedef const size_t csize_t;

constexpr int ByteToBits = 8;//there are 8 bits in a byte
constexpr fp bytemult0to1 = 1.0 / 0xff;
constexpr byte bytemax = (byte)0xff;