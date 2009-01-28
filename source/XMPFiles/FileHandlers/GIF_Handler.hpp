#ifndef __GIF_Handler_hpp__
#define __GIF_Handler_hpp__	1

// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2002-2007 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// Derived from PNG_Handler.hpp by Ian Jacobi
// =================================================================================================

#include "GIF_Support.hpp"

// =================================================================================================
/// \file GIF_Handler.hpp
/// \brief File format handler for GIF.
///
/// This header ...
///
// =================================================================================================

// *** Could derive from Basic_Handler - buffer file tail in a temp file.

extern XMPFileHandler* GIF_MetaHandlerCTor ( XMPFiles* parent );

extern bool GIF_CheckFormat ( XMP_FileFormat format,
							   XMP_StringPtr  filePath,
                               LFA_FileRef    fileRef,
                               XMPFiles*     parent );

static const XMP_OptionBits kGIF_HandlerFlags = ( kXMPFiles_CanInjectXMP | 
												  kXMPFiles_CanExpand | 
												  kXMPFiles_PrefersInPlace | 
												  kXMPFiles_AllowsOnlyXMP | 
												  kXMPFiles_ReturnsRawPacket |
												  kXMPFiles_NeedsReadOnlyPacket
													);

class GIF_MetaHandler : public XMPFileHandler
{
public:

	void CacheFileData();
	void ProcessTNail();
	void ProcessXMP();
	
	void UpdateFile ( bool doSafeUpdate );
    void WriteFile  ( LFA_FileRef sourceRef, const std::string& sourcePath );

	bool SafeWriteFile ();

	GIF_MetaHandler ( XMPFiles* parent );
	virtual ~GIF_MetaHandler();

};	// GIF_MetaHandler

// =================================================================================================

#endif /* __GIF_Handler_hpp__ */
