#include <Windows.h>
#include <Psapi.h>
#include <fstream>
#include <sstream>
#include <TlHelp32.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Wininet.h>
#include <Iphlpapi.h>
#include <Shlwapi.h>

#include "resource.h"

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "Wininet.lib" )
#pragma comment ( lib, "Iphlpapi.lib" )
#pragma comment ( lib, "Shlwapi.lib" )
#pragma comment ( lib, "Winmm.lib" )

using namespace std;

/*
	1. Obtain HWID
	2. Read the output online
	3. Decrypt
	4. Compare
*/

// Certification Keys.
BYTE BytesOne [] = { 0x59, 0x6f, 0x75, 0x72, 0x20, 0x6c, 0x69, 0x63, 0x65, 0x6e, 0x73, 0x65, 0x20, 0x69, 0x73, 0x3a, 0x20 };

BYTE BytesTwo [] = { 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58,
					 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58 };

BYTE Certification    [] = { 0x4e, 0x4f, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49, 0x46, 0x49, 
							 0x43, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x20, 0x4b, 0x45, 0x59, 
							 0x20, 0x41, 0x56, 0x41, 0x49, 0x4c, 0x41, 0x42, 0x4c, 0x45, 
							 0x2e };

BYTE CertificationKey [] = { 0x53, 0x59, 0x4d, 0x42, 0x4f, 0x4c, 0x49, 0x53, 0x4d, 0x20, 
							 0x49, 0x53, 0x20, 0x41, 0x20, 0x50, 0x4f, 0x57, 0x45, 0x52, 
							 0x46, 0x55, 0x4c, 0x20, 0x54, 0x45, 0x43, 0x48, 0x4e, 0x49, 
							 0x51, 0x55, 0x45, 0x20, 0x54, 0x4f, 0x20, 0x45, 0x58, 0x50, 
							 0x52, 0x45, 0x53, 0x53, 0x20, 0x4d, 0x45, 0x53, 0x53, 0x41, 
							 0x47, 0x45, 0x53, 0x2e, 0x20, 0x4e, 0x49, 0x4b, 0x45, 0x20, 
							 0x4c, 0x4f, 0x47, 0x4f, 0x2e };

// Text Encryption/Decryption.
string encryptText ( string regularText )
{
	string encryptedText;

	for ( SIZE_T i = 0; i < regularText.length (); i ++ )
		encryptedText += regularText.at ( i ) + 0x01;

	return encryptedText;
}
string decryptText ( string encryptedText )
{
	string decryptedText;

	for ( SIZE_T i = 0; i < encryptedText.length (); i ++ )
		decryptedText += encryptedText.at ( i ) - 0x01;

	return decryptedText;
}
//

// User Information.
string License;
string Name;

// General Information.
string labelOne = "w32g";
string Version;

// Misc Functions.
string ToLowercase ( string Text )
{
	string newStr = "";

	for ( SIZE_T i = 0; i < Text.length (); i ++ )
	{
		if ( Text.at ( i ) >= 'A' &&
			 Text.at ( i ) <= 'Z' )
			newStr += Text.at ( i ) + 32;
		else
			newStr += Text.at ( i );
	}

	return newStr;
}
template <class T> void la ( T Value )
{
	stringstream ss;
	ss << Value;
	string str;
	str += ss.str ();
	MessageBox ( 0, str.c_str (), 0, 0 );
}
//

void ObtainVersion ()
{
	// Create the url equivelant
	string Url = decryptText ( "iuuq;00xxx/qspkfdusbjo/ofu0XpmgUfbn0Wfstjpo/qiq" );
	
	// Delete Cache
	DeleteUrlCacheEntry ( Url.c_str () );

	// Create the connection
	HINTERNET hInternet = InternetOpen ( NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );
	HINTERNET hUrl      = InternetOpenUrlA ( hInternet, Url.c_str (), NULL, NULL, NULL, NULL );

	// Read
	char szBuffer [256];
	DWORD bytesRead;
	InternetReadFile ( hUrl, &szBuffer, 256, &bytesRead );

	// Close the connection
	InternetCloseHandle ( hInternet );
	InternetCloseHandle ( hUrl );

	// Create the string
	Version = "";
	Version += (LPSTR) szBuffer;
	Version = Version.substr ( 0, labelOne.length () );
	Version = encryptText ( Version );
}
//

string GetMac ()
{
    char data[4096];
    ZeroMemory( data, 4096 );
     unsigned long  len = 4000;
    PIP_ADAPTER_INFO pinfo = ( PIP_ADAPTER_INFO ) data;
    char sbuf[20];
    string sret;

    DWORD ret = GetAdaptersInfo( pinfo, &len );
    if( ret != ERROR_SUCCESS )
        return string("");

    for(int k = 0; k < 5; k++ ) {
        sprintf_s(sbuf,"%02X-",pinfo->Address[k]);
        sret += sbuf;
    }
    sprintf_s(sbuf,"%02X",pinfo->Address[5]);
    sret += sbuf;
	string trimDash;

	for ( SIZE_T i = 0; i < sret.length (); i ++ )
		if ( sret.at ( i ) != '-' )
			trimDash += sret.at ( i );

    return( trimDash );
}
//

string GetLicense ()
{
	// Encrypt
	string LicenseNum;
	string FinalNum;

	// System Information
	SYSTEM_INFO SI;
	GetSystemInfo ( &SI );

	// Convert HDD to string
	stringstream ss;
	ss << GetMac ();
	ss << (void*) SI.dwProcessorType;
	ss << SI.dwNumberOfProcessors;
	LicenseNum += ss.str ();

	// Encrypt
	for ( SIZE_T i = 0; i < LicenseNum.length (); i ++ )
	{		
		if ( ( LicenseNum.at ( i ) >= '0' ) &&
			 ( LicenseNum.at ( i ) <= '9' ) )
			FinalNum += (char) ( 65 + ( LicenseNum.at ( i ) - 48 ) );
		else
			FinalNum += (char) ( 48 + ( LicenseNum.at ( i ) - 65 ) );
	}

	return FinalNum;
}
//

BOOL CheckOnline ( string License )
{
	// Create the url equivelant
	string Url = decryptText ( "iuuq;00qspkfdusbjo/ofu0XpmgUfbn0DifdlMjdfotf/qiq@mjdfotf>" );
	Url += License;

	// Delete cache
	DeleteUrlCacheEntry ( Url.c_str () );

	// Create the connection
	HINTERNET hInternet = InternetOpen ( NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );
	HINTERNET hUrl      = InternetOpenUrlA ( hInternet, Url.c_str (), NULL, NULL, NULL, NULL );

	// Read
	char szBuffer [256];
	DWORD bytesRead;
	InternetReadFile ( hUrl, &szBuffer, 256, &bytesRead );

	// Close the connection
	InternetCloseHandle ( hInternet );
	InternetCloseHandle ( hUrl );

	// Decrypt the encrypted string
	string message;
	int Counter = 0;

	for ( SIZE_T i = 0; i < strlen ( szBuffer ); i ++ )
	{
		if ( (char) ( szBuffer [i] + 20 ) == '-' )
			++Counter;

		if ( Counter < 2 )
			message += (char) ( szBuffer [i] + 20 );
	}

	// Obtain information from message
	int dash = message.find_first_of ( "-" );

	License = message.substr ( 0, dash );
	Name = message.substr ( dash + 1, message.length () );

	if ( !strcmp ( ToLowercase ( License ).c_str (), ToLowercase ( License ).c_str () ) )
		return TRUE;
	else
		return FALSE;
}
//