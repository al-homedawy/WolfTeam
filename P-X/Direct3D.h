#include "Memory.h"

// Game Device.
IDirect3DDevice9 *gameDev;

// Texture.
IDirect3DTexture9* textureOne;
IDirect3DTexture9* textureTwo;

// Cham Colors
DWORD dwPixel;

DWORD Colors [] = { D3DCOLOR_ARGB ( 255, 255, 0, 0 ),
					D3DCOLOR_ARGB ( 255, 0, 255, 0 ),
					D3DCOLOR_ARGB ( 255, 0, 0, 255 ) };

// Stride variables.
IDirect3DVertexBuffer9* pStreamData;
UINT OffsetBytes;
UINT Stride;

// Other variables.
bool AllowHeightIncrease = false;
DWORD dwZFunc = NULL;
int Counter = 0;

// Speed Related Variables.
float currentX = 0;
float currentY = 0;
float currentZ = 0;
float DiffX    = 0;
float DiffY    = 0;
float DiffZ    = 0;
int  SpeedMultiple  = 1;
int  HeightMultiple = 1;

// Player Coordinates.
float prevY = 0;

// Redirection Variables.
DWORD dwRedirect  = NULL; // Present
DWORD dwRedirect2 = NULL; // Reset

// Continuation Variables.
DWORD dwContinue  = NULL;
DWORD dwContinue2 = NULL;
DWORD dwContinue3 = NULL; // Player Movement
DWORD dwContinue4 = NULL; // DrawIndexedPrimitive
DWORD dwContinue5 = NULL; // Return To Base

// Check 'dwObjectsY' for repetition.
BOOL CheckObjects ( DWORD dwEcx )
{
	if ( ( dwEcx + 0xC8 ) == playerY )
		return FALSE;

	for ( int i = 0; i < numObjects; i ++ )
		if ( dwObjectY [i] == ( dwEcx + 0xC8 ) )
			return FALSE;

	return TRUE;
}
//

