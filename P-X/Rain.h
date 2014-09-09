#include "Authorization.h"

// Module Variables.
HINSTANCE hInst = NULL;

// Current directory.
string currentDir;

// PushToConsole declaration.
typedef int (__cdecl *PUSHCONSOLE) ( const char* szCommand );

// Ingame.
bool Ingame = false;
int  IngameCount = 0;

// DrawIndexedPrimitive
// Target: vTable [82].
DWORD dwDrawIndexedPrim = NULL;

// Hardware Breakpoints.
DWORD dwTarget [4];

// WolfTeam options.
BOOL WIS = FALSE;
BOOL WTS = FALSE;

// Set Focus on 'MainDlg'.
bool setFocus = false;

// WolfTeam.
HWND hwndWolfTeam = NULL;
BOOL diffVersion = FALSE;
BOOL diffReset   = FALSE;

// 'Player Movement' Registers.
DWORD dwEcx = NULL;
DWORD dwEbx = NULL;
DWORD dwEax = NULL;
DWORD dwEdx = NULL;
DWORD dwEsi = NULL;
DWORD dwEdi = NULL;
DWORD dwEbp = NULL;
DWORD dwEsp = NULL;

// Teleport Variables.
float teleportX = 0;
float teleportY = 0;
float teleportZ = 0;

// Respawn Where Died.
float respawnX = 0;
float respawnY = 0;
float respawnZ = 0;

// Player Coordinate Locations.
DWORD playerX = NULL;
DWORD playerY = NULL;
DWORD playerZ = NULL;

// Return to Base.
DWORD dwRTB = NULL;

// Respawn Where Died.
BOOL RespawnNow = FALSE;
BOOL ReturnBase = FALSE;

// Telekill.
DWORD  dwObjectY [500];
DWORD  Blacklist [500];

int blacklistNum;
int numObjects;    // Total objects
int currentObject; // Current object

BOOL CheckList ( DWORD dwVal )
{
	for ( int i = 0; i < blacklistNum; i ++ )
		if ( dwVal == Blacklist [i] )
			return FALSE;
	return TRUE;
}
//

// Infinite Ammo.
bool shooting = false;

// Direct3D Options.
BOOL Chams    = FALSE;
BOOL WallHack = FALSE;
BOOL XRay     = FALSE;
BOOL FullBright = FALSE;
BOOL WireFrame  = FALSE;
BOOL NoFog    = FALSE;

// Memory Options.
BOOL NoFallDamage = FALSE;
BOOL NoClip       = FALSE;
BOOL InfiniteAmmo = FALSE;
BOOL bTelekill    = FALSE;
BOOL NoGravity    = FALSE;
BOOL SpeedHack    = FALSE;
BOOL SuperJump    = FALSE;
BOOL NoReload     = FALSE;
BOOL sj1          = FALSE;
BOOL sj2          = FALSE;
BOOL sj3          = FALSE;
BOOL sh1          = FALSE;
BOOL sh2          = FALSE;
BOOL sh3          = FALSE;

// No Clip.
BOOL falling = FALSE;

// No Spread.
DWORD dwSpreadAddr = NULL;

// Inventory.
DWORD dwMainWpn = 7102;
DWORD dwSecdWpn = 1009;

// Return To Base.
BOOL Spawned = FALSE;

// Direct3D Resources.
BOOL ResetResources = FALSE;

// No Spread.
DWORD dwLocateJmp = NULL;

// Debugging.
BOOL  dbgMode    = FALSE;
BOOL  stopBrk    = FALSE;
DWORD dwDebug    = NULL;
DWORD dwDebugJmp = NULL;
DWORD dwDbgEdx   = NULL;

// Destroy signatures here.
void destroySig ()
{
	currentObject = numObjects - 1;
}
void destroySig2 ()
{
	--currentObject;
}
void destroySig3 ()
{
	++currentObject;
}
void destroySig4 ()
{
	currentObject = 0;
}
//