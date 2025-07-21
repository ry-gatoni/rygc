/**
 * NOTE: this file is written following the suggestions in the freetype
 * `docs/CUSTOMIZE` file. The example code is
 * ```c
 * #ifndef FT2_BUILD_MY_PLATFORM_H_
 * #define FT2_BUILD_MY_PLATFORM_H_
 *
 * #define FT_CONFIG_OPTIONS_H  <custom/my-ftoption.h>
 * #define FT_CONFIG_MODULES_H  <custom/my-ftmodule.h>
 *
 * #include <freetype/config/ftheader.h>
 *
 * #endif // FT2_BUILD_MY_PLATFORM_H_
 * ``` 
*/

#ifndef FT2_BUILD_RYGC_H_
#define FT2_BUILD_RYGC_H_

#define FT_CONFIG_OPTIONS_H  "rygc-ftoption.h"
#define FT_CONFIG_MODULES_H  "rygc-ftmodule.h"

#include <freetype/config/ftheader.h>

#endif /* FT2_BUILD_RYGC_H_ */

