#include "pch.h"
#include "RakLuaBitStream.h"

RakLuaBitStream::RakLuaBitStream()
{
	bs = new BitStream;
}

RakLuaBitStream::RakLuaBitStream(unsigned char* _data, unsigned int lengthInBits, bool _copyData)
{
	bs = new BitStream(_data, BITS_TO_BYTES(lengthInBits), _copyData);
}

RakLuaBitStream::RakLuaBitStream(BitStream* bitStream)
{
	bs = bitStream;
	deleteBitStream = false;
}

RakLuaBitStream::~RakLuaBitStream()
{
	deleteBs();
}

int RakLuaBitStream::getNumberOfBitsUsed()
{
	return bs->GetNumberOfBitsUsed();
}

int RakLuaBitStream::getNumberOfBytesUsed()
{
	return bs->GetNumberOfBytesUsed();
}

int RakLuaBitStream::getNumberOfUnreadBits()
{
	return bs->GetNumberOfUnreadBits();
}

int RakLuaBitStream::getNumberOfUnreadBytes()
{
	return BITS_TO_BYTES(bs->GetNumberOfUnreadBits());
}

void RakLuaBitStream::resetReadPointer()
{
	bs->ResetReadPointer();
}

void RakLuaBitStream::resetWritePointer()
{
	bs->ResetWritePointer();
}

void RakLuaBitStream::resetBs()
{
	bs->Reset();
}

int RakLuaBitStream::getReadOffset()
{
	return bs->GetReadOffset();
}

int RakLuaBitStream::getWriteOffset()
{
	return bs->GetWriteOffset();
}

void RakLuaBitStream::setReadOffset(int offset)
{
	bs->SetReadOffset(offset);
}

void RakLuaBitStream::setWriteOffset(int offset)
{
	bs->SetWriteOffset(offset);
}

void RakLuaBitStream::ignoreBits(int count)
{
	bs->IgnoreBits(count);
}

void RakLuaBitStream::ignoreBytes(int count)
{
	bs->IgnoreBits(BYTES_TO_BITS(count));
}

bool RakLuaBitStream::readBool()
{
	bool ret;
	bs->Read(ret);
	return ret;
}

int8_t RakLuaBitStream::readInt8()
{
	int8_t ret;
	bs->Read(ret);
	return ret;
}

int16_t RakLuaBitStream::readInt16()
{
	int16_t ret;
	bs->Read(ret);
	return ret;
}

int32_t RakLuaBitStream::readInt32()
{
	int32_t ret;
	bs->Read(ret);
	return ret;
}

uint8_t RakLuaBitStream::readUInt8()
{
	uint8_t ret;
	bs->Read(ret);
	return ret;
}

uint16_t RakLuaBitStream::readUInt16()
{
	uint16_t ret;
	bs->Read(ret);
	return ret;
}

uint32_t RakLuaBitStream::readUInt32()
{
	uint32_t ret;
	bs->Read(ret);
	return ret;
}

float RakLuaBitStream::readFloat()
{
	float ret;
	bs->Read(ret);
	return ret;
}

std::string RakLuaBitStream::readString(int32_t len)
{
	std::string ret(len, 0);
	bs->Read(ret.data(), len);
	return ret;
}

std::string RakLuaBitStream::readEncoded(int len)
{
	std::string str(len, '\0');
	reinterpret_cast<bool(__thiscall*)(uintptr_t, char*, int, BitStream*, int)>
		(sampGetEncodedReaderPtr())(sampGetEncodeDecodeBasePtr(), str.data(), len, bs, 0);
	str.resize(str.find('\0'));
	return str;
}

bool RakLuaBitStream::readBuffer(uintptr_t destination, int size)
{
	return bs->ReadBits(reinterpret_cast<unsigned char*>(destination), BYTES_TO_BITS(size), false);
}

