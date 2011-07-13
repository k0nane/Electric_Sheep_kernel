/*
* Nestle filesystem platform Developed by Flash Software Group.
*
* Copyright 2006-2009 by Memory Division, Samsung Electronics, Inc.,
* San #16, Banwol-Dong, Hwasung-City, Gyeonggi-Do, Korea
*
* All rights reserved.
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information").  You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*/
/**
* @file		ns_api.h
* @author	ByungJune Song (byungjune.song@samsung.com)
* @author	InHwan Choi (inhwan.choi@samsung.com)
* @brief	Declarations for interfaces that are exported to the OS glue 
* This file contains declarations of interfaces used by OS glue.
*/
/**
* @mainpage		NESTLE Programmers Guide
* @brief		NESTLE Programmers Guide For OS glue Developers
* @version		nestle 1.0.0
* @since		2007
* @author		FS core nestle team
* @par Basic Rules

* \li BR1 : Interfaces for OS glue are defined by "ns_api.h". Do not include any other header files except for "ns_api.h"
* \li BR2 : OS glue can access directly nestle's structures.
* \li BR3 : Check configuration items at "ns_config.h". Refer to Nestle porting guide document.
*/

#ifndef __NS_API_H__
#define __NS_API_H__

#include "ns_types.h"
#include "ns_misc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 
* @defgroup nestle	NESTLE framework
* @{
*/	

/******************************************************************************/
/* Nestle Interface                                                           */
/******************************************************************************/
/** 
* @defgroup nestle_interface	Nestle Interface
* Nestle interface
* Nestle's APIs have 'Ns' prefix.
* @{
*/	

/******************************************************************************/
/* DIR PUBLIC API                                                             */
/******************************************************************************/
/** 
* @defgroup DirOps	Dir Operations
* @{
*/

#define	NsDirOpenDirectory			DirOpenDirectory
/**
 * @brief Open a directory and make a fcb
 * @param[in]	pwszPath	Path for a directory
 * @param[in]	pVcb		Volume control block
 * @param[out]	ppFcb		Reference of new fcb
 * @returns		FERROR_NO_ERROR is returned on success. Error number is returned on error
 */
FERROR
NsDirOpenDirectory(
	IN	const wchar_t*			pwszPath,
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IO	PFILE_CONTROL_BLOCK*	ppFcb);


#define	NsDirReadDirectory			DirReadDirectory
/**
 * @brief Read an opened directory
 * @param[in]	pFcb		Fcb of a direcory
 * @param[in]	pwszFileNameToSearch	Name or wide character to find
 * @param[out]	ppFcb		Reference of new fcb
 * @returns		FERROR_NO_ERROR is returned on success. Error number is returned on error
 */
FERROR
NsDirReadDirectory(
	IN	PFILE_CONTROL_BLOCK		pFcb,
	IN	const wchar_t*			pwszFileNameToSearch,
	OUT	PDIRECTORY_ENTRY		pEntry);


#define	NsDirReadDirUnlink			DirReadDirUnlink
/**
 * @brief Unlink all files (not a directory) in a directory
 * @param[in]	pFcb		File control block pointer
 * @param[in]	dwAttribute	Attribute to remove
 * @param[out]	pEntry		Entry Data
 * @returns		FERROR
 */
FERROR
NsDirReadDirUnlink(
	IN	PFILE_CONTROL_BLOCK	pFcb,
	IN	FILE_ATTR			dwAttribute,
	OUT	PDIRECTORY_ENTRY	pEntry);


#define	NsDirCleanDirectory			DirCleanDirectory
/**
 * @brief Unlink all the children in a directory
 * @param[in]	pVnode		Vnode pointer
 * @returns		FERROR
 */
FERROR
NsDirCleanDirectory(
	IN	PVNODE	pVnode);


#define	NsDirCreateDirectory		DirCreateDirectory
/**
 * @brief Creates a directory
 * @param[in]	pVcb			Mounted volume
 * @param[in]	pwszPathName	Path to create
 * @param[in]	dwAttr			Attributes for open
 * @returns		FERROR
 */
FERROR
NsDirCreateDirectory(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const wchar_t*			pwszPathName,
	IN	FILE_ATTR				dwAttr);


#define	NsDirRewindDirectory		DirRewindDirectory
/**
 * @brief Set 0 into directory's offset (NsDirReadDirectory() will read a first child)
 * @param[in]	pFcb	File contro block pointer
 * @returns		FERROR
 */
FERROR
NsDirRewindDirectory(
	IN PFILE_CONTROL_BLOCK	pFcb);


#define	NsDirCreateSymlink			DirCreateSymlink
/**
 * @brief Creates a symlink
 * @param[in]	pVcb	Volume control block pointer
 * @param[in]	pwszPathName	Path to create
 * @param[in]	pwszTargetPath	Target path
 * @param[in]	dwAttr			Attributes
 * @returns		FERROR
 */
FERROR
NsDirCreateSymlink(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const wchar_t*			pwszPathName,
	IN	const wchar_t*			pwszTargetPath,
	IN	FILE_ATTR				dwAttr);


#define NsDirReadSymlink			DirReadSymlink
/**
 * @brief NsDirReadSymlink read a symlink
 * @param[in]	pVcb			Volume control block pointer
 * @param[in]	pwszPath		Source Path
 * @param[out]	pwszLinkBuff	Target link path buffer
 * @param[in]	dwLinkBuffSize	Target link buffer size
 * @param[out]	dwLinkLength	Target path length
 * @returns		FERROR
 */
FERROR
NsDirReadSymlink(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	const wchar_t*			pwszPath,
	OUT	wchar_t*				pwszLinkBuff,
	IN	unsigned int			dwLinkBuffSize,
	OUT unsigned int*			pLinkLen);


#define	NsDirDelete					DirDelete
/**
 * @brief Deletes a directory or a file
 * @param[in]	pVcb		Volume control block pointer
 * @param[in]	pwszPath	Path to delete
 * @param[in]	bDirectory	directory or not
 * @returns		FERROR
 */
FERROR
NsDirDelete(
	IN	PVOLUME_CONTROL_BLOCK	pVcb, 
	IN	const wchar_t*			pwszPath,
	IN	BOOL					bDirectory);

#define	NsDirMove					DirMove

/**
 * @brief Moves a directory.
 * @param[in]	pVcb			Volume for source
 * @param[in]	pTargetVcb		Volume for target for checking cross-volumes rename
 * @param[in]	pwszSourcePath	Source path string
 * @param[in]	pwszTargetPath	Target path string
 * @returns		FERROR
 */
FERROR
NsDirMove(
	IN PVOLUME_CONTROL_BLOCK	pVcb, 
	IN PVOLUME_CONTROL_BLOCK	pTargetVcb,
	IN const wchar_t*			pwszSourcePath, 
	IN const wchar_t*			pwszTargetPath);

#define	NsDirLookupPath				DirLookupPath

/**
 * @brief Resolves full-path and finds a child node
 * @param[in] pwszPath			Path string pointer
 * @param[in] pVcb				volume pointer
 * @param[in] pBase				Parent node pointer to find
 * @param[out] ppVnode			Vnode to be made
 * @param[in] bSymlinkItself	Whether lookup a symlink or not
 * @returns				FERROR
 * @remarks				After this function run, NsVnodeRelease function should call
 */
FERROR
NsDirLookupPath(
	IN const wchar_t*			pwszPath,
	IN PVOLUME_CONTROL_BLOCK	pVcb,
	IN PVNODE					pBase,
	OUT PVNODE*					ppVnode,
	IN const BOOL				bSymlinkItself);


#define NsDirOpenParent		DirOpenParent
/**
 * @brief Open parent and return last symbol for FindFirstFile()
 * @param[in]	pwszPath	Full path
 * @param[in]	pVcb		Volume control block pointer
 * @param[out]	ppFcb		Reference for new fcb
 * @param[out]	ppwsTail	Pointer to last symbol in pwszPath to support search handle for WinCE
 * @param[out]	pcchTail	The length of last symbol
 * @returns		FERROR
 */
FERROR
NsDirOpenParent(
	IN	const wchar_t*			pwszPath,
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	OUT	PFILE_CONTROL_BLOCK*	ppFcb,
	OUT	wchar_t**				ppwsTail,
	OUT unsigned int*			pcchTail);


#define	NsDirSetXattr	DirSetXattr
/**
* @brief               Set extended attribute
* @param[in]   pVcb                   volume control block
* @param[in]   pwszPath               file path
* @param[in]   pwszName               extended attribute name
* @param[in]   pszValue               extended attribute value
* @param[in]   dwSize                 size of value
* @param[in]   dwFlag                 this param set extended attribute mode
* @param[in]   dwID                   this param set namespace of extended attribute
* @return              FERROR
*/

FERROR
NsDirSetXattr(
	IN	PVOLUME_CONTROL_BLOCK		pVcb, 
	IN	const wchar_t*				pwszPath,
	IN	const char*					pszName,
	IN	const char*					pszValue,
	IN	int							dwSize,
	IN	XATTR_SET_FLAG				dwFlag,
	IN	int							dwItSymlink);


#define	NsDirGetXattr	DirGetXattr
/**
* @brief               Get extended attribute value
* @param[in]   pVcb                   volume control block
* @param[in]   pwszPath               file path
* @param[in]   pwszName               extended attribute name
* @param[out]  pszValue               extended attribute value
* @param[in]   dwSize                 size of value
* @param[in]   dwID                   this param set namespace of extended attribute
* @return              FERROR
*/

FERROR
NsDirGetXattr(
	IN	PVOLUME_CONTROL_BLOCK		pVcb, 
	IN	const wchar_t*				pwszPath,
	IN	const char*					pszName,
	OUT	char*						pszValue,
	IN	int							dwSize,
	IN	int							dwItSymlink);

#define	NsDirListXattr	DirListXattr
/**
* @brief               Get extended attribute name list
* @param[in]   pVcb                   volume control block
* @param[in]   pwszPath               file path
* @param[out]  pszNameList            extended attribute name list
* @param[in]   dwNameListSize size of name list
* @return              FERROR
*/
FERROR
NsDirListXattr(
	IN	PVOLUME_CONTROL_BLOCK		pVcb,
	IN	const wchar_t*				pwszPath,
	OUT	char*						pszNameList,
	IN	int							dwNameListSize,
	IN	int							dwItSymlink);

#define	NsDirRemoveXattr	DirRemoveXattr
/**
* @brief               remove extended attribute entry
* @param[in]   pVcb                   volume control block
* @param[in]   pwszPath               file path
* @param[in]   pszName                extended attribute name
* @param[in]   dwID                   namespace id
* @return              FERROR
*/
FERROR
NsDirRemoveXattr(
	IN	PVOLUME_CONTROL_BLOCK		pVcb,
	IN	const wchar_t*				pwszPath,
	IN	char*						pszName,
	IN	int							dwItSymlink);

#define NsDirSetGuid	DirSetGuid
/** 
* @brief		set gid, uid and permission
* @param[in]	pVcb		nestle vcb
* @param[in]	pwszPath	file or directory path
* @param[in]	dwUID		user ID
* @param[in]	dwGID		group ID
* @param[in]	wPerm		file or directory's permission
* @returns		FERROR
*/

FERROR
NsDirSetGuid(
			 IN	PVOLUME_CONTROL_BLOCK		pVcb, 
			 IN	const wchar_t*				pwszPath,
			 IN	unsigned int				dwUID,
			 IN	unsigned int				dwGID,
			 IN	unsigned short				wPerm,
			 IN	int							dwItSymlink);

#define NsDirGetGuid	DirGetGuid
/** 
* @brief		get gid, uid and permission
* @param[in]	pVcb		nestle vcb
* @param[in]	pwszPath	file or directory path
* @param[out]	dwUID		user ID
* @param[out]	dwGID		group ID
* @param[out]	wPerm		file or directory's permission
* @returns		FERROR
*/
FERROR
NsDirGetGuid(
			 IN		PVOLUME_CONTROL_BLOCK		pVcb, 
			 IN		const wchar_t*				pwszPath,
			 OUT	unsigned int*				dwUID,
			 OUT	unsigned int*				dwGID,
			 OUT	unsigned short*				wPerm,
			 IN		int							dwItSymlink);

#if defined(CONFIG_WINCE)
#define	NsFileLockUnlockFile	FileLockUnlockFile
/**
* @brief		Lock or unlock a file
* @param[in]	pFcb			file control block
* @param[in]	dwCmd			set lock or get lock command
* @param[in]	pFlock			lock information (lock types, owner, flag, ..)
* @return		FERROR
*/
FERROR
NsFileLockUnlockFile(
					 IN PFILE_CONTROL_BLOCK			pFcb, 
					 IN unsigned int					dwCmd,
					 IO PFLOCK						pFlock);

#define NsDirOpenName		DirOpenName
/**
* @brief NsDirOpenName makes a name handle
* @param[in]	pwszPath	Path of directory
* @param[in]	pVcb		Volume control block pointer
* @param[out]	ppFcb		Reference for new fcb
* @param[out]	ppwsTail	Pointer to last symbol in pwszPath to support search handle for WinCE
* @param[out]	pcchTail	The length of last symbol
* @param[out]	pEntry		directory entry information
* @returns		FERROR
*/
FERROR
NsDirOpenName(
	IN	const wchar_t*			pwszPath,
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	OUT	PFILE_CONTROL_BLOCK*	ppFcb,
	OUT	wchar_t**				ppwsTail,
	OUT unsigned int*			pcchTail,
	OUT	PDIRECTORY_ENTRY		pEntry);
#endif /*defined(CONFIG_WINCE)*/

#if defined(CONFIG_SYMBIAN)
#define NsDirConvertName		DirConvertName

/**
* @brief NsDirConvertName
* @param[in]	pwszInputName		input path of entry name
* @param[in]	pVcb				Volume control block pointer
* @param[in]	pwszOutputName		output path of entry name
* @param[in]	dwOutputNameSize	path size
* @param[in]	dwConvertType		convert type (long -> short or short -> long)
* @returns		FERROR_NO_ERROR
*/
FERROR
NsDirConvertName(
	IN	const wchar_t*			pwszInputName,
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	OUT	wchar_t*				pwszOutputName,
	IN	unsigned int			dwOutputNameSize,
	IN	unsigned int			dwConvertType);

#endif /*defined(CONFIG_SYMBIAN)*/

#if defined(CONFIG_UNUSED)
#endif


/** @} */ // end of DirOps


/******************************************************************************/
/* VNODE PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup VnodeOps	Vnode Operations
* @{
*/

/**
* @brief		Is vnode dir?
* @param[in]	pVnode : nestle vnode
* @returns		BOOL : TRUE if it's dir, otherwise FALSE
*/
#define NsVnodeIsDir(pVnode)		((pVnode)->dwAttr & VNODE_ATTR_DIRECTORY)


#define NsVnodeInitialize	VnodeInitialize
/**
* @brief		Initialize vnode module (e.g. create global lock)
* @returns		FERROR	
*/
FERROR
NsVnodeInitialize(void);


#define NsVnodeUninitialize	VnodeUninitialize	
/**
* @brief		Uninitialize vnode module (e.g. destroy global lock)
* @returns		void	
*/
void
NsVnodeUninitialize(void);


#define NsVnodeGetVcb	VnodeGetVcb

/**
 * @brief		Get vcb from vnode
 * @param[in]	pVnode vnode
 * @returns		vcb
 */
PVOLUME_CONTROL_BLOCK
NsVnodeGetVcb(
	IN	PVNODE	pVnode);


#define NsVnodeGetNative	VnodeGetNative
/**
* @brief		Get native node from vnode
* @param[in]	pVnode	vnode
* @returns		void*	native node pointer
*/
void*
NsVnodeGetNative(
	IN	PVNODE	pVnode);


#define NsVnodeGetIndex	VnodeGetIndex
/**
* @brief		Get vnode's index
* @param[in]	pVnode		vnode
* @returns		unique vnode's index
*/
unsigned long long
NsVnodeGetIndex(
	IN	PVNODE	pVnode);


#define	NsVnodeRelease	VnodeRelease
/**
* @brief		Release vnode
* @param[in]	pVnode		vnode
* @returns		void
*/
void
NsVnodeRelease(
	IN	PVNODE	pVnode);

/** @} */ // end of VnodeOps


/******************************************************************************/
/* FCB PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup FCBOps	FCB Operations
* @{
*/

#define NsFcbGetPrivate(pFcb)			((pFcb)->pPrivate)
#define NsFcbSetPrivate(pFcb, pData)	((pFcb)->pPrivate = (pData))

#define NsFcbCheckVol(pFcb)				((pFcb)->dwAccessMode == FCB_ACCESS_VOL)

#ifdef CONFIG_RTOS
	#define NsFcbGetIndex(pFcb)			(pFcb)->wIndex
	#define NsFcbGetChecksum(pFcb)		(pFcb)->wChecksum

	#define NsFcbGetFcbWithIndex		FcbGetFcbWithIndex
	/**
	* @brief		get Fcb prt
	* @param[in]	wIndex	FCB index in Fcb Pool
	* @returns		Fcb
	*/
	PFILE_CONTROL_BLOCK
	NsFcbGetFcbWithIndex(
		IN	unsigned short	wIndex);
#endif /*CONFIG_RTOS*/

#define	NsFcbInitialize			FcbInitialize
/**
* @brief		Initialize fcb module
* @returns		FERROR
*/
FERROR
NsFcbInitialize(void);


#define	NsFcbUninitialize		FcbUninitialize
/**
* @brief		Uninitialize fcb module
* @returns		void
*/
void
NsFcbUninitialize(void);


#define	NsFcbGetVnode			FcbGetVnode
/**
* @brief		Get vnode from fcb
* @param[in]	pFcb	fcb
* @returns		Related vnode with pFcb
*/
PVNODE
NsFcbGetVnode(
	IN	PFILE_CONTROL_BLOCK	pFcb);


#define	NsFcbCheckAccessMode	FcbCheckAccessMode
/**
 * @brief		Checks the RW access mode
 * @param[in]	pFcb		Open file
 * @param[in]	bRead		Whether check read mode or not
 * @returns		If access is allowed, return FERROR_NO_ERROR. Otherwise, non-zero.
 */
FERROR
NsFcbCheckAccessMode(
	IN PFILE_CONTROL_BLOCK	pFcb,
	IN BOOL					bRead);


#define	NsFcbGetFcb				FcbGetFcb
/**
* @brief		Increase fcb's reference count
* @param[in]	pFcb	referenced fcb
* @returns		FERROR
*/
FERROR
NsFcbGetFcb(
	IN	PFILE_CONTROL_BLOCK	pFcb);


#define	NsFcbPutFcb				FcbPutFcb
/**
* @brief		Release fcb's reference
* @param[in]	pFcb	pointer to the file control block
* @returns		void
*/
void
NsFcbPutFcb(
	IN	PFILE_CONTROL_BLOCK	pFcb);


#define	NsFcbGetOffset			FcbGetOffset
/**
 * @brief		Get file offset
 * @param[in]	pFcb	pointer to the file control block
 * @returns		current offset
 */
FILE_OFFSET
NsFcbGetOffset(
	IN	PFILE_CONTROL_BLOCK	pFcb);

/** @} */ // end of FCBOps


/******************************************************************************/
/* FILE PUBLIC API                                                            */
/******************************************************************************/
/** 
* @defgroup FileOps	File Operations
* @{
*/

#define	NsFileCreateFile	FileCreateFile
/**
* @brief			Create a File
* @param[in]		pVcb				mounted volume
* @param[in]		pwszPath			name string (not include mount-point(volume name))
* @param[in]		dwAttr				file attribute
* @param[in]		dwAccessMode		file access mode
* @param[in]		dwCreationMode		file creation mode
* @param[out]		pdwCreationResult	notice for creation and truncation
* @param[out]		ppFcb				file handle
 * @return			FERROR
 */
FERROR
NsFileCreateFile(
	IN	PVOLUME_CONTROL_BLOCK		pVcb, 
	IN	const wchar_t*				pwszPath, 
	IN	FILE_ATTR					dwAttr,
	IN	FILE_ACCESS_TYPES			dwAccessMode,
	IN	CREATION_MODE				dwCreationMode,
	OUT	CREATION_MODE*				pdwCreationResult,
	OUT	PFILE_CONTROL_BLOCK*		ppFcb);


#define	NsFileReadFile	FileReadFile
/**
 * @brief			Read a File
 * @param[in]		pFcb			file handle to be read
 * @param[in,out]	pBuffer			user buffer 
 * @param[in]		dwBytesToRead	bytes to request
 * @param[out]		pdwBytesRead	bytes to read
 * @return			FERROR
 */
FERROR
NsFileReadFile(
	IN	PFILE_CONTROL_BLOCK			pFcb, 
	OUT	unsigned char*				pBuffer, 
	IN	unsigned int				dwBytesToRead, 
	OUT unsigned int*				pdwBytesRead);


#define	NsFileWriteFile	FileWriteFile
/**
 * @brief			Write a File
 * @param[in,out]	pFcb			file control block	
 * @param[out]		pBuffer			buffer pointer
 * @param[in]		dwBytesToWrite	bytes to request
 * @param[out]		pdwBytesWritten	bytes to write
 * @return			FERROR
 */
FERROR
NsFileWriteFile(
	IO PFILE_CONTROL_BLOCK			pFcb, 
	IN unsigned char*				pBuffer, 
	IN unsigned int					dwBytesToWrite, 
	OUT unsigned int*				pdwBytesWritten);


#define	NsFileGetFileSize 	FileGetFileSize 
/**
 * @brief		Get file size
 * @param[in]	pVnode		vnode pointer
 * @param[out]	pdwSizeLow	low-quater-bytes of the file size
 * @param[out]	pdwSizeHigh	high-quater-bytes of the file size
 * @return		FERROR
 */
void
NsFileGetFileSize (
	IN PVNODE						pVnode, 
	OUT unsigned int*				pdwSizeLow, 
	OUT unsigned int*				pdwSizeHigh);


#define	NsFileSeekFile	FileSeekFile
/**
 * @brief			Move file offset
 * @param[in,out]	pFcb			file control block	
 * @param[in]		dwMethod		whence
 * @param[in]		llDistance		8byte distance to move
 * @param[out]		pdwOffsetHigh	4byte current high offset after the file-seek 
 * @param[out]		pdwOffsetLow	4byte current low offset after the file-seek 
 * @return			FERROR
 */
FERROR
NsFileSeekFile(
	IO PFILE_CONTROL_BLOCK			pFcb, 
	IN SEEKFILE_METHOD				dwMethod, 
	IN long long					llDistance, 
	OUT unsigned int*				pdwOffsetLow, 
	OUT unsigned int*				pdwOffsetHigh);


#define	NsFileSetFileSize	FileSetFileSize
/**
 * @brief		Truncate a file (shrink or expand file's space)
 * @param[in]	pFcb		Fcb for fruncate(fcb). NULL for truncate(path)
 * @param[in]	pVnode		vnode pointer
 * @param[in]	llSize		new file size
 * @return		FERROR
 */
FERROR
NsFileSetFileSize(
	IN PFILE_CONTROL_BLOCK			pFcb, 
	IN PVNODE						pVnode,
	IN FILE_SIZE					llSize);


#define	NsFileSyncFile	FileSyncFile
/**
 * @brief		Sync a file
 * @param[in]	pVnode			pVnode
 * @return		FERROR
 */
FERROR
NsFileSyncFile(
	IN PVNODE						pVnode);


#define	NsFileGetAttributes	FileGetAttributes
/**
 * @brief		Get the file attributes
 * @param[in]	pVnode				pVnode
 * @param[out]	pdwFileAttributes	file attributes
 * @return		FERROR
 */
FERROR
NsFileGetAttributes(
	IN		PVNODE					pVnode,
	OUT	FILE_ATTR*					pdwFileAttributes);


#define	NsFileSetAttributes	FileSetAttributes
/**
 * @brief		Set the file attributes
 * @param[in]	pVnode			pVnode
 * @param[in]	dwAttribute		file attributes
 * @return		FERROR
 */
FERROR
NsFileSetAttributes(
	IN	PVNODE						pVnode, 
	IN	FILE_ATTR					dwFileAttributes);


#define	NsFileIoControl	FileIoControl
/**
 * @brief		IO control
 * @param[in]	pFcb			file control block
 * @param[in]	dwControlCode	Iocontrol code
 * @param[in]	pInBuf			Input Buffer pointer
 * @param[in]	dwInBufSize		Input Buffer size
 * @param[out]	pOutBuf			Out buffer pointer
 * @param[in]	dwOutBufSize	Out buffer size
 * @return		FERROR
 */
FERROR
NsFileIoControl(
	IN PFILE_CONTROL_BLOCK			pFcb, 
	IN unsigned int					dwControlCode,
	IN void*						pInBuf,
	IN unsigned int					dwInBufSize,
	OUT void*						pOutBuf,
	IN unsigned int					dwOutBufSize);


#define	NsFileGetFileTime	FileGetFileTime
/**
 * @brief		Get time related to a file
 * @param[in]	pVnode			pVnode
 * @param[out]	pCreated		creation time
 * @param[out]	pLastAccessed	access time
 * @param[out]	pLastModified	modification time
 * @param[out]	pftCreated		(WinCE use only) creation time
 * @param[out]	pftLastAccessed	(WinCE use only) access time
 * @param[out]	pftLastModified	(WinCE use only) modification time
 * @return		void
 */
void
NsFileGetFileTime(
	IN PVNODE						pVnode, 
	OUT PSYS_TIME					pCreated,
	OUT PSYS_TIME					pLastAccess,
	OUT PSYS_TIME					pLastModified,
	IN ...);


#define	NsFileSetFileTime	FileSetFileTime
/**
 * @brief		Set time to the file
 * @param[in]	pVnode			pVnode
 * @param[in]	pCreated		creation time
 * @param[in]	pLastAccessed	access time
 * @param[in]	pLastModified	modification time
 * @param[in]	pftCreated		(WinCE use only) creation time
 * @param[in]	pftLastAccessed	(WinCE use only) access time
 * @param[in]	pftLastModified	(WinCE use only) modification time
 * @return		FERROR
 */
FERROR
NsFileSetFileTime(
	IN PVNODE						pVnode, 
	IN PSYS_TIME					pCreated,
	IN PSYS_TIME					pLastAccess,
	IN PSYS_TIME					pLastModified,
	IN ...);


#define	NsFileClose	FileClose
/**
* @brief			Close a File
* @param[in]		pFcb	file handle to be close
* @return			FERROR
*
*/
FERROR
NsFileClose(
	IN PFILE_CONTROL_BLOCK			pFcb);


#define	NsFileGetFileIndex	FileGetFileIndex
/**
* @brief		Get Vnode Index (ID)
* @param[in]	pVnode			vnode pointer
* @param[out]	pdwIndexLow		low-quater-bytes of the vnode index which is manadory
* @param[out]	pdwIndexHigh	high-quater-bytes of the vnode index which is optional
* @return		void
*/
void
NsFileGetFileIndex (
	IN PVNODE				pVnode, 
	OUT unsigned int*		pdwIndexLow, 
	OUT unsigned int*		pdwIndexHigh);


#if defined(CONFIG_SYMBIAN)

#define NsFileMapBlocks	FileMapBlocks
/**
* @brief			Get mapblock of free cluster
* @param[in,out]	pVnode			virtual node 
* @param[in]		dwBlockIndex	start number
* @param[in]		dwBlockCnt		total required size block count
* @param[out]		pdwBlockNum		allocated block number
* @param[out]		pdwContBlockCnt	contiguous allocated block count
* @return			FERROR
* @return			FERROR_NOT_A_FILE
* @return			FERROR_NOT_SUPPORTED
*/
FERROR
NsFileMapBlocks(
	IO	PVNODE				pVnode, 
	IN	unsigned int		dwBlockIndex,
	IN	unsigned int		dwBlockCnt,
	OUT unsigned int*		pdwBlockNum,
	OUT unsigned int*		pdwContBlockCnt);

#endif /*defined(CONFIG_SYMBIAN)*/

/** @} */ // end of FileOps


/******************************************************************************/
/* VCB_LIST PUBLIC API                                                        */
/******************************************************************************/
/** 
* @defgroup VCBListOps	VCB List Operations
* @{
*/

#define	NsVlmInitialize			VlmInitialize
/**
 * @brief		Initialize Vcb list
 * @returns		FERROR
 */
FERROR
NsVlmInitialize(void);


#define	NsVlmUninitialize		VlmUninitialize
/**
 * @brief		Uninitialize Vcb list
 * @returns		void
 */
void
NsVlmUninitialize(void);


#define	NsVlmSyncAll			VlmSyncAll
/**
* @brief		Sync all volumes in vcb list
* @param[in]	dwVcbFlags		target vcb's flag to sync 
* @return		FERROR
*/
FERROR
NsVlmSyncAll(MOUNT_FLAG	dwVcbFlags);


#define	NsVlmCloseAll			VlmCloseAll
/**
* @brief		Close all fcbs for each volume in vcb list
* @return		FERROR
*/
FERROR
NsVlmCloseAll(void);

#if defined(CONFIG_SYMBIAN)
#define	NsVlmUnmountAll			VlmUnmountAll
/**
* @brief		Unmount all volumes in vcb list
*/
FERROR
NsVlmUnmountAll(void);
#endif /*defined(CONFIG_SYMBIAN)*/

#define	NsVlmPrepareUnmount		VlmPrepareUnmount
/**
 * @brief		Check whether umount is possible
 * @param[in]	pwszMountPoint		access path to mounted volume
 * @param[in]	dwUnmountFlags		unmount flag (refer to UNMOUNT_FLAG)
 * @param[out]	ppVcb				double pointer of Vcb matched by MountPoint
 * @returns		
 *		- FERROR_NO_ERROR : All right to unmount
 *		- FERROR_NOT_MOUNTED : Volume is not mounted
 *		- FERROR_BUSY : Volume has open files						
 */
FERROR
NsVlmPrepareUnmount(
	IN	const wchar_t*			pwszMountPoint,
	IN	UNMOUNT_FLAG			dwUnmountFlags,
	OUT	PVOLUME_CONTROL_BLOCK*	ppVcb);


#define	NsVlmGetActiveVcb		VlmGetActiveVcb
/**
* @brief		Get active VCB
* @returns		PVOLUME_CONTROL_BLOCK
*/
PVOLUME_CONTROL_BLOCK
NsVlmGetActiveVcb(void);


#define NsVlmFindVcb			VlmFindVcb
/**
* @brief			Search the mounted volume
* @param[in,out]	ppwszPath		access path to mounted volume
* @param[out]		ppVcb			double pointer of Vcb matched by MountPoint
* @returns			return FERROR_NO_ERROR when vcb exits. Otherwise return FERROR_NOT_MOUNTED
*/
FERROR
NsVlmFindVcb(
	IN	const wchar_t**			ppwszPath,
	OUT	PVOLUME_CONTROL_BLOCK*	ppVcb);


#define NsVlmGetVcbNodeCount		VlmGetVcbNodeCount
/**
 * @brief			Fill FS_VOL_INFO and copy to user buffer 
 * @param[in,out]	pBuf		buffer to fill
 * @returns			FERROR_NO_ERROR
 */
FERROR
NsVlmGetVcbNodeCount(
	IO	unsigned char*			pBuf);

/** @} */ // end of VCBListOps


/******************************************************************************/
/* VOLUME PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup VolumeOps	Volume Operations
* @{
*/

#define NsVolMountVolume	VolMountVolume
/**
 * @brief		Mount a volume
 * @param[in]	pLogDisk		    logical disk to mount
 * @param[in]	pwszMountPoint		access path to mounted volume
 * @param[in]	pwszFileSystemName	native file system to mount
 * @param[in]	dwMountFlags		mount flags (refer to MOUNT_FLAG)
 * @return		On success, FEEROR_NO_ERROR is returned. On error, error code is returned
 */
FERROR
NsVolMountVolume(
	IN	PLOGICAL_DISK			pLogDisk,
	IN	const wchar_t*			pwszMountPoint,
	IN	const wchar_t*			pwszFileSystemName,
	IN	MOUNT_FLAG				dwMountFlags);


#define NsVolUnmountVolume	VolUmountVolume
/**
 * @brief		Unmount a volume
 * @param[in]	pVcb			mounted volume   
 * @param[in]	dwUnmountFlags  unmount flags
 * @return		On success, FEEROR_NO_ERROR is returned. On error, error code is returned
 */
FERROR
NsVolUnmountVolume(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	UNMOUNT_FLAG			dwUnmountFlags);


#define NsVolGetVolumeInformation	VolGetVolumeInformation
/**
 * @brief		Get a volume information
 * @param[in]	pVcb				mounted volume   
 * @param[out]	pVolumeInformation	volume info structure pointer to fill
 * @return		On success, FEEROR_NO_ERROR is returned. On error, error code is returned
 */
FERROR
NsVolGetVolumeInformation(
	IN PVOLUME_CONTROL_BLOCK	pVcb, 
	OUT PVOLUME_INFORMATION		pVolumeInformation);


#define NsVolSyncVolume	VolSyncVolume
FERROR
NsVolSyncVolume(
	IN PVOLUME_CONTROL_BLOCK	pVcb);


#define NsVolIoControl	VolIoControl
/**
 * @brief		Interface to FsIoCtl
 * @param[in]	pVcb			mounted volume  
 * @param[in]	dwControlCode	file system control command
 * @param[in]	pInBuf			input buffer pointer for FsIoCtl
 * @param[in]	dwInBufSize		input buffer size for FsIoCtl
 * @param[out]	pOutBuf			output buffer pointer for FsIoCtl
 * @param[in]	dwOutBufSize	output buffer size for FsIoCtl
 * @return		On success, FEEROR_NO_ERROR is returned. On error, error code is returned
 */
FERROR
NsVolIoControl(
	IN	PVOLUME_CONTROL_BLOCK	pVcb,
	IN	unsigned int			dwControlCode,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	IN	unsigned int			dwOutBufSize);

/** @} */ // end of VolumeOps


/******************************************************************************/
/* VCB PUBLIC API                                                             */
/******************************************************************************/
/** 
* @defgroup VCBOps	VCB Operations
* @{
*/

/**
* @brief		check device filesystem
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @returns		TRUE	This volume is mounted by device filesystem
* @returns		FALSE	This volume is not mounted by device filesystem
*/
#define NsVcbIsForDevfs(pVcb)			((pVcb)->dwFlag & MOUNT_NO_DEVICE)

/**
* @brief		check whether a volume has readonly attribute or not
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @returns		TRUE	This volume has readonly attribute
* @returns		FALSE	This volume don't have readonly attribute
*/
#define NsVcbIsReadOnlyVolume(pVcb)		((pVcb)->dwFlag & MOUNT_READ_ONLY)

/**
* @brief		get volume name
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @returns		wchar_t*	volume name
*/
#define NsVcbGetName(pVcb)				((pVcb)->wszName)

/**
* @brief		get private struct pointer
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @returns		void*	private struct pointer 
*/
#define NsVcbGetPrivate(pVcb)			((pVcb)->pPrivate)

/**
* @brief		set private struct pointer
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @param[in]	pData	private struct pointer (void*)
* @returns		NULL
*/
#define NsVcbSetPrivate(pVcb, pData)	((pVcb)->pPrivate = (pData))

/**
* @brief		get native filesystem pointer
* @param[in]	pVcb	mounted volume (PVOLUME_CONTROL_BLOCK)
* @returns		PNATIVEFS_OPERATIONS	native filesytem pointer
*/
#define NsVcbGetNative(pVcb)			((pVcb)->pNativeFSOps)

#define NsVcbInitialize			VcbInitialize
/**
* @brief		Initialize vcb module
* @returns		FERROR_NO_ERROR 
*/
FERROR
NsVcbInitialize(void);


#define NsVcbUninitialize		VcbUninitialize
/**
* @brief		Uninitialize vcb module
* @returns		void
*/
void
NsVcbUninitialize(void);


#define NsVcbGetRoot			VcbGetRoot
/**
* @brief		Get a root vnode from vcb
* @param[in]	pVcb	mounted volume
* @returns		PVNODE	root vnode
*/
PVNODE
NsVcbGetRoot(
	IN	PVOLUME_CONTROL_BLOCK	pVcb);


#define NsVcbGetLogicalDisk		VcbGetLogicalDisk
/**
 * @brief		Get a logical disk from vcb
 * @param[in]	pVcb			mounted volume
 * @returns		PLOGICAL_DISK	logical disk where volume mounts
 */
PLOGICAL_DISK
NsVcbGetLogicalDisk(
	IN	PVOLUME_CONTROL_BLOCK	pVcb);

/** @} */ // end of VCBOps


/******************************************************************************/
/* PCACHE PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup PCacheOps	Path cache Operations
* @{
*/

#define NsPchInitialize		PchInitialize
/**
* @brief		Create a path cache
* @return		FERROR
*/
FERROR
NsPchInitialize(void);


#define NsPchUninitialize	PchUninitialize
/**
* @brief		Destroy a path cache
* @return		void
*/
void
NsPchUninitialize(void);

/** @} */ // end of PCacheOps


/******************************************************************************/
/* NATIVES PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup NativeOps	Native Operations
* @{
*/

#define NsNtvInitialize		NtvInitialize
/**
 * @brief		Initialize native manager
 * @returns		FERROR			
 */
FERROR 
NsNtvInitialize(void);


#define NsNtvUninitialize	NtvUninitialize
/**
 * @brief		Uninitialize native manager
 * @returns		FERROR		
 */
FERROR
NsNtvUninitialize(void);


#define NsNtvGetNativeFileSystemCount	NtvGetNativeFileSystemCount
/**
 * @brief		Get the number of native file systems.
 * @returns		unsigned int		the number of native file system
 */
unsigned int
NsNtvGetNativeFileSystemCount(void);


#define NsNtvGetNativeFileSystemByIndex	NtvGetNativeFileSystemByIndex
/**
 * @brief		Get a suitable native file system by Index.
 * @param[in]	dwIndex	Native file system  index
 * @returns		Native		filesystem pointer
 */
PNATIVEFS_OPERATIONS
NsNtvGetNativeFileSystemByIndex(
	IN unsigned int dwIndex);


#define NsNtvGetNativeFileSystem	NtvGetNativeFileSystem
/**
* @brief		Get a suitable native file system by name.
* @param[in]	pwszFsName	Native file system  name
* @returns		Native		filesystem pointer
* @see			VolMountVolume, UtlExeUtility
*/
PNATIVEFS_OPERATIONS
NsNtvGetNativeFileSystem(
	IN const wchar_t* pwszFsName);


#define NsNtvFsControl				NtvFsControl
/**
* @brief		Interface to FsCtl
* @param[in]	pNativeFS		native filesystem 
* @param[in]	dwControlCode	File system control command
* @param[in]	pInBuf			input buffer pointer for FsIoCtl
* @param[in]	dwInBufSize		input buffer size for FsIoCtl
* @param[out]	pOutBuf			output buffer pointer for FsIoCtl
* @param[in]	dwOutBufSize	output buffer size for FsIoCtl
* @return	
*		- FERROR_NOT_SUPPORTED
*		- Native FsCtl errors
*/
FERROR
NsNtvFsControl(
	IN	PNATIVEFS_OPERATIONS	pNativeFS,
	IN	unsigned int			dwControlCode,
	IN	void*					pInBuf,
	IN	unsigned int			dwInBufSize,
	OUT	void*					pOutBuf,
	IN	unsigned int			dwOutBufSize);
/** @} */ // end of NativeOps


/******************************************************************************/
/* UTIL PUBLIC API                                                            */
/******************************************************************************/
/** 
* @defgroup UtilOps	Utility Operations
* @{
*/

#define  NsUtlExeUtility		UtlExeUtility
/**
 * @brief		Executes utility like format, scan, etc.
 * @param[in]	pLogDisk	Logical disk pointer
 * @param[in]	dwControlCode	io control code
 * @param[in]	pwszFileSystemName	File system name (string)
 * @param[in]	pInBuf		Input buffer pointer
 * @param[in]	dwInBufSize	Input buffer size
 * @returns		FERROR
 */
FERROR
NsUtlExeUtility(
	IN	PLOGICAL_DISK	pLogDisk, 
	IN	unsigned int	dwControlCode,
	IN	const wchar_t*	pwszFileSystemName,
	IN	void*			pInBuf,
	IN	unsigned int	dwInBufSize);


#define  NsUtlInitReservedName		UtiReservedName
/**
* @brief		Notify the set of reserved names of nestle
* @param[in]	pwsReservedNameSet	the set of reserved names encoded by unicode
* @returns		FERROR
*/
void 
NsUtlInitReservedName(
	IN const wchar_t		(*psReservedNameSet)[RSVD_NAME_LEN]);


/** @} */ // end of NativeOps


/******************************************************************************/
/* BCACHE PUBLIC API                                                          */
/******************************************************************************/
/** 
* @defgroup BCacheOps	Buffer cache Operations
* @{
*/

#define  NsBcmInitialize		BcmInitialize
/**
* @brief		Initialize a system buffer cache
* @param[in]	dwEntrySize		Buffer cache entry size, Set Optimal I/O size of Block device.
* @returns		On success, FERROR_NO_ERROR is returned. On error, error code is returned
*/
FERROR
NsBcmInitialize(
	IN unsigned int		dwEntrySize);


#define  NsBcmUninitialize		BcmUninitialize
/**
* @brief		Uninitialize a system buffer cache
* @returns		On success, FERROR_NO_ERROR is returned. On error, error code is returned
*/
FERROR
NsBcmUninitialize(void);

/** @} */ // end of BCacheOps

/** @} */ // end of nestle_interface

/** @} */ // end of nestle

#ifdef __cplusplus
}
#endif
#endif // __NS_API_H__
