#include "GUI.h"

/*
	-> All memory related functions and locations go here along with their byte signatures.
*/

// Structures.
struct MODULE_INFO
{
	DWORD dwBaseAddr;
	DWORD dwSize;
};
//

// Module Information.
MODULE_INFO WolfTeam;
MODULE_INFO ClientFX;
MODULE_INFO CShell;
MODULE_INFO d3d9;

// vTable.
DWORD vTable [118];

// Scan variables.
DWORD dwResults [999];
int   results;

// Scan memory locations (FindPattern).
bool blDataCompare( const BYTE* pData, const BYTE* bMask, const char* szMask )
{ 
    for(; *szMask; ++szMask, ++pData, ++ bMask ) 
        if( *szMask == 'x' && *pData != *bMask )  
            return false; 
    return ( *szMask ) == NULL; 
}

DWORD dwFindPattern( DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask )
{
    for( DWORD i=0; i < dwLen; i++ ) 
  if( blDataCompare( ( BYTE* )( dwAddress+i ), bMask, szMask ) ) 
            return ( DWORD )( dwAddress + i ); 
 
    return 0; 
}
//

// Scan memory locations (ScanMemory).
void ScanMemory ( DWORD dwStartAddr, DWORD dwEndAddr, VOID* Signature, SIZE_T szSig )
{
	results = 0;

	// Change this to dwStartAddr
	for ( dwStartAddr; dwStartAddr < dwEndAddr; dwStartAddr ++ )
	{
		// Obtain region information
		MEMORY_BASIC_INFORMATION MBI;
		VirtualQuery ( (LPCVOID) dwStartAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		if ( MBI.State == MEM_COMMIT )
		{
			// End Address
			DWORD dwStopAddr = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize - 1 - szSig;

			for ( DWORD addr = (DWORD) MBI.BaseAddress; addr < dwStopAddr; addr ++ )
			{
				__try
				{
					// Check its signature
					if ( !memcmp ( (void*) addr, Signature, szSig ) )
					{
						dwResults [results] = addr;
						++results;
					}
				}
				__except ( true )
				{
					// :(
					break;
				}
			}
		}

		// Go to the next region
		dwStartAddr = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
	}
}
void QuickScan  ( DWORD dwStartAddr, DWORD dwEndAddr, VOID* Signature, SIZE_T szSig )
{
	BOOL quitScan = FALSE;

	results = 0;

	// Change this to dwStartAddr
	for ( dwStartAddr; dwStartAddr < dwEndAddr; dwStartAddr ++ )
	{
		// Obtain region information
		MEMORY_BASIC_INFORMATION MBI;
		VirtualQuery ( (LPCVOID) dwStartAddr, &MBI, sizeof ( MEMORY_BASIC_INFORMATION ) );

		if ( MBI.State == MEM_COMMIT )
		{
			// End Address
			DWORD dwStopAddr = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize - 1 - szSig;

			for ( DWORD addr = (DWORD) MBI.BaseAddress; addr < dwStopAddr; addr ++ )
			{
				__try
				{
					// Check its signature
					if ( !memcmp ( (void*) addr, Signature, szSig ) )
					{
						quitScan = TRUE;

						dwResults [results] = addr;
						++results;
						break;
					}
				}
				__except ( true )
				{
					// :(
					break;
				}
			}
		}

		// Go to the next region
		if ( quitScan )
			break;

		dwStartAddr = (DWORD) MBI.BaseAddress + (DWORD) MBI.RegionSize;
	}
}
//

// Locate module base.
MODULE_INFO LocateModule ( string ModuleName )
{
	string currentModule = "";

	// Create the 'MODULE_INFO' structure
	MODULE_INFO modInfo;
	
	// Other variables
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPALL, 0 );
	MODULEENTRY32 ME;
	ME.dwSize = sizeof ( MODULEENTRY32 );

	if ( Module32First ( hSnapshot, &ME ) )
	{
		currentModule = "";
		currentModule += ME.szModule;
		currentModule = currentModule.substr ( 0, ModuleName.length () );

		if ( !strcmp ( ToLowercase ( ModuleName ).c_str (), ToLowercase ( currentModule ).c_str () ) )
		{
			modInfo.dwBaseAddr = (DWORD) ME.modBaseAddr;
			modInfo.dwSize     = (DWORD) ME.modBaseSize;
		}

		while ( Module32Next ( hSnapshot, &ME ) )
		{			
			currentModule = "";
			currentModule += ME.szModule;
			currentModule = currentModule.substr ( 0, ModuleName.length () );

			if ( !strcmp ( ToLowercase ( ModuleName ).c_str (), ToLowercase ( currentModule ).c_str () ) )
			{
				modInfo.dwBaseAddr = (DWORD) ME.modBaseAddr;
				modInfo.dwSize     = (DWORD) ME.modBaseSize;
			}
		}
	}

	// Return
	return modInfo;
}
//

