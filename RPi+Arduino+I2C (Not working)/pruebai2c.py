import smbus
import time
#Dirección del esclavo I2C
I2C_ADDRESS = 0x08 
#Bus número 1
bus=smbus.SMBus(1) 
#Establecer los puertos como entradas
bus.write_byte(I2C_ADDRESS, 0xFF)
#Lecturas cada segundo de las entradas
for i in range (1,255):
    value=bus.read_byte(I2C_ADDRESS)
    print ("Dato leido = " + str(value))
    time.sleep(1)

