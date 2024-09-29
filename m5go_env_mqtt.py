import network
import time
import umqtt.simple as mqtt
from m5stack import *
from m5stack import imu
from m5stack import mpu6886

# Wifi credentials
ssid = "OPPO A54"
password = "waitforwifi"

mqtt_server = "e87ade77417e4192b935704fa90aa325.s1.eu.hivemq.cloud"
mqtt_port = 8883
mqtt_user = "photo_mqtt1"
mqtt_password = "PHOTO_mqtt1"
mqtt_topic_env = "esp32/m5go/sensor/env"
mqtt_topic_accele = "esp32/m5go/sensor/accelero"

# Connect Wifi
Wifi = network.WLAN(network.STA_IF)
Wifi.active(True)
Wifi.connect(ssid, password)

while not Wifi.isconnected():
    pass

print('Wi-Fi connected, IP address:', Wifi.ifconfig()[0])

client = mqtt.MQTTClient("M5GOClient", mqtt_server, port=mqtt_port, user=mqtt_user, password=mqtt_password, ssl=True)
client.connect()

mpu6886.MPU6886().begin()
env_sensor = mpu6886.MPU6886()

while True:
    # ENV sensor reading
    temperature = env_sensor.temperature
    humidity = env_sensor.humidity
    pressure = env_sensor.pressure
    # Create payload
    payload_env = {
        "temperature": temperature,
        "humidity": humidity,
        "pressure": pressure
    }

    # Accelerometer reading
    accel_x, accel_y, accel_z = imu.IMU().acceleration
    # Create payload
    payload_accelero = {
        "accel_x": accel_x,
        "accel_y": accel_y,
        "accel_z": accel_z
    }


    client.publish(mqtt_topic_env, str(payload_env))
    client.publish(mqtt_topic_accele, str(payload_accelero))
    print(f"ENV Data sent: {payload_env}")
    print(f"Accelerometer Data sent: {payload_accelero}")

    # Wait 5 seconds
    time.sleep(5)