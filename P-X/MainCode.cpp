#include "Direct3D.h"

/*
	Things you need to change (in order):
		-> GUI (Graphical User Interface)
			-> Download ResEdit from http://www.resedit.net/
				-> Launch it, File -> Open Project (Browse to the P-X sourcecode) -> Dlg.rc
					-> Change it so that there is no text, only images.
		-> RunScanner ()
			-> This function scans for addresses. The addresses are done my style, if you want to change it to your
			   style its simple. Instead of:
							        example: dwNoFallDamage = dwFindPattern ( ... );
									to: dwNoFallDamage = CShell.dwBaseAddr + 0x50000;
		-> VectoredHandler ()
			-> If you changed an address in the autoupdate, verify to see if it works the way you want it to.
			   For example, if you want dwNoFallDamage to do a "NOP" for 6 bytes, the vectored exception handler
			   should look like:
				
				if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwNoFallDamage )
				{
					ExceptionInfo->ContextRecord->Eip = (DWORD) dwNoFallDamage + 0x06;

					return EXCEPTION_CONTINUE_EXECUTION;
				}

	-> There are strings in the source code that are decrypted.
				example: decryptText ( "Qmfbtf!epxompbe!uif!mbuftu!wfstjpo" ).
	   Every character within the string has been incremented by one.

	-> Always make sure that you have the correct addresses.
*/

// Thread Variables.
HANDLE hWnd		  = NULL;
HANDLE Menu       = NULL;
HANDLE hMisc	  = NULL;
HANDLE hFalling   = NULL;
HANDLE hHotkeys   = NULL;
HANDLE hIngame    = NULL;
HANDLE hMain      = NULL;
HANDLE hRain      = NULL;
HANDLE hFly       = NULL;
HANDLE hTelekill  = NULL;
HANDLE hWeapon    = NULL;

// 'VectoredExceptionHandler'.
PVOID  pVEH    = NULL;

// Thread Related.
void CloseThread ( HANDLE hThread )
{
	DWORD dwExitCode;
	GetExitCodeThread ( hThread, &dwExitCode );
	TerminateThread ( hThread, dwExitCode );
}
void SetBreakpoint ( DWORD dwThreadId )
{
	// Open the thread
	HANDLE hThread = OpenThread ( THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, false, dwThreadId );

	if ( !stopBrk )
	{
		// Adjust the thread
		CONTEXT threadContext;
		threadContext.ContextFlags = CONTEXT_DEBUG_REGISTERS;

		// Suspend the thread
		SuspendThread ( hThread );

		// Get the thread context
		GetThreadContext ( hThread, &threadContext );

		// Adjust the thread context
		if ( dbgMode )
			for ( int i = 0; i < 3; i ++ )
				*(DWORD*) ( (DWORD) &threadContext.Dr0 + ( i * 4 ) ) = dwTarget [i];
		else
			for ( int i = 0; i < 4; i ++ )
				*(DWORD*) ( (DWORD) &threadContext.Dr0 + ( i * 4 ) ) = dwTarget [i];

		threadContext.Dr7 = ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 4 ) | ( 1 << 6 );

		// Set the thread context	
		SetThreadContext ( hThread, &threadContext );

		// Resume the thread
		ResumeThread ( hThread );
	}

	// Close the handle
	CloseHandle ( hThread );
}
void QueryThreads ()
{
	// Create a snapshot
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPTHREAD, 0 );

	// Setup variables
	THREADENTRY32 ThreadEntry;
	ThreadEntry.dwSize = sizeof ( THREADENTRY32 );

	// Start
	if ( Thread32First ( hSnapshot, &ThreadEntry ) )
	{
		// Set the breakpoint on current thread
		if ( ThreadEntry.th32OwnerProcessID == GetCurrentProcessId () && ThreadEntry.th32ThreadID != GetCurrentThreadId () )
			SetBreakpoint ( ThreadEntry.th32ThreadID );

		// Set the breakpoint on the next threads
		while ( Thread32Next ( hSnapshot, &ThreadEntry ) )
			if ( ThreadEntry.th32OwnerProcessID == GetCurrentProcessId () && ThreadEntry.th32ThreadID != GetCurrentThreadId () )
				SetBreakpoint ( ThreadEntry.th32ThreadID );
	}

	// Close Snapshot
	CloseHandle ( hSnapshot );
}
//

