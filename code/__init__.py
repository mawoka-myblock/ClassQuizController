# from .captive_portal import CaptivePortal
from .CONFIG import config
from .physical import Physical
import uasyncio as asyncio
# from .startup import connect_to_game
from .captive_portal.credentials import Creds
# from .websocket import main as ws_main
import micropython
from bt import main as bt_main
# import gc


async def main():
    micropython.alloc_emergency_exception_buf(100)
    # portal = CaptivePortal()
    # portal.start()
    phys = Physical(config)
    creds = Creds().load()
    bt_main()
    # session_id = await connect_to_game(phys, creds)
    # gc.collect()
    # await ws_main(phys, creds, session_id)


asyncio.run(main())
