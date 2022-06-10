import spidev
import time
#Selección de bus SPI y dispositivo esclavo
spi_bus=0
spi_device=0

#Activación SPI
spi = spidev.SpiDev()
spi.open(spi_bus,spi_device)
#1MBps de velocidad de transmisión
spi.max_speed_hz=1000000
send_byte = 0  #Byte enviado al esclavo


#Secuencia de envío
while True:
    spi.xfer2([send_byte])
    send_byte = send_byte + 1
    time.sleep(0.5)  #Delay para el registro de Arduino (pruebas) 
    print ("Dato enviado = " + str(send_byte))
    if send_byte == 255:
        send_byte = 0

