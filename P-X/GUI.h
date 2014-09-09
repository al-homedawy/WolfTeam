#include "Rain.h"

// Main Window.
HWND MainDlg;
HWND InstDlg;

// Title.
string title = "mfu/ju/sbjo";

// Labels.
string labelTwo = "Jotfsu.!Pqfo0Ijef!Xjoepx";
string labelThree = "Tijgu,DUSM.!Dmptf!XpmgUfbn";
string labelFour = "Zpv!nvtu!ejf!up!bdujwbuf!ibdl";
string labelFive = "Dpefe!cz!mfu/ju/sbjo";
string labelSix  = "xxx/qspkfdusbjo/ofu";

// Combo Box.
LPCSTR ColorNames [] = { "Red", "Green", "Blue" };

// Conversion.
template <class T> T ConvertData ( string str )
{
	stringstream ss;
	ss << str;
	T Value;
	ss >> Value;
	return Value;
}
//

// CheckBoxes.
BOOL GetCheck ( int CHECKBOX )
{
	return ( SendMessageA ( GetDlgItem ( MainDlg, CHECKBOX ), BM_GETCHECK, 0, 0 ) == BST_CHECKED ? true : false );
}
VOID SetCheck ( int CHECKBOX, DWORD CHECK )
{
	SendMessageA ( GetDlgItem ( MainDlg, CHECKBOX ), BM_SETCHECK, CHECK, 0 );
}
//

