#pragma once

#include "pch.h"
#include "RakLuaBitStream.h"

bool isSampLoaded()
{
	return sampGetBase() != NULL;
}

bool isSampAvailable()
{
	return isSampLoaded() && sampGetSampInfoPtr() != NULL;
}

RakLuaBitStream* raknetNewBitStream()
{
	return new RakLuaBitStream;
}

void raknetDeleteBitStream(RakLuaBitStream* bs)
{
	delete bs;
}

void raknetResetBitStream(RakLuaBitStream* bs)
{
	bs->resetBs();
}

void raknetBitStreamSetReadOffset(RakLuaBitStream* bs, int offset)
{
	bs->setReadOffset(offset);
}

void raknetBitStreamSetWriteOffset(RakLuaBitStream* bs, int offset)
{
	bs->setWriteOffset(offset);
}

void raknetBitStreamResetReadPointer(RakLuaBitStream* bs)
{
	bs->resetReadPointer();
}

void raknetBitStreamResetWritePointer(RakLuaBitStream* bs)
{
	bs->resetWritePointer();
}

void raknetBitStreamIgnoreBits(RakLuaBitStream* bs, int amount)
{
	bs->ignoreBits(amount);
}

void raknetBitStreamIgnoreBytes(RakLuaBitStream* bs, int amount)
{
	bs->ignoreBytes(amount);
}

int raknetBitStreamGetNumberOfBitsUsed(RakLuaBitStream* bs)
{
	return bs->getNumberOfBitsUsed();
}

int raknetBitStreamGetNumberOfBytesUsed(RakLuaBitStream* bs)
{
	return bs->getNumberOfBytesUsed();
}

int raknetBitStreamGetNumberOfUnreadBits(RakLuaBitStream* bs)
{
	return bs->getNumberOfUnreadBits();
}

int raknetBitStreamGetNumberOfUnreadBytes(RakLuaBitStream* bs)
{
	return bs->getNumberOfUnreadBytes();
}

int raknetBitStreamGetWriteOffset(RakLuaBitStream* bs)
{
	return bs->getWriteOffset();
}

int raknetBitStreamGetReadOffset(RakLuaBitStream* bs)
{
	return bs->getReadOffset();
}

bool raknetBitStreamReadBool(RakLuaBitStream* bs)
{
	return bs->readBool();
}

uint8_t raknetBitStreamReadInt8(RakLuaBitStream* bs)
{
	return bs->readUInt8();
}

uint16_t raknetBitStreamReadInt16(RakLuaBitStream* bs)
{
	return bs->readUInt16();
}

int32_t raknetBitStreamReadInt32(RakLuaBitStream* bs)
{
	return bs->readInt32();
}

float raknetBitStreamReadFloat(RakLuaBitStream* bs)
{
	return bs->readFloat();
}

std::string raknetBitStreamReadString(RakLuaBitStream* bs, int length)
{
	return bs->readString(length);
}

std::string raknetBitStreamDecodeString(RakLuaBitStream* bs, int size)
{
	return bs->readEncoded(size);
}

void raknetBitStreamWriteBool(RakLuaBitStream* bs, bool value)
{
	bs->writeBool(value);
}

void raknetBitStreamWriteInt8(RakLuaBitStream* bs, uint8_t value)
{
	bs->writeUInt8(value);
}

void raknetBitStreamWriteInt16(RakLuaBitStream* bs, uint16_t value)
{
	bs->writeUInt16(value);
}

void raknetBitStreamWriteInt32(RakLuaBitStream* bs, int32_t value)
{
	bs->writeInt32(value);
}

void raknetBitStreamWriteFloat(RakLuaBitStream* bs, float value)
{
	bs->writeFloat(value);
}

void raknetBitStreamWriteString(RakLuaBitStream* bs, std::string_view value)
{
	bs->writeString(value);
}

void raknetBitStreamEncodeString(RakLuaBitStream* bs, std::string_view value)
{
	bs->writeEncoded(value);
}

bool raknetSendRpc(int rpcId, RakLuaBitStream* bs)
{
	return bs->sendRPC(rpcId);
}

bool raknetSendRpcEx(int rpcId, RakLuaBitStream* bs, PacketPriority priority, PacketReliability reliability, uint8_t channel, bool timestamp)
{
	return bs->sendRPCEx(rpcId, priority, reliability, channel, timestamp);
}

bool raknetSendBitStream(RakLuaBitStream* bs)
{
	return bs->sendPacket();
}

bool raknetSendBitStreamEx(RakLuaBitStream* bs, PacketPriority priority, PacketReliability reliability, uint8_t channel)
{
	return bs->sendPacketEx(priority, reliability, channel);
}

bool raknetEmulRpcReceiveBitStream(uint8_t rpcId, RakLuaBitStream* bs)
{
	return bs->emulIncomingRPC(rpcId);
}

bool raknetEmulPacketReceiveBitStream(uint8_t packetId /* Why, FYP, why?!*/, RakLuaBitStream* bs)
{
	BitStream send_bs;
	send_bs.Write(packetId);
	send_bs.WriteBits(bs->getBitStream()->GetData(), bs->getNumberOfBitsUsed(), false);
	return RakLuaBitStream(&send_bs).emulIncomingPacket();
}

