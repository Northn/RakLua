#include "pch.h"
#include "lua_module.h"
#include "RakLua.h"
#include "RakLuaBitStream.h"
#include "MoonFunctions.hpp"

void initEnums(sol::state_view& lua)
{
	lua.new_enum<RakLua::eEventType>("RakLuaEvents",
		{
			{ "INCOMING_RPC", RakLua::eEventType::INCOMING_RPC },
			{ "INCOMING_PACKET", RakLua::eEventType::INCOMING_PACKET },
			{ "OUTGOING_RPC", RakLua::eEventType::OUTGOING_RPC },
			{ "OUTGOING_PACKET", RakLua::eEventType::OUTGOING_PACKET }
		}
	);

	lua.new_enum<SAMPVER>("RakLuaSampVersions",
		{
			{ "SAMP_NOT_LOADED", SAMPVER::SAMP_NOT_LOADED },
			{ "SAMP_UNKNOWN", SAMPVER::SAMP_UNKNOWN },
			{ "SAMP_037_R1", SAMPVER::SAMP_037_R1 },
			{ "SAMP_037_R3_1", SAMPVER::SAMP_037_R3_1 },
		}
	);

	lua.new_enum<RakLua::eInitState>("RakLuaState",
		{
			{ "OK", RakLua::eInitState::OK },
			{ "SAMP_NOT_LOADED", RakLua::eInitState::SAMP_NOT_LOADED },
			{ "SAMP_UNKNOWN", RakLua::eInitState::SAMP_UNKNOWN },
			{ "INITIALIZING", RakLua::eInitState::INITIALIZING }
		}
	);
}

#define INIT_METHOD(name) bs_type[#name] = &RakLuaBitStream::name

void initBitStream(sol::state_view& lua)
{
	sol::usertype<RakLuaBitStream> bs_type = lua.new_usertype<RakLuaBitStream>("RakLuaBitStream", sol::constructors<RakLuaBitStream()>());
	
	INIT_METHOD(getNumberOfBitsUsed);
	INIT_METHOD(getNumberOfBytesUsed);

	INIT_METHOD(getNumberOfUnreadBits);
	INIT_METHOD(getNumberOfUnreadBytes);

	INIT_METHOD(resetReadPointer);
	INIT_METHOD(resetWritePointer);

	INIT_METHOD(setReadOffset);
	INIT_METHOD(setWriteOffset);

	INIT_METHOD(getReadOffset);
	INIT_METHOD(getWriteOffset);

	INIT_METHOD(ignoreBits);
	INIT_METHOD(ignoreBytes);

	INIT_METHOD(readBool);
	INIT_METHOD(readInt8);
	INIT_METHOD(readInt16);
	INIT_METHOD(readInt32);
	INIT_METHOD(readUInt8);
	INIT_METHOD(readUInt16);
	INIT_METHOD(readUInt32);
	INIT_METHOD(readFloat);
	INIT_METHOD(readString);
	INIT_METHOD(readEncoded);
	INIT_METHOD(readBuffer);

	INIT_METHOD(writeBool);
	INIT_METHOD(writeInt8);
	INIT_METHOD(writeInt16);
	INIT_METHOD(writeInt32);
	INIT_METHOD(writeUInt8);
	INIT_METHOD(writeUInt16);
	INIT_METHOD(writeUInt32);
	INIT_METHOD(writeFloat);
	INIT_METHOD(writeString);
	INIT_METHOD(writeEncoded);
	INIT_METHOD(writeBuffer);
	INIT_METHOD(writeBitStream);

	INIT_METHOD(emulIncomingRPC);
	INIT_METHOD(emulIncomingPacket);

	INIT_METHOD(sendRPC);
	INIT_METHOD(sendPacket);
	INIT_METHOD(sendRPCEx);
	INIT_METHOD(sendPacketEx);

	INIT_METHOD(getBitStream);
	INIT_METHOD(getDataPtr);

	// Pseudonimes
	bs_type["skipBits"] = &RakLuaBitStream::ignoreBits;
	bs_type["skipBytes"] = &RakLuaBitStream::ignoreBytes;
	bs_type["reset"] = &RakLuaBitStream::resetBs;
}

