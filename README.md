# PUERTO_SERIE_SIM800L

Cambios:

    * Se agregaron divisor resistivo entre el RX y TX del Raspberry y GSM
    * Se agregaron capacitores de alimentación.
    * Se conectó la batería (3V7 1400mAh) directo al módulo GSM.
    * Se arregló la doble inicialización del puerto serie.

---

__NOTA:__
   hay que correr cmake para que regenere el proyecto:

    1. Cmd + Shift + P → Pico: Configure CMake (o CMake: Configure).
    2. Esperá a que termine sin errores.
    3. Después Cmd + Shift + P → Pico: Compile Project (o CMake: Build).

Yo lo realicé (Linux):

    1. rm -rf build
    2. mkdir build
    3. cd build
    4. cmake -G Ninja ..
    (windows ?: pico-sdk/cmake/v3.29.9/bin/cmake -G Ninja .. )
