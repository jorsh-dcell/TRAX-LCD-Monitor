import time
import math
import random
import psutil
import serial
import pyaudiowpatch as pyaudio
import numpy as np

# Configura tu puerto COM aquí (ej: 'COM3', 'COM4')
PUERTO_SERIAL = 'COM3'

# Límite ideal calibrado con tus datos reales de la ASUS F16
UMBRAL_SILENCIO = 0.02

volumen_actual = 0.0

try:
    arduino = serial.Serial(PUERTO_SERIAL, 115200, timeout=1)
    time.sleep(2)
    print(f"Conectado al sistema de audio nativo en el puerto {PUERTO_SERIAL}")

    # Inicializamos PyAudio especializado para Windows WASAPI (Loopback)
    p = pyaudio.PyAudio()

    # BUSCADOR CORREGIDO: Buscamos el dispositivo loopback por defecto de Windows
    dispositivo_loopback = p.get_default_wasapi_loopback()
    print(f"[🎯] Capturando audio nativo desde: {dispositivo_loopback['name']}")


    # Abrimos el flujo de audio digital para medir los altavoces de tu ASUS
    def callback_audio(in_data, frame_count, time_info, status):
        global volumen_actual
        # Convertimos los bytes binarios de audio a valores numéricos para medir la potencia
        valores_num = np.frombuffer(in_data, dtype=np.int16)
        if len(valores_num) > 0:
            volumen_actual = np.linalg.norm(valores_num) / np.sqrt(len(valores_num)) / 32768.0
        return (None, pyaudio.paContinue)


    stream = p.open(format=pyaudio.paInt16,
                    channels=dispositivo_loopback["maxInputChannels"],
                    rate=int(dispositivo_loopback["defaultSampleRate"]),
                    input=True,
                    input_device_index=dispositivo_loopback["index"],
                    stream_callback=callback_audio)

    ultimo_envio_sys = 0
    tiempo_ultimo_sonido = time.time()

    while True:
        t = time.time()

        # Filtro de escala: Si es sonido real por encima del umbral corregido de 0.02
        if volumen_actual > UMBRAL_SILENCIO:
            tiempo_ultimo_sonido = t

        # Si ha sonado música en los últimos 1.5 segundos, manda el ecualizador
        if t - tiempo_ultimo_sonido < 1.5:
            bandas = []
            for i in range(16):
                # Maximizamos la sensibilidad visual multiplicando el volumen real de Spotify
                multiplicador = min(2.5, volumen_actual * 40)
                ritmo = math.sin(t * 8 + i * 0.5) * (35 * multiplicador) + math.cos(t * 4 - i * 0.3) * (
                            25 * multiplicador) + 50
                ritmo += random.randint(-10, 10)
                bandas.append(str(int(max(10, min(100, ritmo)))))

            cadena_audio = "AUD:" + ",".join(bandas) + "\n"
            arduino.write(bytes(cadena_audio, 'utf-8'))
            time.sleep(0.04)  # Alta velocidad para que el ecualizador vaya súper fluido

        else:
            # Si hay silencio real, envía el monitor de recursos CPU/RAM
            if t - ultimo_envio_sys > 0.5:
                uso_cpu = int(psutil.cpu_percent(interval=None))
                uso_ram = int(psutil.virtual_memory().percent)
                cadena_sistema = f"SYS:{uso_cpu},{uso_ram}\n"
                arduino.write(bytes(cadena_sistema, 'utf-8'))

                print(f"Monitor Activo (Volumen real: {volumen_actual:.4f}) -> CPU: {uso_cpu}% | RAM: {uso_ram}%")
                ultimo_envio_sys = t
            time.sleep(0.1)

except serial.SerialException:
    print(f"Error: No se pudo conectar al puerto {PUERTO_SERIAL}")
except KeyboardInterrupt:
    print("\nSistema detenido.")
    if 'stream' in locals():
        stream.stop_stream()
        stream.close()
    if 'p' in locals():
        p.terminate()
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
