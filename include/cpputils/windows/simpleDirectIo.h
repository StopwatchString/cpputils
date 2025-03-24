#ifndef CPPUTILS_WINDOWS_SIMPLE_DIRECT_IO_H
#define CPPUTILS_WINDOWS_SIMPLE_DIRECT_IO_H

#include "cpputils/windows/AlignedBuffer.h"

#include <filesystem>
#include <optional>
#include <array>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winioctl.h>

namespace cpputils {
namespace win {
    constexpr size_t MAXIMUM_SUBREADS = 64;

    template<size_t S>
    struct ReadContexts {
        std::array<OVERLAPPED, S> overlapped{};
        std::array<HANDLE, S> event{ NULL };
    };

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

    AlignedBuffer directFileToBuffer(const std::filesystem::path& file, size_t subreads = 1)
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
            FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN,
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
        size_t alignedBufferSize = fileSize.QuadPart + ((diskAlignment - (fileSize.QuadPart % diskAlignment)) % diskAlignment);
        AlignedBuffer alignedBuffer(alignedBufferSize, diskAlignment);

        if (alignedBuffer.buf == nullptr) {
            CloseHandle(hFile);
            return AlignedBuffer();
        }

        size_t maxDiskSubreads = alignedBufferSize / diskAlignment;
        subreads = std::min(subreads, std::min(maxDiskSubreads, MAXIMUM_SUBREADS));

        // Figure out aligned subsectors to read. Final sector will usually not divide cleanly, so calculate
        // 'subsector' size and use it for final sector if it's greater than 0.
        size_t subreadSectorSize = (alignedBufferSize / subreads) + ((diskAlignment - ((alignedBufferSize / subreads) % diskAlignment)) % diskAlignment);
        size_t endcapSectorSize = alignedBufferSize % subreadSectorSize;

        ReadContexts<MAXIMUM_SUBREADS> readContexts{};

        for (size_t i = 0; i < subreads - 1; i++) {
            size_t offset = i * subreadSectorSize;
            readContexts.overlapped[i].Offset = static_cast<DWORD>(offset & 0xFFFFFFFF);
            readContexts.overlapped[i].OffsetHigh = static_cast<DWORD>((offset >> 32) & 0xFFFFFFFF);

            HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
            readContexts.overlapped[i].hEvent = event;
            readContexts.event[i] = event;

            ReadFile(
                hFile,
                (char*)alignedBuffer.buf + offset,
                subreadSectorSize,
                nullptr,
                &readContexts.overlapped[i]
            );
        }

        // Endcap sector
        size_t endcapOffset = (subreads - 1) * subreadSectorSize;
        size_t endcapReadSize = endcapSectorSize > 0 ? endcapSectorSize : subreadSectorSize;
        readContexts.overlapped[subreads - 1].Offset = static_cast<DWORD>(endcapOffset & 0xFFFFFFFF);
        readContexts.overlapped[subreads - 1].OffsetHigh = static_cast<DWORD>((endcapOffset >> 32) & 0xFFFFFFFF);

        HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL);
        readContexts.overlapped[subreads - 1].hEvent = event;
        readContexts.event[subreads - 1] = event;
        ReadFile(
            hFile,
            (char*)alignedBuffer.buf + endcapOffset,
            endcapReadSize,
            nullptr,
            &readContexts.overlapped[subreads - 1]
        );

        WaitForMultipleObjects(static_cast<DWORD>(subreads), readContexts.event.data(), TRUE, INFINITE);

        bool successful = true;

        // Check each read result
        for (size_t i = 0; i < subreads; i++) {
            DWORD transferred = 0;

            successful &= GetOverlappedResult(hFile, &readContexts.overlapped[i], &transferred, FALSE);

            CloseHandle(readContexts.event[i]);
        }

        if (!successful) {
            std::cout << "Not successful" << std::endl;
        }

        CloseHandle(hFile);
        return alignedBuffer;
    }

} // End windows
} // End cpputils

#endif
