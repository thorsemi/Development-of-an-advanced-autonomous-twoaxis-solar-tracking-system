import paho.mqtt.client as mqtt
from pysolar.solar import get_altitude, get_azimuth
from datetime import datetime
import pytz  # Tilføjet pytz for tidszonehåndtering

# MQTT Konfiguration
BROKER = "localhost"  # IP-adressen på din MQTT-broker
MQTT_IN_TOPIC = "solartracker/gps"  # Topic for at modtage Latitude og Longitude
MQTT_OUT_TOPIC = "solartracker/output"  # Topic for at sende resultater tilbage

# Variabler til midlertidig lagring af data
latitude = None
longitude = None

# Funktion til at beregne Azimuth og Altitude
def calculate_azimuth_altitude(lat, lon):
    time_now = datetime.now(pytz.utc)  # Gør datetime timezone-aware med pytz
    altitude = float(get_altitude(lat, lon, time_now))  # Konverter til standard float
    azimuth = float(get_azimuth(lat, lon, time_now))    # Konverter til standard float
    return round(altitude, 2), round(azimuth, 2)

# Callback når klienten forbinder til MQTT-broker
def on_connect(client, userdata, flags, rc):
    print("Forbundet til MQTT-broker med statuskode:", rc)
    client.subscribe(MQTT_IN_TOPIC)  # Lyt på input topic

# Callback når der modtages en besked
def on_message(client, userdata, msg):
    global latitude, longitude

    try:
        # Modtag værdien og konverter til float
        value = msg.payload.decode().strip()
        value = float(value.replace(',', '.'))  # Erstat komma med punktum

        # Identificer, om det er Latitude eller Longitude
        if latitude is None:
            latitude = value
            print(f"Latitude modtaget: {latitude}")
        elif longitude is None:
            longitude = value
            print(f"Longitude modtaget: {longitude}")

        # Når begge værdier er modtaget, beregn Azimuth og Altitude
        if latitude is not None and longitude is not None:
            altitude, azimuth = calculate_azimuth_altitude(latitude, longitude)
            print(f"Beregnet - Altitude: {altitude}, Azimuth: {azimuth}")

            # Send resultatet tilbage via MQTT
            result = {"Altitude": altitude, "Azimuth": azimuth}
            client.publish(MQTT_OUT_TOPIC, str(result))
            print("Resultater sendt via MQTT:", result)

            # Nulstil værdier for næste beregning
            latitude, longitude = None, None
    except Exception as e:
        print("Fejl i behandling af besked:", str(e))

# MQTT-klient opsætning
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Forbind til MQTT-broker
client.connect(BROKER, 1883, 60)

# Start MQTT-loop
client.loop_forever()
