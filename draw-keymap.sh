#!/usr/bin/env bash
# Render the Piantor keymap to an SVG showing every layer.
#
# Pipeline: keymap.c -> qmk c2json -> keymap-drawer -> SVG
#
# Requires keymap-drawer:  pipx install keymap-drawer
set -euo pipefail

export PATH="/opt/homebrew/opt/arm-gcc-bin@14/bin:$HOME/.local/bin:$PATH"

KEYMAP_C="$HOME/code/qmk_userspace/keyboards/beekeeb/piantor/keymaps/shaun/keymap.c"
OUT_DIR="$HOME/code/qmk_userspace/docs"
mkdir -p "$OUT_DIR"

command -v keymap >/dev/null || { echo "keymap-drawer missing: pipx install keymap-drawer"; exit 1; }

echo "==> Exporting keymap to JSON..."
# c2json emits the layer enum names (MO(_NUM)); keymap-drawer needs indices to
# recognise a layer-tap as tap/hold, so substitute them. Keep in sync with the
# enum in keymap.c.
qmk c2json -kb beekeeb/piantor -km shaun "$KEYMAP_C" \
    | sed -e 's/_SYM/1/g' -e 's/_NAV/2/g' -e 's/_NUM/3/g' -e 's/_FN/4/g' \
    > "$OUT_DIR/keymap.json"

echo "==> Parsing..."
keymap parse -q "$OUT_DIR/keymap.json" \
    -l BASE SYM NAV NUM FN \
    -o "$OUT_DIR/keymap.yaml"

echo "==> Drawing..."
keymap draw "$OUT_DIR/keymap.yaml" -o "$OUT_DIR/keymap.svg"

echo "==> Wrote $OUT_DIR/keymap.svg"

# PNG is optional; SVG is sharper and opens in any browser.
if command -v rsvg-convert >/dev/null; then
    rsvg-convert -z 2 "$OUT_DIR/keymap.svg" -o "$OUT_DIR/keymap.png"
    echo "==> Wrote $OUT_DIR/keymap.png"
elif command -v magick >/dev/null; then
    magick -density 192 "$OUT_DIR/keymap.svg" "$OUT_DIR/keymap.png"
    echo "==> Wrote $OUT_DIR/keymap.png"
else
    echo "    (no rsvg-convert or magick, so SVG only)"
fi