#undef INIT_METHOD

#define INIT_FUNCTION(name) lua.set_function(#name, &name)

void defineGlobals(sol::this_state ts)
{
	sol::state_view lua(ts);

	INIT_FUNCTION(raknetNewBitStream);
	INIT_FUNCTION(raknetDeleteBitStream);
	INIT_FUNCTION(raknetResetBitStream);

	INIT_FUNCTION(raknetBitStreamSetReadOffset);
	INIT_FUNCTION(raknetBitStreamSetWriteOffset);

	INIT_FUNCTION(raknetBitStreamResetReadPointer);
	INIT_FUNCTION(raknetBitStreamResetWritePointer);

	INIT_FUNCTION(raknetBitStreamIgnoreBits);
	INIT_FUNCTION(raknetBitStreamIgnoreBytes);

	INIT_FUNCTION(raknetBitStreamGetNumberOfBitsUsed);
	INIT_FUNCTION(raknetBitStreamGetNumberOfBytesUsed);

	INIT_FUNCTION(raknetBitStreamGetNumberOfUnreadBits);
	INIT_FUNCTION(raknetBitStreamGetNumberOfUnreadBytes);

	INIT_FUNCTION(raknetBitStreamGetWriteOffset);
	INIT_FUNCTION(raknetBitStreamGetReadOffset);

	INIT_FUNCTION(raknetBitStreamReadBool);
	INIT_FUNCTION(raknetBitStreamReadInt8);
	INIT_FUNCTION(raknetBitStreamReadInt16);
	INIT_FUNCTION(raknetBitStreamReadInt32);
	INIT_FUNCTION(raknetBitStreamReadFloat);
	INIT_FUNCTION(raknetBitStreamReadString);
	INIT_FUNCTION(raknetBitStreamDecodeString);
	INIT_FUNCTION(raknetBitStreamReadBuffer);

	INIT_FUNCTION(raknetBitStreamWriteBool);
	INIT_FUNCTION(raknetBitStreamWriteInt8);
	INIT_FUNCTION(raknetBitStreamWriteInt16);
	INIT_FUNCTION(raknetBitStreamWriteInt32);
	INIT_FUNCTION(raknetBitStreamWriteFloat);
	INIT_FUNCTION(raknetBitStreamWriteString);
	INIT_FUNCTION(raknetBitStreamEncodeString);
	INIT_FUNCTION(raknetBitStreamWriteBuffer);
	INIT_FUNCTION(raknetBitStreamWriteBitStream);

	INIT_FUNCTION(raknetSendRpc);
	INIT_FUNCTION(raknetSendBitStream);
	INIT_FUNCTION(raknetSendRpcEx);
	INIT_FUNCTION(raknetSendBitStreamEx);

	INIT_FUNCTION(raknetEmulRpcReceiveBitStream);
	INIT_FUNCTION(raknetEmulPacketReceiveBitStream);

	INIT_FUNCTION(raknetBitStreamGetDataPtr);

	INIT_FUNCTION(sampSendRconCommand);
	INIT_FUNCTION(sampSendClickPlayer);
	INIT_FUNCTION(sampSendDialogResponse);
	INIT_FUNCTION(sampSendClickTextdraw);
	INIT_FUNCTION(sampSendGiveDamage);
	INIT_FUNCTION(sampSendTakeDamage);
	INIT_FUNCTION(sampSendEditObject);
	INIT_FUNCTION(sampSendEditAttachedObject);
	INIT_FUNCTION(sampSendInteriorChange);
	INIT_FUNCTION(sampSendRequestSpawn);
	INIT_FUNCTION(sampSendPickedUpPickup);
	INIT_FUNCTION(sampSendMenuSelectRow);
	INIT_FUNCTION(sampSendMenuQuit);
	INIT_FUNCTION(sampSendVehicleDestroyed);
	INIT_FUNCTION(sampSendDeathByPlayer);
	INIT_FUNCTION(sampSendChat);
	INIT_FUNCTION(sampSendEnterVehicle);
	INIT_FUNCTION(sampSendExitVehicle);
	INIT_FUNCTION(sampSendSpawn);
	INIT_FUNCTION(sampSendDamageVehicle);
	INIT_FUNCTION(sampSendUpdatePlayers);
}

