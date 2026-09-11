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

# macOS likes to drop AppleDouble sidecars and Spotlight state onto a freshly
# mounted volume. The RP2040 bootloader presents a minimal fake FAT that does
# not really cope with that, and the stray entries are one reason a copy can
# appear to succeed while the board never reboots. Clear them first; they are
# regenerated harmlessly if the flash does not take.
rm -f "$VOL"/._* 2>/dev/null || true
rm -rf "$VOL"/.Spotlight-V100 "$VOL"/.fseventsd "$VOL"/.TemporaryItems 2>/dev/null || true

# The board reboots the instant it receives the last block, which yanks the
# volume out from under the OS. The copy therefore often reports an I/O error
# on a *successful* flash, and macOS may show "Disk Not Ejected Properly".
# Neither indicates failure, so don't let `set -e` abort here.
#
# What the error text says still matters when the flash does NOT take, though,
# so capture it rather than routing it to /dev/null the way this script used to
# -- a silent failure leaves nothing to go on.
ERRLOG="$(mktemp)"
trap 'rm -f "$ERRLOG"' EXIT

# -X omits extended attributes. Without it macOS writes xattrs and a ._sidecar
# alongside the data, which the bootloader's FAT does not expect.
cp -X "$UF2" "$VOL/" 2>"$ERRLOG" || true
sync 2>/dev/null || true

# The real success signal is the volume disappearing (the board rebooted into
# firmware). If it's still mounted after a few seconds, the copy didn't take.
for _ in $(seq 1 10); do
    [[ -d "$VOL" ]] || break
    sleep 1
done

# Second attempt with dd, which writes the bytes straight through without any
# of the metadata operations cp performs. Only worth trying if the volume is
# still there, i.e. the board has not already rebooted.
if [[ -d "$VOL" ]]; then
    echo "==> cp did not take; retrying with dd..."
    [[ -s "$ERRLOG" ]] && sed 's/^/    cp: /' "$ERRLOG"
    dd if="$UF2" of="$VOL/fw.uf2" bs=1m 2>"$ERRLOG" || true
    sync 2>/dev/null || true
    for _ in $(seq 1 10); do
        [[ -d "$VOL" ]] || break
        sleep 1
    done
fi

if [[ -d "$VOL" ]]; then
    echo "!!  $VOL is still mounted — the flash did NOT complete."
    echo
    echo "    Last error output:"
    if [[ -s "$ERRLOG" ]]; then sed 's/^/      /' "$ERRLOG"; else echo "      (none — the copy reported success but the board never rebooted)"; fi
    echo
    echo "    Volume state:"
    df -h "$VOL" 2>/dev/null | sed 's/^/      /'
    ls -la "$VOL" 2>/dev/null | sed 's/^/      /'
    echo
    # "Operation not permitted" is EPERM, not an I/O error: macOS gates access
    # to removable volumes behind TCC, per application. A script inherits the
    # grant of whatever terminal it runs in, so the fix is to authorise the
    # terminal, not the script. macOS 26 moved FAT onto the new FSKit driver
    # (`mount` reports fskit), which is roughly when this started biting.
    if grep -q "Operation not permitted" "$ERRLOG" 2>/dev/null; then
        TERM_APP="${TERM_PROGRAM:-your terminal}"
        echo "    This is a macOS permission problem, not a keyboard problem."
        echo "    'Operation not permitted' means $TERM_APP has not been granted"
        echo "    access to removable volumes."
        echo
        echo "    Fix it in System Settings > Privacy & Security > Files and Folders,"
        echo "    under $TERM_APP, by enabling 'Removable Volumes'. If it is not"
        echo "    listed, a prompt was denied at some point; clear that and retry"
        echo "    so it asks again (substitute your terminal's bundle id):"
        echo "        tccutil reset SystemPolicyRemovableVolumes com.mitchellh.ghostty"
        echo "    Then quit the terminal entirely and reopen it."
        echo
        echo "    To flash without touching any of that, drag the .uf2 onto the"
        echo "    RPI-RP2 volume in Finder -- Finder has its own grant."
        echo
    fi
    echo "    picotool avoids this class of problem altogether, talking to the"
    echo "    RP2040 over USB rather than through a mounted filesystem:"
    echo "        brew install picotool"
    echo "        picotool load -x '$UF2'"
    exit 1
fi

echo "==> Flashed. The half rebooted and $VOL disappeared, which is what success looks like."
echo "    (A 'Disk Not Ejected Properly' notice from macOS here is normal.)"
echo
echo "    Now repeat for the other half, plugging USB into that one."
echo "    When both are done, plug USB into the LEFT half for normal use."
