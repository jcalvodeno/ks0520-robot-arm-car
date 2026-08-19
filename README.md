# Keyestudio KS0520 — 4DOF Mechanical Robot Arm Car

A personal, organized rebuild of the official Keyestudio documentation for the **KS0520 4DOF Mechanical Robot Arm Car** kit — a 3-in-1 Arduino educational kit that assembles into a smart car, a 4-DOF robot arm, or both combined. The stock docs bundle everything into one giant, inconsistently-formatted 4000-line file; this repo splits it into a clean per-project guide with the matching sketches ready to open and upload.

Original vendor sources: [Keyestudio Wiki](https://wiki.keyestudio.com/KS0520_KEYESTUDIO_4DOF_Mechanical_Robot_Arm_Car_Learning_Kit) · [docs.keyestudio.com](https://docs.keyestudio.com/projects/KS0520/en/latest/docs/KS0520.html) · [GitHub source](https://github.com/keyestudio/KS0520-4DOF-Mechanical-Robot-Arm-Car) · [App/code/library download](https://fs.keyestudio.com/KS0520)

## Overview

The kit builds three ways:

- **Car only** — 2WD chassis + ultrasonic sensor, driven manually or autonomously.
- **Arm only** — 4-DOF metal arm (base rotate, shoulder, elbow, claw) on a fixed stand.
- **Combined** — the arm mounted on the car, controlled simultaneously.

Control paths: a wired-in **HM-10 Bluetooth 4.0** module paired with the *"keyes arm"* phone app, or a **PS2 wireless 2.4G controller**. Two autonomous modes exist on the car: obstacle avoidance and ultrasonic follow.

**Specs**: 5V logic / 7–12V input, TB6612FNG dual motor driver, 4× MG90S 14g servos, HC-SR04 ultrasonic sensor (2–400cm, <15° beam), Bluetooth range 20–50m.

## Repo layout

```
projects/<NN>_<name>/lesson_X.Y_<name>/*.ino   20 numbered projects (plus a Project 0 blank/stop sketch), each folder = one sketch to open in Arduino IDE
libraries/PS2X_lib/                             PS2 controller library (not bundled with Arduino IDE — must be installed)
libraries/Servo/                                Standard Arduino Servo library (Arduino IDE already ships this; included for reference/offline use)
```

## Getting started

1. Install the Arduino IDE and the CP2102 USB driver the KEYESTUDIO V4.0 board (Uno-compatible, ATmega328P) needs to show up as a serial port. Guide: [getting-started-with-arduino.readthedocs.io](https://getting-started-with-arduino.readthedocs.io/en/latest/Arduino%20IDE%20Tutorial.html).
2. Install `libraries/PS2X_lib` into your Arduino `libraries/` folder (Sketch → Include Library → Add .ZIP Library, or copy the folder directly). The `Servo` library ships with the IDE already — no action needed unless you want the exact bundled version.
3. In the IDE: **Tools → Board → Arduino Uno**, **Tools → Port → (your COM port)**.
4. For any project below: open its `.ino`, upload, then follow that project's "Run it" steps.
5. **Before uploading anything**, unplug the Bluetooth module from the shield — it shares the same TX/RX lines used to program the board and will block uploads if left connected.

## Building from VS Code instead of the Arduino IDE

You don't need the Arduino IDE app itself — its compiler/uploader toolchain (`arduino-cli`) can be driven from VS Code directly.

### One-time setup

1. **Install the CP2102 driver.** Silicon Labs' download ([silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers)) is driver files only, no `setup.exe`. Install it either by:
   - Right-clicking `silabser.inf` inside the extracted folder → **Install**, or
   - From an elevated PowerShell: `pnputil /add-driver "<path>\silabser.inf" /install`
2. **Install `arduino-cli`**: `winget install --id ArduinoSA.CLI -e`
3. **Install the VS Code extensions**:
   ```
   code --install-extension vscode-arduino.vscode-arduino-community
   code --install-extension ms-vscode.vscode-serial-monitor
   ```
   (The original Microsoft `vscode-arduino` extension was removed from the Marketplace in 2024 — `vscode-arduino-community` is the maintained fork.)

   By default the extension looks for the classic Arduino IDE's `arduino_debug.exe`, which isn't installed here — point it at `arduino-cli` instead via `.vscode/settings.json`:
   ```json
   {
       "arduino.useArduinoCli": true,
       "arduino.path": "C:\\Program Files\\Arduino CLI",
       "arduino.commandPath": "arduino-cli.exe"
   }
   ```
   (Without this, Verify/Upload fail with `spawn arduino_debug.exe ENOENT`.) Reload the VS Code window after changing it.
4. **Install the AVR board core** (targets the ATmega328P this board uses):
   ```
   arduino-cli config init
   arduino-cli core update-index
   arduino-cli core install arduino:avr
   ```
5. **Install `libraries/PS2X_lib` and `libraries/Servo`** into `arduino-cli`'s sketchbook so any project can `#include` them. Unlike the classic Arduino IDE, `arduino-cli`'s AVR core does **not** bundle `Servo` — without this step, any sketch using it (Projects 3, 14, 15+) fails with `Servo.h: No such file or directory`.
   ```
   mkdir "$env:USERPROFILE\Documents\Arduino\libraries"
   Copy-Item -Recurse libraries\PS2X_lib "$env:USERPROFILE\Documents\Arduino\libraries\"
   Copy-Item -Recurse libraries\Servo "$env:USERPROFILE\Documents\Arduino\libraries\"
   ```

### Per-project workflow

Each project folder gets its own `.vscode/arduino.json` pointing at the sketch to build:
```json
{
    "sketch": "projects/01_led_light/lesson_1.1_Blink/lesson_1.1_Blink.ino",
    "board": "arduino:avr:uno",
    "port": "COM3",
    "output": "build"
}
```
- `board` is always `arduino:avr:uno` for this kit's V4.0 board (Uno-compatible).
- To find your `port`: plug the board in, then in PowerShell run
  ```
  Get-CimInstance -ClassName Win32_PnPEntity | Where-Object { $_.Name -match 'COM\d+' } | Select-Object Name, Status
  ```
  It should list `Silicon Labs CP210x USB to UART Bridge (COMx)` — that's your port.

Then either:
- **Via the extension**: open the sketch's `.ino`, click the upload icon (checkmark→arrow, top-right) or press `Ctrl+Alt+U`. It reads `board`/`port`/`sketch` straight from `arduino.json` above.
- **Via `arduino-cli` directly** (what the extension runs under the hood — useful for seeing exact pass/fail output instead of a GUI spinner):
  ```
  arduino-cli compile --fqbn arduino:avr:uno "projects\01_led_light\lesson_1.1_Blink"
  arduino-cli upload -p COM3 --fqbn arduino:avr:uno "projects\01_led_light\lesson_1.1_Blink"
  ```
  A successful upload ends with `Writing | ##...100% | N bytes of flash written`.

### Switching between projects

Three config files live in `.vscode/` — only one of them changes per project:

| File | Changes per project? | What to do |
|---|---|---|
| `arduino.json` | **Yes** | Edit `"sketch"` to the new project's `.ino` path. `board` and `port` stay the same unless you change hardware. |
| `c_cpp_properties.json` | No manual edit | Regenerates itself the first time you run **Verify** on the new sketch — the extension re-derives include paths/defines from the actual build. IntelliSense may show stale red squiggles for a few seconds until that finishes. |
| `settings.json` | No | One-time global setting (`arduino.path` etc.) — set once, ignore afterwards. |

So the only manual step when moving to the next lesson is updating `arduino.json`'s `"sketch"` field, then running Verify once to let IntelliSense catch up.

**Faster way — the "Switch Arduino Project" task**: `Ctrl+Shift+P` → **Tasks: Run Task** → **Switch Arduino Project** → pick a project from the list. This runs `.vscode/switch-project.ps1`, which rewrites `arduino.json`'s `"sketch"` field for you (preserving `board`/`port`) — no manual JSON editing needed. Then Verify/Upload as usual.

### Project 0 — Blank (stopping the board)

There's no software "pause" for a flashed sketch — it runs forever until you flash something else or cut power. `projects/00_blank/lesson_0_blank/lesson_0_blank.ino` is an empty `setup()`/`loop()` sketch for exactly this: pick **"0 — Blank (stop the board)"** from the project switcher above, Upload, and whatever the board was doing (blinking, spinning motors, sweeping servos) stops — while staying connected and ready for the next project.

## Global pinout reference

These pin assignments are fixed across every sketch that uses that subsystem — wire once, reuse for every later project:

| Subsystem | Pin(s) |
|---|---|
| LED module | `S` → D6 |
| Motor driver (car) | `AIN2`→D2, `PWMA`→D3, `BIN2`→D4, `PWMB`→D5 |
| Ultrasonic sensor (HC-SR04) | `Trig`→A4, `Echo`→A3 |
| Arm servos | Servo1 (base)→A1, Servo2 (shoulder)→A0, Servo3 (elbow)→D8, Servo4 (claw)→D9 |
| PS2 receiver | `DAT`→D12, `CMD`→D11, `SEL`→D10, `CLK`→D13 |
| HM-10 Bluetooth | `TXD`→shield `RXD`, `RXD`→shield `TXD`, `VCC`→`VCC`, `GND`→`GND` (do **not** insert reversed) |

## Bluetooth app setup (needed for projects 5, 11, 12, 15, 16, 20)

1. iOS: search **"keyes arm"** in the App Store. Android: search **"keyes arm car"** in Google Play.
2. Grant the app **Location** permission — pairing silently fails without it.
3. Turn on phone Bluetooth, open the app, tap Connect, select the device named **"HMSoft"**.

| App key | Function |
|---|---|
| Forward / Back / Left / Right | drive while held, stop on release |
| Accelerate `a` / Decelerate `d` | ramp car speed up/down |
| Open `V` / Close `P` claw | claw servo |
| Small arm up `Q` / down `E` | shoulder servo |
| Big arm forward `f` / back `b` | elbow servo |
| Base left `l` / right `r` | base-rotation servo |
| Record `t` / Execute `i` | save current arm pose / replay saved sequence |
| `Y` ... `S` | start / stop autonomous obstacle avoidance |
| `U` ... `S` | start / stop autonomous ultrasonic follow |

## Part A — Electronics basics (build on the bare board, before assembly)

### Project 1 — LED Light
**Goal:** blink an LED; learn `pinMode`/`digitalWrite` and how `delay()` sets blink rate.
**Wiring:** LED module `−`/`+`/`S` → shield `G`/`V`/D6.
**Run it:** upload `projects/01_led_light/lesson_1.1_Blink` → LED blinks every 1s. Try `lesson_1.2_Blink` for a 0.1s fast blink.

### Project 2 — Adjust LED Brightness
**Goal:** PWM-fade the LED ("breathing" effect) with `analogWrite`.
**Wiring:** same as Project 1 (`S`→D6).
**Run it:** upload `projects/02_led_brightness_pwm/lesson_2.2_pwm` → brightness ramps up and down smoothly on a loop. (`lesson_2.1_pwm` is the intro variant.)

### Project 3 — Servo Control
**Goal:** sweep a servo 0°↔180°; learn pulse-width control, both manual and via `Servo.h`.
**Wiring:** servo brown/red/orange → `G`/`V`/D9.
**⚠️ Power the servo from the external battery pack, not just USB** — the vendor doc explicitly warns the onboard supply can be damaged driving a servo directly.
**Run it:** `lesson_3.1_Servo` (raw pulses, no library) or `lesson_3.2_servo` (uses `Servo.h`) → servo sweeps continuously.

### Project 4 — Ultrasonic Sensor
**Goal:** measure distance with the HC-SR04; learn `pulseIn()`.
**Wiring:** `VCC`→`V`, `Trig`→A4, `Echo`→A3, `GND`→`G`.
**Run it:** `projects/04_ultrasonic_sensor/lesson_4.1_Ultrasonic_sensor` → open Serial Monitor at 9600 baud, see live distance in cm/inch. Extension `lesson_4.2_Ultrasonic_LED` also wires the LED to D6 — it lights when an object is 2–10cm away.

### Project 5 — Bluetooth Module Test
**Goal:** verify the HM-10 module and learn the app pairing flow (see [Bluetooth app setup](#bluetooth-app-setup-needed-for-projects-5-11-12-15-16-20) above).
**Wiring:** Bluetooth `TXD`/`RXD`/`VCC`/`GND` → shield `RXD`/`TXD`/`VCC`/`GND`.
**Run it:** `projects/05_bluetooth_module_test/lesson_5_Bluetooth` → pair via the app, send a character, watch it echo on the Serial Monitor.

### Project 6 — Motor Driving and Speed Control
**Goal:** open-loop test of the TB6612FNG driver; establishes the motor pinout every car project reuses.
**Wiring:** `AIN2`→D2, `PWMA`→D3, `BIN2`→D4, `PWMB`→D5; both DC motors into the shield's motor terminals.
**Run it:** `projects/06_motor_driving_speed/lesson_6_motor_driver_shield` → motors run forward 1s, reverse 1s, then demo a left turn, right turn, and stop, looping.

### Project 7 — PS2 Joypad Test
**Goal:** confirm the PS2 receiver wiring and read raw stick/button values.
**Wiring:** `DAT`→D12, `CMD`→D11, `SEL`→D10, `CLK`→D13.
**Run it:** `projects/07_ps2_controller_test/lesson_7_PS2` → Serial Monitor at **57600 baud** shows button/stick values; pressing X triggers rumble.

## Part B — Build and drive the car

### Project 8 — Assemble the Smart Car Chassis
No code — pure mechanical build. Order: peel the protective film off the baseplate → mount the wheels → assemble the ultrasonic sensor's pan bracket → screw in the copper standoffs → mount the V4.0 board → mount the battery holder (2-slot 18650 **or** 6-slot AA — pick one) and the motor driver shield on top → wire the two motors into the shield → wire the ultrasonic sensor → connect the battery. Result: the "Turtle Smart Car."

### Project 9 — Ultrasonic Follow Robot
**Goal:** autonomous following — drives forward at 20–35cm from an obstacle, backs off under 15cm, otherwise stops.
**Wiring:** motor pins as Project 6 + ultrasonic pins as Project 4.
**Setup:** after powering on, **set the shield's power DIP switch to ON** or the motors won't run.
**Run it:** `projects/09_ultrasonic_follow_robot/lesson_9.1_Ultrasonic_Follow_Robot` → wave a hand in front; the car tracks it.

### Project 10 — Ultrasonic Avoiding Robot
**Goal:** autonomous obstacle avoidance — reverses under 18cm, turns right at 18–30cm, drives forward beyond that.
**Wiring/setup:** identical to Project 9 (same DIP switch step).
**Run it:** `projects/10_ultrasonic_avoiding_robot/lesson_10.1_Ultrasonic_avoiding_robot` → car drives and steers away from obstacles automatically.
*(Note: the sketch's own header comment mislabels this "lesson 9.1" — a vendor doc typo, not a wiring issue.)*

### Project 11 — Bluetooth Control Car
**Goal:** drive the car live from the phone app.
**Wiring:** motor pins as Project 6 + Bluetooth as Project 5.
**Run it:** pair via the app (see above), then use the direction buttons — `projects/11_bluetooth_control_car/lesson_11.1_Bluetooth_Remote_Control`.

### Project 12 — Speed Control Robot
**Goal:** adds variable speed on top of Project 11 via the accelerate/decelerate (`a`/`d`) app buttons.
**Wiring:** identical to Project 11.
**Run it:** `projects/12_speed_control_robot/lesson_12.1_Speed_control_robot` → hold accelerate to ramp to max speed, decelerate to ramp down/stop.

### Project 13 — PS2 Control Car
**Goal:** drive with the PS2 controller instead of the app: Triangle=forward, Circle=right, X=back, Square=left, L2/R2=stop.
**Wiring:** PS2 pins as Project 7 + motor pins as Project 6.
**Run it:** `projects/13_ps2_control_car/lesson_13.1_PS2_control_robot`.

## Part C — Build and control the arm

### Project 14 — Servo Init & Arm Assembly
**Goal:** a calibration step. This **must** run before any arm parts are attached to the servos.
**Wiring:** Servo1(base)→A1, Servo2(shoulder)→A0, Servo3(elbow)→D8, Servo4(claw)→D9.
**⚠️ Do this in order:**
1. Wire only the 4 servos (loose, not yet mounted to arm links) to the pins above.
2. Upload `projects/14_servo_init_arm_install/lesson_14` and power on. Each servo snaps to its reference angle: Servo1=90°, Servo2=100°, Servo3=80°, Servo4=90°.
3. **While powered and holding those exact angles**, attach the arm horns/links so the mechanical zero matches the servo's electrical zero.
4. Assemble in order: mount Servo 1 (base) with the purple/black screwdriver → install the bearing → attach the arm link parts → mount Servo 3 (elbow) → mount Servo 2 (shoulder) → mount Servo 4 (claw) → wire all four servos into the shield.

Skipping this calibration is the vendor doc's own #1 cause of "abnormal robotic-arm movement."

**🛑 If a servo ends up unresponsive/stuck after assembly — even if you followed the steps above —** here's why and how to recover. On every power-up, the instant a servo is attached in code, the Arduino Servo library drives it straight to its last-commanded angle (Lesson 14's reference angles above) at full speed, unconditionally — there is no feedback sensor, so neither the library nor any sketch can know where the arm physically is first. If the horn got seated even one spline tooth off from true, or an arm link is blocking the path to that reference angle, the servo will try to slam into that obstruction on every single power-up and stall there — it will look and sound identical no matter what angle you command it to, because it physically cannot leave the obstruction to prove otherwise. Symptoms: a joint that moves once then goes completely unresponsive, or buzzes/strains without moving, or an arm segment that snaps hard into another part on power-up. **The only fix is mechanical, not code**: physically disassemble that joint enough to free the servo horn from whatever it's hitting, let the bare servo reach its reference angle unobstructed (e.g. via `tools/servo_diagnostic`), then re-seat the horn/link at that now-confirmed position before reassembling.

### Project 15 — App Control Robot Arm
**Goal:** drive the assembled arm from the phone app.
**Wiring:** servo pins as Project 14 + Bluetooth as Project 5.
**Run it:** pair via the app, then `Q`/`E` shoulder, `l`/`r` base, `f`/`b` elbow, `V`/`P` claw — `projects/15_bluetooth_control_arm/lesson_15.1_Bluetooth_control_robotic_arm`. On boot the arm returns to the Project 14 home pose.
*(Fixed here: the vendor sketch's `loop()` used `while('Q')`, `while('V')`, etc. as its hold-to-move condition — a character literal is always non-zero/true in C++, so each of those was actually an unconditional infinite loop that only a `Serial.read()=='s'` byte could ever break out of. If the app's stop signal was ever delayed or dropped, whichever joint was moving would keep going indefinitely regardless of commanded angle — e.g. the claw servo opening fully and never stopping. Rewritten as a non-blocking state machine (moves once per loop() iteration while a command is "held," stops on an explicit `'s'` or a new command) — same fix applied to Project 16.)*

*(Also recalibrated: the vendor's per-joint clamp bounds (`pos1..pos4` limits in `T_left`/`T_right`/`LF`/`LB`/`RF`/`RB`/`ZK`/`ZB`) assumed a factory horn alignment that won't match a rebuilt arm. Current bounds — base 0–180° (unclamped), shoulder 0–120°, elbow 75–180°, claw 18–180° (fully closed) — are measured/requested values for one specific rebuild, not universal constants; recalibrate for your own build using `tools/servo_diagnostic`. Note the two lateral servos (shoulder/elbow) mechanically constrain each other — their true safe range depends on the other joint's current position, which these fixed per-joint clamps don't model, so a bound that's safe in one arm pose may not be in another.)*

### Project 16 — Bluetooth Arm Memory
**Goal:** record and replay arm pose sequences over Bluetooth.
**Wiring:** identical to Project 15.
**Run it:** `projects/16_bluetooth_arm_memory/lesson_16_Bluetooth_memory_handling` → move the arm, press `t` to record each pose (up to 20 slots), press `i` to replay the whole sequence on a loop; press `t` again to exit.

### Project 17 — PS2 Control Robot Arm
**Goal:** drive the arm with PS2 joysticks: left stick X=base, left stick Y=shoulder, right stick Y=elbow, right stick X=claw.
**Wiring:** PS2 pins as Project 7 + servo pins as Project 14.
**Run it:** `projects/17_ps2_control_arm/lesson_17.1_PS2_control_robotic_arm`.
*(Fixed here: header mislabel ("lesson 16.1" → 17.1, vendor typo); `pos3` (elbow) was initialized to `60` at boot — below the `75` floor this rebuild's elbow actually needs, driving it to an unconfirmed-unsafe position on every power-up, independent of anything the joystick did. Also recalibrated the same way as Projects 15/16 — shoulder ceiling `100→120`, elbow floor `80→75`, claw floor `95→18` (was unable to fully close). And slowed things down: base/shoulder/elbow moved 1°/~5ms with no pacing beyond the outer loop, easily overshooting a clamp before you could react to an unintended stick deflection — loop delay raised `5ms→20ms`, and the claw's step size cut from 3°/tick to 1°/tick to match the other joints' finer control.)*

### Project 18 — PS2 Arm Memory
**Goal:** PS2 equivalent of Project 16.
**Wiring:** identical to Project 17.
**Run it:** `projects/18_ps2_arm_memory/lesson_18.1_PS2_memory_handling` → click **L3** to record a pose, click **R3** to replay the saved sequence on a loop, hold **START** to exit playback.
*(Same recalibration and joystick-pacing fixes as Project 17 applied here too — same vendor template, same bugs.)*

## Part D — Combine car + arm

### Project 19 — PS2 Combined Car + Arm
**Goal:** one PS2 controller drives the car and poses the arm at the same time.
**Wiring:** motor pins (D2/D3/D4/D5) + PS2 pins (D12/D11/D10/D13) + servo pins (A1/A0/D8/D9), all simultaneously.
**Run it:** `projects/19_ps2_combined_car_arm/lesson_19_PS2_control_car` → D-pad drives the chassis, joysticks pose the arm, L3/R3 record/replay poses — all concurrently.
*(Same recalibration and joystick-pacing fixes as Project 17 applied here too — same stale bounds, same step sizes. Also fixed: `advance()`/`turnL()`/`turnR()`/`back()` set motor PWM via `analogWrite()`, which holds its value on its own once set — the D-pad's `ps2x.Button()` check correctly stopped being true on release, but nothing ever told the motors to stop, so they kept coasting at the last commanded speed until an explicit L2 press. Added an auto-stop when no direction button is held. Also slowed `turnL()`/`turnR()` specifically (100→60) — both wheels spin opposite directions for a pivot turn, which turns much sharper than the same PWM value driving straight.)*

### Project 20 — Multifunctional Robot (capstone)
**Goal:** everything at once — Bluetooth app control (car + arm + memory), PS2 control (car + arm + memory), and both autonomous modes, in a single sketch.
**Fixed here — this one's important:** the sketch's `loop()` began with `if (error != 0) return;`, where `error` is the PS2 controller's boot-time detection result. That made the *entire* loop a no-op — including Bluetooth handling — the instant no PS2 receiver was detected at boot, even though Bluetooth control doesn't depend on PS2 at all. Symptom: app appears to connect and send commands fine (visible in the Serial Monitor), but nothing moves — car or arm. Restructured so Bluetooth control always runs regardless of PS2 status; only the actual PS2-dependent block (D-pad, joysticks, PS2 memory buttons) is gated on controller presence now. Also note: PS2 detection only happens once, in `setup()` at boot — plug the receiver in *before* powering on if you want PS2 features too, or leave it out entirely for Bluetooth-only use, now that it's no longer required either way.

**Fixed here too:** the PS2-motor-auto-stop fix above ran on *every* loop iteration whenever a PS2 receiver was connected, regardless of what had last commanded the motors — so it was also silently canceling Bluetooth-issued car movement about 10ms after it started (Bluetooth-only commands like `F` don't repeat, so PS2's "nothing held → stop" logic looked identical to "nothing commanding movement" and stopped it immediately). Added a `lastDriveSource` flag so PS2's auto-stop only fires when PS2 itself was actually driving, leaving Bluetooth-issued movement alone. Arm control was never affected — Bluetooth arm commands use entirely separate functions from this block.

**App note:** the app's **"gravity sense"** button is a pure input-mode toggle (tilt-to-drive instead of tap-buttons) — confirmed from the vendor's own doc, which shows no serial character for it at all, unlike every other button; it just sends the same `F`/`B`/`L`/`R` etc. via tilt instead of taps. The **"auto-carry"** button isn't documented anywhere in the vendor's materials — traced by adding temporary debug logging to find it sends `H` (on) / `S` (off, shared with car-stop). `S` was already handled; `H` wasn't, so it silently did nothing before this fix. Implemented as "send the arm home to its startup pose" (a common `H`ome convention) — an educated guess, not vendor-confirmed, but low-risk and easy to verify by watching what it actually does.
*(Same recalibration fixes as Projects 15–19 applied to both the Bluetooth-mode and PS2-mode arm code here — this sketch has its own separate copy of each. Note: unlike Projects 15/16's `while('X')` bug, this sketch's Bluetooth-mode movement functions already used a correct boolean flag (`left_flag`, `ZB_flag`, etc.) rather than a truthy character literal — that part was fine as shipped, only the calibration bounds were stale. Also fixed the same PS2 D-pad "motors never auto-stop" bug as Project 19, in this sketch's own copy of the D-pad handling — Bluetooth-mode driving (`F`/`B`/`L`/`R`/`S`) is a separate, deliberately tap-to-go/explicit-stop design and wasn't affected. Turn speed here already uses the shared, user-adjustable `speeds` variable (`a`/`d` to change it) rather than a hardcoded value, so it wasn't hardcoded slower the way Project 19's was.)*
**Wiring:** full kit — motor, ultrasonic, Bluetooth, PS2, and all 4 servos, per the pin table above.
**Run it:** `projects/20_multifunctional_combined/lesson_20_Multifunctional_robotic_arm_robot`. Pair Bluetooth and/or plug in PS2; send `Y` to start autonomous obstacle avoidance or `U` for ultrasonic follow, `S` to stop either. Use the app or PS2 together to pick up and carry objects.

## Troubleshooting

- **Car does nothing** — check battery charge and wiring first.
- **PC doesn't detect the board's USB port** — install the CP2102 driver (see [Getting started](#getting-started)), or try another USB cable.
- **Upload fails** — unplug the Bluetooth module before uploading (it occupies the same TX/RX lines as the programmer); also try uploading with only the bare V4.0 board connected, sensors/shield disconnected.
- **Arm moves erratically** — the servos weren't calibrated: re-run [Project 14](#project-14--servo-init--arm-assembly) before reattaching arm parts.
- **A joint is completely unresponsive, or buzzes/strains without moving, no matter what angle you send it** — that servo's horn is very likely obstructed at its reference angle (a spline tooth off, or blocked by an arm link) and is stalling against it on every power-up regardless of command. This is mechanical, not a wiring or code fault — see the recovery steps under [Project 14](#project-14--servo-init--arm-assembly).
- **Bluetooth pairing fails** — the phone app needs Location permission granted, or pairing silently fails.
- **PS2 and Bluetooth prints garbled together** — they share the single hardware serial port; don't expect clean simultaneous debug output from both.

## Notice

Hardware, official documentation, and example sketches are products of **Keyestudio**. This repository reorganizes and documents the vendor-provided example code (verified against the official [KS0520 GitHub source](https://github.com/keyestudio/KS0520-4DOF-Mechanical-Robot-Arm-Car)) for personal reference while building the kit — it does not claim original authorship of the sketches or the PS2X/Servo libraries. Refer to the vendor links at the top of this file for the original sources and licensing terms.
