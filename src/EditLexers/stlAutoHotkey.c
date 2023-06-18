#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_AHK = {{
//++Autogenerated -- start of section automatically generated
"and as base break case catch class contains continue critical default else exit ExitApp extends false finally for "
"get global goto if in is isset local loop new not OnError OnExit or pause reload return "
"set SetTimer sleep static super suspend switch this thread throw true try unset until while "

, // 1 directives
"ClipboardTimeout CriticalObjectSleepTime CriticalObjectTimeOut DefineDefaultObjectValue DllImport DllLoad ErrorStdOut "
"HotIf HotIfTimeout Hotstring Include IncludeAgain InputLevel MaxThreads MaxThreadsBuffer MaxThreadsPerHotkey NoTrayIcon "
"Requires SingleInstance SuspendExempt UseHook "
"Warn WarnContinuableException WinActivateForce WindowClassGui WindowClassMain "

, // 2 compiler directives
"Ahk2Exe-AddResource Ahk2Exe-Base Ahk2Exe-Bin Ahk2Exe-ConsoleApp Ahk2Exe-Cont Ahk2Exe-Debug Ahk2Exe-ExeName "
"Ahk2Exe-IgnoreBegin Ahk2Exe-IgnoreEnd Ahk2Exe-Keep Ahk2Exe-Let Ahk2Exe-Nop Ahk2Exe-Obey Ahk2Exe-PostExec "
"Ahk2Exe-ResourceID Ahk2Exe-Set Ahk2Exe-SetMainIcon Ahk2Exe-UpdateManifest Ahk2Exe-UseResourceLang "

, // 3 objects
"ActiveX Any Array BoundFunc Buffer Button "
"CheckBox Class ClipboardAll Closure ComboBox ComObjArray ComObject ComValue ComValueRef Control Custom DateTime DDL "
"Edit Enumerator Error File Float Func GroupBox Gui Hotkey IndexError InputHook Integer Link List ListBox ListView "
"Map MemberError MemoryError Menu MenuBar MethodError MonthCal Number Object OSError "
"Pic Primitive Progress PropertyError Radio RegExMatchInfo Slider StatusBar String "
"Tab TargetError Text TimeoutError TreeView TypeError UnsetError UnsetItemError UpDown ValueError VarRef "
"ZeroDivisionError "

, // 4 built-in variables
"A_AhkPath A_AhkVersion A_AllowMainWindow A_AppData A_AppDataCommon A_Args A_Clipboard A_ComputerName A_ComSpec "
"A_ControlDelay A_CoordModeCaret A_CoordModeMenu A_CoordModeMouse A_CoordModePixel A_CoordModeToolTip A_Cursor "
"A_DD A_DDD A_DDDD A_DefaultMouseSpeed A_Desktop A_DesktopCommon A_DetectHiddenText A_DetectHiddenWindows "
"A_EndChar A_EventInfo A_FileEncoding A_HotkeyInterval A_HotkeyModifierTimeout A_Hour "
"A_IconFile A_IconHidden A_IconNumber A_IconTip A_Index A_InitialWorkingDir "
"A_Is64bitOS A_IsAdmin A_IsCompiled A_IsCritical A_IsPaused A_IsSuspended "
"A_KeyDelay A_KeyDelayPlay A_KeyDuration A_KeyDurationPlay A_Language A_LastError A_LineFile A_LineNumber A_ListLines "
"A_LoopField A_LoopFileAttrib A_LoopFileDir A_LoopFileExt A_LoopFileFullPath A_LoopFileName A_LoopFilePath "
"A_LoopFileShortName A_LoopFileShortPath A_LoopFileSize A_LoopFileSizeKB A_LoopFileSizeMB "
"A_LoopFileTimeAccessed A_LoopFileTimeCreated A_LoopFileTimeModified "
"A_LoopReadLine A_LoopRegKey A_LoopRegName A_LoopRegTimeModified A_LoopRegType "
"A_MaxHotkeysPerInterval A_MenuMaskKey A_Min A_MM A_MMM A_MMMM A_MouseDelay A_MouseDelayPlay A_MSec A_MyDocuments "
"A_Now A_NowUTC A_OSVersion A_PriorHotkey A_PriorKey A_ProgramFiles A_Programs A_ProgramsCommon A_PtrSize A_RegView "
"A_ScreenDPI A_ScreenHeight A_ScreenWidth A_ScriptDir A_ScriptFullPath A_ScriptHwnd A_ScriptName "
"A_Sec A_SendLevel A_SendMode A_Space A_StartMenu A_StartMenuCommon A_Startup A_StartupCommon A_StoreCapsLockMode A_Tab "
"A_Temp A_ThisFunc A_ThisHotkey A_TickCount "
"A_TimeIdle A_TimeIdleKeyboard A_TimeIdleMouse A_TimeIdlePhysical A_TimeSincePriorHotkey A_TimeSinceThisHotkey "
"A_TitleMatchMode A_TitleMatchModeSpeed A_TrayMenu A_UserName A_WDay A_WinDelay A_WinDir A_WorkingDir "
"A_YDay A_YWeek A_YYYY "

