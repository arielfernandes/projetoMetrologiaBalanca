from gc import callbacks
import paho.mqtt.client as mqtt
import time

# callBack = ''
class Mqtt:
    def __init__(self):
        self.mqttBroker = "broker.hivemq.com"
        self.callback = None
        
    def on_message(self,client, userdata, message):
        self.callBack = str(message.payload.decode("utf-8"))
        print("received message: ", self.callBack)

    def connect_mqtt(self):
        client = mqtt.Client()
        client.connect(self.mqttBroker)

        client.loop_start()

        client.subscribe("INCB_ESP32_envia_informacao")

        # client.subscribe("TEMPERATURE")
        client.on_message = self.on_message

        time.sleep(1.8)
        client.loop_stop()
        if self.callback == " ":
           return "Error"
        else:
            return self.callBack
if __name__ == '__main__':
    ...