#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

enum SAMPVER {
	SAMP_NOT_LOADED,
	SAMP_UNKNOWN,
	SAMP_037_R1,
	SAMP_037_R3_1
};

const uintptr_t samp_addressess[][10]
{
    //  CNetGame    RakClientInterface    StringWriteEncoder   StringReadDecoder   CompressorPtr   HandleRpc   RakClientIntfConstr, alloc_packet, write_lock, write_unlock
    {0x21A0F8,      0x3C9,                0x506B0,             0x507E0,            0x10D894,       0x372f0,    0x33DC0, 0x347e0, 0x35b10, 0x35b50},	    // SAMP_037_R1
    {0x26E8DC,      0x2C,                 0x53A60,             0x53B90,            0x121914,       0x3a6a0,	  0x37170, 0x37b90, 0x38ec0, 0x38f00}      // SAMP_037_R3_1
};

inline uintptr_t sampGetBase()
{
    static uintptr_t sampBase = SAMPVER::SAMP_NOT_LOADED;
    if (sampBase == SAMPVER::SAMP_NOT_LOADED)
        sampBase = reinterpret_cast<uintptr_t>(GetModuleHandleA("samp.dll"));
    return sampBase;
}

// https://github.com/imring/TimeFormat/blob/master/samp.hpp#L19

inline SAMPVER sampGetVersion()
{
    static SAMPVER sampVersion = SAMPVER::SAMP_NOT_LOADED;
    if (sampVersion <= SAMPVER::SAMP_UNKNOWN)
	{
        uintptr_t base = sampGetBase();
        if (base == SAMPVER::SAMP_NOT_LOADED) return SAMPVER::SAMP_NOT_LOADED;

        IMAGE_NT_HEADERS* ntHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);

        DWORD ep = ntHeader->OptionalHeader.AddressOfEntryPoint;
        switch (ep)
        {
        case 0x31DF13:
            sampVersion = SAMPVER::SAMP_037_R1;
            break;
        case 0xCC4D0:
            sampVersion = SAMPVER::SAMP_037_R3_1;
            break;
        default:
            sampVersion = SAMPVER::SAMP_UNKNOWN;
        }
    }

    return sampVersion;
}

#define SAMP_OFFSET samp_addressess[sampGetVersion() - 2]

inline uintptr_t sampGetSampInfoPtr()
{
    static uintptr_t sampInfo = 0;
    if (sampInfo == 0)
        sampInfo = *reinterpret_cast<uintptr_t*>(sampGetBase() + SAMP_OFFSET[0]);

	return sampInfo;
}

inline uintptr_t sampGetRakClientInterface()
{
	return sampGetSampInfoPtr() + SAMP_OFFSET[1];
}

inline uintptr_t sampGetEncodedReaderPtr()
{
	return sampGetBase() + SAMP_OFFSET[3];
}

inline uintptr_t sampGetEncodedWriterPtr()
{
	return sampGetBase() + SAMP_OFFSET[2];
}

inline uintptr_t sampGetEncodeDecodeBasePtr()
{
	return *reinterpret_cast<uintptr_t*>(sampGetBase() + SAMP_OFFSET[4]);
}

inline uintptr_t sampGetIncomingRpcHandlerPtr()
{
    return sampGetBase() + SAMP_OFFSET[5];
}

inline uintptr_t sampGetRakClientIntfConstructorPtr()
{
    return sampGetBase() + SAMP_OFFSET[6];
}

inline uintptr_t sampGetAllocPacketPtr()
{
    return sampGetBase() + SAMP_OFFSET[7];
}

inline uintptr_t sampGetWriteLockPtr()
{
    return sampGetBase() + SAMP_OFFSET[8];
}

inline uintptr_t sampGetWriteUnlockPtr()
{
    return sampGetBase() + SAMP_OFFSET[9];
}

#undef SAMP_OFFSET
