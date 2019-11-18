#include <string.h>
#include "zephyr/types.h"
#include "util.h"
#include "dc.h"

enum {
    DC_Z = 0,
    DC_Y,
    DC_X,
    DC_D,
    DC_RD_UP,
    DC_RD_DOWN,
    DC_RD_LEFT,
    DC_RD_RIGHT,
    DC_C,
    DC_B,
    DC_A,
    DC_START,
    DC_LD_UP,
    DC_LD_DOWN,
    DC_LD_LEFT,
    DC_LD_RIGHT,
};

const uint8_t dc_axes_idx[6] =
{
/*  AXIS_LX, AXIS_LY, AXIS_RX, AXIS_RY, TRIG_L, TRIG_R  */
    7,       6,       5,       4,       0,      1
};

const struct ctrl_meta dc_btns_meta =
{
    .polarity = 1,
};

const struct ctrl_meta dc_axes_meta[6] =
{
    {.neutral = 0x80, .abs_max = 0x80},
    {.neutral = 0x80, .abs_max = 0x80},
    {.neutral = 0x80, .abs_max = 0x80},
    {.neutral = 0x80, .abs_max = 0x80},
    {.neutral = 0x00, .abs_max = 0xFF},
    {.neutral = 0x00, .abs_max = 0xFF},
};

struct dc_map {
    union {
        struct {
            uint16_t reserved;
            uint16_t buttons;
        };
        uint8_t axes[8];
    };
} __packed;

const uint32_t dc_btns_mask[32] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    BIT(DC_LD_LEFT), BIT(DC_LD_RIGHT), BIT(DC_LD_DOWN), BIT(DC_LD_UP),
    BIT(DC_RD_LEFT), BIT(DC_RD_RIGHT), BIT(DC_RD_DOWN), BIT(DC_RD_UP),
    BIT(DC_B), BIT(DC_X), BIT(DC_A), BIT(DC_Y),
    BIT(DC_START), BIT(DC_D), 0, 0,
    0, BIT(DC_Z), 0, 0,
    0, BIT(DC_C), 0, 0,
};

static void dc_init_map(struct dc_map *map) {
    map->buttons = 0xFFFF;
    for (uint32_t i = 0; i < ARRAY_SIZE(dc_axes_meta); i++) {
        map->axes[dc_axes_idx[i]] = dc_axes_meta[i].neutral;
    }
}

void dc_from_generic(struct generic_ctrl *ctrl_data, struct wired_data *wired_data) {
    struct dc_map map_tmp;

    dc_init_map(&map_tmp);

    for (uint32_t i = 0; i < ARRAY_SIZE(generic_btns_mask); i++) {
        if (ctrl_data->btns[0].value & generic_btns_mask[i]) {
            map_tmp.buttons &= ~dc_btns_mask[i];
        }
    }

    for (uint32_t i = 0; i < ARRAY_SIZE(dc_axes_meta); i++) {
        map_tmp.axes[dc_axes_idx[i]] = ctrl_data->axes[i].value;
    }

    memcpy(wired_data->output, (void *)&map_tmp, sizeof(map_tmp));
}
