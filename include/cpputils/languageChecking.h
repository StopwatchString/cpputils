#ifndef CPPUTILS_LANGUAGE_CHECKING_H
#define CPPUTILS_LANGUAGE_CHECKING_H

#define LANGUAGE_VERSION(version) (__cplusplus >= version || _MSVC_LANG >= version)

#endif
