#include "pch.h"
#include "RakLua.h"

RakLua::eInitState RakLua::initialize()
{
	if (mState != NOT_INITIALIZED)
		goto returnState;

	uintptr_t samp = sampGetBase();
	if (samp == SAMP_NOT_LOADED)
	{
		mState = SAMP_NOT_LOADED;
		goto returnState;
	}

	SAMPVER sampVersion = sampGetVersion();
	if (sampVersion == SAMP_UNKNOWN)
	{
		mState = SAMP_UNKNOWN;
		goto returnState;
	}

	mState = INITIALIZING;

	std::thread([&]() {
		uintptr_t sampInfo = sampGetSampInfoPtr();
		while (!sampInfo)
			sampInfo = sampGetSampInfoPtr();

		mVmtHook = new rtdhook_vmt(*reinterpret_cast<uintptr_t*>(sampGetRakClientInterface()));
		mVmtHook->install(6, &handleOutgoingPacket);
		mVmtHook->install(8, &handleIncomingPacket);
		mVmtHook->install(25, &handleOutgoingRpc);

		mState = OK;
	}).detach();

	mIncomingRpcHandlerHook = new rtdhook(sampGetIncomingRpcHandlerPtr(), &handleIncomingRpc);
	mIncomingRpcHandlerHook->install();

	mRakClientIntfConstructor = new rtdhook(sampGetRakClientIntfConstructorPtr(), &hookRakClientIntfConstructor, 7);
	mRakClientIntfConstructor->install();

returnState:
	return mState;
}

bool RakLua::addEventHandler(sol::this_state& ts, eEventType type, sol::function detour)
{
	sol::state_view lua{ ts };
	int id = lua["script"]["this"]["id"];

	switch (type)
	{
	case INCOMING_RPC:
		mHandlers.incomingRpc.push_back({ id, detour });
		break;
	case OUTGOING_RPC:
		mHandlers.outgoingRpc.push_back({ id, detour });
		break;
	case INCOMING_PACKET:
		mHandlers.incomingPacket.push_back({ id, detour });
		break;
	case OUTGOING_PACKET:
		mHandlers.outgoingPacket.push_back({ id, detour });
		break;
	default:
		return false;
	}
	return true;
}

void RakLua::destroyHandlers(sol::this_state& ts)
{
	sol::state_view lua{ ts };
	int id = lua["script"]["this"]["id"];

	size_t i = 0;
	for (auto& handler : mHandlers.incomingRpc)
	{
		if (id == handler.id)
		{
			mHandlers.incomingRpc.erase(mHandlers.incomingRpc.begin() + i);
			//break; maybe several hooks?
		}
		i++;
	}
	i = 0;
	for (auto& handler : mHandlers.incomingPacket)
	{
		if (id == handler.id)
		{
			mHandlers.incomingPacket.erase(mHandlers.incomingPacket.begin() + i);
			//break; maybe several hooks?
		}
		i++;
	}
	i = 0;
	for (auto& handler : mHandlers.outgoingRpc)
	{
		if (id == handler.id)
		{
			mHandlers.outgoingRpc.erase(mHandlers.outgoingRpc.begin() + i);
			//break; maybe several hooks?
		}
		i++;
	}
	i = 0;
	for (auto& handler : mHandlers.outgoingPacket)
	{
		if (id == handler.id)
		{
			mHandlers.outgoingPacket.erase(mHandlers.outgoingPacket.begin() + i);
			//break; maybe several hooks?
		}
		i++;
	}
}

template <typename... Args>
static bool RakLua::safeCall(sol::function handler, Args&&... args)
{
	try
	{
		sol::protected_function_result result = handler(std::forward<Args>(args)...);
		if (result.valid())
		{
			int count = result.return_count();
			if (count == 1 && result.get_type() == sol::type::boolean)
			{
				bool ret = result;
				if (!ret)
					return false;
			}
		}
	}
	catch (...)
	{
		return true;
	}
	return true;
}

bool __fastcall handleOutgoingPacket(void* ptr, void*, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	RakLuaBitStream luaBs(bitStream);

	uint8_t eventId = luaBs.readUInt8();

	for (auto& handler : gRakLua.getHandlers().outgoingPacket)
	{
		luaBs.resetReadPointer();
		if (!RakLua::safeCall(handler.handler, eventId, &luaBs))
			return false;
	}
	return reinterpret_cast<bool(__thiscall*)(void*, BitStream*, PacketPriority, PacketReliability, char)>
		(gRakLua.getVmtHook()->getOriginalMethod(6))(ptr, bitStream, priority, reliability, orderingChannel);
}