, // 5 keys
"Alt AppsKey "
"Backspace Browser_Back Browser_Favorites Browser_Forward Browser_Home Browser_Refresh Browser_Search Browser_Stop BS "
"CapsLock Ctrl CtrlBreak Del Delete Down End Enter Esc Escape Help Home Ins Insert "
"JoyAxes JoyButtons JoyInfo JoyName JoyPOV JoyR JoyU JoyV JoyX JoyY JoyZ "
"LAlt Launch_App1 Launch_App2 Launch_Mail Launch_Media LButton LControl LCtrl Left LShift LWin "
"MButton Media_Next Media_Play_Pause Media_Prev Media_Stop "
"NumLock Numpad0 Numpad1 Numpad2 Numpad3 Numpad4 Numpad5 Numpad6 Numpad7 Numpad8 Numpad9 NumpadAdd NumpadClear "
"NumpadDel NumpadDiv NumpadDot NumpadDown NumpadEnd NumpadEnter NumpadHome NumpadIns NumpadLeft NumpadMult "
"NumpadPgDn NumpadPgUp NumpadRight NumpadSub NumpadUp "
"Pause PgDn PgUp PrintScreen RAlt RButton RControl RCtrl Right RShift RWin ScrollLock Shift Sleep Space Up "
"Volume_Down Volume_Mute Volume_Up WheelDown WheelLeft WheelRight WheelUp XButton1 XButton2 "

