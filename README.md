# esp-idf
```bash
# https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html

mkdir -p ~/esp
cd ~/esp
git clone -b v5.3.1 --recursive https://github.com/espressif/esp-idf.git

cd ~/esp/esp-idf
./install.sh esp32

. $HOME/esp/esp-idf/export.sh
```

# Flash
```bash
git clone https://github.com/qgerman2/esp-idf-can2mqtt
cd esp-idf-can2mqtt
idf.py set-target esp32
idf.py menuconfig  # Application configuration -> CAN TX RX / WIFI / MQTT BROKER
idf.py flash
```