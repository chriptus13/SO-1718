#ifndef BMP_UTILS_H
#define BMP_UTILS_H

#include <Windows.h>

#ifdef DLL_BMP_EXPORTS
#define DLL_BMP_API __declspec(dllexport)
#else
#define DLL_BMP_API __declspec(dllimport)
#endif

typedef enum {
	ROTATE_HORIZONTAL, ROTATE_VERTICAL, ROTATE_90, ROTATE_180, ROTATE_270
} ROTATION_enum_t;
typedef enum {
	FLIP_HORIZONTALLY, FLIP_VERTICALLY
} FLIP_enum_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNICODE
#define bmp_xchg_px bmp_xchg_px_w
#define bmp_rot bmp_rot_w
#define BMP_Flip BMP_FlipW
#else
#define bmp_xchg_px bmp_xchg_px_a
#define bmp_rot bmp_rot_a
#define BMP_Flip BMP_FlipA
#endif

DLL_BMP_API VOID bmp_xchg_px_w(LPCWSTR file, DWORD oldPx, DWORD newPx);
DLL_BMP_API VOID bmp_xchg_px_a(LPCSTR file, DWORD oldPx, DWORD newPx);
DLL_BMP_API VOID bmp_rot_w(LPCWSTR file_in, LPCWSTR file_out, ROTATION_enum_t rot);
DLL_BMP_API VOID bmp_rot_a(LPCSTR file_in, LPCSTR file_out, ROTATION_enum_t rot);
DLL_BMP_API VOID BMP_FlipA(PSTR fileIn, PSTR fileOut, FLIP_enum_t flipType);
DLL_BMP_API VOID BMP_FlipW(PWSTR fileIn, PWSTR fileOut, FLIP_enum_t flipType);
DLL_BMP_API VOID BMP_FlipMem(PUCHAR fileBaseAddressIn, PUCHAR fileBaseAddressOut, FLIP_enum_t flipType);

#ifdef __cplusplus
}
#endif


#endif/*BMP_UTILS_H*/