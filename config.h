#ifndef _GODZILLA_CONFIG_H_
#define _GODZILLA_CONFIG_H_

#ifdef _WIN32
#define _WINDOWS_SYSTEM_

#ifdef _WIN64
#define _ARCH_64_
#endif

#ifndef _WIN64
#define _ARCH_86_
#endif

#endif

#ifdef _LINUX
#define _LINUX_SYSTEM_

#ifdef _arm64
#define _ARCH_64_
#endif

#ifndef _arm64
#define _ARCH_86_
#endif

#endif




#endif
