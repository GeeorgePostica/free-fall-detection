/*
 * Integration of FatFs filesystem module in Miosix by Terraneo Federico
 * based on original files diskio.c and mmc.c by ChaN
 */

#include "diskio.h"
#include "interfaces/disk.h"

using namespace miosix;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \internal
 * Initializes drive.
 */
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
    if(drv!=0) return STA_NOINIT;//Only drive 0 is supported
    if(Disk::isAvailable()==false) return STA_NODISK;
    Disk::init();
    if(Disk::isInitialized()) return RES_OK;
    else return STA_NOINIT;
}

/**
 * \internal
 * Return status of drive.
 */
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
    if(drv!=0) return STA_NOINIT;//Only drive 0 is supported
    if(Disk::isInitialized()) return RES_OK;
    else return STA_NOINIT;
}

/**
 * \internal
 * Read one or more sectors from drive
 */
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,           /* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
    if(drv!=0 || count==0) return RES_PARERR;
    if(Disk::isInitialized()==false) return RES_NOTRDY;
    if(Disk::read(buff,sector,count)) return RES_OK;
    else return RES_ERROR;
}

/**
 * \internal
 * Write one or more sectors to drive
 */
DRESULT disk_write (
	BYTE drv,		/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count		/* Number of sectors to write (1..255) */
)
{
    if(drv!=0 || count==0) return RES_PARERR;
    if(Disk::isInitialized()==false) return RES_NOTRDY;
    if(Disk::write(buff,sector,count)) return RES_OK;
    else return RES_ERROR;
}

/**
 * \internal
 * To perform disk functions other thar read/write
 */
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    if(drv!=0) return RES_PARERR;
    if(Disk::isInitialized()==false) return RES_NOTRDY;
    switch(ctrl)
    {
        case CTRL_SYNC:
            if(Disk::sync()) return RES_OK; else return RES_ERROR;
        case GET_SECTOR_COUNT:
            return RES_ERROR; //unimplemented, so f_mkfs() does not work
        case GET_BLOCK_SIZE:
            return RES_ERROR; //unimplemented, so f_mkfs() does not work
        default:
            return RES_PARERR;
    }
}

/**
 * \internal
 * Return current time, used to save file creation time
 */
DWORD get_fattime()
{
    return 0x210000;//TODO: this stub just returns date 01/01/1980 0.00.00
}

#ifdef __cplusplus
}
#endif
