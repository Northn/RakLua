#pragma once

#include "samp.hpp"
#include "RakLua.h"

class RakLuaBitStream
{
	BitStream*	bs;
	bool		deleteBitStream = true;
public:
	RakLuaBitStream();
	RakLuaBitStream(unsigned char* _data, unsigned int lengthInBytes, bool _copyData);
	RakLuaBitStream(BitStream* bitStream);
	~RakLuaBitStream();

	int			getNumberOfBitsUsed();
	int			getNumberOfBytesUsed();

	int			getNumberOfUnreadBits();
	int			getNumberOfUnreadBytes();

	void		resetReadPointer();
	void		resetWritePointer();
	void		resetBs();

	int			getReadOffset();
	int			getWriteOffset();

	void		setReadOffset(int offset);
	void		setWriteOffset(int offset);

	void		ignoreBits(int count);
	void		ignoreBytes(int count);

	bool		readBool();
	int8_t		readInt8();
	int16_t		readInt16();
	int32_t		readInt32();
	uint8_t		readUInt8();
	uint16_t	readUInt16();
	uint32_t	readUInt32();
	float		readFloat();
	std::string	readString(int32_t len);
	std::string	readEncoded(int len);

	void		writeBool(bool value);
	void		writeInt8(int8_t value);
	void		writeInt16(int16_t value);
	void		writeInt32(int32_t value);
	void		writeUInt8(uint8_t value);
	void		writeUInt16(uint16_t value);
	void		writeUInt32(uint32_t value);
	void		writeFloat(float value);
	void		writeString(std::string_view value);
	void		writeEncoded(std::string_view value);

	bool		emulIncomingRPC(uint8_t rpcId);
	bool		emulIncomingPacket();

	bool		sendRPC(int rpcId);
	bool		sendPacket();

	bool		sendRPCEx(int rpcId, PacketPriority priority, PacketReliability reliability, uint8_t channel, bool timestamp);
	bool		sendPacketEx(PacketPriority priority, PacketReliability reliability, uint8_t channel);

	BitStream*	getBitStream();
	
	void		deleteBs(); // for Lua-deletion
};
