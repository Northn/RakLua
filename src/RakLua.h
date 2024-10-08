#pragma once

#include "RakLuaBitStream.h"
#include "kthook/kthook.hpp"
#include "samp.hpp"

class RakLua
{
public:
	enum eEventType { INCOMING_RPC, INCOMING_PACKET, OUTGOING_RPC, OUTGOING_PACKET };
	enum eInitState { OK, SAMP_NOT_LOADED, SAMP_UNKNOWN, INITIALIZING, NOT_INITIALIZED /* for internal usage */ };
	struct event_handler_t { int id; sol::function handler; };
private:
	eInitState		mState = eInitState::NOT_INITIALIZED;

	rtdhook_vmt*								mVmtHook = nullptr;
	kthook::kthook_simple<bool(__thiscall*)(void*, unsigned char*, int, PlayerID)>       mIncomingRpcHandlerHook;
	kthook::kthook_simple<uintptr_t(*)()>		mRakClientIntfConstructor;

	struct handlers {
		std::vector<event_handler_t> incomingRpc;
		std::vector<event_handler_t> outgoingRpc;

		std::vector<event_handler_t> incomingPacket;
		std::vector<event_handler_t> outgoingPacket;
	} mHandlers;
public:
	eInitState	initialize();
	bool		destroy();
	
	void		postRakClientInitialization(uintptr_t rakClientIntf);

	bool		addEventHandler(sol::this_state& ts, eEventType type, sol::function &detour_f);
	void		destroyHandlers(sol::this_state& ts);

	inline bool			isInitialized() { return mState == eInitState::OK; }
	inline eInitState	getState()		{ return mState; }
	inline rtdhook_vmt* getVmtHook()	{ return mVmtHook; };
	inline handlers&	getHandlers()	{ return mHandlers; };
	inline auto&        getRpcHook()	{ return mIncomingRpcHandlerHook; };
	inline auto&		getIntfConstructorHook() { return mRakClientIntfConstructor; }

	template <typename... Args>
	static bool safeCall(sol::function handler, Args&&... args);
};

inline RakLua gRakLua;
inline void* gRakPeer = nullptr;
inline PlayerID gPlayerId;

#define ID_TIMESTAMP 40
#define MAX_ALLOCA_STACK_ALLOCATION 1048576
#define ID_RPC 20
#define RECEIVE_PACKET(ptr) reinterpret_cast<Packet*(__thiscall*)(void*)>(gRakLua.getVmtHook()->getOriginalMethod(8))(ptr)
#define DESTROY_HANDLERS(handlerList) \
	for (auto ptr = mHandlers.handlerList.begin(); ptr < mHandlers.handlerList.end();)	\
	{																					\
		if (id == ptr->id)																\
		{																				\
			ptr = mHandlers.handlerList.erase(ptr);										\
		}																				\
		else ptr++;																		\
	}
#define ADD_EVENT_HANDLER(handlerList) mHandlers.handlerList.push_back({ id, detour }); break

bool __fastcall handleOutgoingPacket(void* ptr, void*, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
Packet* __fastcall handleIncomingPacket(void* ptr, void*);
bool __fastcall handleOutgoingRpc(void* ptr, void*, int* id, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp);
bool handleIncomingRpc(const kthook::kthook_simple<bool(__thiscall*)(void*, unsigned char*, int, PlayerID)>& hook, void* ptr, unsigned char* data, int length, PlayerID playerId);
uintptr_t hookRakClientIntfConstructor(const kthook::kthook_simple<uintptr_t(*)()>& hook);