// Handle Breakpoints.
LONG CALLBACK VectoredHandler ( PEXCEPTION_POINTERS ExceptionInfo )
{
	if ( !dwDebug )
	{
		if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwAntiMS )
		{
			ExceptionInfo->ContextRecord->Eax = 0;
			ExceptionInfo->ContextRecord->Eip = dwAntiMS + 0x07;  

			return EXCEPTION_CONTINUE_EXECUTION;
		} 
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwAntiHwid )
	{
		ExceptionInfo->ContextRecord->Eip = (DWORD) &getHwProfileA;  

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwRTBPtr )
	{
		ExceptionInfo->ContextRecord->Eip = (DWORD) &RedirectRTB;

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	
	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwDrawIndexedPrim )
	{
		++IngameCount;
		ExceptionInfo->ContextRecord->Eip = (DWORD) &DrawIndexedPrimitive;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwNoClip )
	{
		// Set the hook.
		ExceptionInfo->ContextRecord->Eip = dwMovementJmp;

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	
	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwPlayerMovement )
	{
		// Set the hook.
		ExceptionInfo->ContextRecord->Eip = (DWORD) &RedirectMovement;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwNoFallDamage )
	{
		ExceptionInfo->ContextRecord->Eip = (DWORD) dwNoFallDamage + szJump;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwNameTags )
	{
		ExceptionInfo->ContextRecord->Eip = dwNameJump;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwRapidFire )
	{
		ExceptionInfo->ContextRecord->Eip = dwRapidJump;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwSetInv )
	{
		ExceptionInfo->ContextRecord->Eip = dwSetCon;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwSetWpn )
	{
		ExceptionInfo->ContextRecord->Eax = dwMainWpn;
		ExceptionInfo->ContextRecord->Eip = dwSetWpn + 0x05;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwSetWpn2 )
	{
		ExceptionInfo->ContextRecord->Eax = dwSecdWpn;
		ExceptionInfo->ContextRecord->Eip = dwSetWpn2 + 0x05;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwInfiniteAmmo )
	{
		ExceptionInfo->ContextRecord->Eip = (DWORD) dwInfiniteAmmo + 0x02;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwLocateSpread )
	{
		++IngameCount;

		// Setup 'dwSpreadAddr'.
		ExceptionInfo->ContextRecord->Eip = (DWORD) &stealSpread;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwNoSpread )
	{
		++IngameCount;

		// Edit spread value.
		if ( !WIS )
			wV <float> ( ExceptionInfo->ContextRecord->Ecx, 5 );
		else
			wV <float> ( dwSpreadAddr, 5 );

		// Jump.
		ExceptionInfo->ContextRecord->Eip = dwSpreadJmp;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if ( (DWORD) ExceptionInfo->ExceptionRecord->ExceptionAddress == dwDebug )
	{
		// Setup 'dwFunc'.
		dwDbgEdx = ExceptionInfo->ContextRecord->Eax;

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}


//

// No Fall Damage/No Clip.
void Falling ()
{
	while ( true )
	{
		// Setup 'falling'.
		if ( ( Ingame ) &&
		     ( playerY > 0xC8 ) )
		{
			if ( ( NoClip ) ||
				 ( NoFallDamage ) )
			{
				if ( !prevY )
					prevY = rV <float> ( playerY );

				while ( ( prevY - rV <float> ( playerY ) ) > 0 )
				{
					falling = TRUE;
					prevY = rV <float> ( playerY );

					Sleep ( 500 );
				}
				
				falling = FALSE;
				prevY = rV <float> ( playerY );
			}
		}

		// Rest.
		Sleep ( 1 );
	}
}
//

// Misc.
void Misc ()
{
	while ( true )
	{
		if ( dwRTB != 0 )
			Spawned = rV <DWORD> ( dwRTB ) == 1 ? TRUE : FALSE;

		// Enable anti-hwid.
		SetCheck ( IDC_CHECKBOX26, BST_CHECKED );

		// Check resources.			
		if ( gameDev != NULL )
		{
			if ( ResetResources )
			{
				// Refresh Direct3D.
				RefreshDirect3D ( gameDev );

				// Reset resources.
				ResetResources = FALSE;
			}
		}

		// Set options.
		InfiniteAmmo = GetCheck ( IDC_CHECKBOX1 );
		Chams      = ( GetCheck ( IDC_CHECKBOX18 ) ) ? true : false;
		WallHack   = ( GetCheck ( IDC_CHECKBOX19 ) ) ? true : false;
		XRay       = ( GetCheck ( IDC_CHECKBOX22 ) ) ? true : false;
		NoFog      = ( GetCheck ( IDC_CHECKBOX21 ) ) ? true : false;
		WireFrame  = ( GetCheck ( IDC_CHECKBOX23 ) ) ? true : false;
		FullBright = ( GetCheck ( IDC_CHECKBOX24 ) ) ? true : false;
		bTelekill  = GetCheck ( IDC_CHECKBOX17 );
		NoGravity  = GetCheck ( IDC_CHECKBOX2 );
		NoReload   = GetCheck ( IDC_CHECKBOX29 );
		SuperJump  = GetCheck ( IDC_CHECKBOX3 );
		sj1        = GetCheck ( IDC_CHECKBOX4 );
		sj2        = GetCheck ( IDC_CHECKBOX5 );
		sj3        = GetCheck ( IDC_CHECKBOX6 );
		SpeedHack  = GetCheck ( IDC_CHECKBOX7 );
		sh1        = GetCheck ( IDC_CHECKBOX8 );
		sh2        = GetCheck ( IDC_CHECKBOX9 );
		sh3        = GetCheck ( IDC_CHECKBOX10 );
		NoClip     = GetCheck ( IDC_CHECKBOX35 );
		NoFallDamage = GetCheck ( IDC_CHECKBOX11 );

		// Check Speed/Jump.
		if ( !GetCheck ( IDC_CHECKBOX3 ) )
		{
			// Disable
			SetCheck ( IDC_CHECKBOX4, BST_UNCHECKED );
			SetCheck ( IDC_CHECKBOX5, BST_UNCHECKED );
			SetCheck ( IDC_CHECKBOX6, BST_UNCHECKED );
		}
		if ( !GetCheck ( IDC_CHECKBOX7 ) )
		{
			// Disable
			SetCheck ( IDC_CHECKBOX8, BST_UNCHECKED );
			SetCheck ( IDC_CHECKBOX9, BST_UNCHECKED );
			SetCheck ( IDC_CHECKBOX10, BST_UNCHECKED );
		}

		// Rest
		Sleep ( 100 );
	}
}
//

// Hotkeys.
void Hotkeys ()
{
	while ( true )
	{
		// Set focus on 'MainDlg'
		if ( ( GetKeyStateX ( VK_MENU  ) & 0xFF80 ) &&
			 ( GetKeyStateX ( VK_SHIFT ) & 0xFF80 ) )
			 setFocus = false;

		// Return to base.
		if ( GetKeyStateX ( VK_F3 ) & 0xFF80 )
		{
			SendMessage ( GetDlgItem ( MainDlg, IDC_CHECKBOX25 ), BM_SETCHECK, BST_CHECKED, 0 );
			Sleep ( 500 );
		}

		// Respawn Where Died.
		if ( GetKeyStateX ( VK_F6 ) & 0xFF80 )
		{
			if ( GetCheck ( IDC_CHECKBOX14 ) )
				SetCheck ( IDC_CHECKBOX14, BST_UNCHECKED );
			else
				SetCheck ( IDC_CHECKBOX14, BST_CHECKED );

			Sleep ( 500 );
		}

		// Close WolfTeam.
		if ( ( GetKeyStateX ( VK_SHIFT ) & 0xFF80 ) &&
			 ( GetKeyStateX ( VK_F4 ) & 0xFF80 ) ) 
		{
			DWORD dwExitCode;
			GetExitCodeProcess ( GetCurrentProcess (), &dwExitCode );
			TerminateProcess ( GetCurrentProcess (), dwExitCode );
		}

		Sleep ( 1 );
	}
}
//

// Telekill/Fly/Infinite Ammo/Rapid Fire/No Spread.
void Telekill ()
{
	while ( true )
	{
		// Check prerequisites.
		if ( ( Ingame ) &&
			 ( playerY > 0xC8 ) &&
			 ( dwRTB != 0 ) )
		{
			// Check to see if Telekill is enabled.
			if ( GetCheck ( IDC_CHECKBOX17 ) )
			{
				// Ensure that breakpoint is set
				dwTarget [2] = dwPlayerMovement;

				// Decrease 'numObjects'
				if ( GetKeyStateX ( VK_F2 ) & 0xFF80 )
					( currentObject - 1 ) < 0 ? destroySig () : destroySig2 ();

				// Increase 'numObjects'
				if ( GetKeyStateX ( VK_F1 ) & 0xFF80 )
					( currentObject + 1 ) < numObjects ? destroySig3 () : destroySig4 ();

				// Blacklist
				if ( GetKeyStateX ( VK_F4 ) & 0xFF80 )
				{
					Blacklist [blacklistNum] = dwObjectY [currentObject];
					++blacklistNum;
				}

				if ( rV <DWORD> ( dwRTB ) != 2 )
				{
					if ( CheckList ( dwObjectY [currentObject] ) )
					{
						// Check if the object is in the correct range.
						if ( ( ( rV <float> ( dwObjectY [currentObject] ) > 20 ) ||
							   ( rV <float> ( dwObjectY [currentObject] ) < 300 ) ||
							   ( rV <float> ( dwObjectY [currentObject] - 0x04 ) < 2000 ) ||
							   ( rV <float> ( dwObjectY [currentObject] - 0x04 ) > -2000 ) ) &&
							 ( dwObjectY [currentObject] != playerY ) &&
							 ( ( rV <float> ( dwObjectY [currentObject] - 0x04 ) != 0 ) &&
							   ( rV <float> ( dwObjectY [currentObject] ) != 0 ) &&
							   ( rV <float> ( dwObjectY [currentObject] ) != 0 ) ) )
						{
							// Change position
							wV <float> ( playerX, rV <float> ( dwObjectY [currentObject] - 0x04 ) );
							wV <float> ( playerY, rV <float> ( dwObjectY [currentObject] ) + 50 );
							wV <float> ( playerZ, rV <float> ( dwObjectY [currentObject] + 0x04 ) - 10 );
						}
					}
					else
					{
						// Next object
						if ( (currentObject + 1) < numObjects )
							++currentObject;
						else
							currentObject = 0;
					}
				}
			}
		}

		// Rest
		Sleep ( 100 );
	}
}
void Fly ()
{
	BOOL bGravityOn = FALSE;

	while ( true )
	{
		// Check prerequisites.
		if ( ( Ingame ) &&
			 ( playerY > 0xC8 ) &&
			 ( dwRTB != 0 ) &&
			 ( rV <DWORD> ( dwRTB ) != 2 ) )
		{
			// Check to see if Fly Hack is enabled.
			if ( GetCheck ( IDC_CHECKBOX13 ) )
			{
				// Obtain 'bGravityOn'
				bGravityOn = GetCheck ( IDC_CHECKBOX2 );

				// Ensure that breakpoint is set
				dwTarget [2] = dwPlayerMovement;

				// Handle space key
				while ( GetKeyStateX ( VK_SPACE ) & 0xFF80 )
				{
					// Enable 'No Gravity'
					SetCheck ( IDC_CHECKBOX2, BST_CHECKED );

					// Increase height
					wV <float> ( playerY, rV <float> ( playerY ) + 50 );

					// Rest
					Sleep ( 100 );
				}

				// Disable 'No Gravity'
				if ( !bGravityOn )
					SetCheck ( IDC_CHECKBOX2, BST_UNCHECKED );
			}
		}

		// Rest
		Sleep ( 100 );
	}
}
void WeaponAdjustment ()
{
	while ( true )
	{
		// Check prerequisites.
		if ( ( Ingame ) &&
			 ( playerY > 0xC8 ) &&
			 ( dwRTB != 0 ) &&
			 ( rV <DWORD> ( dwRTB ) != 2 ) )
		{			
			// Check to see if 'Infinite Ammo', 'Rapid Fire', 'No Reload' or 'No Spread' is ticked.
			if ( GetCheck ( IDC_CHECKBOX1 ) ||
				 GetCheck ( IDC_CHECKBOX27 ) ||
				 GetCheck ( IDC_CHECKBOX29 ) ||
				 GetCheck ( IDC_CHECKBOX33 ) )
			{
				// Make a copy of breakpoints.
				DWORD dwOrigTarg  = dwTarget [0];
				DWORD dwOrigTarg2 = dwTarget [1];
				DWORD dwOrigTarg3 = dwTarget [3];

				// Check to see if the player is shooting.
				if ( GetKeyStateX ( VK_LBUTTON ) & 0xFF80 )
				{
					// Enable 'shooting'
					shooting = true;

					// Enable 'No Spread'
					if ( GetCheck ( IDC_CHECKBOX33 ) )
					{
						if ( !dwSpreadAddr )
							dwTarget [0] = dwLocateSpread;
						else
							dwTarget [0] = dwNoSpread;
					}

					// Enable 'Rapid Fire'
					if ( GetCheck ( IDC_CHECKBOX27 ) )
						dwTarget [1] = dwRapidFire;

					// Enable 'Infinite Ammo' and/or 'No Reload'
					if ( GetCheck ( IDC_CHECKBOX1 ) ||
						 GetCheck ( IDC_CHECKBOX29 ) )
						 dwTarget [3] = dwInfiniteAmmo;						

					// Rest
					Sleep ( 100 );
				}

				while ( GetKeyStateX ( VK_LBUTTON ) & 0xFF80 )
				{
					// Enable 'shooting'
					shooting = true;

					// Enable 'No Spread'
					if ( GetCheck ( IDC_CHECKBOX33 ) )
					{
						if ( !dwSpreadAddr )
							dwTarget [0] = dwLocateSpread;
						else
							dwTarget [0] = dwNoSpread;
					}

					// Enable 'Rapid Fire'
					if ( GetCheck ( IDC_CHECKBOX27 ) )
						dwTarget [1] = dwRapidFire;

					// Enable 'Infinite Ammo' and/or 'No Reload'
					if ( GetCheck ( IDC_CHECKBOX1 ) ||
						 GetCheck ( IDC_CHECKBOX29 ) )
						 dwTarget [3] = dwInfiniteAmmo;	

					Sleep ( 250 );
				}

				// Disable 'shooting'
				shooting = false;
				
				// Restore
				dwTarget [0] = dwOrigTarg;
				dwTarget [1] = dwOrigTarg2;
				dwTarget [3] = dwOrigTarg3;
			}
		}

		// Rest
		Sleep ( 1 );
	}
}
//

// Ingame Func.
void CheckGame ()
{
	while ( true )
	{
		if ( !shooting ) 
		{
			if ( GetForegroundWindow () == hwndWolfTeam )
			{
				int currentNum = IngameCount;

				// Rest
				Sleep ( 250 );

				if ( currentNum == IngameCount )
					Ingame = false;
				else
					Ingame = true;
			}
		}
		else
			// Rest
			Sleep ( 250 );
	}
}
//

// Scan for signatures and locate memory targets.
void RunScanner ()
{
	// Obtain the current directory.
	ifstream File ( decryptText ( "D;]Tfuujoht/joj" ).c_str () );
	int Counter = 0;

	while ( !File.eof () )
	{
		string currentLine;
		getline ( File, currentLine );

		if ( Counter == 7 )
			currentDir = currentLine;

		++Counter;
	}

	File.close ();

	// Check LPMS pathway.
	if ( PathFileExists ( decryptText ( "D;]Nfnpsz!Fohjof/emm" ).c_str () ) )
	{
		// Load LPMS.
		HMODULE hLPMS = NULL;

		while ( !hLPMS )
			hLPMS = LoadLibraryA ( decryptText ( "D;]Nfnpsz!Fohjof/emm" ).c_str () );
	}

	// Setup 'MODULE_INFO' variables.
	RtlZeroMemory ( &WolfTeam, sizeof ( MODULE_INFO ) );
	RtlZeroMemory ( &ClientFX, sizeof ( MODULE_INFO ) );
	RtlZeroMemory ( &CShell,   sizeof ( MODULE_INFO ) );
	RtlZeroMemory ( &d3d9,     sizeof ( MODULE_INFO ) );

	// Locate modules.
	while ( !WolfTeam.dwBaseAddr )
		WolfTeam = LocateModule ( decryptText ( "XpmgUfbn/cjo" ) );

	while ( !ClientFX.dwBaseAddr )
		ClientFX = LocateModule ( "cfx" );

	while ( !CShell.dwBaseAddr )
		CShell = LocateModule ( "csh" );

	while ( !d3d9.dwBaseAddr )
		d3d9 = LocateModule ( "d3d9.dll" );

	// Setup Anti-Hwid.
	dwAntiHwid = (DWORD) GetCurrentHwProfileA;

	// Scan for Player Pointers.
	ScanMemory ( ClientFX.dwBaseAddr, ClientFX.dwBaseAddr + ClientFX.dwSize, (void*) "\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x55\x8b\xec\x8b\x45\x08\xa3", sizeof ( "\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\x55\x8b\xec\x8b\x45\x08\xa3" ) - 1 );
	dwPlayerPtr = dwResults [0] + 0x11;

	// Scan for Player Setup (AeriaWT).
	dwPlayerSetup = dwFindPattern ( WolfTeam.dwBaseAddr, WolfTeam.dwSize, 
										(BYTE*) "\x52\xD9\x00\xF0\xFD\xFF\xFF\x8D\x45\x0C",
										"xx?xxxxxxx" );

	// Scan for Player Setup (WIS).
	if ( !dwPlayerSetup ) 
		dwPlayerSetup = dwFindPattern ( WolfTeam.dwBaseAddr, WolfTeam.dwSize, 
								    (BYTE*) "\x50\x8D\x4D\x00\x51\x8D\x8D\x00\x00\x00\x00\xE8",
									"xxx?xxx????x" );

	// Scan for Player Movement.
	ScanMemory ( WolfTeam.dwBaseAddr, WolfTeam.dwBaseAddr + WolfTeam.dwSize, 
				(void*) "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xB8\x01\x00\x00\x00\xD9\x99\xCC\x00\x00\x00", 
				sizeof ( "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xB8\x01\x00\x00\x00\xD9\x99\xCC\x00\x00\x00" ) - 1 );

	if ( !results )
	{
		diffVersion = TRUE;

		// Scan for Player Movement.
		ScanMemory ( WolfTeam.dwBaseAddr, WolfTeam.dwBaseAddr + WolfTeam.dwSize, (void*) "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xD9\x99\xCC\x00\x00\x00", sizeof ( "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xD9\x99\xCC\x00\x00\x00" ) - 1 );
		dwPlayerMovement = dwResults [0]; 
	}
	else
	{
		WIS = TRUE;
		dwPlayerMovement = dwResults [0]; 
	}

	// Find 'No Clip'.
	DWORD i;
	DWORD movingFunc;

	if ( dwPlayerSetup != 0 )
	{
		for ( i = 0; 
			  i < 0x100;
			  i ++ )
		{
			if ( rV<BYTE> ( dwPlayerSetup + i ) == 0xE8 )
			{
				movingFunc = rV<DWORD> ( dwPlayerSetup + i + 0x01 ) + ( dwPlayerSetup + i + 0x05 );

				if ( ( movingFunc >= WolfTeam.dwBaseAddr ) &&
					 ( movingFunc <= ( WolfTeam.dwBaseAddr + WolfTeam.dwSize ) ) )
					 break;
			}
		}

		for ( i = 0; 
			  i < 0x100;
			  i ++ )
			  if ( rV<BYTE> ( movingFunc + i ) == 0xE8 )
				  break;
	}

	// Setup 'dwNoClip'.
	dwNoClip = movingFunc + i;
	dwMovementJmp = dwNoClip + 0x05;

	// Scan for No Fall Damage (WIS).
	dwNoFallDamage = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
										 (BYTE*) "\xD9\x86\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\xD9\x45\xDC\xD9\xC0\xDE\xEA\xD9\xC9",
										 "xx????x?????xxxxxxxxx" );
	if ( !dwNoFallDamage )
	{
		// Scan for No Fall Damage (WIS).
		if ( !dwNoFallDamage )
			dwNoFallDamage = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
											(BYTE*) "\xD9\x86\x00\x00\x00\x00\xD9\x45\xDC\xD9\xC0\xDE\xEA\xD9\xC9",
											"xx????xxxxxxxxx" );

		// Scan for No Fall Damage (AeriaWT.)
		if ( !dwNoFallDamage )
			dwNoFallDamage = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
											(BYTE*) "\xD9\x00\x00\xD9\x00\x00\x8B\x8D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x95\x00\x00\x00\x00\x83",
											"x??x??xx????x????xx????x" );

		// Scan for No Fall Damage.
		if ( !dwNoFallDamage )
			dwNoFallDamage = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
											 (BYTE*) "\xD9\x46\x00\x00\x00\x00\x00\x00\x00\xD9\x45\xDC\xD9\xC0\xDE\xEA\xD9\xC9",
											 "xx???????xxxxxxxxx" );

		// Scan for No Fall Damage.
		if ( !dwNoFallDamage )
			dwNoFallDamage = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
											(BYTE*) "\xD9\x46\x00\xD9\x45\xDC\xD9\xC0\xDE\xEA\xD9\xC9",
											"xx?xxxxxxxxx" );
	}

	if ( dwNoFallDamage != NULL )
	{
		if ( !memcmp ( (void*) ( dwNoFallDamage + 0x03 ), (void*) "\xD9", 1 ) )
			szJump = 0x03;
		if ( !memcmp ( (void*) ( dwNoFallDamage + 0x03 ), (void*) "\x01", 1 ) )
			szJump = 0x03;
		if ( !memcmp ( (void*) ( dwNoFallDamage + 0x06 ), (void*) "\xD9", 1 ) )
			szJump = 0x06;
		if ( !memcmp ( (void*) ( dwNoFallDamage + 0x06 ), (void*) "\x01", 1 ) )
			szJump = 0x06;
	}

	// Scan for Return To Base (WIS).
	DWORD dwBuffer = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
									 (BYTE*) "\xA1\x00\x00\x00\x00\x80\xB8\x00\x00\x00\x00\x00\x75\x00\x8B\x81\x00\x00\x00\x00", 
									 "x????xx?????x?xx????" );

	if ( dwBuffer != 0 )
	{
		int result = 0;

		// Scan back for the mov eax+off.
		for ( DWORD i = 1; i < 90; i ++ )
		{
			// Compare bytes.
			if ( !memcmp ( (void*) ( dwBuffer - i ), (void*) "\x83\x79", 2 ) )
			{
				// Enable 'WIS'.
				WIS = TRUE;

				// Setup 'szRead'.
				szRead = 2;
				
				// Setup 'dwRTBPtr'.
				dwRTBPtr = dwBuffer - i;

				break;
			}
		}
		
		if ( !dwRTBPtr )
		{
			for ( DWORD i = 1; i < 90; i ++ )
			{
				// Compare bytes.
				if ( !memcmp ( (void*) ( dwBuffer - i ), (void*) "\x83\xB8", 2 ) )
				{
					// Enable 'WIS'.
					WIS = TRUE;

					// Setup 'szRead'.
					szRead = 4;
					
					// Setup 'dwRTBPtr'.
					dwRTBPtr = dwBuffer - i;

					break;
				}
			}
		}
	}	

	if ( !dwRTBPtr )
	{
		// Scan for Return To Base (AeriaWT).
		dwRTBPtr = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								   (BYTE*) "\x75\x13\x8B\x0D\x00\x00\x00\x00\x0F\xB6\x91\x00\x00\x00\x00",
								   "xxxx????xxx????" );

		// AeriaWT.
		if ( dwRTBPtr != 0 )
		{
			for ( DWORD i = 1; i < 90; i ++ )
			{
				// Compare bytes.
				if ( !memcmp ( (void*) ( dwRTBPtr - i ), (void*) "\x83\xb8", 2 ) )
				{
					dwRTBPtr = dwRTBPtr - i;
					break;
				}
			}
		}
		else
		{
			// Scan for Return To Base (WTS).
			ScanMemory ( CShell.dwBaseAddr, CShell.dwBaseAddr + CShell.dwSize, (void*) "\x85\xc0\x74\x21\x83\xb9", sizeof ( "\x85\xc0\x74\x21\x83\xb9" ) - 1 );
		
			// WTS.
			if ( results != 0 )
			{
				for ( DWORD i = 1; i < 90; i ++ )
				{
					// Compare bytes
					if ( !memcmp ( (void*) ( dwResults [0] - i ), (void*) "\x83\xb9", 2 ) )
					{
						dwRTBPtr = dwResults [0] - i;
						break;
					}
				}
			}
			else
			{
				// Error.
				MessageBoxA ( 0, decryptText ( "Uif!bvupvqebuf!ibt!gbjmfe-!zpv!njhiu!fyqfsjfodf!qspcmfnt" ).c_str (), decryptText ( "Fssps!1y211" ).c_str (), MB_ICONERROR );
			}
		}
	}

	// Scan for Name Tags.
	dwNameTags = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								 (BYTE*) "\x75\x00\x8B\x00\xE8\x00\x00\x00\xFF\x84\xC0\x75\x00\x80\x00\x00", 
								 "x?x?x???xxxx?x??" ) + 0xB;

	if ( dwNameTags < 0x100 )
		dwNameTags = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								 (BYTE*) "\x75\x00\x0F\xBE\x00\x00\x00\x00\x00\x83\x00\x00\x75\x00\xE9\x00\x00\x00\x00", 
								 "x?xx?????x??x?x????" );

	dwNameJump = dwNameTags + (DWORD) rV <BYTE> ( dwNameTags + 0x01 ) + 0x02;

	// Scan for Rapid Fire (WIS).
	dwRapidFire = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								  (BYTE*) "\xEB\x00\x8B\x00\x00\x00\x00\x00\xFF\xD2\xFF\x05", 
								  "x?x?????xxxx" );

	// Scan for Rapid Fire (AeriaWT).
	if ( !dwRapidFire )
		dwRapidFire = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize,
										  (BYTE*) "\xEB\x00\x8B\x4D\xF4\x8B\x00\x8B\x4D\xF4\x8B\x82\x00\x00\x00\x00\xFF\xD0\x8B\x4D\xF4\x0F\xB6\x00\x00\x85\xD2\x74\x00",
										  "x?xxxx?xxxxx????xxxxxxx??xxx?" );

	if ( !dwRapidFire )
	{
		// Scan for Rapid Fire (WAS).
		if ( !dwRapidFire )
			dwRapidFire = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
										  (BYTE*) "\xEB\x00\x8B\x00\x00\x00\x00\x00\xFF\xD2\x38\x5E", 
										  "x?x?????xxxx" );

		if ( !dwRapidFire )
		{
			dwRapidFire = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize,
										  (BYTE*) "\xEB\x00\x8B\x90\x00\x00\x00\x00\xFF\xD2\x38\x00\x00\x00\x00\x00\x74\x00\x8B\x0D\x00\x00\x00\x00\x0F\x00\x00\x00\x8B\x01\x8B\x00\x00\x00\x00\x00\x52\xFF\xD0",
										  "x?xx????xxx?????x?xx????x???xxx?????xxx" );
		}
		else
			WTS = true;
	}

	if ( dwRapidFire != 0 )
	{
		for ( DWORD i = 0; i < 90; i ++ )
		{
			if ( !memcmp ( (void*) ( dwRapidFire + i ), (void*) "\x74", 1 ) )
			{
				dwRapidFire += i;
				break;
			}
		}
	}

	// Scan for Infinite Ammo.
	if ( dwRapidFire != 0 )
	{
		int Counter = 0;

		// Setup 'dwInfiniteAmmo'.
		for ( DWORD i = 0; i < 90; i ++ )
		{
			if ( ( !memcmp ( (void*) ( dwRapidFire + i ), (void*) "\xE8", 1 ) ) )
			{			
				dwInfiniteAmmo = dwRapidFire + i;
				RPD = FALSE;
				break;
			}

			if ( ( !memcmp ( (void*) ( dwRapidFire + i ), (void*) "\xFF\xD0", 2 ) ) )
			{
				if ( Counter > 0 )
				{
					dwInfiniteAmmo = dwRapidFire + i;
					RPD = TRUE;
					break;
				}
				else
					++Counter;
			}
		}

		if ( !RPD )
		{
			DWORD dwFunc = rV <DWORD> ( dwInfiniteAmmo + 0x01 ) + ( dwInfiniteAmmo + 0x05 );

			// Scan for the return.
			for ( DWORD i = 0; i < 0xFF; i ++ )
			{
				if ( !memcmp ( (void*) ( dwFunc + i ), (void*) "\xC2\x04\x00", 3 ) )
				{
					dwInfiniteAmmo = dwFunc + i;
					break;
				}
			}

			// Scan upwards for the call eax, or call edx.
			for ( DWORD i = 0; i < 0xFF; i ++ )
			{
				if ( ( !memcmp ( (void*) ( dwInfiniteAmmo - i ), (void*) "\xFF\xD0", 2 ) ) ||
					 ( !memcmp ( (void*) ( dwInfiniteAmmo - i ), (void*) "\xFF\xD2", 2 ) ) )
				{
					dwInfiniteAmmo -= i;
					break;
				}
			}
		}
	}

	// Scan for Inventory Hack.
	ScanMemory ( CShell.dwBaseAddr, CShell.dwBaseAddr + CShell.dwSize, (void*) "\xB8\x38\x0C\x00\x00", sizeof ( "\xB8\x38\x0C\x00\x00" ) - 1 );
	dwSetWpn = dwResults [0];

	// Find the address.
	for ( int i = 0; i < results; i ++ )
	{
		for ( DWORD off = 0; off < 0x60; off ++ )
		{
			if ( !memcmp ( (void*) ( dwResults [i] - off ), (void*) "\xff\xd0\x84\xc0\x0f\x84", sizeof ( "\xff\xd0\x84\xc0\x0f\x84" ) - 1 ) ) 
			{
				dwSetInv = dwResults [i] - off + 0x04;
				break;
			}
		}
	}

	// Find Slot #2.
	for ( DWORD off = 1; off < 0x70; off ++ )
	{
		if ( rV <BYTE> ( dwSetWpn + off ) == (BYTE) 0xB8 )
		{
			dwSetWpn2 = dwSetWpn + off;
			break;
		}
	}

	// Setup Inventory Hack.
	dwSetCon = dwSetInv + 0x06;

	// Scan for No Spread (WIS).
	dwLocateSpread = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
									(BYTE*) "\xD9\x19\x01\x3D\x00\x00\x00\x00\xD9\x01\xD9\x42\x00\xDE\xD9\xDF\xE0\xF6\xC4\x00", 
									"xxxx????xxxx?xxxxxx?" );

	// Scan for No Spread (WIS).
	if ( !dwLocateSpread )
		dwLocateSpread = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
										(BYTE*) "\xD9\x19\xD9\x01\xD9\x42\x00\xDE\xD9\xDF\xE0\xF6\xC4\x00", 
										"xxxxxx?xxxxxx?" );

	// Scan for No Spread (AeriaWT).
	if ( !dwLocateSpread )
		dwLocateSpread = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
										(BYTE*) "\x8B\x4D\x08\xD9\x01\xD9\x18\x8B\x95", 
										"xxxxxxxxx" );

	dwLocateJmp = dwLocateSpread;

	// Scan for No Spread Function (WIS, call eax).
	dwNoSpread = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								(BYTE*) "\xFF\xD0\x84\x00\x0F\x84\x00\x00\x00\x00\x8B\x00\x8B\x82\x00\x00\x00\x00\x8B\x00\xFF\xD0\x8B\xF8", 
								"xxx?xx????x?xx????x?xxxx" );

	// Scan for No Spread Function (AeriaWT).
	if ( !dwNoSpread )
		dwNoSpread = dwFindPattern ( CShell.dwBaseAddr, CShell.dwSize, 
								(BYTE*) "\xD9\x55\xF4\x83\x05\x00\x00\x00\x00\x00\xD9\x55\x00\xD9\x55\x00\x89\x45\x00\xD9\x5D\x00", 
								"xxxxx?????xx?xx?xx?xx?" ) + sizeof ( "\xD9\x55\xF4\x83\x05\x00\x00\x00\x00\x00\xD9\x55\x00\xD9\x55\x00\x89\x45\x00\xD9\x5D\x00" ) - 1;

	if ( dwNoSpread > 0x100 )
	{
		int Counter = 0;

		for ( DWORD i = 0; i < 0xFF; i ++ )
		{
			if ( ( !memcmp ( (void*) ( dwNoSpread + i ), (void*) "\xE8", 1 ) ) ||
				 ( !memcmp ( (void*) ( dwNoSpread + i ), (void*) "\xFF\xD0", 2 ) ) )
			{
				if ( !memcmp ( (void*) ( dwNoSpread + i ), (void*) "\xFF\xD0", 2 ) )
				{
					if ( Counter > 0 )
					{
						dwNoSpread += i;
						break;
					}
					else
						++Counter;
				}
				else
				{
					dwNoSpread += i;
					break;
				}
			}
		}

		if ( !memcmp ( (void*) dwNoSpread, (void*) "\xE8", 1 ) )
			dwSpreadJmp = dwNoSpread + 0x05;
		else
			dwSpreadJmp = dwNoSpread + 0x02;
	}

	// Scan for PushToConsole.
	dwPushToConsole = dwFindPattern ( WolfTeam.dwBaseAddr, 
									  WolfTeam.dwSize, 
									  (BYTE*) "\xE8\x00\x00\x00\x00\x8B\x00\x20\xFF\xFF\xFF", 
									  "x????x?xxxx" );

	if ( dwPushToConsole != 0 )
	{
		// Read the jump.	
		dwPushToConsole = rV <DWORD> ( dwPushToConsole + 1 ) + ( dwPushToConsole + 0x05 );

		// Setup 'PushToConsole'
		PushToConsole = (PUSHCONSOLE) dwPushToConsole;
	}

	// Scan for Anti-C113.
	dwAntiMS = dwFindPattern ( WolfTeam.dwBaseAddr, WolfTeam.dwSize,
							  (BYTE*) "\x8B\x84\x81\x38\x28\xC8\x00",
							  "xxxxxxx");
} 
//

