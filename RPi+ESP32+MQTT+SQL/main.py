import paho.mqtt.client as mqtt
import sqlite3
import json
from time import time
 
MQTT_HOST = '192.168.0.107'
MQTT_PORT = 1883
MQTT_CLIENT_ID = 'Python MQTT client'
MQTT_USER = 'carlos'
MQTT_PASSWORD = 'facendo40'
TOPIC = 'home/energy/sensor'
 
DATABASE_FILE = 'mqtt.db'
 
 
def on_connect(mqtt_client, user_data, flags, conn_result):
    mqtt_client.subscribe(TOPIC)
 
 
def on_message(mqtt_client, user_data, message):
    payload = message.payload.decode('utf-8')
    variables=json.loads(payload)
    
    V=str(variables["V1"])
    I=str(variables["I1"])
    AP=str(variables["AP"])
    PF=str(variables["PF"])
    
    db_conn = user_data['db_conn'] #Conexion
    cursor = db_conn.cursor() #Inicio cursor
    cursor.execute("INSERT INTO datos_electricos (tension,intensidad,potencia,factor) VALUES (" + V + "," + I + "," + AP + "," + PF + ")")
    db_conn.commit() #Envio de la instruccion SQL
    cursor.close() #Cierre cursor
 
def main():
    db_conn = sqlite3.connect(DATABASE_FILE)
    sql = """
    CREATE TABLE IF NOT EXISTS datos_electricos (   
        tension FLOAT,
        intensidad FLOAT,
        potencia FLOAT,
        factor FLOAT
    )
    """
    cursor = db_conn.cursor()
    cursor.execute(sql)
    cursor.close()
 
    mqtt_client = mqtt.Client(MQTT_CLIENT_ID)
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.user_data_set({'db_conn': db_conn})
 
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
 
    mqtt_client.connect(MQTT_HOST, MQTT_PORT)
    mqtt_client.loop_forever()
 
 
main()