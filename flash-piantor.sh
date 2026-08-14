#!/usr/bin/env bash
# Build and flash the Piantor. Run once per half.
#
# The Piantor is RP2040/UF2: it flashes by mounting as a USB mass-storage
# volume named RPI-RP2 and having the .uf2 copied onto it. There is no
# `qmk flash` serial protocol involved.
set -euo pipefail

export PATH="/opt/homebrew/opt/arm-gcc-bin@14/bin:$PATH"

UF2="$HOME/code/qmk_userspace/beekeeb_piantor_shaun.uf2"
VOL="/Volumes/RPI-RP2"

if [[ "${1:-}" != "--no-build" ]]; then
    echo "==> Building..."
    qmk compile -kb beekeeb/piantor -km shaun
fi

[[ -f "$UF2" ]] || { echo "No firmware at $UF2"; exit 1; }

echo "==> Waiting for a half to appear in bootloader mode (RPI-RP2)..."
echo "    Hold BOOTSEL on the half you want to flash, then plug in USB."
echo "    Ctrl-C to cancel."

while [[ ! -d "$VOL" ]]; do sleep 1; done

echo "==> Found $VOL — copying firmware..."
cp "$UF2" "$VOL/"
sync

# The board reboots itself the moment the UF2 lands, which yanks the volume
# out from under the OS. That is expected, not an error.
echo "==> Done. The half rebooted itself and $VOL disappeared; that means it worked."
echo "    Now repeat for the other half."
