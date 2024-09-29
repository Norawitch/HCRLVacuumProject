import urequests
import network
from m5stack import *  # Assuming you're using M5Stack libraries for sensors

# Connect to Wi-Fi
wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect('OPPO A54', 'waitforwifi')

# Wait for connection
while not wifi.isconnected():
    pass

print('Wi-Fi connected, IP address:', wifi.ifconfig()[0])

# Initialize the ENV sensor (assuming ENV sensor is connected)
env_sensor = unit.get(unit.ENV, unit.PORTA)

# Get sensor readings
temperature = env_sensor.temperature
humidity = env_sensor.humidity
pressure = env_sensor.pressure

# Define the Node-RED server URL
url = 'http://192.168.134.89:1880/esp32/m5go/sensor/env'

# Data to send
data = {
    "sensor": "env_sensor",
    "temperature": temperature,
    "humidity": humidity,
    "pressure": pressure
}

# Send the POST request with sensor data
response = urequests.post(url, json=data)

# Print the response from Node-RED
print(response.text)

# Close the response
response.close()
