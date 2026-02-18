#include "ext_flash_recorder.h"

#include <string.h>

#include "fsl_flexspi_nor_flash.h"

#define EDGEAI_FLEXSPI_INSTANCE (0u)
#define EDGEAI_REC_MAGIC (0x52454335u)  /* "REC5" */
#define EDGEAI_META_MAGIC (0x4D455441u) /* "META" */
#define EDGEAI_MAX_PAGE_SIZE (1024u)

typedef struct
{
    uint32_t magic;
    uint32_t generation;
    uint32_t reserved0;
    uint32_t reserved1;
} ext_flash_meta_t;

typedef struct
{
    uint32_t magic;
    uint32_t seq;
    uint16_t generation;
    uint16_t reserved0;
    int16_t ax_mg;
    int16_t ay_mg;
    int16_t az_mg;
    int16_t gx_mdps;
    int16_t gy_mdps;
    int16_t gz_mdps;
    int16_t temp_c10;
    int16_t mag_x_mgauss;
    int16_t mag_y_mgauss;
    int16_t mag_z_mgauss;
    int16_t baro_dhpa;
    int16_t sht_temp_c10;
    int16_t sht_rh_dpct;
    int16_t stts_temp_c10;
    uint16_t reserved1;
    uint32_t ts_ds;
} ext_flash_sample_record_t;

typedef union
{
    uint32_t words[EDGEAI_MAX_PAGE_SIZE / 4u];
    uint8_t bytes[EDGEAI_MAX_PAGE_SIZE];
} ext_flash_page_buf_t;

static serial_nor_config_option_t s_norOption = {
    .option0.U = 0xC0000001u,
    .option1.U = 0u,
};

static flexspi_nor_config_t s_norConfig;
static ext_flash_page_buf_t s_pageBuf;
static uint32_t s_regionStart;
static uint32_t s_dataStart;
static uint32_t s_capacityPages;
static uint32_t s_writeIndex;
static uint32_t s_oldestIndex;
static uint32_t s_sampleCount;
static uint32_t s_seq;
static uint32_t s_generation;
static bool s_ready;
static uint32_t s_playStartIndex;
static uint32_t s_playCount;
static uint32_t s_playOffset;

static uint32_t ExtFlashRecorder_PageAddr(uint32_t page_index)
{
    return s_dataStart + (page_index * s_norConfig.pageSize);
}

static bool ExtFlashRecorder_ReadRawPage(uint32_t addr)
{
    status_t st = FLEXSPI_NorFlash_Read(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, s_pageBuf.words, addr, s_norConfig.pageSize);
    return (st == kStatus_Success);
}

static bool ExtFlashRecorder_ReadRecordAt(uint32_t page_index, ext_flash_sample_record_t *rec)
{
    if ((rec == NULL) || (page_index >= s_capacityPages))
    {
        return false;
    }
    if (!ExtFlashRecorder_ReadRawPage(ExtFlashRecorder_PageAddr(page_index)))
    {
        return false;
    }
    memcpy(rec, s_pageBuf.bytes, sizeof(*rec));
    return (rec->magic == EDGEAI_REC_MAGIC) && (rec->generation == (uint16_t)(s_generation & 0xFFFFu));
}

static bool ExtFlashRecorder_WriteMeta(uint32_t generation)
{
    ext_flash_meta_t meta;
    status_t st;

    meta.magic = EDGEAI_META_MAGIC;
    meta.generation = generation;
    meta.reserved0 = 0u;
    meta.reserved1 = 0u;

    st = FLEXSPI_NorFlash_Erase(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, s_regionStart, s_norConfig.sectorSize);
    if (st != kStatus_Success)
    {
        return false;
    }

    memset(s_pageBuf.bytes, 0xFF, s_norConfig.pageSize);
    memcpy(s_pageBuf.bytes, &meta, sizeof(meta));
    st = FLEXSPI_NorFlash_ProgramPage(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, s_regionStart, s_pageBuf.words);
    return (st == kStatus_Success);
}

static bool ExtFlashRecorder_ReadMeta(uint32_t *generation_out)
{
    ext_flash_meta_t meta;

    if (generation_out == NULL)
    {
        return false;
    }
    if (!ExtFlashRecorder_ReadRawPage(s_regionStart))
    {
        return false;
    }

    memcpy(&meta, s_pageBuf.bytes, sizeof(meta));
    if (meta.magic != EDGEAI_META_MAGIC)
    {
        return false;
    }

    *generation_out = meta.generation;
    return true;
}