// Redirection Functions.
__declspec ( naked ) void RedirectMovement ()
{
	__asm
	{
		// Save registers
		mov dwEcx, ecx
		mov dwEbx, ebx
		mov dwEax, eax
		mov dwEdx, edx
		mov dwEsi, esi
		mov dwEdi, edi
		mov dwEbp, ebp
		mov dwEsp, esp
	}

	// Check to see if Telekill is on.
	if ( bTelekill )
	{
		if ( ( numObjects + 1 ) > 20 )
			numObjects = 0;

		dwObjectY [numObjects] = dwEcx + 0xC8;

		++numObjects;
	}

	// Check to see if we're on the current player and if any hacks are on.
	if ( ( ( NoGravity ) || 
		   ( SuperJump ) ||
		   ( SpeedHack ) ) &&
		 ( ( ( ( dwEcx + 0xC8 ) == playerY ) ||
		     ( ( dwEax + 0x04 ) == playerY ) ) &&
		     ( Spawned ) &&
		     ( Counter >= 5 ) ) )
	{
		// Reset variables
		AllowHeightIncrease = true;
		SpeedMultiple       = 1;
		HeightMultiple      = 1;

		// No Gravity / Super Jump / Speed Hack
		if ( ( NoGravity ) || 
			 ( SuperJump ) ||
			 ( SpeedHack ) )
		{
			// Allow height increase
			if ( NoGravity )	
			{
				if ( !WIS )
				{
					if ( rV <float> ( dwEax + 0x04 ) < rV <float> ( playerY ) )
						AllowHeightIncrease = false;
				}
				else	
					AllowHeightIncrease = false;
			}
			else
				AllowHeightIncrease = true;			

			// Super Jump
			if ( SuperJump )
			{
				// Adjust height multiple
				if ( sj1 )
					HeightMultiple = 2;
				if ( sj2 )
					HeightMultiple = 4;
				if ( sj3 )
					HeightMultiple = 6;

				// Obtain 'currentY'
				currentY = rV <float> ( playerY );
			}

			// Speed Hack
			if ( SpeedHack )
			{
				// Adjust speed multiple
				if ( sh1 )
					SpeedMultiple = 2;
				if ( sh2 )
					SpeedMultiple = 4;
				if ( sh3 )
					SpeedMultiple = 6;

				// Obtain 'currentX' and 'currentZ'
				currentX = rV <float> ( playerX );
				currentZ = rV <float> ( playerZ );
			}

			if ( AllowHeightIncrease )
			{
				if ( WIS )
				{
					__asm
					{
						// Restore registers
						mov ecx, dwEcx
						mov ebx, dwEbx
						mov eax, dwEax
						mov edx, dwEdx
						mov esi, dwEsi
						mov edi, dwEdi
						mov ebp, dwEbp
						mov esp, dwEsp

						// Continue regular execution
						fld dword ptr [eax]
						fstp dword ptr [ecx+0xc4]
						fld dword ptr [eax+0x04]
						fstp dword ptr [ecx+0xc8]
						fld dword ptr [eax+0x08]
						mov eax, 01
						fstp dword ptr [ecx+0xcc]

						// Save registers
						mov dwEcx, ecx
						mov dwEbx, ebx
						mov dwEax, eax
						mov dwEdx, edx
						mov dwEsi, esi
						mov dwEdi, edi
						mov dwEbp, ebp
						mov dwEsp, esp
					}
				}
				else
				{
					__asm
					{
						// Restore registers
						mov ecx, dwEcx
						mov ebx, dwEbx
						mov eax, dwEax
						mov edx, dwEdx
						mov esi, dwEsi
						mov edi, dwEdi
						mov ebp, dwEbp
						mov esp, dwEsp

						// Continue regular execution
						fld dword ptr [eax]
						fstp dword ptr [ecx + 0xc4] 
						fld dword ptr [eax+04]
						fstp dword ptr [ecx + 0xc8] 
						fld dword ptr [eax + 0x08]
						fstp dword ptr [ecx + 0xcc]

						// Save registers
						mov dwEcx, ecx
						mov dwEbx, ebx
						mov dwEax, eax
						mov dwEdx, edx
						mov dwEsi, esi
						mov dwEdi, edi
						mov dwEbp, ebp
						mov dwEsp, esp
					}
				}
			}
			else
			{
				if ( WIS )
				{
					__asm
					{
						// Restore registers
						mov ecx, dwEcx
						mov ebx, dwEbx
						mov eax, dwEax
						mov edx, dwEdx
						mov esi, dwEsi
						mov edi, dwEdi
						mov ebp, dwEbp
						mov esp, dwEsp

						// Continue irregular execution
						fld dword ptr [eax]
						fstp dword ptr [ecx+0xc4]
						fld dword ptr [eax+0x04]
						fld dword ptr [eax+0x08]
						mov eax, 01
						fstp dword ptr [ecx+0xcc]

						// Save registers
						mov dwEcx, ecx
						mov dwEbx, ebx
						mov dwEax, eax
						mov dwEdx, edx
						mov dwEsi, esi
						mov dwEdi, edi
						mov dwEbp, ebp
						mov dwEsp, esp
					}
				}
				else
				{
					__asm
					{
						// Restore registers
						mov ecx, dwEcx
						mov ebx, dwEbx
						mov eax, dwEax
						mov edx, dwEdx
						mov esi, dwEsi
						mov edi, dwEdi
						mov ebp, dwEbp
						mov esp, dwEsp

						// Continue irregular execution
						fld dword ptr [eax]
						fstp dword ptr [ecx + 0xc4] 
						fld dword ptr [eax+04]
						fld dword ptr [eax + 0x08]
						fstp dword ptr [ecx + 0xcc]

						// Save registers
						mov dwEcx, ecx
						mov dwEbx, ebx
						mov dwEax, eax
						mov dwEdx, edx
						mov dwEsi, esi
						mov dwEdi, edi
						mov dwEbp, ebp
						mov dwEsp, esp
					}
				}
			}

			// Super Jump
			if ( SuperJump )
			{
				// Calculate difference
				rV <float> ( playerY ) > currentY ? DiffY = rV <float> ( playerY ) - currentY : DiffY = currentY - rV <float> ( playerY );

				// Increment/Decrease height
				if ( rV <float> ( playerY ) > currentY )
					wV <float> ( playerY, currentY + ( DiffY * HeightMultiple ) );
				else if ( !NoGravity )
					wV <float> ( playerY, currentY - DiffY );
			}

			// Speed Hack
			if ( SpeedHack )
			{
				// Calculate difference
				rV <float> ( playerX ) > currentX ? DiffX = rV <float> ( playerX ) - currentX : DiffX = currentX - rV <float> ( playerX );
				rV <float> ( playerZ ) > currentZ ? DiffZ = rV <float> ( playerZ ) - currentZ : DiffZ = currentZ - rV <float> ( playerZ );

				// Increment/Decrease 'X'
				if ( rV <float> ( playerX ) > currentX )
					wV <float> ( playerX, currentX + ( DiffX * SpeedMultiple ) );
				else 
					wV <float> ( playerX, currentX - ( DiffX * SpeedMultiple ) );

				// Increment/Decrease 'Z'
				if ( rV <float> ( playerZ ) > currentZ )
					wV <float> ( playerZ, currentZ + ( DiffZ * SpeedMultiple ) );
				else 
					wV <float> ( playerZ, currentZ - ( DiffZ * SpeedMultiple ) );
			}

			__asm
			{
				// Restore registers
				mov ecx, dwEcx
				mov ebx, dwEbx
				mov eax, dwEax
				mov edx, dwEdx
				mov esi, dwEsi
				mov edi, dwEdi
				mov ebp, dwEbp
				mov esp, dwEsp
			}
		}	
	}
	else
	{
		if ( ( ( dwEcx + 0xC8 ) == playerY  ) ||
			 ( ( dwEax + 0x04 ) == playerY  ) )
			++Counter;

		if ( WIS )
		{
			__asm
			{
				// Restore registers
				mov ecx, dwEcx
				mov ebx, dwEbx
				mov eax, dwEax
				mov edx, dwEdx
				mov esi, dwEsi
				mov edi, dwEdi
				mov ebp, dwEbp
				mov esp, dwEsp

				// Continue regular execution
				fld dword ptr [eax]
				fstp dword ptr [ecx+0xc4]
				fld dword ptr [eax+0x04]
				fstp dword ptr [ecx+0xc8]
				fld dword ptr [eax+0x08]
				mov eax, 01
				fstp dword ptr [ecx+0xcc]
			}
		}
		else
		{
			__asm
			{
				// Restore registers
				mov ecx, dwEcx
				mov ebx, dwEbx
				mov eax, dwEax
				mov edx, dwEdx
				mov esi, dwEsi
				mov edi, dwEdi
				mov ebp, dwEbp
				mov esp, dwEsp

				// Continue regular execution
				fld dword ptr [eax]
				fstp dword ptr [ecx + 0xc4] 
				fld dword ptr [eax+04]
				fstp dword ptr [ecx + 0xc8] 
				fld dword ptr [eax + 0x08]
				fstp dword ptr [ecx + 0xcc]
			}
		}
	}

	__asm
	{
		// Save registers
		mov dwEcx, ecx
		mov dwEbx, ebx
		mov dwEax, eax
		mov dwEdx, edx
		mov dwEsi, esi
		mov dwEdi, edi
		mov dwEbp, ebp
		mov dwEsp, esp
	}

	__asm
	{
		// Restore registers
		mov ecx, dwEcx
		mov ebx, dwEbx
		mov eax, dwEax
		mov edx, dwEdx
		mov esi, dwEsi
		mov edi, dwEdi
		mov ebp, dwEbp
		mov esp, dwEsp

		// Continue.
		jmp dword ptr [ dwContinue3 ]
	}
}
__declspec ( naked ) void RedirectRTB ()
{
	__asm
	{
		pushad
		mov dwEax, eax
		mov dwEcx, ecx
	}

	// Set Return To Base
	if ( WIS )
		dwRTB = dwEcx + dwRTBOff;
	else
		dwRTB = dwEax + dwRTBOff;

	// Restore the hook
	dwTarget [1] = NULL;

	__asm
	{
		popad
		jmp dword ptr [ dwContinue5 ]
	}
}
//

