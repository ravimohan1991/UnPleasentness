/*=============================================================================
	ANavigationPoint.h: Class functions residing in the ANavigationPoint class.
	Copyright 1997-1999 Epic Games, Inc. All Rights Reserved.
=============================================================================*/

	// Constructors.
#if OLDUNREAL_BINARY_COMPAT
	NO_DEFAULT_CONSTRUCTOR(ANavigationPoint)
#else
	ANavigationPoint() {}
#endif

	// ANavigationPoint interface.
	virtual void Destroyed(); //TODO: Move to AActor interface

	void SortPaths();
	void FreePaths();
	INT CompactSortPathList( INT* Paths);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
