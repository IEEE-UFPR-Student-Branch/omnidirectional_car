from kivy.app import App
from kivy.lang import Builder

from kivy.uix.boxlayout import BoxLayout
from kivy.properties import StringProperty

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

        self.root.ids.StickDir.bind(pad=self.on_joystick_moved)
        self.root.ids.StickRot.bind(pad=self.on_joystick_moved)

        return self.root

    def on_joystick_moved(self, instance, value):
        self.root.update_stick_values(self.root.ids.StickDir, self.root.ids.StickRot)

if __name__ == '__main__':
    MyApp().run()