// Rain Callback.
void RunWnd ()
{
	DialogBoxA ( hInst, MAKEINTRESOURCE ( IDD_DIALOG1 ), NULL, MainProc );
}
void Rain ()
{
	while ( true )
	{
		// Set focus on 'MainDlg'.
		if ( !setFocus )
		{
			if ( MainDlg != NULL )
			{
				while ( GetForegroundWindow () != MainDlg )
				{
					SetForegroundWindow ( MainDlg );
					Sleep ( 200 );
				}

				setFocus = true;
			}
		}

		// Refresh Settings
		if ( !Ingame )
		{
			// Reset player addresses.
			playerX = NULL;
			playerZ = NULL;

			// Reset spawn/respawn coordinates.
			respawnX = 0;
			respawnY = 0;
			respawnZ = 0;

			// Reset Telekill variables.
			blacklistNum  = 0;
			currentObject = 0;
			numObjects    = 0;

			// Reset breakpoints.
			if ( dwTarget [0] != dwDrawIndexedPrim )
				dwTarget [0] = NULL;

			if ( dwTarget [1] != dwRTBPtr )
				dwTarget [1] = NULL;

			if ( dwTarget [2] != dwAntiHwid )
				dwTarget [2] = dwAntiHwid;
			
			if ( GetCheck ( IDC_CHECKBOX36 ) )
			{
				if ( dwTarget [3] != dwAntiMS )
					dwTarget [3] = dwAntiMS;
			}
			else
				dwTarget [3] = NULL;

			// Reset other settings.
			RespawnNow = FALSE;
			ReturnBase = FALSE;

			// Reset 'Counter'.
			Counter = 0;
		}

		// Calculate 'playerY'
		playerY = rV <DWORD> ( rV <DWORD> ( dwPlayerPtr ) ) + 0xC8;

		if ( GetKeyState ( VK_F11 ) & 0xFF80 )
		{
			la ( (void*) playerY );
			la ( (void*) dwRTB );
			la ( (void*) dwPushToConsole );
		}

		// Start the hack.
		if ( playerY > 0xC8 )// &&
			 //( dwRTB != 0 ) &&
			// ( !dbgMode ) )
		{
			// Calculate 'playerX' and 'playerZ'
			playerX = playerY - 0x04;
			playerZ = playerY + 0x04;

			// Locate 'dwDrawIndexedPrim'
			if ( !dwDrawIndexedPrim )
			{
				// Update vTable.
				UpdateVTable ();
				
				// Reestablish targets.				
				dwDrawIndexedPrim = vTable [82] + 0x05;
				dwContinue4 = dwDrawIndexedPrim + 0x02;
			}

			// Start.
			if ( dwDrawIndexedPrim != 0 )
			{
				// Enable Direct3D/No Spread.
				if ( ( !GetCheck ( IDC_CHECKBOX33 ) ) ||
					 ( !shooting ) )
					dwTarget [0] = dwDrawIndexedPrim;

				// Check 'Ingame' status.
				if ( Ingame )
				{
					// No Recoil.
					if ( GetCheck ( IDC_CHECKBOX15 ) ) 
					{
						PushToConsole ( decryptText ( "BjnTfuujohFobcmf!2" ).c_str () );
						PushToConsole ( decryptText ( "SbujpDnsQjudiQfsGjsf!1" ).c_str () );
						PushToConsole ( decryptText ( "SbujpDnsZbxQfsGjsf!1" ).c_str () );
						PushToConsole ( decryptText ( "SbujpBjnQjudiEfdbz!::::::::" ).c_str () );
						PushToConsole ( decryptText ( "SbujpBjnZbxEfdbz!::::::::" ).c_str () );
					}
					else
						PushToConsole ( decryptText ( "BjnTfuujohFobcmf!2" ).c_str () );

					// No Weapon Sway.
					if ( GetCheck ( IDC_CHECKBOX16 ) )
						PushToConsole ( decryptText ( "XfbqpoTxbz!1" ).c_str () );
					else
						PushToConsole ( decryptText ( "XfbqpoTxbz!2" ).c_str () );

					// Return to Base.
					if ( GetCheck ( IDC_CHECKBOX25 ) )
					{
						// Set 'ReturnBase' to true
						ReturnBase = TRUE;

						// Change value of rtb.
						if ( rV <DWORD> ( dwRTB ) == 1 )
							wV <DWORD> ( dwRTB, 2 );

						// Set Return To Base off
						SetCheck ( IDC_CHECKBOX25, BST_UNCHECKED );
					}

					// Teleport.
					if ( GetCheck ( IDC_CHECKBOX12 ) )
					{
						// F7, save location
						if ( GetKeyStateX ( VK_F7 ) & 0xFF80 )
						{
							teleportX = rV <float> ( playerX );
							teleportY = rV <float> ( playerY );
							teleportZ = rV <float> ( playerZ );
						}

						// F8, set location
						if ( ( teleportX != 0 ) &&
							 ( teleportY != 0 ) &&
							 ( teleportZ != 0 ) )
						{
							if ( GetKeyStateX ( VK_F8 ) & 0xFF80 )
							{
								wV <float> ( playerX, teleportX );
								wV <float> ( playerY, teleportY );
								wV <float> ( playerZ, teleportZ );
							}
						}
					}

					// Player Movement.
					if ( ( ( GetCheck ( IDC_CHECKBOX3 ) ) ||
						   ( GetCheck ( IDC_CHECKBOX7 ) ) ||
						   ( GetCheck ( IDC_CHECKBOX17 ) ) ||
						   ( GetCheck ( IDC_CHECKBOX13 ) ) ||
						   ( GetCheck ( IDC_CHECKBOX2 ) ) ) &&
						   ( rV <DWORD> ( dwRTB ) == 1 ) )
						   dwTarget [2] = dwPlayerMovement;
					else if ( dwTarget [2] == dwPlayerMovement ) 
						dwTarget [2] = NULL;

					// Name Tags.
					if ( ( !GetCheck ( IDC_CHECKBOX27 ) ) ||
						 ( !shooting ) )
					{
						if ( GetCheck ( IDC_CHECKBOX20 ) )
							dwTarget [1] = dwNameTags;
						else if ( dwTarget [1] == dwNameTags )
							dwTarget [1] = NULL;
					}

					// No Clip / No Fall Damage.
					if ( ( ( !GetCheck ( IDC_CHECKBOX1 ) ) ||
						   ( !shooting ) ) &&
						   ( !falling ) )
					{
						if ( GetCheck ( IDC_CHECKBOX35 ) )
							dwTarget [3] = dwNoClip;
						else if ( dwTarget [3] == dwNoClip )
							dwTarget [3] = NULL;
					} 						
					else if ( ( ( !GetCheck ( IDC_CHECKBOX1 ) ) ||
								( !shooting ) ) &&
								( falling ) )
					{
						if ( GetCheck ( IDC_CHECKBOX11 ) )
							dwTarget [3] = dwNoFallDamage;
						else 
							dwTarget [3] = NULL;
					}
				}

				// Position Hack.
				if ( GetCheck ( IDC_CHECKBOX34 ) )
				{ 
					// Increment 'X'
					if ( GetKeyStateX ( VK_OEM_PLUS ) & 0xFF80 )
						 wV <float> ( playerX, rV <float> ( playerX ) + 5 );

					// Decrement 'X'
					if ( GetKeyStateX ( VK_OEM_MINUS ) & 0xFF80 )
						 wV <float> ( playerX, rV <float> ( playerX ) - 5 );

					// Increment 'Z'
					if ( GetKeyStateX ( 0x4F ) & 0xFF80 )
						 wV <float> ( playerZ, rV <float> ( playerZ ) + 5 );

					// Decrement 'Z'
					if ( GetKeyStateX ( 0x50 ) & 0xFF80 )
						 wV <float> ( playerZ, rV <float> ( playerZ ) - 5 );
				}

				// Respawn Where Died.
				if ( (  GetCheck ( IDC_CHECKBOX14 ) ) &&
					 ( !GetCheck ( IDC_CHECKBOX25 ) ) )
				{
					// Store position.
					if ( rV <DWORD> ( dwRTB ) == 2 )
					{
						respawnX = rV <float> ( playerX );
						respawnY = rV <float> ( playerY );
						respawnZ = rV <float> ( playerZ );
						RespawnNow = TRUE;
					}

					// Spawn back when alive.
					if ( RespawnNow )
					{
						if ( rV <DWORD> ( dwRTB ) == 1 )
						{
							if ( !ReturnBase )
							{
								if ( ( respawnX != 0 ) &&
									 ( respawnY != 0 ) &&
									 ( respawnZ != 0 ) )
								{
									wV <float> ( playerX, respawnX );
									wV <float> ( playerY, respawnY );
									wV <float> ( playerZ, respawnZ );
									RespawnNow = FALSE;
								}
								else
									RespawnNow = FALSE;
							}
							else
							{
								// Reset settings
								RespawnNow = FALSE;
								ReturnBase = FALSE;
							}
						}
					}
				}
				else
				{
					// Reset respawn coordinates
					respawnX = 0;
					respawnY = 0;
					respawnZ = 0;
				}
			}
		}

		// Rest
		Sleep ( 10 );
	}
}
//

