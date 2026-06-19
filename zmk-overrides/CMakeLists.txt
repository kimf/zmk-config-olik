# Minimal module to inject custom display bits without forking ZMK.
zephyr_include_directories(include)

if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
  zephyr_library_sources(src/display/custom_status_screen.c)
endif()

if(CONFIG_ZMK_WIDGET_PERIPHERAL_BATTERY_STATUS)
  zephyr_library_sources(src/display/widgets/peripheral_battery_status.c)
endif()
