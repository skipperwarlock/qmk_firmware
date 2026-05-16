# ErgoDox Infinity — build & flash

From the **repository root** (`qmk_firmware`). Use your keymap name instead of `dev` if needed.

**Build** — compile only:

```bash
qmk compile -kb input_club/ergodox_infinity -km dev
```

**Flash** — one half plugged in, **program button** held to enter bootloader; run the same command **per half**:

```bash
qmk flash -kb input_club/ergodox_infinity -km dev
```

**Alternative** if you prefer Make:

```bash
make input_club/ergodox_infinity:dev
sudo make input_club/ergodox_infinity:dev:dfu-util   # when that half is in bootloader mode; repeat for other half
```

The Infinity is **two devices**: flash the **same** firmware to **left** and **right**, each time with only that half on USB. Full notes: `readme.md` here.
