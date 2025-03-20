#ifndef CPPUTILS_WINDOWS_SIMPLE_DIRECT_IO_H
#define CPPUTILS_WINDOWS_SIMPLE_DIRECT_IO_H

#include "cpputils/windows/AlignedBuffer.h"

#include <filesystem>
#include <optional>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winioctl.h>

namespace cpputils {
namespace win {
    std::optional<STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR> getAlignmentInfoForFile(const std::filesystem::path& file)
    {
        if (!std::filesystem::is_regular_file(file)) return std::nullopt;

        const std::filesystem::path absolutePath = std::filesystem::absolute(file);
        char driveLetter = absolutePath.c_str()[0];
        std::string volumePath = std::string("\\\\.\\") + driveLetter + ":";

        HANDLE hDevice = CreateFileA(
            volumePath.c_str(),
            NULL,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            NULL,
            NULL
        );

        if (hDevice == INVALID_HANDLE_VALUE) return std::nullopt;

        STORAGE_PROPERTY_QUERY query{};
        query.PropertyId = StorageAccessAlignmentProperty;
        query.QueryType = PropertyStandardQuery;

        STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR alignmentDescriptor{};
        DWORD bytesReturned{};

        BOOL result = DeviceIoControl(
            hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            &query,
            sizeof(query),
            &alignmentDescriptor,
            sizeof(alignmentDescriptor),
            &bytesReturned,
            NULL
        );

        CloseHandle(hDevice);

        if (!result) return std::nullopt;

        return alignmentDescriptor;
    }

    AlignedBuffer directFileToBuffer(const std::filesystem::path& file)
    {
        std::optional<STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR> alignmentInfoOpt = getAlignmentInfoForFile(file);
        if (!alignmentInfoOpt.has_value()) return AlignedBuffer();
        const STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR& alignmentInfo = alignmentInfoOpt.value();

        const char* file_cstr = reinterpret_cast<const char*>(file.c_str());
        HANDLE hFile = CreateFileA(
            file.string().c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_NO_BUFFERING,
            NULL
        );

        if (hFile == NULL) return AlignedBuffer();

        LARGE_INTEGER fileSize{};
        BOOL bResult = GetFileSizeEx(hFile, &fileSize);
        if (!bResult) {
            CloseHandle(hFile);
            return AlignedBuffer();
        }

        size_t diskAlignment = alignmentInfo.BytesPerPhysicalSector;
        size_t alignedBufferSize = fileSize.QuadPart + (diskAlignment - (fileSize.QuadPart % diskAlignment));
        AlignedBuffer alignedBuffer(alignedBufferSize, diskAlignment);

        if (alignedBuffer.buf == nullptr) {
            CloseHandle(hFile);
            return AlignedBuffer();
        }

        DWORD bytesRead = 0;
        bResult = ReadFile(hFile, alignedBuffer.buf, alignedBuffer.size, &bytesRead, NULL);
        if (!bResult) {
            CloseHandle(hFile);
            return AlignedBuffer();
        }

        return alignedBuffer;
    }

} // End windows
} // End cpputils

#endif