, // 6 functions
"__Call( __Delete( __Enum( __Get( __Init( __Item __New( __Set( Abs( ACos( Add( AddControlType( AddStandard( ASin( ATan( "
"Bind( BlockInput "
"Call( CallbackCreate( CaretGetPos( Ceil( Check( Choose( Chr( Clear( Click ClipboardAll( ClipWait Clone( Close( ComCall( "
"ComObjActive( ComObjArray( ComObjConnect ComObject( ComObjFlags( ComObjFromPtr( ComObjGet( ComObjQuery( ComObjType( "
"ComObjValue( ComValue( ControlAddItem ControlChooseIndex ControlChooseString ControlClick ControlDeleteItem "
"ControlFindItem( ControlFocus ControlGetChecked( ControlGetChoice( ControlGetClassNN( "
"ControlGetEnabled( ControlGetExStyle( ControlGetFocus( ControlGetHwnd( ControlGetIndex( ControlGetItems( ControlGetPos "
"ControlGetStyle( ControlGetText( ControlGetVisible( ControlHide ControlHideDropDown ControlMove "
"ControlSend ControlSendText ControlSetChecked ControlSetEnabled ControlSetExStyle ControlSetStyle ControlSetText "
"ControlShow ControlShowDropDown CoordMode Cos( "
"DateAdd( DateDiff( DefineProp( Delete( DeleteProp( Destroy( DetectHiddenText DetectHiddenWindows "
"DirCopy DirCreate DirDelete DirExist( DirMove DirSelect( Disable( DllCall( Download DriveEject DriveGetCapacity( "
"DriveGetFileSystem( DriveGetLabel( DriveGetList( DriveGetSerial( DriveGetSpaceFree( DriveGetStatus( DriveGetStatusCD( "
"DriveGetType( DriveLock DriveRetract DriveSetLabel DriveUnlock "
"Edit EditGetCurrentCol( EditGetCurrentLine( EditGetLine( EditGetLineCount( EditGetSelectedText( EditPaste "
"Enable( EnvGet( EnvSet Exp( "
"FileAppend FileCopy FileCreateShortcut FileDelete FileEncoding FileExist( "
"FileGetAttrib( FileGetShortcut FileGetSize( FileGetTime( FileGetVersion( FileInstall FileMove FileOpen( "
"FileRead( FileRecycle FileRecycleEmpty FileSelect( FileSetAttrib FileSetTime Flash( Float( Floor( "
"Focus( Format( FormatTime( "
"Get( GetClientPos( GetKeyName( GetKeySC( GetKeyState( GetKeyVK( GetMethod( GetOwnPropDesc( GetPos( "
"GroupActivate( GroupAdd GroupClose GroupDeactivate GuiCtrlFromHwnd( GuiFromHwnd( "
"Has( HasBase( HasMethod( HasOwnProp( HasProp( Hide( HotIf Hotkey Hotstring Hotstring( "
"ImageSearch "
"IniDelete IniRead( IniWrite InputBox( InputHook( Insert( InsertAt( InstallKeybdHook InstallMouseHook InStr( Integer( "
"IsAlnum( IsAlpha( IsByRef( IsDigit( IsFloat( IsInteger( IsLabel( IsLower( IsNumber( IsObject( IsOptional( "
"IsSet( IsSetRef( IsSpace( IsTime( IsUpper( IsXDigit( "
"KeyHistory KeyWait ListHotkeys ListLines ListVars ListViewGetContent( Ln( LoadPicture( Log( LTrim( "
"Max( Maximize( MenuFromHandle( MenuSelect Min( Minimize( Mod( "
"MonitorGet( MonitorGetCount( MonitorGetName( MonitorGetPrimary( MonitorGetWorkArea( "
"MouseClick MouseClickDrag MouseGetPos MouseMove Move( MsgBox MsgBox( "
"Number( NumGet( NumPut "
"ObjAddRef( ObjGetBase( ObjGetCapacity( ObjOwnPropCount( ObjRelease( ObjSetBase( ObjSetCapacity( "
"OnClipboardChange OnEvent( OnMessage OnNotify( Opt( Ord( OutputDebug OwnProps( "
"Persistent PixelGetColor( PixelSearch Pop( PostMessage ProcessClose ProcessExist( "
"ProcessGetName( ProcessGetParent( ProcessGetPath( ProcessSetPriority ProcessWait( ProcessWaitClose( Push( "
"Random( RawRead( RawWrite( Read( ReadLine( Redraw( "
"RegCreateKey RegDelete RegDeleteKey RegExMatch( RegExReplace( RegRead( RegWrite RemoveAt( Rename( Restore( Round( "
"RTrim( Run RunAs RunWait( "
"Seek( Send SendEvent SendInput SendLevel SendMessage( SendMode SendPlay SendText Set( "
"SetCapsLockState SetColor( SetControlDelay SetDefaultMouseSpeed SetFont( SetFormat( SetIcon( SetKeyDelay SetMouseDelay "
"SetNumLockState SetRegView SetScrollLockState SetStoreCapsLockMode SetTitleMatchMode SetWinDelay SetWorkingDir "
"Show( Shutdown Sin( "
"Sort( SoundBeep SoundGetInterface( SoundGetMute( SoundGetName( SoundGetVolume( SoundPlay SoundSetMute SoundSetVolume "
"SplitPath Sqrt( StatusBarGetText( StatusBarWait "
"StrCompare( StrGet( String( StrLen( StrLower( StrPtr( StrPut StrReplace( StrSplit( StrTitle( StrUpper( "
"Submit( SubStr( Suspend SysGet( SysGetIPAddresses( "
"Tan( ToggleCheck( ToggleEnable( ToolTip TraySetIcon( TrayTip Trim( Uncheck( UseTab( VarSetStrCapacity( VerCompare( "
"WinActivate WinActivateBottom WinActive( WinClose WinExist( "
"WinGetClass( WinGetClientPos WinGetControls( WinGetControlsHwnd( WinGetCount( WinGetExStyle( WinGetID( WinGetIDLast( "
"WinGetList( WinGetMinMax( WinGetPID( WinGetPos WinGetProcessName( WinGetProcessPath( WinGetStyle( "
"WinGetText( WinGetTitle( WinGetTransColor( WinGetTransparent( WinHide WinKill "
"WinMaximize WinMinimize WinMinimizeAll WinMinimizeAllUndo WinMove WinMoveBottom WinMoveTop WinRedraw WinRestore "
"WinSetAlwaysOnTop WinSetEnabled WinSetExStyle WinSetRegion WinSetStyle WinSetTitle WinSetTransColor WinSetTransparent "
"WinShow WinWait WinWaitActive WinWaitClose WinWaitNotActive Write( WriteLine( "

, // 7 misc
"AtEOF BackColor Break "
"Capacity Case CaseSense Catch ClassNN ClickCount Com Comment Comments Contains Continue Count Critical Default "
"Else Enabled Encoding EndChars Exit False Files Finally Focused FocusedCtrl For Force Global Goto "
"HKCU HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG HKEY_CURRENT_USER HKEY_LOCAL_MACHINE HKEY_USERS HKLM Handle Hwnd "
"If Ignore IsBuiltIn IsVariadic Join LTrim Len Length Local Loop MarginX MarginY Mark MaxParams MinParams Name NoMouse "
"Off Parse Pos Prompt Ptr "
"REG_BINARY REG_DWORD REG_DWORD_BIG_ENDIAN REG_EXPAND_SZ REG_FULL_RESOURCE_DESCRIPTOR REG_LINK REG_MULTI_SZ REG_QWORD "
"REG_RESOURCE_LIST REG_RESOURCE_REQUIREMENTS_LIST REG_SZ RTrim Read Reg Reload Return "
"Size Static Suspend Switch Thread Throw Title True Try Type Until Value Visible While "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
//--Autogenerated -- end of section automatically generated
}};

