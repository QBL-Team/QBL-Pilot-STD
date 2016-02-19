/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
#include "SD.h" /* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
#define ATA 0 /* Example: Map ATA harddisk to physical drive 0 */
#define MMC 1 /* Example: Map MMC/SD card to physical drive 1 */
#define USB 2 /* Example: Map USB MSD to physical drive 2 */

static SD_CardInfo Info;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
    )
{
    DSTATUS stat = RES_NOTRDY;

    switch (pdrv) {
    case ATA:

        // translate the reslut code here

        return stat;

    case MMC:
        switch (SD_GetState()) {

        case SD_CARD_READY:
            stat = RES_OK;
            break;

        case SD_CARD_SENDING:
            stat = RES_NOTRDY;
            break;

        case SD_CARD_RECEIVING:
            stat = RES_NOTRDY;
            break;

        default:
            stat = RES_ERROR;
            break;
        }

        // translate the reslut code here

        return stat;

    case USB:

        // translate the reslut code here

        return stat;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
    )
{
    DSTATUS stat = RES_NOTRDY;

    switch (pdrv) {
    case ATA:

        // translate the reslut code here

        return stat;

    case MMC:

        // translate the reslut code here

        if (SD_OK == SD_Init()) {
            if (SD_OK == SD_GetCardInfo(&Info)) {
                stat = RES_OK;
            }
        }

        return stat;

    case USB:

        // translate the reslut code here

        return stat;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv, /* Physical drive nmuber to identify the drive */
    BYTE* buff, /* Data buffer to store read data */
    DWORD sector, /* Sector address in LBA */
    UINT count /* Number of sectors to read */
    )
{
    DRESULT res = RES_NOTRDY;

    switch (pdrv) {
    case ATA:
        // translate the arguments here

        // translate the reslut code here

        return res;

    case MMC:
        // translate the arguments here

        if(SD_OK == SD_ReadMultiBlocks(buff,sector,Info.CardBlockSize,count)){
            res = RES_OK;
        }

        // translate the reslut code here

        return res;

    case USB:
        // translate the arguments here

        // translate the reslut code here

        return res;
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write(
    BYTE pdrv, /* Physical drive nmuber to identify the drive */
    const BYTE* buff, /* Data to be written */
    DWORD sector, /* Sector address in LBA */
    UINT count /* Number of sectors to write */
    )
{
    DRESULT res = RES_NOTRDY;

    switch (pdrv) {
    case ATA:
        // translate the arguments here

        // translate the reslut code here

        return res;

    case MMC:
        // translate the arguments here

        if(SD_OK == SD_WriteMultiBlocks((uint8_t *)buff,sector,Info.CardBlockSize,count)){
            res = RES_OK;
        }

        // translate the reslut code here

        return res;

    case USB:
        // translate the arguments here

        // translate the reslut code here

        return res;
    }

    return RES_PARERR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd, /* Control code */
    void* buff /* Buffer to send/receive control data */
    )
{
    DRESULT res;

    switch (pdrv) {
    case ATA:

        // Process of the command for the ATA drive

        return res;

    case MMC:

        // Process of the command for the MMC/SD card

        return res;

    case USB:

        // Process of the command the USB drive

        return res;
    }

    return RES_PARERR;
}
#endif
