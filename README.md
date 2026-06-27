# Ten64 Microcontroller tool

The microcontroller tool can be used to communicate with the board microcontroller.
Features include:
* View current firmware version
* View current settings and state (for example, if system powered from ATX)
* View LPC804 unique device serial number
* Set RESET button behaviour (change required duration or ignore)
* Install a new board microcontroller firmware

## Usage
```
ten64-controller --get-info                        Get uC info
ten64-controller --get-state                       Get system control state
ten64-controller --set-reset-holdtime <{3:10}>     Set the hold time for the external reset button
ten64-controller --set-reset-enabled <0|1>         Enable/disable the external button reset
ten64-controller --image-info                      Get current image data stored in both flash slots
ten64-controller --fwup-run <a|b> <new_fw_file>    Firmware upgrade run
ten64-controller --fwup-boot                       Firmware upgrade boot into new firmware. NOTE: will reset main CPU
```

NOTE: We recommend doing microcontroller firmware updates only from the
onboard recovery firmware.

Using `--fwup-boot` to boot into the new firmware will cause the board to
run through it's cold power on sequence.

### Sample outputs:
```
# ten64-controller --get-info

LPC uC info:
    MAC: 00:0A:FA:24:25:2F
    UUID: 0805B031:AF2A1CE1:1412031B:F5000701
    Firmware: v1.1.3

# ten64-controller --get-state
Ten64 uC system control state:
    ATX powered: Yes
    Current boot source: nand
    Next boot source: nand
    Switch boot source: nand
    Reset enabled: yes
    Reset hold time: 3 seconds
    Switch set to ignore resets: no

# ten64-controller --image-info
LPC uC image bank A info
   Type: Old stable
    Size: 11760 B
    Version: 1.1.2
    Git sha: 98abc22
LPC uC image bank B info
    Type: Stable
    Size: 11760 B
    Version: 1.1.3
    Git sha: 5b57568

# ten64-controller --fwup-run a /root/lpcfw_1.1.3_5b57568_banka.img
Fwup init: Success
Fwup transfer: Start transfer
Fwup transfer: Progress = 184/184 Fwup transfer block 184: Result addr=000041C0, block=183, error=0
Fwup transfer: End transfer
Fwup check: Success
Fwup transfer: Transfer done

```

## Install via apt (Ten64, Debian arm64)

A pre-built arm64 package is published as a signed apt repository at
<https://mithro.github.io/ten64-microcontroller-utility/>:

```bash
sudo install -d -m0755 /etc/apt/keyrings
curl -fsSL https://mithro.github.io/ten64-microcontroller-utility/ten64-controller.gpg \
  | sudo tee /etc/apt/keyrings/ten64-controller.gpg > /dev/null
echo "deb [signed-by=/etc/apt/keyrings/ten64-controller.gpg] https://mithro.github.io/ten64-microcontroller-utility/ ./" \
  | sudo tee /etc/apt/sources.list.d/ten64-controller.list
sudo apt update
sudo apt install ten64-controller
```

The package installs `/usr/sbin/ten64-controller` and a modules-load snippet that
loads `i2c-dev` so `/dev/i2c-0` is present. The tool is arm64/Ten64-specific.

## Building from source

```bash
cmake -S . -B build
cmake --build build
# binary: build/ten64-controller (statically linked)
```

Requires `cmake` (>= 3.30) and a C toolchain. Talking to the microcontroller
needs the `i2c-dev` module loaded (`sudo modprobe i2c-dev`) so `/dev/i2c-0`
exists, and root (or i2c group) access to that device.