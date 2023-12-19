# esp-idf-can2mqtt
CANbus to mqtt bridge using esp32.   
It's purpose is to be a bridge between a CAN-Bus and a MQTT-Broker.    
You can map CAN-ID to MQTT-Topics and map each payload to a message.   
I inspired from [here](https://github.com/c3re/can2mqtt).   

![can2mqtt](https://user-images.githubusercontent.com/6020549/123542717-20c06000-d786-11eb-9938-65af6b57fa94.jpg)

I quoted this image from [here](http://www.adfweb.com/download/filefold/MN67939_ENG.pdf).

You can visualize CAN-Frame using a JavaScript library such as Epoch.   

![slide0001](https://user-images.githubusercontent.com/6020549/124378463-603dfd80-dcec-11eb-8325-12e3b1fd6ad2.jpg)


# Software requirement
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.1 is required when using ESP32C6.   

# Hardware requirements
- SN65HVD23x CAN-BUS Transceiver   
SN65HVD23x series has 230/231/232.   
They differ in standby/sleep mode functionality.   
Other features are the same.   

- Termination resistance   
I used 150 ohms.   

# Wireing   
|SN65HVD23x||ESP32|ESP32-S2/S3|ESP32-C3/C6||
|:-:|:-:|:-:|:-:|:-:|:-:|
|D(CTX)|--|GPIO21|GPIO17|GPIO0|(*1)|
|GND|--|GND|GND|GND||
|Vcc|--|3.3V|3.3V|3.3V||
|R(CRX)|--|GPIO22|GPIO18|GPIO1|(*1)|
|Vref|--|N/C|N/C|N/C||
|CANL|--||||To CAN Bus|
|CANH|--||||To CAN Bus|
|RS|--|GND|GND|GND|(*2)|

(*1) You can change using menuconfig. But it may not work with other GPIOs.  

(*2) N/C for SN65HVD232



# Test Circuit   
```
   +-----------+   +-----------+   +-----------+ 
   | Atmega328 |   | Atmega328 |   |   ESP32   | 
   |           |   |           |   |           | 
   | Transmit  |   | Receive   |   | 21    22  | 
   +-----------+   +-----------+   +-----------+ 
     |       |      |        |       |       |   
   +-----------+   +-----------+     |       |   
   |           |   |           |     |       |   
   |  MCP2515  |   |  MCP2515  |     |       |   
   |           |   |           |     |       |   
   +-----------+   +-----------+     |       |   
     |      |        |      |        |       |   
   +-----------+   +-----------+   +-----------+ 
   |           |   |           |   | D       R | 
   |  MCP2551  |   |  MCP2551  |   |   VP230   | 
   | H      L  |   | H      L  |   | H       L | 
   +-----------+   +-----------+   +-----------+ 
     |       |       |       |       |       |   
     +--^^^--+       |       |       +--^^^--+
     |   R1  |       |       |       |   R2  |   
 |---+-------|-------+-------|-------+-------|---| BackBorn H
             |               |               |
             |               |               |
             |               |               |
 |-----------+---------------+---------------+---| BackBorn L

      +--^^^--+:Terminaror register
      R1:120 ohms
      R2:150 ohms(Not working at 120 ohms)
```

__NOTE__   
3V CAN Trasnceviers like VP230 are fully interoperable with 5V CAN trasnceviers like MCP2551.   
Check [here](http://www.ti.com/lit/an/slla337/slla337.pdf).


# Installation
```
git clone https://github.com/nopnop2002/esp-idf-can2mqtt
cd esp-idf-can2mqtt
chmod 777 getpem.sh
./getpem.sh
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```


# Configuration
![config-main](https://user-images.githubusercontent.com/6020549/123541714-dbe5fa80-d780-11eb-85da-648c201b9a9c.jpg)
![config-app](https://user-images.githubusercontent.com/6020549/123541716-df798180-d780-11eb-82d4-78b82b8fb3b1.jpg)

## CAN Setting
![config-can](https://user-images.githubusercontent.com/6020549/123541727-ebfdda00-d780-11eb-9c83-3f01db84e339.jpg)

## WiFi Setting
![config-wifi](https://user-images.githubusercontent.com/6020549/123541729-f4eeab80-d780-11eb-90b9-f9583764acb8.jpg)

## MQTT Server Setting

MQTT broker is specified by one of the following.
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```mqtt-broker.local```   
- Fully Qualified Domain Name   
 ```broker.emqx.io```

You can download the MQTT broker from [here](https://github.com/nopnop2002/esp-idf-mqtt-broker).   

![config-mqtt-1](https://github.com/nopnop2002/esp-idf-can2mqtt/assets/6020549/f06f22bb-fae2-4339-ba4b-8798e90f4bdb)

Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   

![config-mqtt-2](https://github.com/nopnop2002/esp-idf-can2mqtt/assets/6020549/b2ca49af-a0d5-4183-b57e-70616e108590)

You can use a secure MQTT server.   
A secure MQTT server uses the MQTTS protocol instead of the MQTT protocol.   
The default secure MQTT server is ```mqtt.eclipseprojects.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
![config-mqtt-11](https://github.com/nopnop2002/esp-idf-can2mqtt/assets/6020549/8c80e868-18bb-4dc6-b026-4cbcfb95a533)


# Definition from CANbus to MQTT
When CANbus data is received, it is sent by MQTT according to csv/can2mqtt.csv.   
The file can2mqtt.csv has three columns.   
In the first column you need to specify the CAN Frame type.   
The CAN frame type is either S(Standard frame) or E(Extended frame).   
In the second column you have to specify the CAN-ID as a __hexdecimal number__.    
In the last column you have to specify the MQTT-Topic.   
Each CAN-ID and each MQTT-Topic is allowed to appear only once in the whole file.   

```
S,101,/can/std/101
E,101,/can/ext/101
S,103,/can/std/103
E,103,/can/ext/103
```

When a Standard CAN frame with ID 0x101 is received, it is sent by TOPIC of "/can/std/101".   
When a Extended CAN frame with ID 0x101 is received, it is sent by TOPIC of "/can/ext/101".   


# Definition from MQTT to CANbus
When MQTT data is received, it is sent by CANbus according to csv/mqtt2can.csv.   
Same format as can2mqtt.csv.   
```
S,201,/can/std/201
E,201,/can/ext/201
S,203,/can/std/203
E,203,/can/ext/203
```

When receiving the TOPIC of "/can/std/201", send the Standard CAN frame with ID 0x201.   
When receiving the TOPIC of "/can/ext/201", send the Extended CAN frame with ID 0x201.   


# Receive MQTT data using mosquitto_sub
```mosquitto_sub -h broker.emqx.io -p 1883 -t '/can/#' -F %X -d```

1011121314151617 indicates 8 bytes of CAN-BUS data at 0x11-0x12-0x13-0x14-0x15-0x16-017.   

![can2mqtt-1](https://user-images.githubusercontent.com/6020549/123541739-0637b800-d781-11eb-9e4d-1645cfdd28f1.jpg)

# Transmit MQTT data using mosquitto_pub
- Send standard frame data with CANID = 0x201.   
```echo -ne "\x01\x02\x03" | mosquitto_pub -h broker.emqx.io -p 1883 -t '/can/std/201' -s```

Receive CANbus using UNO.   
![can2mqtt-12](https://user-images.githubusercontent.com/6020549/123543372-06d44c80-d789-11eb-80a3-411781a8519d.jpg)

- Send extended frame data with CANID = 0x201.   
```echo -ne "\x11\x12\x13" | mosquitto_pub -h broker.emqx.io -p 1883 -t '/can/ext/201' -s```

Receive CANbus using UNO.   
![can2mqtt-14](https://user-images.githubusercontent.com/6020549/123543408-36835480-d789-11eb-9ce8-ac90216fc247.jpg)

# Receive MQTT data using python
```
python3 -m pip install -U paho-mqtt
python3 mqtt_sub.py
```

![python-screen](https://github.com/nopnop2002/esp-idf-can2mqtt/assets/6020549/d04bc287-4092-4808-a46a-919c191fc1b7)


# MQTT client Example
Example code in various languages.   
https://github.com/emqx/MQTT-Client-Examples


# Visualize CAN-Frame
## Using python
There is a lot of information on the internet about the Python + visualization library.   
- [matplotlib](https://matplotlib.org/)
- [seaborn](https://seaborn.pydata.org/index.html)
- [bokeh](https://bokeh.org/)
- [plotly](https://plotly.com/python/)

## Using node.js
There is a lot of information on the internet about the node.js + __real time__ visualization library.   
- [epoch](https://epochjs.github.io/epoch/real-time/)
- [plotly](https://plotly.com/javascript/streaming/)
- [chartjs-plugin-streaming](https://nagix.github.io/chartjs-plugin-streaming/1.9.0/)

## Using postman application
![postman](https://github.com/nopnop2002/esp-idf-can2mqtt/assets/6020549/8b917332-9726-4375-ac14-488c0d176aa4)
Postman supports MQTT.   
[Here's](https://blog.postman.com/postman-supports-mqtt-apis/) how to get started with MQTT with Postman.   


# Troubleshooting   
There is a module of SN65HVD230 like this.   
![SN65HVD230-1](https://user-images.githubusercontent.com/6020549/80897499-4d204e00-8d34-11ea-80c9-3dc41b1addab.JPG)

There is a __120 ohms__ terminating resistor on the left side.   
![SN65HVD230-22](https://user-images.githubusercontent.com/6020549/89281044-74185400-d684-11ea-9f55-830e0e9e6424.JPG)

I have removed the terminating resistor.   
And I used a external resistance of __150 ohms__.   
A transmission fail is fixed.   
![SN65HVD230-33](https://user-images.githubusercontent.com/6020549/89280710-f7857580-d683-11ea-9b36-12e36910e7d9.JPG)

If the transmission fails, these are the possible causes.   
- There is no receiving app on CanBus.
- The speed does not match the receiver.
- There is no terminating resistor on the CanBus.
- There are three terminating resistors on the CanBus.
- The resistance value of the terminating resistor is incorrect.
- Stub length in CAN bus is too long. See [here](https://e2e.ti.com/support/interface-group/interface/f/interface-forum/378932/iso1050-can-bus-stub-length).

# Reference
https://github.com/nopnop2002/esp-idf-candump

https://github.com/nopnop2002/esp-idf-can2http

https://github.com/nopnop2002/esp-idf-can2usb

https://github.com/nopnop2002/esp-idf-can2websocket

https://github.com/nopnop2002/esp-idf-CANBus-Monitor

https://github.com/nopnop2002/esp-idf-mqtt-client
