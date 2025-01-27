#ifndef CPPUTILS_WINDOWS_CREDENTIAL_UTILS_H
#define CPPUTILS_WINDOWS_CREDENTIAL_UTILS_H

//-----------------------------------------------------
// credential_utils.h
// 
// A set of functions for quick and simple interaction
// with the Windows Credential Manager.
//-----------------------------------------------------

#include "cpputils/SharedLibraryLoader.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <wincred.h>

#include <string>

const std::string ADV_API_DLL_NAME{ "Advapi32.dll" };
static SharedLibraryLoader dwmApiLibrary(ADV_API_DLL_NAME);

//-----------------------------------------------------
// writeCredential()
// - Target name length must be <= 32767
// - Credential must be <= 2560 Bytes
// Returns if credential write was successful.
//-----------------------------------------------------
static bool writeCredential(std::string name, std::string credentialString)
{
    if (name.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH) return false;
    if (credentialString.size() > CRED_MAX_CREDENTIAL_BLOB_SIZE) return false;

    _CREDENTIALA credential = { 0 };
    credential.Flags;
    credential.Type = CRED_TYPE_GENERIC;
    credential.TargetName = (LPSTR)name.c_str();
    credential.Comment;
    credential.LastWritten;
    credential.CredentialBlobSize = credentialString.size();
    credential.CredentialBlob = (LPBYTE)credentialString.data();
    credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
    credential.AttributeCount;
    credential.Attributes;
    credential.TargetAlias;
    credential.UserName; // Optional for CRED_TYPE_GENERIC, Corresponds to stored username for credential.

    BOOL result = CredWriteA(
        &credential,
        NULL
    );

    return result == TRUE;
}

//-----------------------------------------------------
// readCredential()
// - Target name length must be <= 32767
// Returns credential string if valid, otherwise empty string.
//-----------------------------------------------------
static std::string readCredential(std::string name)
{
    if (name.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH) return "";

    PCREDENTIALA pCredential = nullptr;
    BOOL result = CredReadA(
        (LPSTR)name.c_str(),
        CRED_TYPE_GENERIC,
        NULL,
        &pCredential
    );

    if (result == FALSE) return "";

    std::string credentialString;
    credentialString.resize(pCredential->CredentialBlobSize);
    memcpy(credentialString.data(), pCredential->CredentialBlob, pCredential->CredentialBlobSize);

    CredFree(pCredential);

    return credentialString;
}

//-----------------------------------------------------
// deleteCredential()
// - Target name length must be <= 32767
// Returns if credential was successfully deleted.
//-----------------------------------------------------
static bool deleteCredential(std::string name)
{
    if (name.length() > CRED_MAX_GENERIC_TARGET_NAME_LENGTH) return false;

    BOOL result = CredDeleteA(
        (LPSTR)name.c_str(),
        CRED_TYPE_GENERIC,
        NULL
    );

    return result == TRUE;
}

#endif