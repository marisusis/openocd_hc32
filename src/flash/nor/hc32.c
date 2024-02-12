#include "imp.h"
#include <helper/binarybuffer.h>
#include <target/algorithm.h>

#define HC32_SECTOR_SIZE 0x1ff

struct hc32_flash_bank {
    bool probed;
    uint32_t user_bank_size;  
    uint16_t num_sectors;
};


static int hc32_get_flash_size(struct flash_bank *bank, uint32_t *flash_size) {
    return target_read_u32(bank->target, 0x00100c70, flash_size);
}

FLASH_BANK_COMMAND_HANDLER(hc32_flash_bank_command) {
    struct hc32_flash_bank *hc32_info;

    if (CMD_ARGC < 6)
        return ERROR_COMMAND_SYNTAX_ERROR;

    hc32_info = malloc(sizeof(struct hc32_flash_bank));
    bank->driver_priv = hc32_info;

    hc32_info->probed = false;

    return ERROR_OK;
}

static int hc32_erase(struct flash_bank *bank, uint32_t address, uint32_t size) {


    return ERROR_OK;
}

static int hc32_protect(struct flash_bank *bank, int set, 
        unsigned int first, unsigned int last) {

    return ERROR_OK;
}

static int hc32_write(struct flash_bank *bank, const uint8_t *buffer,
		uint32_t offset, uint32_t count) {
    

    return ERROR_OK;
}

// static int hc32_read(struct flash_bank *bank, uint32_t address, uint8_t *data, uint32_t size) {

//     return ERROR_OK;
// }

static void setup_sector(struct flash_bank *bank, unsigned int i,
		unsigned int size) {
    assert(i < bank->num_sectors);
    bank->sectors[i].offset = bank->size;
    bank->sectors[i].size = size;
    bank->sectors[i].is_erased = -1;
    bank->sectors[i].is_protected = 0;
    bank->size += bank->sectors[i].size;
    LOG_DEBUG("sector %u: %ukBytes", i, size >> 10);
}

static void setup_bank(struct flash_bank *bank, unsigned int start,
	uint32_t flash_size, uint32_t sector_size) {
    uint32_t remaining_flash_size = flash_size;
    unsigned int sector_index = 0;
    while (remaining_flash_size > 0) {
        uint32_t size = HC32_SECTOR_SIZE;
        if (size > remaining_flash_size) {
            size = remaining_flash_size;
        }

        setup_sector(bank, start + sector_index, size);
        remaining_flash_size -= size;
        sector_index++;
    }

}

static int hc32_probe(struct flash_bank *bank) {
    struct target *target = bank->target;
    struct hc32_flash_bank *hc32_info = bank->driver_priv;
    uint16_t flash_size_in_kb;

    hc32_info->probed = false;

    if (!target_was_examined(target)) {
        LOG_ERROR("Target not examined yet");
        return ERROR_TARGET_NOT_EXAMINED;
    }

    uint32_t flash_size = 0;
    int retval = hc32_get_flash_size(bank, &flash_size);
    if (retval != ERROR_OK)
        return retval;

    flash_size_in_kb = flash_size / 1024;

    if (hc32_info->user_bank_size) {
        LOG_INFO("ignoring flash probed value, using configured bank size");
        flash_size = hc32_info->user_bank_size;
        flash_size_in_kb = hc32_info->user_bank_size / 1024;
    }

    LOG_INFO("Flash size = %u", flash_size_in_kb);

    if (flash_size_in_kb == 64) {
        hc32_info->num_sectors = 128;
    } else if (flash_size_in_kb == 128) {
        hc32_info->num_sectors = 256;
    } else if (flash_size_in_kb == 192) {
        hc32_info->num_sectors = 384;
    } else if (flash_size_in_kb == 256) {
        hc32_info->num_sectors = 512;
    } else {
        LOG_ERROR("Invalid flash size: %u", flash_size_in_kb);
        return ERROR_FLASH_BANK_INVALID;
    }

    bank->base = 0x00000000;
    bank->num_sectors = hc32_info->num_sectors;
    bank->sectors = calloc(bank->num_sectors, sizeof(struct flash_sector));
    for (unsigned int i = 0; i < bank->num_sectors; i++) {
        bank->sectors[i].is_erased = -1;
        bank->sectors[i].is_protected = 0;
    }
    bank->size = 0;

    setup_bank(bank, 0, flash_size, HC32_SECTOR_SIZE);
    bank->num_prot_blocks = 0;
    assert((bank->size >> 10) == flash_size_in_kb);

    LOG_INFO("Number of sectors = %u", hc32_info->num_sectors);

    hc32_info->probed = true;
    return ERROR_OK;
}

static int hc32_auto_probe(struct flash_bank *bank) {

    struct hc32_flash_bank *hc32_info = bank->driver_priv;
    if (hc32_info->probed)
        return ERROR_OK;

    return hc32_probe(bank);
}

static int hc32_protect_check(struct flash_bank *bank) {
    
    return ERROR_OK;
}

static int get_hc32_info(struct flash_bank *bank, struct command_invocation *cmd) {

    // Read from FlashSize, giving us flash size in bytes
    uint32_t flash_size = 0;
    int retval = hc32_get_flash_size(bank, &flash_size);
    if (retval != ERROR_OK)
        return retval;

    // Print flash size
    command_print_sameline(cmd, "FlashSize = 0x%x", flash_size);

    return ERROR_OK;
}



static const struct command_registration hc32_command_handlers[] = {
    COMMAND_REGISTRATION_DONE
};


const struct flash_driver hc32_flash = {
    .name = "hc32",
	.commands = hc32_command_handlers,
	.flash_bank_command = hc32_flash_bank_command,
	.erase = hc32_erase,
	.protect = hc32_protect,
	.write = hc32_write,
	.read = default_flash_read,
	.probe = hc32_probe,
	.auto_probe = hc32_auto_probe,
	.erase_check = default_flash_blank_check,
	.protect_check = hc32_protect_check,
	.info = get_hc32_info,
	.free_driver_priv = default_flash_free_driver_priv,
};