// Initialize the dll.
void Redirection ()
{
	dwRedirect  = NULL;
	dwRedirect2 = NULL;
}
void Continuation ()
{
	dwContinue  = NULL;
	dwContinue2 = NULL;

	if ( WIS )
		dwContinue3 = dwPlayerMovement + sizeof ( "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xB8\x01\x00\x00\x00\xD9\x99\xCC\x00\x00\x00" ) - 1;
	else
		dwContinue3 = dwPlayerMovement + sizeof ( "\xD9\x00\xD9\x99\xC4\x00\x00\x00\xD9\x40\x04\xD9\x99\xC8\x00\x00\x00\xD9\x40\x08\xD9\x99\xCC\x00\x00\x00" ) - 1;

	// dwContinue4 is reserved for DrawIndexedPrimitive.
	// It it setup in a seperate function.

	dwContinue5 = dwRTBPtr;
}
void Run ()
{
	// Obtain the current GUID.
	GetCurrentHwProfileA ( &hwInfo );

	// Check for Debugger.
	if ( !IsDebuggerPresent () )
	{
		// Check License.
		if ( true )//CheckOnline ( GetLicense () ) )
		{
			// Set the fake GUID.
			memcpy ( &hwInfo.szHwProfileGuid, 
					 (void*) "{64f92bc0-9226-11e2-8023-806d6172696f}", 
					 strlen ( "{64f92bc0-9226-11e2-8023-806d6172696f}" ) - 1 );

			// Obtain Version.
			ObtainVersion ();

			// Check Version.
			if ( true )// !strcmp ( decryptText ( labelOne.c_str () ).c_str (), decryptText ( Version.c_str () ).c_str () ) )
			{
				// Setup 'dwGetKeyState'.
				dwGetKeyState = (DWORD) GetKeyState;

				// Run the Scanner.
				RunScanner ();

				// Setup Redirection Variables.
				Redirection ();

				// Setup Continuation Variables.
				Continuation ();

				// Read the RTB offset.
				if ( szRead == 1 )
					dwRTBOff = rV <BYTE> ( dwRTBPtr + 0x02 );
				else if ( szRead == 2 )
					dwRTBOff = rV <WORD> ( dwRTBPtr + 0x02 );
				else
					dwRTBOff = rV <DWORD> ( dwRTBPtr + 0x02 );

				// Adjust default hardware breakpoint targets.
				dwTarget [0] = NULL;
				dwTarget [1] = dwRTBPtr;
				dwTarget [2] = dwAntiHwid;
				dwTarget [3] = dwAntiMS;

				// Add the vectored exception handler.
				pVEH = AddVectoredExceptionHandler ( 1, VectoredHandler );

				while ( true )
				{						
					// Setup 'hWnd'.
					if ( !hWnd )
						hWnd = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) RunWnd, NULL, NULL, NULL );
					
					// Setup 'Falling'.
					if ( !hFalling )
						hFalling = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Falling, NULL, NULL, NULL );

					// Setup 'Misc'.
					if ( !hMisc )
						hMisc = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Misc, NULL, NULL, NULL );

					// Setup 'Rain'.
					if ( !hRain )
						hRain = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Rain, NULL, NULL, NULL );	

					// Setup 'Ingame'.
					if ( !hIngame )
						hIngame = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) CheckGame, NULL, NULL, NULL );

					// Setup 'Fly Hack'
					if ( !hFly )
						hFly = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Fly, NULL, NULL, NULL );

					// Setup 'Hotkeys'.
					if ( !hHotkeys )
						hHotkeys = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Hotkeys, NULL, NULL, NULL );

					// Setup 'Telekill'.
					if ( !hTelekill )
						hTelekill = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Telekill, NULL, NULL, NULL );

					// Setup 'Weapon Adjustment'.
					if ( !hWeapon )
						hWeapon = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) WeaponAdjustment, NULL, NULL, NULL );					
					
					// Install hardware breakpoints.
					QueryThreads ();

					// Rest
					Sleep ( 100 );
				}
			}
			else
				MessageBoxA ( 0, decryptText ( "Qmfbtf!epxompbe!uif!mbuftu!wfstjpo" ).c_str (), decryptText ( "Wfstjpo!Difdl" ).c_str (), MB_ICONINFORMATION );
		}
	}
}
//

// Exit.
void Exit ()
{
	// Remove exception handler
	RemoveVectoredExceptionHandler ( pVEH );

	// Close the Window
	EndDialog ( MainDlg, 0 );

	// Stop threads 
	CloseThread ( Menu );
	CloseThread ( hFalling );
	CloseThread ( hHotkeys );
	CloseThread ( hIngame );
	CloseThread ( hWnd );
	CloseThread ( hWeapon );
	CloseThread ( hTelekill );
	CloseThread ( hFly );
	CloseThread ( hRain );
	CloseThread ( hMain );

	// Force the exit
	DWORD uExitCode;
	GetExitCodeProcess ( GetCurrentProcess (), &uExitCode );
	TerminateProcess ( GetCurrentProcess, uExitCode );
}


//

// DLL Entry-point.
BOOL WINAPI DllMain ( HINSTANCE hInstance, DWORD fdwReason, LPVOID lpReserved )
{
	hInst = hInstance;

	switch ( fdwReason )
	{
	case DLL_PROCESS_ATTACH:
		{
			hMain = CreateThread ( NULL, NULL, (LPTHREAD_START_ROUTINE) Run, NULL, NULL, NULL );
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			Exit ();
		}
		break;
	}

	return TRUE;
}

//