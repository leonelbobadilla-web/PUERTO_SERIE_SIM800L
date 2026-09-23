# ==============================================================================
# ARCHIVO: CMakeLists.txt
# DESCRIPCIÓN: 
#   Archivo de configuración para proyectos de Raspberry Pi Pico utilizando CMake.
#   Define las reglas de compilación, vinculación con el SDK y gestión de dependencias.
# ==============================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# ==============================================================================
# 1. NOMBRE DEL PROYECTO
# ==============================================================================
# Modifica "nombre_proyecto" para cambiar el nombre de tu proyecto de forma global. 
# Esto afectará el nombre del archivo ejecutable y del archivo .uf2 resultante.
project(SIM800L C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ==============================================================================
# 2. BÚSQUEDA AUTOMÁTICA INTELIGENTE DE FUENTES
# ==============================================================================
# CONFIGURE_DEPENDS le indica a CMake que vigile la carpeta src/. 
# Si agregas o borras un .c  CMake se reconfigura solo antes de compilar.
file(GLOB SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
)

# ==============================================================================
# 3. DECLARACIÓN DEL EJECUTABLE Y CABECERAS
# ==============================================================================
# Se compilan automáticamente todos los archivos encontrados en SOURCES
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

# Le indicamos al compilador dónde buscar los archivos .h dentro de src/
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# Incrusta el nombre en los metadatos del binario compilado, cumple dos funciones:
# -1. Descriptor USB automático: Si se usa la Pico como un dispositivo USB
# compuesto puede utilizar este nombre para identificar el dispositivo ante la
# computadora.
# -2. Funciones de depuración e información del sistema
pico_set_program_name(${PROJECT_NAME} "SIM800L")
# Define la versión actual del firmware
pico_set_program_version(${PROJECT_NAME} "0.1")

# ==============================================================================
# 4. CONFIGURACIÓN DE LA CONSOLA STANDAR (stdio)
# ==============================================================================
# Modifica esta lineas para habilitar/deshabilitar la consola por UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# ==============================================================================
# 5. LIBRERÍAS DEL SDK DE LA RASPBERRY PI PICO
# ==============================================================================
# `pico_stdlib` es la base obligatoria (incluye gpio, time, uart básica, etc.).
# Descomenta (quita el #) las librerías de hardware adicionales que vayas a usar:
target_link_libraries(${PROJECT_NAME} PRIVATE
    pico_stdlib
    
    # --- Periféricos de hardware ---
    hardware_irq        # 

    # --- Periféricos de Comunicación ---
    hardware_i2c        # Comunicación I2C (sensores, pantallas LCD/OLED, etc.)
    # hardware_spi      # Comunicación SPI (tarjetas SD, pantallas TFT, etc.)
    hardware_uart       # Control avanzado de puertos UART por hardware
    # hardware_usb      # Control avanzado de USB
    
    # --- Periféricos Analógicos y de Control ---
    # hardware_adc      # Conversor Analógico-Digital (leer pines ADC0-ADC2/Temperatura)
    # hardware_pwm      # Modulación por Ancho de Pulso (control de motores, servos, LED dimming)
    # hardware_rtc      # Reloj de Tiempo Real interno
    
    # --- Almacenamiento y Sistema ---
    # hardware_flash    # Manipulación de la memoria Flash interna (guardar datos/configuraciones)
    # hardware_watchdog # Temporizador Watchdog (reinicio automático ante bloqueos)
    # hardware_dma      # Acceso Directo a Memoria (transferencias rápidas sin CPU)
    # hardware_pio      # Programable I/O (para protocolos personalizados, WS2812, etc.)
    # hardware_clocks   # Gestión avanzada de relojes del sistema
)

# ==============================================================================
# 6. GENERACION DE EJECUTABLES y SALIDAS ADICIONALES
# ==============================================================================
pico_add_extra_outputs(${PROJECT_NAME})
# Este macor toma el archivo compilado por defecto (.elf) 
# y procesarlo (post-build actions) para generar formatos adicionales.
# En la carpeta build genera automáticamente los siguientes archivos:
# -1. El archivo .uf2 (USB Flashing Format) Qué es: Es el formato estándar que 
# entiende el gestor de arranque (bootloader) nativo de la Raspberry Pi Pico.
# Es el archivo que puedes arrastras directamente a la unidad virtual RPI-RP2 
# cuando conectas la Pico manteniendo presionado el botón BOOTSEL.
# -2. El archivo .bin (Binario crudo): Una copia exacta de los datos binarios 
# puros que deben cargarse en la memoria flash del microcontrolador.
# Se utiliza principalmente si estás usando herramientas de depuración avanzadas 
# y hardware externo (como una sonda SWD/JTAG usando OpenOCD, un Raspberry Pi 
# actuando como depurador o un CMSIS-DAP).
# -3. El archivo .hex (Intel HEX): Un formato de archivo de texto estándar para microcontroladores
# que representa la información en hexadecimal junto con sus direcciones de memoria.
# Es útil si utilizas programadores de hardware específicos o si necesitas integrar el binario en 
# sistemas de flasheo industrial o herramientas heredadas de terceros.
# -4. El archivo de desensamblado .dis: Un archivo de texto plano que contiene 
# el código de máquina desensamblado junto con las direcciones de memoria de cada instrucción. 
# Es excelente para depuración a bajo nivel y análisis de rendimiento.
# -5. El archivo de mapa de memoria .map: Un reporte detallado generado por el enlazador (linker)
# que detalla cómo se distribuyó el código y las variables en la memoria RAM y Flash de la Pico.
# Te muestra exactamente cuánta memoria está ocupando cada función (main, librerías del SDK, 
# interrupciones) y cada variable global o estática. Útil para optimizar memoria RAM/Flash.
