/*
 * Copyright (c) 2018 naehrwert
 *
 * Copyright (c) 2018-2019 CTCaer
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

#include <string.h>

#include "libs/fatfs/ff.h"
#include "mem/heap.h"
#include "power/max77620.h"
#include "rtc/max77620-rtc.h"
#include "soc/hw_init.h"
#include "storage/sdmmc.h"
#include "utils/util.h"

sdmmc_t sd_sdmmc;
sdmmc_storage_t sd_storage;
__attribute__ ((aligned (16))) FATFS sd_fs;
static bool sd_mounted;

boot_cfg_t __attribute__((section ("._boot_cfg"))) b_cfg;

bool sd_mount()
{
    if (sd_mounted)
        return true;

    if (!sdmmc_storage_init_sd(&sd_storage, &sd_sdmmc, SDMMC_1, SDMMC_BUS_WIDTH_4, 11))
    {
        //EPRINTF("Failed to init SD card.\nMake sure that it is inserted.\nOr that SD reader is properly seated!");
        return false;
    }
    else
    {
        int res = 0;
        res = f_mount(&sd_fs, "sd:", 1);
        if (res == FR_OK)
        {
            sd_mounted = 1;
            return true;
        }
        else
        {
            //EPRINTFARGS("Failed to mount SD card (FatFS Error %d).\nMake sure that a FAT partition exists..", res);
            return false;
        }
    }

    return false;
}

void sd_unmount()
{
    if (sd_mounted)
    {
        f_mount(NULL, "sd:", 1);
        sdmmc_storage_end(&sd_storage);
        sd_mounted = false;
    }
}

void *sd_file_read(const char *path, u32 *fsize)
{
	FIL fp;
	if (f_open(&fp, path, FA_READ) != FR_OK)
		return NULL;

	u32 size = f_size(&fp);
	if (fsize)
		*fsize = size;

	void *buf = malloc(size);

	if (f_read(&fp, buf, size, NULL) != FR_OK)
	{
		free(buf);
		f_close(&fp);

		return NULL;
	}

	f_close(&fp);

	return buf;
}

int sd_save_to_file(void *buf, u32 size, const char *filename)
{
    FIL fp;
    u32 res = 0;
    res = f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res)
    {
        //EPRINTFARGS("Error (%d) creating file\n%s.\n", res, filename);
        return 1;
    }

    f_write(&fp, buf, size, NULL);
    f_close(&fp);

    return 0;
}

#define IRAM_PAYLOAD_MAX_SIZE 0x2F000
#define IRAM_PAYLOAD_BASE 0x40010000

#define LOG_PAYLOAD_MAX_SIZE 0xA500
#define LOG_START (IRAM_PAYLOAD_BASE + LOG_PAYLOAD_MAX_SIZE)

#define IPL_STACK_TOP  0x4003F000
#define IPL_HEAP_START 0x90020000

extern void pivot_stack(u32 stack_top);

void ipl_main()
{
    config_hw();
    pivot_stack(IPL_STACK_TOP);
    heap_init(IPL_HEAP_START);
    sd_mount();

    sd_save_to_file((void*)LOG_START, IRAM_PAYLOAD_MAX_SIZE-LOG_PAYLOAD_MAX_SIZE, "iram.log");
    reboot_rcm();
}
