# Choose one method when configuring the project. The default is no_debounce.
# Example:
#   idf.py -DDEBOUNCE_METHOD=time_based reconfigure
set(DEBOUNCE_METHOD "no_debounce" CACHE STRING "Button debounce method")
set_property(CACHE DEBOUNCE_METHOD PROPERTY STRINGS
             no_debounce time_based state_based polling hardware_rc)

set(METHOD_SOURCE "methods/${DEBOUNCE_METHOD}.c")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${METHOD_SOURCE}")
    message(FATAL_ERROR "Unknown DEBOUNCE_METHOD: ${DEBOUNCE_METHOD}")
endif()

idf_component_register(
    SRCS "main.c" "${METHOD_SOURCE}"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_gpio
)
