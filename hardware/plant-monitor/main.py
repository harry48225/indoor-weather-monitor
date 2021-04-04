from time import sleep

from grow.moisture import Moisture
from ltr559 import LTR559

import paho.mqtt.client as mqtt



# MQTT setup

client = mqtt.Client(client_id="plant-monitor")
client.connect("helevorn.local")


moisture_sensors = {"serrano": Moisture(1), "jalapeno": Moisture(2), "cayenne": Moisture(3)}
light = LTR559()

sleep(5)

while True:

    try:
        
        print("\r", end="")
        for sensor in moisture_sensors:

            moisture = round(moisture_sensors[sensor].moisture, 5) # 5 dp
            print(f"{sensor}, {moisture}Hz ", end= "")

            # only publish if the reading is non-zero
            # you get zero readings when the sensor is initalising
        
            if moisture != 0:
                client.publish(f"conservatory/plants/moisture/{sensor}", moisture)

        light.update_sensor()
        lux = round(light.get_lux())
        print(f" light: {lux}", end ="")

        if lux != 0:
            client.publish("conservatory/plants/light", lux)
        
    except :
        print("an error occured")
    finally:
        client.loop()
        sleep(1)