// Generating Textures.
HRESULT GenerateTexture ( IDirect3DDevice9 *pD3Ddev, IDirect3DTexture9 **ppD3Dtex, DWORD colour32 )
{
	if( FAILED(pD3Ddev->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)) )
		return E_FAIL;
	
	WORD colour16 =	((WORD)((colour32>>28)&0xF)<<12)
			|(WORD)(((colour32>>20)&0xF)<<8)
			|(WORD)(((colour32>>12)&0xF)<<4)
			|(WORD)(((colour32>>4)&0xF)<<0);

	D3DLOCKED_RECT d3dlr;    
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for(int xy=0; xy < 8*8; xy++)
		*pDst16++ = colour16;

	(*ppD3Dtex)->UnlockRect(0);

	return S_OK;
}
//

// Reset Direct3D Resources.
void RefreshDirect3D ( IDirect3DDevice9 *pDevice )
{
	// Obtain 'IDC_COMBO1'.
	char szBuffer [256];
	GetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO1 ), szBuffer, 256 );

	char szBuffer2 [256];
	GetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO2 ), szBuffer2, 256 );

	// Create 'index'.
	int index  = 0;
	int index2 = 0;

	// Setup 'index'.
	if ( !strcmp ( ToLowercase ( szBuffer ).c_str (), ToLowercase ( "Red" ).c_str () ) )
		index = 0;
	else if ( !strcmp ( ToLowercase ( szBuffer ).c_str (), ToLowercase ( "Green" ).c_str () ) ) 
		index = 1;
	else if ( !strcmp ( ToLowercase ( szBuffer ).c_str (), ToLowercase ( "Blue" ).c_str () ) )
		index = 2;

	if ( !strcmp ( ToLowercase ( szBuffer2 ).c_str (), ToLowercase ( "Red" ).c_str () ) )
		index2 = 0;
	else if ( !strcmp ( ToLowercase ( szBuffer2 ).c_str (), ToLowercase ( "Green" ).c_str () ) ) 
		index2 = 1;
	else if ( !strcmp ( ToLowercase ( szBuffer2 ).c_str (), ToLowercase ( "Blue" ).c_str () ) )
		index2 = 2;

	// Regenerate texture.
	if ( textureOne )
	{
		textureOne->Release ();
		textureOne = NULL;
	}

	if ( textureTwo )
	{
		textureTwo->Release ();
		textureTwo = NULL;
	}

	// Reset resources.
	if ( !textureOne )
		GenerateTexture ( pDevice, &textureOne, Colors [index] );

	if ( !textureTwo )
		GenerateTexture ( pDevice, &textureTwo, Colors [index2] );
}
//

