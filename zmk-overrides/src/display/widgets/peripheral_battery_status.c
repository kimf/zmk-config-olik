/*
 * Copyright (c) 2025 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/peripheral_battery_status.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/central.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_battery_status_state {
    bool valid;
    uint8_t level;
};

static void set_battery_symbol(lv_obj_t *label, struct peripheral_battery_status_state state) {
    char text[12] = {};

    if (!state.valid) {
        strcpy(text, "P --");
        lv_label_set_text(label, text);
        return;
    }

    snprintf(text, sizeof(text), "P %3u%%", state.level);
    lv_label_set_text(label, text);
}

static void peripheral_battery_status_update_cb(struct peripheral_battery_status_state state) {
    struct zmk_widget_peripheral_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, state); }
}

static struct peripheral_battery_status_state get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev =
        as_zmk_peripheral_battery_state_changed(eh);

    if (ev != NULL && ev->source == 0) {
        return (struct peripheral_battery_status_state){
            .valid = true,
            .level = ev->state_of_charge,
        };
    }

    uint8_t level = 0;
    int rc = zmk_split_central_get_peripheral_battery_level(0, &level);
    if (rc < 0) {
        return (struct peripheral_battery_status_state){
            .valid = false,
            .level = 0,
        };
    }

    return (struct peripheral_battery_status_state){
        .valid = true,
        .level = level,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_battery_status, struct peripheral_battery_status_state,
                            peripheral_battery_status_update_cb, get_state)

ZMK_SUBSCRIPTION(widget_peripheral_battery_status, zmk_peripheral_battery_state_changed);

int zmk_widget_peripheral_battery_status_init(struct zmk_widget_peripheral_battery_status *widget,
                                              lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_peripheral_battery_status_init();
    return 0;
}

lv_obj_t *zmk_widget_peripheral_battery_status_obj(
    struct zmk_widget_peripheral_battery_status *widget) {
    return widget->obj;
}