void RakLuaBitStream::writeBool(bool value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeInt8(int8_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeInt16(int16_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeInt32(int32_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeUInt8(uint8_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeUInt16(uint16_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeUInt32(uint32_t value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeFloat(float value)
{
	bs->Write(value);
}

void RakLuaBitStream::writeString(std::string_view value)
{
	bs->Write(value.data(), value.size());
}

void RakLuaBitStream::writeEncoded(std::string_view value)
{
	bs->AddBitsAndReallocate(value.size() * 16 + 16); // text size + 18 uint16_t textlen
	std::cout << value.size() << std::endl;
	reinterpret_cast<void(__thiscall*)(uintptr_t, const char*, int, BitStream*, int)>
		(sampGetEncodedWriterPtr())(sampGetEncodeDecodeBasePtr(), value.data(), value.size() + 2, bs, 0); // 0x53b90 r3 0x507E0 r1; 0x53a60 r3 0x506B0 r1
}

void RakLuaBitStream::writeBuffer(uintptr_t destinationFrom, int size)
{
	bs->WriteBits(reinterpret_cast<unsigned char*>(destinationFrom), BYTES_TO_BITS(size), false);
}

void RakLuaBitStream::writeBitStream(RakLuaBitStream* writeBs)
{
	bs->Write(writeBs->getBitStream());
}

bool RakLuaBitStream::emulIncomingRPC(uint8_t rpcId)
{
	if (!gRakLua.isInitialized() || !gRakPeer) return false;
	BitStream sendBs;
	sendBs.Write<uint8_t>(ID_RPC);
	sendBs.Write(rpcId);
	sendBs.WriteCompressed<unsigned int>(BYTES_TO_BITS(bs->GetNumberOfBytesUsed()));
	sendBs.WriteBits(bs->GetData(), BYTES_TO_BITS(bs->GetNumberOfBytesUsed()), false);
	return reinterpret_cast<bool(__thiscall*)(void*, unsigned char*, int, PlayerID)>
		(gRakLua.getRpcHook()->getTrampoline())(gRakPeer, sendBs.GetData(), sendBs.GetNumberOfBytesUsed(), gPlayerId);
}

bool RakLuaBitStream::emulIncomingPacket()
{
	if (!gRakLua.isInitialized() || !gRakPeer) return false;

	Packet* send_packet = reinterpret_cast<Packet*(*)(int)>(sampGetAllocPacketPtr())(bs->GetNumberOfBytesUsed());
	memcpy(send_packet->data, bs->GetData(), send_packet->length);

	// RakPeer::AddPacketToProducer
	char* packets = static_cast<char*>(gRakPeer) + 0xdb6;
	auto write_lock = reinterpret_cast<Packet**(__thiscall*)(void*)>(sampGetWriteLockPtr());
	auto write_unlock = reinterpret_cast<void(__thiscall*)(void*)>(sampGetWriteUnlockPtr());

	*write_lock(packets) = send_packet;
	write_unlock(packets);
	// RakPeer::AddPacketToProducer

	return true;
}

#define RAKCLIENT_INTF *reinterpret_cast<uintptr_t*>(sampGetRakClientInterface())

bool RakLuaBitStream::sendRPC(int rpcId)
{
	return reinterpret_cast<bool(__thiscall*)(uintptr_t, int*, BitStream*, PacketPriority, PacketReliability, char, bool)>
		(gRakLua.getVmtHook()->getOriginalMethod(25))
		(RAKCLIENT_INTF, &rpcId, bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false);
}

bool RakLuaBitStream::sendRPCEx(int rpcId, PacketPriority priority, PacketReliability reliability, uint8_t channel, bool timestamp)
{
	return reinterpret_cast<bool(__thiscall*)(uintptr_t, int*, BitStream*, PacketPriority, PacketReliability, char, bool)>
		(gRakLua.getVmtHook()->getOriginalMethod(25))
		(RAKCLIENT_INTF, &rpcId, bs, priority, reliability, channel, timestamp);
}

bool RakLuaBitStream::sendPacketEx(PacketPriority priority, PacketReliability reliability, uint8_t channel)
{
	return reinterpret_cast<bool(__thiscall*)(uintptr_t, BitStream*, PacketPriority, PacketReliability, char)>
		(gRakLua.getVmtHook()->getOriginalMethod(6))(RAKCLIENT_INTF, bs, priority, reliability, channel);
}

bool RakLuaBitStream::sendPacket()
{
	return reinterpret_cast<bool(__thiscall*)(uintptr_t, BitStream*, PacketPriority, PacketReliability, char)>
		(gRakLua.getVmtHook()->getOriginalMethod(6))(RAKCLIENT_INTF, bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

#undef RAKCLIENT_INTF

BitStream* RakLuaBitStream::getBitStream()
{
	return bs;
}

uintptr_t RakLuaBitStream::getDataPtr()
{
	return reinterpret_cast<uintptr_t>(bs->GetData());
}

void RakLuaBitStream::deleteBs()
{
	if (bs != nullptr && deleteBitStream)
		delete bs;
}
