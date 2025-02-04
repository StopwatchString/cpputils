#ifndef CPPUTILS_WINDOWS_CREDENTIAL_UTILS_H
#define CPPUTILS_WINDOWS_CREDENTIAL_UTILS_H

//-----------------------------------------------------
// credential_utils.h
//
// A set of functions for quick and simple interaction
// with the Windows Credential Manager.
//
// These implementations all default to generic usage
// and only use local machine persistence.
//-----------------------------------------------------

#include "cpputils/SharedLibraryLoader.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wincred.h>

#include <string>

namespace cpputils {
namespace windows {

const std::string ADV_API_DLL_NAME{"Advapi32.dll"};
static SharedLibraryLoader dwmApiLibrary(ADV_API_DLL_NAME);

struct Credential
{
    std::string username;
    std::string credentialBlob;
};

//-----------------------------------------------------
// writeCredential()
// - Target name length must be <= 32767
// - Credential must be <= 2560 Bytes
// Username is optional and can be left blank.
// Returns if credential write was successful.
//-----------------------------------------------------
static bool writeCredential(std::string targetName, Credential credential)
{
    if (targetName.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH)
        return false;
    if (credential.username.length() > CRED_MAX_USERNAME_LENGTH)
        return false;
    if (credential.credentialBlob.size() > CRED_MAX_CREDENTIAL_BLOB_SIZE)
        return false;

    _CREDENTIALA credentialStruct = {0};
    credentialStruct.Flags;
    credentialStruct.Type = CRED_TYPE_GENERIC;
    credentialStruct.TargetName = (LPSTR)targetName.c_str();
    credentialStruct.Comment;
    credentialStruct.LastWritten;
    credentialStruct.CredentialBlobSize = credential.credentialBlob.size();
    credentialStruct.CredentialBlob = (LPBYTE)credential.credentialBlob.data();
    credentialStruct.Persist = CRED_PERSIST_LOCAL_MACHINE;
    credentialStruct.AttributeCount;
    credentialStruct.Attributes;
    credentialStruct.TargetAlias;
    credentialStruct.UserName = (LPSTR)credential.username.c_str();

    BOOL result = CredWriteA(&credentialStruct, NULL);

    return result == TRUE;
}

//-----------------------------------------------------
// readCredential()
// - Target name length must be <= 32767
// Returns credential string if valid, otherwise empty string.
//-----------------------------------------------------
static Credential readCredential(std::string targetName)
{
    if (targetName.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH)
        return Credential();

    PCREDENTIALA pCredential = nullptr;
    BOOL result = CredReadA((LPSTR)targetName.c_str(), CRED_TYPE_GENERIC, NULL, &pCredential);

    if (result == FALSE)
        return Credential();

    Credential credential;

    credential.username = pCredential->UserName;

    credential.credentialBlob.resize(pCredential->CredentialBlobSize);
    memcpy(credential.credentialBlob.data(), pCredential->CredentialBlob, pCredential->CredentialBlobSize);

    CredFree(pCredential);

    return credential;
}

//-----------------------------------------------------
// deleteCredential()
// - Target name length must be <= 32767
// Returns if credential was successfully deleted.
//-----------------------------------------------------
static bool deleteCredential(std::string targetName)
{
    if (targetName.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH)
        return false;

    BOOL result = CredDeleteA((LPSTR)targetName.c_str(), CRED_TYPE_GENERIC, NULL);

    return result == TRUE;
}

} // namespace windows
} // namespace cpputils

#endif