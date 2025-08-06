##################
## Marcio Bulla ##
##     IEEE     ##
##################

import asyncio
from bleak import BleakScanner, BleakClient
import keyboard

# UUID for the control characteristic
CONTROL_UUID = "c7be25a0-d82d-44e0-8155-3cbac410e2ed"
BUZZER_UUID = "1e29d664-837a-42cd-8051-451e8985c08b"
DEVICE_NAME = "RamoCar"

# Map arrow keys to values
direction_map = {
    "up": "1",
    "down": "-1",
    "left": "-1",
    "right": "1",
}

# Map 'r' and 't' to values
r_map = {
    "r": "-1",
    "t": "1",
}

async def send_command(client, command, UUID):
    await client.write_gatt_char(UUID, command.encode())

async def scan_and_connect():
    devices = await BleakScanner.discover()
    for device in devices:
        if device.name == DEVICE_NAME:
            client = BleakClient(device.address)
            await client.connect()
            if client.is_connected:
                return client
    return None

def build_command(x, y, r):
    return f"x{x}y{y}r{r}"

async def main():
    client = await scan_and_connect()
    if client is None:
        print("Could not connect to device")
        return

    print("Connected. Use arrow keys and 'r'/'t' to build your command.")

    x = "0"
    y = "0"
    r = "0"
    previous_command = build_command(x, y, r)
    previous_buzzer = "00"

    while True:
        # Default values
        new_x = "0"
        new_y = "0"
        new_r = "0"
        buzzer = "00"

        # Check arrow keys
        if keyboard.is_pressed("up"):
            new_x = direction_map["up"]
        elif keyboard.is_pressed("down"):
            new_x = direction_map["down"]

        if keyboard.is_pressed("left"):
            new_y = direction_map["left"]
        elif keyboard.is_pressed("right"):
            new_y = direction_map["right"]

        # Check 'r' and 't'
        if keyboard.is_pressed("r"):
            new_r = r_map["r"]
        elif keyboard.is_pressed("t"):
            new_r = r_map["t"]

        # Build new command
        new_command = build_command(new_x, new_y, new_r)
        if new_command != previous_command:
            await send_command(client, new_command, CONTROL_UUID)
            print(f"Sent command: {new_command}")
            previous_command = new_command

        if keyboard.is_pressed("b"):
            buzzer = "A4"

        if buzzer != previous_buzzer:
            await send_command(client, buzzer, BUZZER_UUID)
            print("BUZINAAAAA")
            previous_buzzer = buzzer

        # Update values
        x = new_x
        y = new_y
        r = new_r

        await asyncio.sleep(0.1)

if __name__ == "__main__":
    asyncio.run(main())
