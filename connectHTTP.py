import urequests
import network

# Connect to Wi-Fi
wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect('OPPO A54', 'waitforwifi')

# Wait for connection
while not wifi.isconnected():
    pass

print('Wi-Fi connected, IP address:', wifi.ifconfig()[0])

# Define the Node-RED server URL (replace with your actual server IP or hostname)
url = 'http://192.168.134.89:1880/location'

# Example data to send (e.g., sensor values)
data = {
    "sensor": "accelerometer",
    "x": 0.12,
    "y": 0.34,
    "z": 0.56
}

# Send the POST request
response = urequests.post(url, json=data)

# Print the response from Node-RED
print(response.text)

# Close the response
response.close()