static EDITSTYLE Styles_AHK[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_AHK_KEYWORD, NP2StyleX_Keyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_AHK_DIRECTIVE_SHARP, SCE_AHK_DIRECTIVE_AT, 0, 0), NP2StyleX_Directive, L"fore:#C65D09" },
	{ SCE_AHK_CLASS, NP2StyleX_Class, L"bold; fore:#1E90FF" },
	{ SCE_AHK_BUILTIN_FUNCTION, NP2StyleX_BuiltInFunction, L"fore:#0080C0" },
	{ SCE_AHK_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ SCE_AHK_BUILTIN_VARIABLE, NP2StyleX_PredefinedVariable, L"fore:#B000B0" },
	{ SCE_AHK_DYNAMIC_VARIABLE, NP2StyleX_Variable, L"fore:#808000" },
	{ SCE_AHK_KEY, NP2StyleX_Key, L"fore:#007F7F" },
	{ MULTI_STYLE(SCE_AHK_COMMENTLINE, SCE_AHK_COMMENTBLOCK, SCE_AHK_SECTION_COMMENT, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_AHK_STRING_SQ, SCE_AHK_STRING_DQ, SCE_AHK_HOTSTRING_VALUE, 0), NP2StyleX_String, L"fore:#008000" },
	{ MULTI_STYLE(SCE_AHK_HOTSTRING_OPTION, SCE_AHK_HOTSTRING_KEY, 0, 0), NP2StyleX_HotString, L"fore:#C08000" },
	{ SCE_AHK_HOTKEY, NP2StyleX_HotKey, L"fore:#7C5AF3" },
	{ SCE_AHK_SENTKEY, NP2StyleX_SendKey, L"fore:#8080FF" },
	{ MULTI_STYLE(SCE_AHK_SECTION_SQ, SCE_AHK_SECTION_DQ, SCE_AHK_SECTION_NQ, SCE_AHK_SECTION_OPTION), NP2StyleX_Section, L"fore:#F08000" },
	{ SCE_AHK_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ SCE_AHK_FORMAT_SPECIFIER, NP2StyleX_FormatSpecifier, L"fore:#7C5AF3" },
	{ SCE_AHK_LABEL, NP2StyleX_Label, L"fore:#C80000; back:#F4F4F4" },
	{ SCE_AHK_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_AHK_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexAutoHotkey = {
	SCLEX_AUTOHOTKEY, NP2LEX_AUTOHOTKEY,
//Settings++Autogenerated -- start of section automatically generated
		LexerAttr_PrintfFormatSpecifier,
		TAB_WIDTH_4, INDENT_WIDTH_4,
		(1 << 0) | (1 << 1), // class, function
		0,
		'`', SCE_AHK_ESCAPECHAR, SCE_AHK_FORMAT_SPECIFIER,
		0,
		0, 0,
		SCE_AHK_OPERATOR, 0
		, KeywordAttr32(0, KeywordAttr_MakeLower | KeywordAttr_PreSorted) // keywords
		| KeywordAttr32(1, KeywordAttr_NoLexer | KeywordAttr_NoAutoComp) // directives
		| KeywordAttr32(2, KeywordAttr_NoLexer | KeywordAttr_NoAutoComp) // compiler directives
		| KeywordAttr32(3, KeywordAttr_MakeLower | KeywordAttr_PreSorted) // objects
		| KeywordAttr32(4, KeywordAttr_MakeLower | KeywordAttr_PreSorted) // built-in variables
		| KeywordAttr32(5, KeywordAttr_MakeLower | KeywordAttr_PreSorted) // keys
		| KeywordAttr32(6, KeywordAttr_MakeLower | KeywordAttr_PreSorted) // functions
		| KeywordAttr64(7, KeywordAttr_NoLexer) // misc
		, SCE_AHK_TASKMARKER,
		SCE_AHK_SECTION_SQ, SCE_AHK_FORMAT_SPECIFIER,
//Settings--Autogenerated -- end of section automatically generated
	EDITLEXER_HOLE(L"AutoHotkey Script", Styles_AHK),
	L"ahk; ia; scriptlet",
	&Keywords_AHK,
	Styles_AHK
};