static void ExtFlashRecorder_RecoverState(void)
{
    ext_flash_sample_record_t rec;
    uint32_t i;
    uint32_t valid_count = 0u;
    uint32_t min_seq = 0xFFFFFFFFu;
    uint32_t max_seq = 0u;
    uint32_t min_idx = 0u;
    uint32_t max_idx = 0u;

    s_writeIndex = 0u;
    s_oldestIndex = 0u;
    s_sampleCount = 0u;
    s_seq = 0u;

    for (i = 0u; i < s_capacityPages; i++)
    {
        if (ExtFlashRecorder_ReadRecordAt(i, &rec))
        {
            if (valid_count == 0u)
            {
                min_seq = rec.seq;
                max_seq = rec.seq;
                min_idx = i;
                max_idx = i;
            }
            else
            {
                if (rec.seq < min_seq)
                {
                    min_seq = rec.seq;
                    min_idx = i;
                }
                if (rec.seq > max_seq)
                {
                    max_seq = rec.seq;
                    max_idx = i;
                }
            }
            valid_count++;
        }
    }

    if (valid_count == 0u)
    {
        return;
    }

    s_sampleCount = valid_count;
    s_oldestIndex = min_idx;
    s_writeIndex = (max_idx + 1u) % s_capacityPages;
    s_seq = max_seq + 1u;
}

bool ExtFlashRecorder_Init(void)
{
    status_t st;
    uint32_t totalSize;
    uint32_t regionSize;
    uint32_t meta_generation = 0u;

    s_ready = false;
    s_writeIndex = 0u;
    s_oldestIndex = 0u;
    s_sampleCount = 0u;
    s_seq = 0u;
    s_generation = 1u;
    s_playStartIndex = 0u;
    s_playCount = 0u;
    s_playOffset = 0u;
    memset(&s_norConfig, 0, sizeof(s_norConfig));

    st = FLEXSPI_NorFlash_GetConfig(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, &s_norOption);
    if (st != kStatus_Success)
    {
        return false;
    }

    st = FLEXSPI_NorFlash_Init(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig);
    if (st != kStatus_Success)
    {
        return false;
    }

    totalSize = s_norConfig.memConfig.sflashA1Size;
    if ((totalSize == 0u) || (s_norConfig.pageSize == 0u) || (s_norConfig.sectorSize == 0u))
    {
        return false;
    }
    if ((s_norConfig.pageSize > EDGEAI_MAX_PAGE_SIZE) || ((s_norConfig.pageSize % 4u) != 0u))
    {
        return false;
    }
    if (sizeof(ext_flash_sample_record_t) > s_norConfig.pageSize)
    {
        return false;
    }

    regionSize = totalSize / 2u;
    regionSize -= (regionSize % s_norConfig.sectorSize);
    if (regionSize <= s_norConfig.sectorSize)
    {
        return false;
    }

    s_regionStart = totalSize - regionSize;
    s_dataStart = s_regionStart + s_norConfig.sectorSize; /* reserve first sector for metadata */
    s_capacityPages = (regionSize - s_norConfig.sectorSize) / s_norConfig.pageSize;
    if (s_capacityPages == 0u)
    {
        return false;
    }

    if (ExtFlashRecorder_ReadMeta(&meta_generation))
    {
        s_generation = (meta_generation == 0u) ? 1u : meta_generation;
    }
    else
    {
        s_generation = 1u;
        if (!ExtFlashRecorder_WriteMeta(s_generation))
        {
            return false;
        }
    }

    s_ready = true;
    ExtFlashRecorder_RecoverState();
    return true;
}

bool ExtFlashRecorder_AppendSampleEx(int16_t ax_mg,
                                     int16_t ay_mg,
                                     int16_t az_mg,
                                     int16_t gx_mdps,
                                     int16_t gy_mdps,
                                     int16_t gz_mdps,
                                     int16_t temp_c10,
                                     int16_t mag_x_mgauss,
                                     int16_t mag_y_mgauss,
                                     int16_t mag_z_mgauss,
                                     int16_t baro_dhpa,
                                     int16_t sht_temp_c10,
                                     int16_t sht_rh_dpct,
                                     int16_t stts_temp_c10,
                                     uint32_t ts_ds)
{
    ext_flash_sample_record_t rec;
    status_t st;
    uint32_t write_addr;

    if (!s_ready || (s_capacityPages == 0u))
    {
        return false;
    }

    if (s_writeIndex >= s_capacityPages)
    {
        s_writeIndex = 0u;
    }

    write_addr = ExtFlashRecorder_PageAddr(s_writeIndex);
    if ((write_addr % s_norConfig.sectorSize) == 0u)
    {
        st = FLEXSPI_NorFlash_Erase(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, write_addr, s_norConfig.sectorSize);
        if (st != kStatus_Success)
        {
            return false;
        }
    }

    rec.magic = EDGEAI_REC_MAGIC;
    rec.seq = s_seq++;
    rec.generation = (uint16_t)(s_generation & 0xFFFFu);
    rec.reserved0 = 0u;
    rec.ax_mg = ax_mg;
    rec.ay_mg = ay_mg;
    rec.az_mg = az_mg;
    rec.gx_mdps = gx_mdps;
    rec.gy_mdps = gy_mdps;
    rec.gz_mdps = gz_mdps;
    rec.temp_c10 = temp_c10;
    rec.mag_x_mgauss = mag_x_mgauss;
    rec.mag_y_mgauss = mag_y_mgauss;
    rec.mag_z_mgauss = mag_z_mgauss;
    rec.baro_dhpa = baro_dhpa;
    rec.sht_temp_c10 = sht_temp_c10;
    rec.sht_rh_dpct = sht_rh_dpct;
    rec.stts_temp_c10 = stts_temp_c10;
    rec.reserved1 = 0u;
    rec.ts_ds = ts_ds;

    memset(s_pageBuf.bytes, 0xFF, s_norConfig.pageSize);
    memcpy(s_pageBuf.bytes, &rec, sizeof(rec));
    st = FLEXSPI_NorFlash_ProgramPage(EDGEAI_FLEXSPI_INSTANCE, &s_norConfig, write_addr, s_pageBuf.words);
    if (st != kStatus_Success)
    {
        return false;
    }

    if (s_sampleCount == 0u)
    {
        s_oldestIndex = s_writeIndex;
    }

    s_writeIndex = (s_writeIndex + 1u) % s_capacityPages;
    if (s_sampleCount < s_capacityPages)
    {
        s_sampleCount++;
    }
    else
    {
        s_oldestIndex = s_writeIndex;
    }

    return true;
}

