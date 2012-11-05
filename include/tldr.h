/*********************
 * TLDR
 * Tiny Little DOM (Reader)
 * Insert zlib license
 *********************/
#ifndef __TLDR_H__
#define __TLDR_H__

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

struct tldrContext;

typedef int tldrReturn;
#define TLDR_SUCCESS       0
#define TLDR_NO_CONTEXT    1
#define TLDR_NO_KEY        2
#define TLDR_NO_MEMORY     3
#define TLDR_NO_VALUE      4
#define TLDR_NO_FILE       5
#define TLDR_WRITE_PROTECT 6

#define TLDR_STRING_MAX 256

tldrContext* tldrInitContext();
tldrReturn   tldrTerminateContext(tldrContext** context);

tldrReturn   tldrWriteValue(tldrContext* context, const char* key, const char* val);
tldrReturn   tldrReadValue(tldrContext* context, const char* key, char* val);
tldrReturn   tldrKeyExists(tldrContext* context, const char* key);

tldrReturn   tldrSaveBinary(tldrContext* context, const char* filename);
tldrReturn   tldrLoadBinary(tldrContext* context, const char* filename, bool clear);
tldrReturn   tldrGenerateBinary(tldrContext* context, char* buffer, long& size); 

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*__TLDR_H__*/
