#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _DEBUG
#include <assert.h>
#endif

enum SAMPVER {
    SAMP_NOT_LOADED,
    SAMP_UNKNOWN,
    SAMP_037_R1,
    SAMP_037_R3_1,
    SAMP_037_R4_2
};

inline uintptr_t sampGetBase()
{
    static uintptr_t sampBase = SAMPVER::SAMP_NOT_LOADED;
    if (sampBase == SAMPVER::SAMP_NOT_LOADED)
        sampBase = reinterpret_cast<uintptr_t>(GetModuleHandleA("samp.dll"));
    return sampBase;
}

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
        case 0xCBCB0:
            sampVersion = SAMPVER::SAMP_037_R4_2;
            break;
        default:
            sampVersion = SAMPVER::SAMP_UNKNOWN;
#ifdef _DEBUG
            assert(0);
#endif
        }
    }

    return sampVersion;
}

enum class SampAddresses {
    CNetGame,
    RakClientInterface,
    StringWriteEncoder,
    StringReadDecoder,
    CompressorPtr,
    HandleRpc,
    RakClientIntfConstr,
    alloc_packet,
    write_lock,
    write_unlock
};

struct SampAddress
{
    uintptr_t R1, R3, R4;

    uintptr_t get() {
        switch (sampGetVersion())
        {
        case SAMPVER::SAMP_037_R1:
            return R1;
        case SAMPVER::SAMP_037_R3_1:
            return R3;
        case SAMPVER::SAMP_037_R4_2:
            return R4;
        default:
#ifdef _DEBUG
            assert(0);
#endif
            return 0;
        }
    }
};

constexpr SampAddress sampGetAddress(SampAddresses element)
{
#define ENTRY(name, r1, r3, r4) case SampAddresses::name: return {r1, r3, r4};
    switch (element)
    {
    ENTRY(CNetGame,                 0x21A0F8,   0x26E8DC,   0x26EA0C);
    ENTRY(StringWriteEncoder,       0x506B0,    0x53A60,    0x541A0 );
    ENTRY(StringReadDecoder,        0x507E0,    0x53B90,    0x542D0 );
    ENTRY(CompressorPtr,            0x10D894,   0x121914,   0x121A3C);
    ENTRY(HandleRpc,                0x372F0,    0x3A6A0,    0x3ADE0 );
    ENTRY(RakClientIntfConstr,      0x33DC0,    0x37170,    0x378B0 );
    ENTRY(alloc_packet,             0x347E0,    0x37B90,    0x382D0 );
    ENTRY(write_lock,               0x35B10,    0x38EC0,    0x39600 );
    ENTRY(write_unlock,             0x35B50,    0x38F00,    0x39640 );
    }
#undef ENTRY
#ifdef _DEBUG
    assert(0);
#endif
    return { 0, 0 };
}

#define SAMP_OFFSET(name) sampGetAddress(SampAddresses::name).get()

inline uintptr_t sampGetSampInfoPtr()
{
    static uintptr_t sampInfo = 0;
    if (sampInfo == 0)
        sampInfo = *reinterpret_cast<uintptr_t*>(sampGetBase() + SAMP_OFFSET(CNetGame));

    return sampInfo;
}

inline uintptr_t sampGetEncodedReaderPtr()
{
    return sampGetBase() + SAMP_OFFSET(StringReadDecoder);
}

inline uintptr_t sampGetEncodedWriterPtr()
{
    return sampGetBase() + SAMP_OFFSET(StringWriteEncoder);
}

inline uintptr_t sampGetEncodeDecodeBasePtr()
{
    return *reinterpret_cast<uintptr_t*>(sampGetBase() + SAMP_OFFSET(CompressorPtr));
}

inline uintptr_t sampGetIncomingRpcHandlerPtr()
{
    return sampGetBase() + SAMP_OFFSET(HandleRpc);
}

inline uintptr_t sampGetRakClientIntfConstructorPtr()
{
    return sampGetBase() + SAMP_OFFSET(RakClientIntfConstr);
}

inline uintptr_t sampGetAllocPacketPtr()
{
    return sampGetBase() + SAMP_OFFSET(alloc_packet);
}

inline uintptr_t sampGetWriteLockPtr()
{
    return sampGetBase() + SAMP_OFFSET(write_lock);
}

inline uintptr_t sampGetWriteUnlockPtr()
{
    return sampGetBase() + SAMP_OFFSET(write_unlock);
}

#undef SAMP_OFFSET
