# QMK Userspace

Shaun's QMK keymaps. Currently: `beekeeb/piantor` (42-key Piantor), migrated
from Vial.

## Local setup

The ARM toolchain is required to build for the Piantor's RP2040:

```sh
brew install osx-cross/arm/arm-gcc-bin@14
```

Do **not** use Homebrew core's `arm-none-eabi-gcc` — it ships without newlib,
so builds die with `fatal error: stdint.h: No such file or directory`. The
`osx-cross/arm` bottles are ARM's official prebuilt toolchains and include it.

It's keg-only, so it needs to be on `PATH` (already handled in dotfiles):

```sh
export PATH="/opt/homebrew/opt/arm-gcc-bin@14/bin:$PATH"
```

Then point QMK at this userspace and build:

```sh
qmk config user.overlay_dir="$HOME/code/qmk_userspace"
qmk compile -kb beekeeb/piantor -km shaun
```

## Flashing

The Piantor is RP2040/UF2 — it flashes as a USB mass-storage volume, not over
a serial protocol. `./flash-piantor.sh` builds, waits for `RPI-RP2` to mount,
and copies the firmware.

Run it **once per half**, holding `BOOTSEL` on that half while plugging in USB.
Both halves take the same `.uf2`.

The board sets no handedness, so QMK falls back to `MASTER_LEFT`: in normal use
**USB must go to the left half**, or the layout comes out mirrored.

## Updating QMK

CI is pinned to a specific QMK tag (`qmk_ref` in
`.github/workflows/build_binaries.yaml`) so that a random upstream commit can't
change the firmware under you. Local and CI should always be on the same tag.

To bump:

```sh
# 1. See what's available and read the breaking changes FIRST.
cd ~/qmk_firmware
git fetch upstream --tags
git tag --sort=-creatordate | grep -E '^[0-9]+\.[0-9]+\.[0-9]+$' | head

# 2. Skim the changelog for the release(s) you're crossing.
#    Breaking changes land in dated files under docs/ChangeLog/.
ls docs/ChangeLog/

# 3. Move to the new tag and sync submodules (easy to forget; builds fail
#    in confusing ways without it).
git checkout <new-tag>
qmk git-submodule -f

# 4. Rebuild and fix whatever broke.
cd ~/code/qmk_userspace
qmk compile -kb beekeeb/piantor -km shaun

# 5. Only once that's clean, update the pin to match.
#    Edit qmk_ref in .github/workflows/build_binaries.yaml

# 6. Commit, flash both halves, and actually type on it before trusting it.
```

Breaking changes are not hypothetical — the 0.30 → 0.33 bump alone required
two keymap fixes:

- `IGNORE_MOD_TAP_INTERRUPT` became the default in 0.21 and now hard-`#error`s
  if defined.
- `tap_dance_action_t` lost its `.state` member ([#25415]); custom tap dance
  code must use `tap_dance_get()` / `tap_dance_get_state()`, which needs
  `#include "keymap_introspection.h"`.

[#25415]: https://github.com/qmk/qmk_firmware/pull/25415

---

Below is the original upstream template README.

---

This is a template repository which allows for an external set of QMK keymaps to be defined and compiled. This is useful for users who want to maintain their own keymaps without having to fork the main QMK repository.

## Howto configure your build targets

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`
1. Add a new keymap for your board using `qmk new-keymap`
    * This will create a new keymap in the `keyboards` directory, in the same location that would normally be used in the main QMK repository. For example, if you wanted to add a keymap for the Planck, it will be created in `keyboards/planck/keymaps/<your keymap name>`
    * You can also create a new keymap using `qmk new-keymap -kb <your_keyboard> -km <your_keymap>`
    * Alternatively, add your keymap manually by placing it in the location specified above.
    * `layouts/<layout name>/<your keymap name>/keymap.*` is also supported if you prefer the layout system
1. Add your keymap(s) to the build by running `qmk userspace-add -kb <your_keyboard> -km <your_keymap>`
    * This will automatically update your `qmk.json` file
    * Corresponding `qmk userspace-remove -kb <your_keyboard> -km <your_keymap>` will delete it
    * Listing the build targets can be done with `qmk userspace-list`
1. Commit your changes

## Howto build with GitHub

1. In the GitHub Actions tab, enable workflows
1. Push your changes above to your forked GitHub repository
1. Look at the GitHub Actions for a new actions run
1. Wait for the actions run to complete
1. Inspect the Releases tab on your repository for the latest firmware build

## Howto build locally

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. `cd` into this repository's clone directory
1. Set global userspace path: `qmk config user.overlay_dir="$(realpath .)"` -- you MUST be located in the cloned userspace location for this to work correctly
    * This will be automatically detected if you've `cd`ed into your userspace repository, but the above makes your userspace available regardless of your shell location.
1. Compile normally: `qmk compile -kb your_keyboard -km your_keymap` or `make your_keyboard:your_keymap`

Alternatively, if you configured your build targets above, you can use `qmk userspace-compile` to build all of your userspace targets at once.

## Extra info

If you wish to point GitHub actions to a different repository, a different branch, or even a different keymap name, you can modify `.github/workflows/build_binaries.yml` to suit your needs.

To override the `build` job, you can change the following parameters to use a different QMK repository or branch:
```
    with:
      qmk_repo: qmk/qmk_firmware
      qmk_ref: master
```

If you wish to manually manage `qmk_firmware` using git within the userspace repository, you can add `qmk_firmware` as a submodule in the userspace directory instead. GitHub Actions will automatically use the submodule at the pinned revision if it exists, otherwise it will use the default latest revision of `qmk_firmware` from the main repository.

This can also be used to control which fork is used, though only upstream `qmk_firmware` will have support for external userspace until other manufacturers update their forks.

1. (First time only) `git submodule add https://github.com/qmk/qmk_firmware.git`
1. (To update) `git submodule update --init --recursive`
1. Commit your changes to your userspace repository
