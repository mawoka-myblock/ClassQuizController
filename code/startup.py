# import uaiohttpclient as aiohttp
from .physical import Physical
from .lib import urequests as ureqs
import uasyncio as a
from .captive_portal.credentials import Creds
import time


async def get_pressed_buttons(buttons: Physical.Buttons, initial_value, activated_buttons: list[bool], pressed):
    if buttons.red.value() != initial_value and activated_buttons[0]:
        activated_buttons[0] = False
        await a.sleep_ms(500)
        if buttons.red.value() == initial_value:
            await pressed("r")
        else:
            await pressed("R")
        await a.sleep_ms(200)
        activated_buttons[0] = True
    if buttons.green.value() != initial_value and activated_buttons[1]:
        activated_buttons[1] = False
        await a.sleep_ms(500)
        if buttons.green.value() == initial_value:
            await pressed("g")
        else:
            await pressed("G")
        await a.sleep_ms(200)
        activated_buttons[1] = True
    if buttons.blue.value() != initial_value and activated_buttons[2]:
        activated_buttons[2] = False
        await a.sleep_ms(500)
        if buttons.blue.value() == initial_value:
            await pressed("b")
        else:
            await pressed("B")
        await a.sleep_ms(200)
        activated_buttons[2] = True
    if buttons.yellow.value() != initial_value and activated_buttons[3]:
        activated_buttons[3] = False
        await a.sleep_ms(500)
        if buttons.yellow.value() == initial_value:
            await pressed("y")
        else:
            await pressed("Y")
        await a.sleep_ms(200)
        activated_buttons[3] = True
    # print(buttons.red.value())


async def get_code(phys: Physical) -> str:
    buttons = phys.buttons
    leds = phys.leds
    pressed_buttons = []
    led_dict = {
        "r": leds.red,
        "b": leds.blue,
        "g": leds.green,
        "y": leds.yellow
    }

    async def pressed(letter: str):
        print(letter)
        pressed_buttons.append(letter)
        led = led_dict[letter.lower()]
        if letter.isupper():
            delay = 600
        else:
            delay = 200
        led.on()
        await a.sleep_ms(delay)
        led.off()

    initial_value = buttons.red.value()

    print("Starting loop")
    activated_buttons = [True, True, True, True]
    while True:
        if len(pressed_buttons) >= 6:
            break
        await get_pressed_buttons(buttons=buttons, activated_buttons=activated_buttons, pressed=pressed,
                                  initial_value=initial_value)
        await a.sleep_ms(50)

    return "".join(pressed_buttons)


async def get_session_id(code: str, creds: Creds) -> str | None:
    print("url", f"{creds.base_url.decode('utf-8')}/api/v1/box-controller/embedded/join")
    resp = ureqs.post(f"{creds.base_url.decode('utf-8')}/api/v1/box-controller/embedded/join",
                      json={"code": code, "name": creds.username})
    resp_json = resp.json()
    try:
        return resp_json["id"]
    except KeyError:
        return None


async def connect_to_game(phys: Physical, creds: Creds) -> str:
    """
    :return: The session-id for the game
    """
    session_id = None
    while session_id is None:
        phys.leds.switch_all_on(True)
        await a.sleep_ms(200)
        phys.leds.switch_all_on(False)
        await a.sleep_ms(200)
        phys.leds.switch_all_on(True)
        await a.sleep_ms(200)
        phys.leds.switch_all_on(False)
        phys_code = await get_code(phys)
        session_id = await get_session_id(phys_code, creds)

    return session_id
