# ESP-IDF: ESP32 hardware interrupts

Native ESP-IDF version of the Arduino laboratory project. The button is on
GPIO 15 and uses the internal pull-up in the initial `no_debounce` method.

## Build and flash

Open an **ESP-IDF PowerShell** in this folder, then run:

```powershell
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

The default build uses `no_debounce`.

## Select a method

Only one method source is compiled at a time. To select another template:

```powershell
idf.py -DDEBOUNCE_METHOD=time_based reconfigure
idf.py build
idf.py flash monitor
```

Available values are `no_debounce`, `time_based`, `state_based`, `polling`,
and `hardware_rc`.

The last four are intentionally templates for now.
