/*=============================================================================
	UnCorObj.h: Standard core object definitions.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack(push,OBJECT_ALIGNMENT)
#endif

/*-----------------------------------------------------------------------------
	UPackage.
-----------------------------------------------------------------------------*/

//
// A package.
//
class CORE_API UPackage : public UObject
{
	DECLARE_CLASS(UPackage,UObject,0,Core)

	// Variables.
	void* DllHandle;
	UBOOL AttemptedBind;
	DWORD PackageFlags;
	FString LastLoadedFrom;

	// Constructors.
	UPackage();

	// UObject interface.
	void Destroy();
	void Serialize( FArchive& Ar );

	// UPackage interface.
	void* GetDllExport( const TCHAR* ExportName, UBOOL Checked );
};

/*-----------------------------------------------------------------------------
	USubsystem.
-----------------------------------------------------------------------------*/

//
// A subsystem.
//
class CORE_API USubsystem : public UObject, public FExec
{
	DECLARE_ABSTRACT_CLASS(USubsystem,UObject,CLASS_Transient,Core)
	NO_DEFAULT_CONSTRUCTOR(USubsystem)

	// USubsystem interface.
	virtual void Tick( FLOAT DeltaTime )
	{}
};

/*-----------------------------------------------------------------------------
	UCommandlet.
-----------------------------------------------------------------------------*/

//
// A command-line applet.
//
struct UCommandlet_eventMain_Parms
{
	FString InParms;
	INT ReturnValue;
};
class CORE_API UCommandlet : public UObject
{
	DECLARE_CLASS(UCommandlet,UObject,CLASS_Transient|CLASS_Abstract|CLASS_Localized,Core)
	FString HelpCmd, HelpOneLiner, HelpUsage, HelpWebLink;
	FStringNoInit HelpParm[16], HelpDesc[16];
	UCommandlet();
	BITFIELD LogToStdout   :1;
	BITFIELD IsServer      :1;
	BITFIELD IsClient      :1;
	BITFIELD IsEditor      :1;
	BITFIELD LazyLoad      :1;
	BITFIELD ShowErrorCount:1;
	BITFIELD ShowBanner    :1;

	virtual INT Main( const TCHAR* Parms );
	DECLARE_FUNCTION(execMain)
    INT eventMain(const FString& InParms)
    {
		UCommandlet_eventMain_Parms Parms;
        Parms.InParms=InParms;
        ProcessEvent(FindFunctionChecked(NAME_Main),&Parms);
		return Parms.ReturnValue;
    }
};

/*-----------------------------------------------------------------------------
	ULanguage.
-----------------------------------------------------------------------------*/

//
// A language (special case placeholder class).
//
class CORE_API ULanguage : public UObject
{
	DECLARE_ABSTRACT_CLASS(ULanguage,UObject,CLASS_Transient,Core)
	NO_DEFAULT_CONSTRUCTOR(ULanguage)
	ULanguage* SuperLanguage;
};

/*-----------------------------------------------------------------------------
	UTextBuffer.
-----------------------------------------------------------------------------*/

//
// An object that holds a bunch of text.  The text is contiguous and, if
// of nonzero length, is terminated by a NULL at the very last position.
//
class CORE_API UTextBuffer : public UObject, public FOutputDevice
{
	DECLARE_CLASS(UTextBuffer,UObject,0,Core)

	// Variables.
	INT Pos, Top;
	FString Text;

	// Constructors.
	UTextBuffer( const TCHAR* Str=TEXT("") );

	// UObject interface.
	void Serialize( FArchive& Ar );

	// FOutputDevice interface.
	void Serialize( const TCHAR* Data, EName Event );

	// OldUnreal: returns TRUE if the Text buffer is either empty or only contains whitespace
	UBOOL IsSourceStripped();
};

/*----------------------------------------------------------------------------
	USystem.
----------------------------------------------------------------------------*/

class CORE_API USystem : public USubsystem
{
	DECLARE_CLASS(USystem,USubsystem,CLASS_Config,Core)

	// Variables.
	//
	// a licensee configurable INI setting that can be used to augment
	// the behavior of LicenseeVer -- especially useful in supporting
	// the transition from "before LiceseeVer" to the new build; for
	// salvaging maps that have licensee-specific data.
	//
	INT LicenseeMode; 
	INT PurgeCacheDays;
	FStringNoInit SavePath;
	FStringNoInit CachePath;
	FStringNoInit CacheExt;
	TArrayNoInit<FString> LangPaths;
	TArrayNoInit<FString> Paths;
	TArrayNoInit<FName> Suppress;

	// Constructors.
	void StaticConstructor();
	USystem();

	// FExec interface.
	UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar=*GLog );
};

#if ((_MSC_VER) || (HAVE_PRAGMA_PACK))
#pragma pack (pop)
#endif

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
