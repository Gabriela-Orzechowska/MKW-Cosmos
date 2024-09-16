/* 
 * This file is part of the Cosmos
 * Copyright (c) 2023-2024 Gabriela Orzechowska
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _RIIVO_
#define _RIIVO_

#include <kamek.hpp>

#define RIIVOMAXPATH 1024
#define RIIVOSHORTPATH 32
#define MAXFILECOUNT 100

enum RiivoIOCtlType{
    RIIVO_IOCTL_INITDISC =	0X30,
	RIIVO_IOCTL_MOUNT,
	RIIVO_IOCTL_UNMOUNT,
	RIIVO_IOCTL_MOUNTPOINT,
	RIIVO_IOCTL_SETDEFAULT,
	RIIVO_IOCTL_SETLOGFS,
	RIIVO_IOCTL_GETLOGFS,
	RIIVO_IOCTL_EPOCH,
	RIIVO_IOCTL_STAT =		0X40,
	RIIVO_IOCTL_CREATEFILE,
	RIIVO_IOCTL_DELETE,
	RIIVO_IOCTL_RENAME,
	RIIVO_SEEK_TELL,
	RIIVO_SEEK_SYNC,
	RIIVO_IOCTL_CREATEDIR =	0X50,
	RIIVO_IOCTL_OPENDIR,
	RIIVO_IOCTL_NEXTDIR,
	RIIVO_IOCTL_CLOSEDIR,
	RIIVO_IOCTL_SHORTEN = 	 0X60,
	RIIVO_IOCTL_LOG =         0X61,
	RIIVO_IOCTL_CONTEXT =     0X62,
	RIIVO_IOCTL_CHECKPHYS,
	RIIVO_IOCTL_GETFREESPACE,
	RIIVO_IOCTL_SETSLOTLED,
};

enum RiivoMode{
    RIIVO_MODE_READ = 0x0,      //open for reading only
    RIIVO_MODE_WRITE = 0x1,     //open for writing only
    RIIVO_MODE_READWRITE = 0x2, //open for reading and writing
    RIIVO_MODE_APPEND = 0x8,    //writes done at eof
    RIIVO_MODE_CREATE = 0x100,  //create and open file
    RIIVO_MODE_TRUNC = 0x200,   //open and truncate
    RIIVO_MODE_EXCL = 0x400,    //open only if file doesn't already exist
    RIIVO_MODE_TEXT,
    RIIVO_MODE_BINARY,
    RIIVO_MODE_TEMPORARY,
    RIIVO_MODE_NOINHERIT,
    RIIVO_MODE_SEQUENTIAL,
    RIIVO_MODE_RANDOM,
};

enum RiivoError{
    RIIVO_ERROR_SUCCESS = 0,
    RIIVO_ERROR_UNRECOGNIZED = -128,
    RIIVO_ERROR_NOTMOUNTED = -129,
    RIIVO_ERROR_DISKNOTSTART = -144,
    RIIVO_ERROR_DISKNOTINSERTED = -145,
    RIIVO_ERROR_DISKNOTMOUNTED = -146,
    RIIVO_ERROR_OUTOFMEMORY = -65,
    RIIVO_ERROR_NOTOPENED = -64,
    RIIVO_ERROR_EPERM           =1,
    RIIVO_ERROR_ENOENT          =2,
    RIIVO_ERROR_ESRCH           =3,
    RIIVO_ERROR_EINTR           =4,
    RIIVO_ERROR_EIO             =5,
    RIIVO_ERROR_ENXIO           =6,
    RIIVO_ERROR_E2BIG           =7,
    RIIVO_ERROR_ENOEXEC         =8,
    RIIVO_ERROR_EBADF           =9,
    RIIVO_ERROR_ECHILD          =10,
    RIIVO_ERROR_EAGAIN          =11,
    RIIVO_ERROR_ENOMEM          =12,
    RIIVO_ERROR_EACCES          =13,
    RIIVO_ERROR_EFAULT          =14,
    RIIVO_ERROR_EBUSY           =16,
    RIIVO_ERROR_EEXIST          =17,
    RIIVO_ERROR_EXDEV           =18,
    RIIVO_ERROR_ENODEV          =19,
    RIIVO_ERROR_ENOTDIR         =20,
    RIIVO_ERROR_EISDIR          =21,
    RIIVO_ERROR_ENFILE          =23,
    RIIVO_ERROR_EMFILE          =24,
    RIIVO_ERROR_ENOTTY          =25,
    RIIVO_ERROR_EFBIG           =27,
    RIIVO_ERROR_ENOSPC          =28,
    RIIVO_ERROR_ESPIPE          =29,
    RIIVO_ERROR_EROFS           =30,
    RIIVO_ERROR_EMLINK          =31,
    RIIVO_ERROR_EPIPE           =32,
    RIIVO_ERROR_EDOM            =33,
    RIIVO_ERROR_EDEADLK         =36,
    RIIVO_ERROR_ENAMETOOLONG    =38,
    RIIVO_ERROR_ENOLCK          =39,
    RIIVO_ERROR_ENOSYS          =40,
    RIIVO_ERROR_ENOTEMPTY       =41
};

struct RiivoStats{
    RiivoStats(){
        Identifier = 0;
        Size = 0;
        Device = 0;
        Mode = 0;
    }
    u64 Identifier;
    u64 Size;
    s32 Device;
    s32 Mode;
};

enum RiivoStatsModes{
    S_IFDIR = 0x4000, 
	S_IFREG = 0x8000 
};


#endif