bool inlineIncomingPacketHandler(uint8_t id, RakLuaBitStream& luaBs)
{
	for (auto& handler : gRakLua.getHandlers().incomingPacket)
	{
		luaBs.resetReadPointer();
		if (!RakLua::safeCall(handler.handler, id, &luaBs))
		{
			return false;
		}
	}
	return true;
}

Packet* __fastcall handleIncomingPacket(void* ptr, void*)
{
	Packet* packet = RECEIVE_PACKET(ptr);
	if (packet == nullptr || packet->data == nullptr || packet->length == 0 || packet->bitSize == 0)
		return packet;
	
	RakLuaBitStream luaBs(packet->data, packet->bitSize, false);
	uint8_t id = luaBs.readUInt8();

	while (packet != nullptr)
	{
		if (inlineIncomingPacketHandler(id, luaBs))
			break;

		reinterpret_cast<void(__thiscall*)(void*, Packet*)>(gRakLua.getVmtHook()->getOriginalMethod(9))(ptr, packet);
		packet = RECEIVE_PACKET(ptr);
	}
	if (packet != nullptr)
	{
		packet->bitSize = luaBs.getNumberOfBitsUsed();
		packet->length = luaBs.getNumberOfBytesUsed();
	}
	return packet;
}

bool __fastcall handleOutgoingRpc(void* ptr, void*, int* id, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	RakLuaBitStream luaBs(bitStream);

	for (auto& handler : gRakLua.getHandlers().outgoingRpc)
	{
		luaBs.resetReadPointer();
		if (!RakLua::safeCall(handler.handler, *id, &luaBs))
			return false;
	}
	return reinterpret_cast<bool(__thiscall*)(void*, int*, BitStream*, PacketPriority, PacketReliability, char, bool)>
		(gRakLua.getVmtHook()->getOriginalMethod(25))(ptr, id, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

bool __fastcall handleIncomingRpc(void* ptr, void*, unsigned char* data, int length, PlayerID playerId)
{
	//gRakPeer = ptr;
	gPlayerId = playerId;

	uint8_t id = 0;
	uint32_t bits_data = 0;
	unsigned char* input = nullptr;
	BitStream* callback_bs;

	BitStream bs(data, length, true);
	bs.IgnoreBits(8);
	if (data[0] == ID_TIMESTAMP)
		bs.IgnoreBits(8 * (sizeof(RakNetTime) + sizeof(unsigned char)));

	int offset = bs.GetReadOffset();

	bs.Read(id);

	if (!bs.ReadCompressed(bits_data))
		return false;

	if (bits_data) {
		bool used_alloca = false;
		if (BITS_TO_BYTES(bs.GetNumberOfUnreadBits()) < MAX_ALLOCA_STACK_ALLOCATION) {
#pragma warning(disable : 6255) // warning C6255: _alloca indicates failure by raising a stack overflow exception. Consider using _malloca instead
			input = reinterpret_cast<unsigned char*>(alloca(BITS_TO_BYTES(bs.GetNumberOfUnreadBits())));
#pragma warning(default : 6255)
			used_alloca = true;
		}
		else input = new unsigned char[BITS_TO_BYTES(bs.GetNumberOfUnreadBits())];

		if (!bs.ReadBits(input, bits_data, false))
		{
			if (!used_alloca)
				delete[] input;
			return false; // Not enough data to read
		}

		callback_bs = new BitStream(input, BITS_TO_BYTES(bits_data), true);

		if (!used_alloca)
			delete[] input;
	}
	else
		return false;

	RakLuaBitStream luaBs(callback_bs);
	for (auto& handler : gRakLua.getHandlers().incomingRpc)
	{
		luaBs.resetReadPointer();
		if (!RakLua::safeCall(handler.handler, id, &luaBs))
		{
			delete callback_bs;
			return false;
		}
	}

	bs.SetWriteOffset(offset);
	bs.Write(id);
	bits_data = callback_bs->GetNumberOfBitsUsed();
	bs.WriteCompressed(bits_data);
	if (bits_data)
		bs.WriteBits(callback_bs->GetData(), bits_data, false);

	delete callback_bs;

	return reinterpret_cast<bool(__thiscall*)(void*, unsigned char*, int, PlayerID)>
		(gRakLua.getRpcHook()->getTrampoline())(ptr, bs.GetData(), bs.GetNumberOfBytesUsed(), playerId);
}

uintptr_t __stdcall hookRakClientIntfConstructor()
{
	uintptr_t rakClientInterface = reinterpret_cast<uintptr_t(__stdcall*)()>(gRakLua.getIntfConstructorHook()->getTrampoline())();
	if (rakClientInterface)
		gRakPeer = reinterpret_cast<void*>(rakClientInterface - 3550);
	return rakClientInterface;
}
