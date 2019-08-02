/*
 * Copyright (c) 2019 shchmue
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include "diskio.h"		/* FatFs lower layer API */
#include "../../mem/heap.h"
#include "../../storage/sdmmc.h"

#define SDMMC_UPPER_BUFFER 0xB8000000
#define DRAM_START         0x80000000

extern sdmmc_storage_t sd_storage;
extern sdmmc_storage_t storage;

DSTATUS disk_status (
    BYTE pdrv /* Physical drive number to identify the drive */
)
{
    return 0;
}

DSTATUS disk_initialize (
    BYTE pdrv /* Physical drive number to identify the drive */
)
{
    return 0;
}

static inline void _gf256_mul_x_le(void *block) {
    u8 *pdata = (u8 *)block;
    u32 carry = 0;

    for (u32 i = 0; i < 0x10; i++) {
        u8 b = pdata[i];
        pdata[i] = (b << 1) | carry;
        carry = b >> 7;
    }

    if (carry)
        pdata[0x0] ^= 0x87;
}

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive number to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Start sector in LBA */
    UINT count		/* Number of sectors to read */
)
{
    switch (pdrv)
    {
    case 0:
        return sdmmc_storage_read(&sd_storage, sector, count, buff) ? RES_OK : RES_ERROR;
    }
    return RES_ERROR;
}

DRESULT disk_write (
    BYTE pdrv,			/* Physical drive number to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Start sector in LBA */
    UINT count			/* Number of sectors to write */
)
{
    return sdmmc_storage_write(&sd_storage, sector, count, (void *)buff) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive number (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    return RES_OK;
}