#undef INIT_FUNCTION

bool registerHandler(sol::this_state ts, RakLua::eEventType type, sol::function fs)
{
	return gRakLua.addEventHandler(ts, type, fs);
}

void destroyHandlers(sol::this_state ts)
{
	gRakLua.destroyHandlers(ts);
}

void defineOtherFunctions(sol::table& module)
{
	module.set_function("sendRconCommand", &sampSendRconCommand);
	module.set_function("sendClickPlayer", &sampSendClickPlayer);
	module.set_function("sendDialogResponse", &sampSendDialogResponse);
	module.set_function("sendClickTextdraw", &sampSendClickTextdraw);
	module.set_function("sendGiveDamage", &sampSendGiveDamage);
	module.set_function("sendTakeDamage", &sampSendTakeDamage);
	module.set_function("sendEditObject", &sampSendEditObject);
	module.set_function("sendEditAttachedObject", &sampSendEditAttachedObject);
	module.set_function("sendInteriorChange", &sampSendInteriorChange);
	module.set_function("sendRequestSpawn", &sampSendRequestSpawn);
	module.set_function("sendPickedUpPickup", &sampSendPickedUpPickup);
	module.set_function("sendMenuSelectRow", &sampSendMenuSelectRow);
	module.set_function("sendMenuQuit", &sampSendMenuQuit);
	module.set_function("sendVehicleDestroyed", &sampSendVehicleDestroyed);
	module.set_function("sendDeathByPlayer", &sampSendDeathByPlayer);
	module.set_function("sendChat", &sampSendChat);
	module.set_function("sendEnterVehicle", &sampSendEnterVehicle);
	module.set_function("sendExitVehicle", &sampSendExitVehicle);
	module.set_function("sendSpawn", &sampSendSpawn);
	module.set_function("sendDamageVehicle", &sampSendDamageVehicle);
	module.set_function("sendUpdatePlayers", &sampSendUpdatePlayers);
}

RakLua::eInitState getState(sol::this_state ts)
{
	return gRakLua.getState();
}

sol::table open(sol::this_state ts)
{
	sol::state_view lua(ts);

	if (gRakLua.getState() == RakLua::eInitState::NOT_INITIALIZED && *reinterpret_cast<uintptr_t*>(0xC8D4C0) == 9)
		lua["print"]("[RakLua] ERROR! Your game is already started.\n\tThis is inacceptible and RakLua can't work properly as long as RakPeer is already initialized.\n\tPlease, restart your game. RakLua must start together with game and SAMP.");
	else
		gRakLua.initialize();

	sol::table module = lua.create_table();
	module["VERSION"] = 2.1;
	module.set_function("getState", &getState);

	module.set_function("registerHandler", &registerHandler);
	module.set_function("destroyHandlers", &destroyHandlers);
	module.set_function("getSampVersion", &sampGetVersion);
	module.set_function("defineGlobals", &defineGlobals);

	initEnums(lua);
	initBitStream(lua);
	defineOtherFunctions(module);

	return module;
}

SOL_MODULE_ENTRYPOINT(open);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// pin DLL to prevent unloading
		HMODULE module;
		GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<LPCWSTR>(&DllMain), &module);
	}
	return TRUE;
}