bool ExtFlashRecorder_AppendSample(int16_t ax_mg, int16_t ay_mg, int16_t az_mg, uint8_t temp_c)
{
    return ExtFlashRecorder_AppendSampleEx(ax_mg,
                                           ay_mg,
                                           az_mg,
                                           0,
                                           0,
                                           0,
                                           (int16_t)temp_c * 10,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0,
                                           0u);
}

bool ExtFlashRecorder_IsReady(void)
{
    return s_ready;
}

bool ExtFlashRecorder_ClearAll(void)
{
    if (!s_ready)
    {
        return false;
    }

    s_generation++;
    if (s_generation == 0u)
    {
        s_generation = 1u;
    }
    if (!ExtFlashRecorder_WriteMeta(s_generation))
    {
        return false;
    }

    s_writeIndex = 0u;
    s_oldestIndex = 0u;
    s_sampleCount = 0u;
    s_seq = 0u;
    s_playStartIndex = 0u;
    s_playCount = 0u;
    s_playOffset = 0u;
    return true;
}

bool ExtFlashRecorder_StartPlayback(void)
{
    if (!s_ready || (s_sampleCount == 0u) || (s_capacityPages == 0u))
    {
        return false;
    }

    s_playCount = s_sampleCount;
    s_playStartIndex = s_oldestIndex;
    s_playOffset = 0u;
    return true;
}

bool ExtFlashRecorder_ReadNextSample(ext_flash_sample_t *sample)
{
    ext_flash_sample_record_t rec;
    uint32_t page_index;
    int32_t temp_c_rounded;

    if (!s_ready || (sample == NULL) || (s_playCount == 0u) || (s_capacityPages == 0u))
    {
        return false;
    }

    page_index = (s_playStartIndex + s_playOffset) % s_capacityPages;
    if (!ExtFlashRecorder_ReadRecordAt(page_index, &rec))
    {
        return false;
    }

    sample->seq = rec.seq;
    sample->ts_ds = rec.ts_ds;
    sample->ax_mg = rec.ax_mg;
    sample->ay_mg = rec.ay_mg;
    sample->az_mg = rec.az_mg;
    sample->gx_mdps = rec.gx_mdps;
    sample->gy_mdps = rec.gy_mdps;
    sample->gz_mdps = rec.gz_mdps;
    sample->temp_c10 = rec.temp_c10;
    sample->mag_x_mgauss = rec.mag_x_mgauss;
    sample->mag_y_mgauss = rec.mag_y_mgauss;
    sample->mag_z_mgauss = rec.mag_z_mgauss;
    sample->baro_dhpa = rec.baro_dhpa;
    sample->sht_temp_c10 = rec.sht_temp_c10;
    sample->sht_rh_dpct = rec.sht_rh_dpct;
    sample->stts_temp_c10 = rec.stts_temp_c10;

    temp_c_rounded = (sample->temp_c10 >= 0) ? (sample->temp_c10 + 5) : (sample->temp_c10 - 5);
    temp_c_rounded /= 10;
    if (temp_c_rounded < 0)
    {
        temp_c_rounded = 0;
    }
    if (temp_c_rounded > 99)
    {
        temp_c_rounded = 99;
    }
    sample->temp_c = (uint8_t)temp_c_rounded;

    s_playOffset++;
    if (s_playOffset >= s_playCount)
    {
        s_playOffset = 0u;
    }
    return true;
}

bool ExtFlashRecorder_GetPlaybackInfo(uint32_t *offset, uint32_t *count)
{
    if (!s_ready || (offset == NULL) || (count == NULL))
    {
        return false;
    }

    *offset = s_playOffset;
    *count = s_playCount;
    return true;
}

bool ExtFlashRecorder_GetRecordInfo(uint32_t *count)
{
    if (!s_ready || (count == NULL))
    {
        return false;
    }

    *count = s_sampleCount;
    return true;
}