// File Operations.
void SaveSettings ()
{
	// Obtain the pathway
	string Path = currentDir;
	Path += "\\Rain.ini";

	// Create the file.
	ofstream File ( Path.c_str () );

	// Write settings
	File << GetCheck ( IDC_CHECKBOX1 ) << endl;
	File << GetCheck ( IDC_CHECKBOX2 ) << endl;
	File << GetCheck ( IDC_CHECKBOX3 ) << endl;
	File << GetCheck ( IDC_CHECKBOX4 ) << endl;
	File << GetCheck ( IDC_CHECKBOX5 ) << endl;
	File << GetCheck ( IDC_CHECKBOX6 ) << endl;
	File << GetCheck ( IDC_CHECKBOX7 ) << endl;
	File << GetCheck ( IDC_CHECKBOX8 ) << endl;
	File << GetCheck ( IDC_CHECKBOX9 ) << endl;
	File << GetCheck ( IDC_CHECKBOX10 ) << endl;
	File << GetCheck ( IDC_CHECKBOX11 ) << endl;
	File << GetCheck ( IDC_CHECKBOX12 ) << endl;
	File << GetCheck ( IDC_CHECKBOX13 ) << endl;
	File << GetCheck ( IDC_CHECKBOX14 ) << endl;
	File << GetCheck ( IDC_CHECKBOX15 ) << endl;
	File << GetCheck ( IDC_CHECKBOX16 ) << endl;
	File << GetCheck ( IDC_CHECKBOX17 ) << endl;
	File << GetCheck ( IDC_CHECKBOX18 ) << endl;
	File << GetCheck ( IDC_CHECKBOX19 ) << endl;
	File << GetCheck ( IDC_CHECKBOX20 ) << endl;
	File << GetCheck ( IDC_CHECKBOX21 ) << endl;
	File << GetCheck ( IDC_CHECKBOX22 ) << endl;
	File << GetCheck ( IDC_CHECKBOX23 ) << endl;
	File << GetCheck ( IDC_CHECKBOX24 ) << endl;
	File << GetCheck ( IDC_CHECKBOX25 ) << endl;
	File << GetCheck ( IDC_CHECKBOX26 ) << endl;
	File << GetCheck ( IDC_CHECKBOX27 ) << endl;
	File << GetCheck ( IDC_CHECKBOX29 ) << endl;
	File << GetCheck ( IDC_CHECKBOX33 ) << endl;
	File << GetCheck ( IDC_CHECKBOX34 ) << endl;
	File << GetCheck ( IDC_CHECKBOX35 ) << endl;
	File << GetCheck ( IDC_CHECKBOX36 ) << endl;

	// Obtain window information.
	char szChamOne   [256];
	char szChamTwo   [256];
	
	GetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO1 ), szChamOne, 256 );
	GetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO2 ), szChamTwo, 256 );

	// Write the Inventory Data
	File << szChamOne << endl;
	File << szChamTwo << endl;

	// Close the file.
	File.close ();
}
int  LoadSettings ()
{
	// Obtain the pathway
	string Path = currentDir;
	Path += "\\Rain.ini";

	// Check if the path exists.
	if ( PathFileExists ( Path.c_str () ) )
	{
		// Open the file.
		ifstream File ( Path.c_str () );

		// Read the file
		int Counter = 0;

		while ( !File.eof () )
		{
			// Obtain line information
			string currentLine;
			getline ( File, currentLine );

			// Obtain checkbox status.
			BOOL CheckStatus = !strcmp ( currentLine.c_str (), "1" ) ? BST_CHECKED : BST_UNCHECKED;
			
			// Set checkbox status.
			if ( Counter == 0 )
				SetCheck ( IDC_CHECKBOX1, CheckStatus );
			if ( Counter == 1 )
				SetCheck ( IDC_CHECKBOX2, CheckStatus );
			if ( Counter == 2 )
				SetCheck ( IDC_CHECKBOX3, CheckStatus );
			if ( Counter == 3 )
				SetCheck ( IDC_CHECKBOX4, CheckStatus );
			if ( Counter == 4 )
				SetCheck ( IDC_CHECKBOX5, CheckStatus );
			if ( Counter == 5 )
				SetCheck ( IDC_CHECKBOX6, CheckStatus );
			if ( Counter == 6 )
				SetCheck ( IDC_CHECKBOX7, CheckStatus );
			if ( Counter == 7 )
				SetCheck ( IDC_CHECKBOX8, CheckStatus );
			if ( Counter == 8 )
				SetCheck ( IDC_CHECKBOX9, CheckStatus );
			if ( Counter == 9 )
				SetCheck ( IDC_CHECKBOX10, CheckStatus );
			if ( Counter == 10 )
				SetCheck ( IDC_CHECKBOX11, CheckStatus );
			if ( Counter == 11 )
				SetCheck ( IDC_CHECKBOX12, CheckStatus );
			if ( Counter == 12 )
				SetCheck ( IDC_CHECKBOX13, CheckStatus );
			if ( Counter == 13 )
				SetCheck ( IDC_CHECKBOX14, CheckStatus );
			if ( Counter == 14 )
				SetCheck ( IDC_CHECKBOX15, CheckStatus );
			if ( Counter == 15 )
				SetCheck ( IDC_CHECKBOX16, CheckStatus );
			if ( Counter == 16 )
				SetCheck ( IDC_CHECKBOX17, CheckStatus );
			if ( Counter == 17 )
				SetCheck ( IDC_CHECKBOX18, CheckStatus );
			if ( Counter == 18 )
				SetCheck ( IDC_CHECKBOX19, CheckStatus );
			if ( Counter == 19 )
				SetCheck ( IDC_CHECKBOX20, CheckStatus );
			if ( Counter == 20 )
				SetCheck ( IDC_CHECKBOX21, CheckStatus );
			if ( Counter == 21 )
				SetCheck ( IDC_CHECKBOX22, CheckStatus );
			if ( Counter == 22 )
				SetCheck ( IDC_CHECKBOX23, CheckStatus );
			if ( Counter == 23 )
				SetCheck ( IDC_CHECKBOX24, CheckStatus );
			if ( Counter == 24 )
				SetCheck ( IDC_CHECKBOX25, CheckStatus );
			if ( Counter == 25 )
				SetCheck ( IDC_CHECKBOX26, CheckStatus );
			if ( Counter == 26 )
				SetCheck ( IDC_CHECKBOX27, CheckStatus );
			if ( Counter == 28 )
				SetCheck ( IDC_CHECKBOX29, CheckStatus );
			if ( Counter == 29 )
				SetCheck ( IDC_CHECKBOX33, CheckStatus );
			if ( Counter == 30 )
				SetCheck ( IDC_CHECKBOX34, CheckStatus );	
			if ( Counter == 31 )
				SetCheck ( IDC_CHECKBOX35, CheckStatus );
			if ( Counter == 32 )
				SetCheck ( IDC_CHECKBOX36, CheckStatus );

			// Chams Visible/Not Visible.
			if ( Counter == 35 )
				SetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO1 ), currentLine.c_str () );
			if ( Counter == 36 )
				SetWindowText ( GetDlgItem ( MainDlg, IDC_COMBO2 ), currentLine.c_str () );

			++Counter;
		}

		// Close the file.
		File.close ();
	}
	else
		// Bad
		return 1;

	return 0;
}
//

