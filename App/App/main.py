from kivy.app import App
from kivy.lang import Builder
import asyncio
from bleak import BleakScanner, BleakClient
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import StringProperty
from threading import Thread

Builder.load_file("style.kv")

class MainMenu(BoxLayout):
    stick_values = StringProperty("Direction: (0, 0)\nRotation: (0, 0)")
   
    def update_stick_values(self, joystick_dir, joystick_rot):
        dir_value = f"Direction: ({joystick_dir.pad_x:.2f}, {joystick_dir.pad_y:.2f})"
        rot_value = f"Rotation: ({joystick_rot.pad_x:.2f}, {joystick_rot.pad_y:.2f})"
        self.stick_values = f"{dir_value}\n{rot_value}"

class MyApp(App):
    def build(self):
        self.root = MainMenu()
        self.last_send_time = 0
        
        loop_thread = Thread(target=self.run_asyncio_loop, daemon=True)
        loop_thread.start()

        self.root.ids.StickDir.bind(pad=self.on_joystick_moved)
        self.root.ids.StickRot.bind(pad=self.on_joystick_moved)
        return self.root

    def on_joystick_moved(self, instance, value):
        if hasattr(self, 'loop') and self.loop.is_running():
            asyncio.run_coroutine_threadsafe(self.send_coordinates(), self.loop)
        self.root.update_stick_values(self.root.ids.StickDir, self.root.ids.StickRot)

    async def send_coordinates(self):
        current_time = asyncio.get_event_loop().time()
        if current_time - self.last_send_time < 0.5:
            return

        command = f"x{self.root.ids.StickDir.pad_x:.2f}y{self.root.ids.StickDir.pad_y:.2f}r{self.root.ids.StickRot.pad_x:.2f}"
        if self.client and self.client.is_connected:
            try:
                await self.client.write_gatt_char("c7be25a0-d82d-44e0-8155-3cbac410e2ed", command.encode())
                print(f"Command '{command}' sent")
            except Exception as e:
                print(f"Failed to send command: {e}")
        else:
            print("BLE client not connected")
        
        self.last_send_time = current_time

    async def keep_alive(self):
        while self.client and self.client.is_connected:
            await asyncio.sleep(5)
            await self.send_coordinates()  # Send a periodic keep-alive command to the BLE device

    async def connect_ble_client(self):
        print("Scanning for BLE devices...")
        devices = await BleakScanner.discover()
        for device in devices:
            print(f"Found device: {device.name} ({device.address})")
            if device.name == "RamoCar":
                self.client = BleakClient(device)
                try:
                    await self.client.connect()
                    print("Connected to BLE device")
                    asyncio.create_task(self.keep_alive())
                except Exception as e:
                    print(f"Failed to connect: {e}")
                return
        self.client = None
        print("BLE device not found")

    def run_asyncio_loop(self):
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)
        self.loop.run_until_complete(self.connect_ble_client())
        self.loop.run_forever()

if __name__ == '__main__':
    MyApp().run()