// Direct3D Functions.
__declspec ( naked ) HRESULT WINAPI CallDip ( IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount )
{
	__asm
	{
		mov edi, edi
		push ebp
		mov ebp, esp
		push 0xFF
		jmp dword ptr [ dwContinue4 ]
	}
}
//

// Direct3D Hooks.
__declspec ( naked ) HRESULT WINAPI DrawIndexedPrimitive ( IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE Type, INT BaseIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount )
{
	__asm
	{
		// Save registers
		pushad
	}

	// Obtain the window of wolfteam.
	if ( !hwndWolfTeam )
		hwndWolfTeam = GetForegroundWindow ();

	// Check for change in device.
	if ( gameDev != pDevice )
	{
		// Set 'gameDev'
		gameDev = pDevice;

		// Enable 'ResetResources'
		ResetResources = TRUE;
	}
	else if ( !ResetResources )
	{
		// Obtain the stride 
		pDevice->GetStreamSource ( 0, &pStreamData, &OffsetBytes, &Stride );

		// X-Ray Vision
		if ( XRay )
		{
			// Disable the 'Z' Buffer
			pDevice->GetRenderState ( D3DRS_ZFUNC, &dwZFunc );
			pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE ); 
			pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_NEVER );
		}

		// Chameleon
		if ( ( Stride == 40 ) ||
			 ( Stride == 44 ) )
		{
			if ( Chams )
			{
				// Disable the 'Z' Buffer.
				pDevice->GetRenderState ( D3DRS_ZFUNC, &dwPixel );
				pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE ); 
				pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_NEVER );

				// Color non-visible objects.
				pDevice->SetTexture ( 0, textureTwo );

				// Call Dip.
				::CallDip ( pDevice, Type, BaseIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount );

				// Enable the 'Z' Buffer
				pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_TRUE ); 
				pDevice->SetRenderState ( D3DRS_ZFUNC, dwPixel );

				// Color visible objects.
				pDevice->SetTexture ( 0, textureOne );
			}
			// Wall Hack
			else if ( WallHack )
			{
				// Disable the 'Z' Buffer
				pDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE ); 
				pDevice->SetRenderState ( D3DRS_ZFUNC, D3DCMP_NEVER );
			}

			// Wireframe
			if ( WireFrame )
				pDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME ); 
		}

		// Fullbright
		if ( FullBright )
		{
			pDevice->SetRenderState ( D3DRS_LIGHTING, false );
			pDevice->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB ( 255, 255, 255, 255 ) ); 
		}

		// No Fog
		if ( NoFog )
			pDevice->SetRenderState ( D3DRS_FOGENABLE, false );
	}

	// Continue the function
	__asm
	{
		// Restore registers
		popad

		// Continue the function
		push 0xFF

		// Continue execution
		jmp dword ptr [ dwContinue4 ]
	}
}
//