#!/usr/bin/env bash
# Build and flash the Piantor. Run once per half.
#
# The Piantor is RP2040/UF2: each half has its own MCU and its own USB port,
# and flashes by mounting as a USB mass-storage volume named RPI-RP2 and
# having the .uf2 copied onto it. There is no `qmk flash` serial protocol,
# and there is no way to reach one half through the other -- you plug the
# cable into whichever half you are flashing.
set -euo pipefail

export PATH="/opt/homebrew/opt/arm-gcc-bin@14/bin:$PATH"

UF2="$HOME/code/qmk_userspace/beekeeb_piantor_shaun.uf2"
VOL="/Volumes/RPI-RP2"

if [[ "${1:-}" != "--no-build" ]]; then
    echo "==> Building..."
    qmk compile -kb beekeeb/piantor -km shaun
fi

[[ -f "$UF2" ]] || { echo "No firmware at $UF2"; exit 1; }

if [[ -d "$VOL" ]]; then
    echo "==> $VOL already mounted."
else
    echo "==> Waiting for a half in bootloader mode (RPI-RP2)..."
    echo "    1. Unplug USB."
    echo "    2. Press and HOLD the BOOTSEL button on the half you're flashing."
    echo "    3. While still holding, plug USB into THAT half."
    echo "    4. Release BOOTSEL."
    echo "    Ctrl-C to cancel."
    while [[ ! -d "$VOL" ]]; do sleep 1; done
fi

echo "==> Found $VOL — copying firmware..."

# The board reboots the instant it receives the last block, which yanks the
# volume out from under the OS. cp and sync therefore often report an I/O
# error on a *successful* flash, and macOS may show "Disk Not Ejected
# Properly". Neither indicates failure, so don't let `set -e` abort here.
cp "$UF2" "$VOL/" 2>/dev/null || true
sync 2>/dev/null || true

# The real success signal is the volume disappearing (the board rebooted into
# firmware). If it's still mounted after a few seconds, the copy didn't take.
for _ in $(seq 1 10); do
    [[ -d "$VOL" ]] || break
    sleep 1
done

if [[ -d "$VOL" ]]; then
    echo "!!  $VOL is still mounted — the flash did NOT complete."
    echo "    Try again, or copy manually:  cp '$UF2' '$VOL/'"
    exit 1
fi

echo "==> Flashed. The half rebooted and $VOL disappeared, which is what success looks like."
echo "    (A 'Disk Not Ejected Properly' notice from macOS here is normal.)"
echo
echo "    Now repeat for the other half, plugging USB into that one."
echo "    When both are done, plug USB into the LEFT half for normal use."
