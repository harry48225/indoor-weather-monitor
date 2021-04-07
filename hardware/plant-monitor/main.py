from time import sleep

from grow.moisture import Moisture
from ltr559 import LTR559

import paho.mqtt.client as mqtt



# MQTT setup

MQTT_SERVER = "helevorn.local"

client = mqtt.Client(client_id="plant-monitor")

client.connect(MQTT_SERVER)

moisture_sensors = {"serrano": Moisture(1), "jalapeno": Moisture(2), "cayenne": Moisture(3)}
light = LTR559()

while True:

    try:
        
        print("\r", end="")
        for plant_name in moisture_sensors:

            sensor = moisture_sensors[plant_name]

            if sensor.active and sensor.new_data:
                moisture = round(sensor.moisture, 6) # 6 dp
                print(f"{plant_name}, {moisture}Hz ", end= "")

                # only publish if the reading is non-zero
                # you get zero readings when the sensor is initalising
                client.publish(f"conservatory/plants/moisture/{plant_name}", moisture)
                sleep(0.5)

        light.update_sensor()
        lux = round(light.get_lux())
        print(f" light: {lux}", end ="")

        if lux != 0:
            client.publish("conservatory/plants/light", lux)
        
    except Exception as e:
        print(e)
        print("an error occured")
    finally:
        client.loop()
        sleep(1)