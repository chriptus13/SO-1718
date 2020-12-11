#pragma once
#ifdef BMPRotation_EXPORTS
#define BMPRotation_API  __declspec(dllexport)
#else
#define BMPRotation_API __declspec(dllimport)
#endif 

#ifdef UNICODE
#define CreateFile CreateFileW
#else
#define CreateFile CreateFileA
#endif

BMPRotation_API enum FLIP_enum_t {
	HORIZONTAL = 0,
	VERTICAL = 1
};

#ifdef __cplusplus
extern "C" {
#endif
	BMPRotation_API void BMP_Flip(PTCHAR filenameIn, PTCHAR filenameOut, FLIP_enum_t type);
#ifdef __cplusplus
}
#endif