// Set a hook safely.
void SetMemory ( void* JumpTo, void* JumpFrom )
{
	// Create the hook
	BYTE bHook [5];
	bHook [0] = 0xE9;
	*(DWORD*) &bHook [1] = (DWORD) JumpTo - ( (DWORD) JumpFrom + 0x05 );

	// Set protection
	DWORD dwOldProtect;
	VirtualProtect ( JumpFrom, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	// Set the hook
	memcpy ( JumpFrom, &bHook, 5 );

	// Restore protection
	VirtualProtect ( JumpFrom, 5, dwOldProtect, NULL );
}
//

// GetKeyState.
DWORD dwGetKeyState = NULL;

__declspec ( naked ) SHORT WINAPI GetKeyStateX ( int nVirtKey )
{
	__asm
		jmp dword ptr [ dwGetKeyState ]
}
//

// Copy memory safely.
void CopyMem ( void* Destination, void* Source, SIZE_T szBytes )
{
	// Set protection
	DWORD dwOldProtect;
	VirtualProtect ( Source, szBytes, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	// Copy memory
	memcpy ( Destination, Source, szBytes );

	// Restore protection
	VirtualProtect ( Source, szBytes, dwOldProtect, NULL );
}
//

// Redirect 'No Spread'.
__declspec ( naked ) void stealSpread ()
{
	__asm
	{
		pushad
		mov dwEcx, ecx
		mov dwEax, eax
	}

	if ( WIS )
		dwSpreadAddr = dwEcx;
	else
		dwSpreadAddr = dwEax;

	__asm
	{
		popad
		jmp dword ptr [ dwLocateJmp ]
	}
}
//

// Anti Hwid.
// Target: GetCurrentHwProfile.
DWORD dwAntiHwid = NULL;
HW_PROFILE_INFO hwInfo;

// Anti-Hwid Ban.
BOOL WINAPI getHwProfileA ( LPHW_PROFILE_INFO lpHwProfileInfo )
{	
	Beep ( 100, 1000 );

	memcpy ( lpHwProfileInfo, &hwInfo, sizeof ( HW_PROFILE_INFO ) );

	return TRUE;
}
//

// Read memory location.
template <class T> T rV ( DWORD dwAddress )
{
	__try
	{
		return *(T*) dwAddress;
	}
	__except ( true )
	{
		return -1;
	}
}
//

// Write memory location.
template <class T> void wV ( DWORD dwAddress, T value )
{
	__try
	{
		*(T*) dwAddress = value;
	}
	__except ( true )
	{
		// :(
	}
}
//

// Direct3D.
DWORD dwVirtualTable = NULL;

// Player Pointer: (cc cc cc cc cc cc cc cc cc cc 55 8b ec 8b 45 08 a3) + 0x11
// Target: mov dword ptr [target], eax
DWORD dwPlayerPtr = NULL;

// Player Movement. (or D9 00 D9 99 C4 00 00 00 D9 40 04 D9 99 C8 00 00 00 D9 40 08 B8 01 00 00 00 D9 99 CC 00 00 00) or (D9 00 D9 99 C4 00 00 00 D9 40 04 D9 99 C8 00 00 00 D9 40 08 D9 99 CC 00 00 00)
DWORD dwPlayerSetup    = NULL;
DWORD dwPlayerMovement = NULL;
DWORD dwMovementFunc   = NULL;
DWORD dwMovementJmp    = NULL;

// No Clip.
DWORD dwNoClip = NULL;

// No Fall Damage: D9 45 C0 DC 1D ? ? ? ? DF E0 F6 C4 41 75 41 8B 95 ? ? ? ? or (D9 45 DC D9 C0 DE EA D9 C9 D9 5D EC D9 9E) or (D9 46 ?? ?? ?? ?? ?? ?? ?? D9 45 DC D9 C0 DE EA D9 C9)
// Target: fld dword ptr [ ... ]
DWORD dwNoFallDamage = NULL;
SIZE_T szJump = NULL;

// Return To Base:
/*
	WIS

	346AB004, cshF98.tmp + 6AB004
	340B8094: 

	75 2d b8 01 00 00 00 89 81, scan back until second 39 51

	ecx + offset

	AeriaWT:

	345E5CD4, csh5817.tmp + 5E5CD4
	3409F2D0:

	85 c0 74 1b 83 79, scan back until 83 b9

	ecx + offset

	WAS

	346D33D0, cshEE6C.tmp + 6D33D0 
	340B7014:

	3b c2 74 27 39 scan back until 39 91 or 39 51

	ecx + offset

	WTS

	345CDAEC, cshF8AB.tmp + 5CDAEC
	3409BF10: 

	85 c0 74 21 83 b9, scan back until 83 b9

	ecx + offset
*/
// Target: cmp ecx + offset, edx
DWORD dwRTBPtr = NULL;
DWORD dwRTBOff = NULL;
SIZE_T szRead = 4;

// Infinite Ammo.
DWORD dwInfiniteAmmo = NULL;
BOOL  RPD = FALSE;

// PushToConsole:
/*
	1. Find out which address has ScreenWidth %d
	2. Look for 68 Address
	3. Scan up for E8
	4. Find the address by reversing the offset
*/

DWORD dwPushToConsole = NULL;

// PushToConsole function.
PUSHCONSOLE PushToConsole;

// Name ESP's: 75 x 8B x E8 x x x FF 84 C0 75 x 80 x x + 0xB or 75 ? 0F BE 8D ? ? ? ? 83 F9 ? 75 ? E9 ? ? ? ?
// Look for the function which calls itow, it should be the last from the list. 
// The func that calls is above has name esp target addr.
DWORD dwNameTags = NULL;
DWORD dwNameJump = NULL;

// Rapid Fire: 75 10 6A 00 x x x x x x x x x 5E x ??
// Search for Berserker Suicidal Explosion, look up for CALL EDX, PUSH ECX and JMP. If you NOP the correct CALL,
// you won't be shooting at all.
DWORD dwRapidFire = NULL;
DWORD dwRapidJump = NULL;

// Inventory Hack
DWORD dwSetInv = NULL;
DWORD dwSetCon = NULL;
DWORD dwSetWpn = NULL;
DWORD dwSetWpn2 = NULL;

/*
	0F 84 ? ? ? ? 8D 8D ? ? ? ? E8 ? ? ? ? 6A 08
	B8 ? ? ? ? 50 8D 8D ? ? ? ? E8 ? ? ? ? B8 ? ? ? ? 50 8D 8D ? ? ? ? E8 ? ? ? ? B8 ? ? ? ? 50 8D 8D ? ? ? ? E8 ? ? ? ? B8
*/

// No Spread (D9 ?? D9 42 ?? DE D9 DF E0 F6 C4 01 75 ?? D9 42 ?? D9 19)
// Target: ecx.
DWORD dwLocateSpread;
DWORD dwNoSpread;
DWORD dwSpreadJmp;

// Anti-C113.
// Target: mov dword ptr [edx+ecx*4+00C82838], eax
DWORD dwAntiMS;

// Update vTable.
void UpdateVTable ()
{
	// Scan for d3d9 vTable.
	dwVirtualTable = dwFindPattern ( d3d9.dwBaseAddr, d3d9.dwSize, 
									 (BYTE*) "\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", 
									 "xx????xx????xx" );

	CopyMem ( (void*) &vTable, (void*) ( rV <DWORD> ( dwVirtualTable + 0x02 ) ), _countof ( vTable ) * 4 );
}
//