// Window Procedures.
INT_PTR CALLBACK InstProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	InstDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}

	case WM_CLOSE:
		{
			InstDlg = NULL;
			EndDialog ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK MainProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	MainDlg = hWndDlg;

	switch ( uMsg )
	{
	case WM_COMMAND:
		{
			switch ( wParam )
			{
				// Instructions
			case IDC_BUTTON1:
				{
					RECT Coord;
					GetWindowRect ( InstDlg, &Coord );

					// Open the instructions window
					if ( !InstDlg )				
						DialogBoxA ( hInst, MAKEINTRESOURCE ( IDD_DIALOG2 ), NULL, InstProc );
					else
						SetWindowPos ( InstDlg, HWND_TOP, Coord.left, Coord.top, Coord.right - Coord.left, Coord.bottom - Coord.top, SWP_SHOWWINDOW );
				}
				break;

				// Save Settings.
			case IDC_BUTTON3:
				{
					SaveSettings ();
				}
				break;

				// Load Settings.
			case IDC_BUTTON4:
				{
					if ( LoadSettings () == 1 )
						MessageBoxA ( hWndDlg, "You haven't saved a settings file.", "Error", MB_ICONERROR );
				}
				break;

				// Help
			case IDC_BUTTON5:
				{
					ShellExecute ( NULL, "open", decryptText ( "iuuq;00xxx/qspkfdusbjo/ofu" ).c_str (), NULL, NULL, SW_SHOWNORMAL );
				}
				break;

				// Not Visible (Chams).
			case IDC_BUTTON6:
				{
					ResetResources = TRUE;
				}
				break;

				// Visible (Chams).
			case IDC_BUTTON7:
				{
					ResetResources = TRUE;
				}
				break;

				// Close.
			case IDC_BUTTON8:
				{
					DWORD dwExitCode;
					GetExitCodeProcess ( GetCurrentProcess (), &dwExitCode );
					TerminateProcess ( GetCurrentProcess (), dwExitCode );
				}
			}

			return TRUE;
		}

	case WM_INITDIALOG:
		{
			// Set default text.
			SetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), "Green" );
			SetWindowText ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), "Blue" );

			// Add options for Chams.
			for ( int i = 0; i < _countof ( ColorNames ); i ++ )
				SendMessageA ( GetDlgItem ( hWndDlg, IDC_COMBO1 ), CB_ADDSTRING, 0, (LPARAM) ColorNames [i] );

			for ( int i = 0; i < _countof ( ColorNames ); i ++ )
				SendMessageA ( GetDlgItem ( hWndDlg, IDC_COMBO2 ), CB_ADDSTRING, 0, (LPARAM) ColorNames [i] );

			// Load default settings.
			LoadSettings ();

			return TRUE;
		}

	case WM_CLOSE:
		{
			// Save Settings.
			SaveSettings ();

			// Close the window.
			EndDialog ( hWndDlg, 0 );
			return TRUE;
		}

	default:
		return FALSE;
	}
}
INT_PTR CALLBACK DummyProc ( HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_INITDIALOG:
		{			
			return TRUE;
		}

	case WM_CLOSE:
		{
			return TRUE;
		}

	default:
		return FALSE;
	}
}
//