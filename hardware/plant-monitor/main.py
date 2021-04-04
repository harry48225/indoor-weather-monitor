from time import sleep

from grow.moisture import Moisture

moisture_sensors = {"Serrano": Moisture(1), "Jalapeno": Moisture(2), "Cayenne": Moisture(3)}

while True:

    print("\r", end="")
    for sensor in moisture_sensors:

        print(f"{sensor}, {moisture_sensors[sensor].moisture}Hz ", end= "")

    sleep(1)