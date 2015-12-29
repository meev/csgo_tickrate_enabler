#include <Windows.h>

#include "igameevents.h"
#include "eiface.h"
#include "tier0/icommandline.h"

#include "detours.h"
#pragma comment(lib, "detours.lib") 

class CSGOPlugin : public IServerPluginCallbacks, public IGameEventListener
{
public:
	CSGOPlugin();
	~CSGOPlugin();

	// IServerPluginCallbacks methods
public:
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void			Unload(void);
	virtual void			Pause(void) {};
	virtual void			UnPause(void) {};
	virtual const char		*GetPluginDescription(void);
	virtual void			LevelInit(char const *pMapName);
	virtual void			ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(edict_t *pEntity);
	virtual void			ClientFullyConnect(edict_t *pEntity);
	virtual void			ClientDisconnect(edict_t *pEntity);
	virtual void			ClientPutInServer(edict_t *pEntity, char const *playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(edict_t *pEdict);
	virtual PLUGIN_RESULT	ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual PLUGIN_RESULT	ClientCommand(edict_t *pEntity, const CCommand &args);
	virtual PLUGIN_RESULT	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
	virtual void			OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);
	virtual void			OnEdictAllocated(edict_t *edict);
	virtual void			OnEdictFreed(const edict_t *edict);

	// IGameEventListener methods
public:
	virtual void			FireGameEvent(KeyValues *event);

	// Internal interface
public:
	int						GetCommandIndex() { return m_iClientCommandIndex; }

private:
	int m_iClientCommandIndex;
};

typedef float(__stdcall* tGetTickInterval)();

// memdbgon must be the last include file in a .cpp file!!!
//#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Interface globals
//-----------------------------------------------------------------------------
IServerGameDLL					*g_pServerGameDll = NULL;

//-----------------------------------------------------------------------------
// Hooks
//-----------------------------------------------------------------------------
tGetTickInterval pGetTickInterval;

//-----------------------------------------------------------------------------
// Singleton accessor
//-----------------------------------------------------------------------------
CSGOPlugin g_CSGOPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CHelperPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_CSGOPlugin);

//-----------------------------------------------------------------------------
// Purpose: constructor/destructor
//-----------------------------------------------------------------------------
CSGOPlugin::CSGOPlugin()
{
	m_iClientCommandIndex = 0;
}

CSGOPlugin::~CSGOPlugin()
{
}

float __stdcall new_GetTickInterval()
{
	float tickinterval = DEFAULT_TICK_INTERVAL;

	if (CommandLine()->CheckParm("-tickrate"))
	{
		float tickrate = CommandLine()->ParmValue("-tickrate", 0);
		if (tickrate > 10)
			tickinterval = 1.0f / tickrate;
	}

	return tickinterval;
}

//-----------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//-----------------------------------------------------------------------------
bool CSGOPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	g_pServerGameDll = (IServerGameDLL*)gameServerFactory("ServerGameDLL005", NULL);
	
	if (!g_pServerGameDll)
	{
		Warning("\n--------------------Could not load CSGO Tickrate Enabler---------------------\n\n\n");
		return false;
	}
	
	DWORD* pdwServerGameDll = (DWORD*)*(DWORD*)g_pServerGameDll;
	pGetTickInterval = (tGetTickInterval)DetourFunction((PBYTE)pdwServerGameDll[9], (PBYTE)new_GetTickInterval);

	Warning("\n\n--------------------CSGO Tickrate Enabler Loaded---------------------\n\n\n");
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//-----------------------------------------------------------------------------
void CSGOPlugin::Unload(void)
{
	DetourRemove((PBYTE)pGetTickInterval, (PBYTE)new_GetTickInterval);
}

//-----------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//-----------------------------------------------------------------------------
const char *CSGOPlugin::GetPluginDescription(void)
{
	return "CSGO Tickrate Enabler";
}

//-----------------------------------------------------------------------------
// Purpose: called on level start
//-----------------------------------------------------------------------------
void CSGOPlugin::LevelInit(char const *pMapName)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CSGOPlugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CSGOPlugin::GameFrame(bool simulating)
{
}

//-----------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//-----------------------------------------------------------------------------
void CSGOPlugin::LevelShutdown(void)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CSGOPlugin::ClientActive(edict_t *pEntity)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is fully connected to server
//---------------------------------------------------------------------------------
void CSGOPlugin::ClientFullyConnect(edict_t *pEntity)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CSGOPlugin::ClientDisconnect(edict_t *pEntity)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CSGOPlugin::ClientPutInServer(edict_t *pEntity, char const *playername)
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CSGOPlugin::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CSGOPlugin::ClientSettingsChanged(edict_t *pEdict)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSGOPlugin::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSGOPlugin::ClientCommand(edict_t *pEntity, const CCommand &args)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CSGOPlugin::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void CSGOPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when an edict gets allocated
//---------------------------------------------------------------------------------
void CSGOPlugin::OnEdictAllocated(edict_t *edict)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when an edict gets freed
//---------------------------------------------------------------------------------
void CSGOPlugin::OnEdictFreed(const edict_t *edict)
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a game event is fired
//---------------------------------------------------------------------------------
void CSGOPlugin::FireGameEvent(KeyValues *event)
{
}

