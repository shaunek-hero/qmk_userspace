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

# The _FN layer is a tri-layer (hold both inner thumbs), which lives in
# layer_state_set_user() rather than in the keymap array, so nothing in the
# parsed JSON points at it. Draw it as a combo on the two inner thumb keys --
# positions 38 and 39 of the 42-key split_3x6_3 layout -- so the diagram still
# shows how FN is reached.
cat >> "$OUT_DIR/keymap.yaml" <<'YAML'
combos:
  - {p: [38, 39], k: FN, l: [BASE], a: bottom}
YAML

echo "==> Drawing..."
keymap -c "$HOME/code/qmk_userspace/keymap-drawer.yaml" \
    draw "$OUT_DIR/keymap.yaml" -o "$OUT_DIR/keymap.svg"

echo "==> Wrote $OUT_DIR/keymap.svg"

# --- Wallpaper -------------------------------------------------------------
#
# macOS System Settings will not take an SVG as a desktop picture, so the
# wallpaper has to be a raster image. It is drawn from the same keymap.yaml but
# with its own drawer config (two columns, fixed light theme -- see
# keymap-drawer-wallpaper.yaml), then centred on a screen-sized canvas.
#
# Override any of these from the environment, e.g. for a different display:
#   WALL_W=3440 WALL_H=1440 ./draw-keymap.sh
WALL_W="${WALL_W:-2560}"
WALL_H="${WALL_H:-1440}"
# In dark mode the keys are #3f4750, so the ground wants to be darker still --
# otherwise the keys stop reading as raised cards and the whole thing goes flat.
WALL_BG="${WALL_BG:-#1e2228}"
# Fraction of the canvas the diagram fills, leaving a margin. Applied as
# width/height plus object-fit:contain rather than max-width/max-height,
# because the max-* pair only ever shrinks an image -- the SVG is smaller than
# the screen, so it would have drawn at its intrinsic 1688x1036 and left the
# wallpaper two-thirds empty.
WALL_FIT="${WALL_FIT:-94%}"

echo "==> Drawing wallpaper..."
keymap -c "$HOME/code/qmk_userspace/keymap-drawer-wallpaper.yaml" \
    draw "$OUT_DIR/keymap.yaml" -o "$OUT_DIR/keymap-wallpaper.svg"

CHROME="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
if [[ ! -x "$CHROME" ]]; then
    echo "!!  Chrome not found at $CHROME — wrote the wallpaper SVG but no PNG."
    echo "    Install a rasteriser (brew install librsvg) or point CHROME at a browser."
    exit 0
fi

# Rasterise by screenshotting the SVG centred in a page the size of the
# display. Chrome is used rather than rsvg-convert because it is already on
# the machine, and because letting CSS do the fit-and-centre means the canvas
# comes out at exactly WALL_W x WALL_H with no separate compositing step.
#
# The work happens in a temp dir because Chrome needs the HTML and the SVG to
# sit side by side to load the <img>.
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
cp "$OUT_DIR/keymap-wallpaper.svg" "$TMP/keymap.svg"

cat > "$TMP/wallpaper.html" <<HTML
<body style="margin:0;width:${WALL_W}px;height:${WALL_H}px;background:${WALL_BG};display:flex;align-items:center;justify-content:center;overflow:hidden">
<img src="keymap.svg" style="width:${WALL_FIT};height:${WALL_FIT};object-fit:contain">
</body>
HTML

# --force-device-scale-factor=2 renders at 2x and so antialiases the small key
# legends properly; the PNG lands at twice WALL_W/WALL_H, which macOS scales
# back down cleanly.
#
# Deliberately NO --user-data-dir: pointing Chrome at a fresh profile directory
# makes headless write the screenshot and then never exit, hanging the script.
# Omitting it exits in a few seconds, and does so happily even with a normal
# Chrome already running.
#
# Run it under a watchdog anyway, since a wedged browser should not be able to
# block the script forever. macOS has no timeout(1), hence the poll loop.
rm -f "$OUT_DIR/keymap-wallpaper.png"
"$CHROME" --headless --disable-gpu --hide-scrollbars \
    --force-device-scale-factor=2 \
    --window-size="${WALL_W},${WALL_H}" \
    --screenshot="$OUT_DIR/keymap-wallpaper.png" \
    "$TMP/wallpaper.html" >/dev/null 2>&1 &
CHROME_PID=$!
WAITED=0
while kill -0 "$CHROME_PID" 2>/dev/null && [[ $WAITED -lt 60 ]]; do
    sleep 1
    WAITED=$((WAITED + 1))
done
if kill -0 "$CHROME_PID" 2>/dev/null; then
    kill -9 "$CHROME_PID" 2>/dev/null || true
    echo "!!  Chrome did not exit after ${WAITED}s; killed it."
fi
wait "$CHROME_PID" 2>/dev/null || true

if [[ -f "$OUT_DIR/keymap-wallpaper.png" ]]; then
    echo "==> Wrote $OUT_DIR/keymap-wallpaper.png ($(sips -g pixelWidth -g pixelHeight "$OUT_DIR/keymap-wallpaper.png" | awk '/pixel/{printf "%s ", $2}')px)"
    echo "    Set it via System Settings > Wallpaper, or drag it onto the desktop."
else
    echo "!!  Chrome did not produce a PNG."
    exit 1
fi
