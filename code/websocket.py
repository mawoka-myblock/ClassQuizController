import gc

import time

import os

from .physical import Physical
from .captive_portal.credentials import Creds
import uasyncio as a
from .lib.ws import AsyncWebsocketClient
from .startup import get_pressed_buttons

ws = AsyncWebsocketClient(200)
lock = a.Lock()
data_from_ws = []


async def run_main_loop(phys: Physical):
    print("Create WS instance...")

    initial_value = phys.buttons.red.value()
    pressed_button = None
    buttons = phys.buttons

    def pressed(letter: str):
        nonlocal pressed_button
        print(letter)
        pressed_button = letter

    print("Starting loop")
    activated_buttons = [True, True, True, True]

    while True:
        if ws is None:
            print("ws is None")
            await a.sleep_ms(100)
            continue
        if not await ws.open():
            print("ws is not open")
            await a.sleep_ms(100)
            continue
        if buttons.red.value() != initial_value and activated_buttons[0]:
            activated_buttons[0] = False
            pressed("r")
            await a.sleep_ms(100)
            activated_buttons[0] = True

        if buttons.yellow.value() != initial_value and activated_buttons[1]:
            activated_buttons[1] = False
            pressed("y")
            await a.sleep_ms(100)
            activated_buttons[1] = True

        if buttons.green.value() != initial_value and activated_buttons[2]:
            activated_buttons[2] = False
            pressed("g")
            await a.sleep_ms(100)
            activated_buttons[2] = True

        if buttons.blue.value() != initial_value and activated_buttons[3]:
            activated_buttons[3] = False
            pressed("b")
            await a.sleep_ms(100)
            activated_buttons[3] = True

        if pressed_button is not None:
            print("button pressed", pressed_button)
            await ws.send(f'{{"type": "bp", "data": "{pressed_button}"}}')

        if buttons.red.value() != initial_value and buttons.yellow.value() != initial_value and buttons.green.value() != initial_value and buttons.blue.value() != initial_value:
            break

        pressed_button = None
        # await lock.acquire()
        # if data_from_ws:
        #     print(data_from_ws)
        # lock.release()
        gc.collect()
        await a.sleep_ms(50)
    try:
        await ws.close()
    except:
        pass


async def read_loop(creds: Creds, id: str):
    # may be, it
    while True:
        gc.collect()
        try:
            print("Handshaking...")
            # connect to test socket server with random client number
            if not await ws.handshake(
                    f"{creds.base_url.decode('utf-8').replace('http', 'ws').replace('https', 'wss')}/api/v1/box-controller/embedded/socket/{os.urandom(5).hex()}?game_id={id}"):
                raise Exception('Handshake error.')
            print("...handshaked.")

            mes_count = 0
            while await ws.open():
                data = await ws.recv()
                print("Data: " + str(data) + "; " + str(mes_count))
                # close socket for every 10 messages (even ping/pong)
                if mes_count == 10:
                    await ws.close()
                    print("ws is open: " + str(await ws.open()))
                mes_count += 1

                if data is not None:
                    await lock.acquire()
                    data_from_ws.append(data)
                    lock.release()

                await a.sleep_ms(50)
        except Exception as ex:
            print("Exception: {}".format(ex))
            await a.sleep(1)


async def main(phys: Physical, creds: Creds, id: str):
    tasks = [read_loop(creds, id), run_main_loop(phys)]
    await a.gather(*tasks)