// samp*()

void sampSendRconCommand(std::string_view cmd)
{
	BitStream bs;
	bs.Write<uint8_t>(201);
	bs.Write(cmd.size());
	bs.Write(cmd.data(), cmd.size());
	RakLuaBitStream(&bs).sendPacket();
}

void sampSendClickPlayer(uint16_t playerId, uint8_t source)
{
	BitStream bs;
	bs.Write(playerId);
	bs.Write(source);
	RakLuaBitStream(&bs).sendRPC(23);
}

void sampSendDialogResponse(uint16_t id, uint8_t button, int16_t listItem, std::string_view string = "")
{
	BitStream bs;
	bs.Write(id);
	bs.Write(button);
	bs.Write(listItem);
#pragma warning(disable : 4267) // 'var' : conversion from 'size_t' to 'type', possible loss of data
	bs.Write<uint8_t>(string.size());
#pragma warning(default : 4267 )
	bs.Write(string.data(), string.size());
	RakLuaBitStream(&bs).sendRPC(62);
}

void sampSendClickTextdraw(uint16_t id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(62);
}

void sampSendGiveDamage(uint16_t id, float damage, int weapon, int bodyPart)
{
	BitStream bs;
	bs.Write(false);
	bs.Write(id);
	bs.Write(damage);
	bs.Write(weapon);
	bs.Write(bodyPart);
	RakLuaBitStream(&bs).sendRPC(115);
}

void sampSendTakeDamage(uint16_t id, float damage, int weapon, int bodyPart)
{
	BitStream bs;
	bs.Write(true);
	bs.Write(id);
	bs.Write(damage);
	bs.Write(weapon);
	bs.Write(bodyPart);
	RakLuaBitStream(&bs).sendRPC(115);
}

void sampSendEditObject(bool playerObject, uint16_t objectId, int response, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	BitStream bs;
	bs.Write(playerObject);
	bs.Write(objectId);
	bs.Write(response);

	bs.Write(posX);
	bs.Write(posY);
	bs.Write(posZ);

	bs.Write(rotX);
	bs.Write(rotY);
	bs.Write(rotZ);

	RakLuaBitStream(&bs).sendRPC(117);
}

// TODO: COLOR?
void sampSendEditAttachedObject(int response, int index, int model, int bone, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
{
	BitStream bs;
	bs.Write(response);
	bs.Write(index);
	bs.Write(model);
	bs.Write(bone);

	bs.Write(posX);
	bs.Write(posY);
	bs.Write(posZ);

	bs.Write(rotX);
	bs.Write(rotY);
	bs.Write(rotZ);

	bs.Write(scaleX);
	bs.Write(scaleY);
	bs.Write(scaleZ);

	bs.Write<int>(0);
	bs.Write<int>(0);

	RakLuaBitStream(&bs).sendRPC(116);
}

void sampSendInteriorChange(uint8_t id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(118);
}

void sampSendRequestSpawn()
{
	RakLuaBitStream().sendRPC(129);
}

void sampSendPickedUpPickup(int id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(131);
}

void sampSendMenuSelectRow(uint8_t id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(132);
}

void sampSendMenuQuit()
{
	RakLuaBitStream().sendRPC(140);
}

void sampSendVehicleDestroyed(uint16_t id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(136);
}

void sampSendDeathByPlayer(uint8_t reason, uint16_t id)
{
	BitStream bs;
	bs.Write(reason);
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(53);
}

void sampSendChat(std::string_view text)
{
	if (text.size() == 0)
		text = ")";

	BitStream bs;
	if (text.at(0) == '/')
	{
		bs.Write<uint32_t>(text.size());
		bs.Write(text.data(), text.size());
		RakLuaBitStream(&bs).sendRPC(50);
	}
	else
	{
#pragma warning(disable : 4267) // 'var' : conversion from 'size_t' to 'type', possible loss of data
		bs.Write<uint8_t>(text.size());
#pragma warning(default : 4267)
		bs.Write(text.data(), text.size());
		RakLuaBitStream(&bs).sendRPC(101);
	}
}

void sampSendEnterVehicle(uint16_t id, bool passenger)
{
	BitStream bs;
	bs.Write(id);
	bs.Write(passenger);
	RakLuaBitStream(&bs).sendRPC(26);
}

void sampSendExitVehicle(uint16_t id)
{
	BitStream bs;
	bs.Write(id);
	RakLuaBitStream(&bs).sendRPC(154);
}

void sampSendSpawn()
{
	RakLuaBitStream().sendRPC(52);
}

void sampSendDamageVehicle(uint16_t id, int panel, int doors, uint8_t lights, uint8_t tires)
{
	BitStream bs;
	bs.Write(id);
	bs.Write(panel);
	bs.Write(doors);
	bs.Write(lights);
	bs.Write(tires);
	RakLuaBitStream(&bs).sendRPC(106);
}

void sampSendUpdatePlayers()
{
	static DWORD dwLastUpdateTick = 0;
	if ((GetTickCount() - dwLastUpdateTick) > 3000) {
		dwLastUpdateTick = GetTickCount();
		RakLuaBitStream().sendRPC(155);
	}
}
