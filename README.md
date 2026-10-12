# e-meishi firmware

Zephyr firmware for the e-meishi, an electronic business card (*meishi* is
Japanese for business card) built around an STM32U375. It drives 7 yellow LEDs
with PWM animations, and exposes its 16 MB external NOR flash to the host as a
USB mass storage device.

## One-time setup

Install the Zephyr SDK and host tools by following Zephyr's
[Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html),
then set up this workspace:

```bash
west init -l manifest
west update
west zephyr-export
```

## Formatting the USB storage

The flash is empty on first use. On Linux, plug the card in and find its
block device (16 MB, model `e-meishi`):

```bash
lsblk -o NAME,SIZE,MODEL
```

Then format it as FAT, replacing `sdX` with the device found above:

```bash
sudo mkfs.vfat -n EMEISHI /dev/sdX
```

Unplug and replug the card for the host to mount it.
