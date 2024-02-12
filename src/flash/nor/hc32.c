#include "imp.h"
#include <helper/binarybuffer.h>
#include <target/algorithm.h>

struct hc32_flash_bank {

};


FLASH_BANK_COMMAND_HANDLER(hc32_flash_bank_command) {

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

static int hc32_probe(struct flash_bank *bank) {

    return ERROR_OK;
}

static int hc32_auto_probe(struct flash_bank *bank) {

    return ERROR_OK;
}

static int hc32_protect_check(struct flash_bank *bank) {

    return ERROR_OK;
}

static int get_hc32_info(struct flash_bank *bank, struct command_invocation *cmd) {

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