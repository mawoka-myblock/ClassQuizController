import uasyncio as asyncio

from machine import Pin


class Physical:
    class Buttons:
        def __init__(self, config):
            self.yellow = Pin(config["yellow"], Pin.IN, Pin.PULL_UP)
            self.green = Pin(config["green"], Pin.IN, Pin.PULL_UP)
            self.red = Pin(config["red"], Pin.IN, Pin.PULL_UP)
            self.blue = Pin(config["blue"], Pin.IN, Pin.PULL_UP)

    class Leds:
        def __init__(self, config):
            self.yellow = Pin(config["yellow"], Pin.OUT)
            self.green = Pin(config["green"], Pin.OUT)
            self.red = Pin(config["red"], Pin.OUT)
            self.blue = Pin(config["blue"], Pin.OUT)
            self.all = [self.yellow, self.green, self.red, self.blue]

        def switch_all_on(self, on: bool):
            for i in self.all:
                if on:
                    i.on()
                else:
                    i.off()

        async def loading(self):
            while True:
                self.blue.on()
                await asyncio.sleep_ms(100)
                self.blue.off()
                self.green.on()
                await asyncio.sleep_ms(100)
                self.green.off()
                self.yellow.on()
                await asyncio.sleep_ms(100)
                self.yellow.off()
                self.red.on()
                await asyncio.sleep_ms(100)
                self.red.off()
                self.yellow.on()
                await asyncio.sleep_ms(100)
                self.yellow.off()
                self.green.on()
                await asyncio.sleep_ms(100)
                self.green.off()

    def __init__(self, config):
        self.leds = self.Leds(config["leds"])
        self.buttons = self.Buttons(config["buttons"])
        self.leds.switch_all_on(False)
