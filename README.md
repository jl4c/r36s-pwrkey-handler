# r36s-pwrkey-handler

Minimal power-key suspend handler for the R36S.

## What it does

Listens for `KEY_POWER` press events on the device's input node
(`/dev/input/event0` by default) and triggers a suspend via
`loginctl suspend`.

## Why a custom handler at all

logind/elogind can already react to the power key on its own. Set
`HandlePowerKey=ignore` in `logind.conf` (or `elogind.conf`) so it
still sees the key but takes no action, leaving the suspend trigger
entirely to this program. This avoids two separate components
racing to suspend the system off the same key press.

## The resume bug this avoids

The same physical button press used to wake the device from
suspend generates a normal `KEY_POWER` press event once interrupts
are processed after resume — the kernel doesn't distinguish "this
press only woke us up" from "this is a new press." A naive handler
reacting to every press would suspend again immediately after
waking up.

This program tracks a single flag across the suspend call and
skips exactly the next genuine `KEY_POWER` press. Sync (`EV_SYN`)
and release events in between are ignored without touching that
flag, so it survives until the real resume-leftover press arrives.

## Build

```sh
gmake
```

## Install

```sh
gmake install PREFIX=/usr
```

This installs:

- `$(PREFIX)/bin/r36s-pwrkey-handler`
- `$(PREFIX)/lib/dinit.d/r36s-pwrkey-handler` (service file, see below)

## Running as a dinit service

A service file is provided in `dinit.d/r36s-pwrkey-handler`:

```
type = process
command = /usr/bin/r36s-pwrkey-handler
restart = true
smooth-recovery = true
```

After installing, enable it with:

```
doas dinitctl enable r36s-pwrkey-handler
```

## Requirements

- Read access to the input device exposing the power key (adjust
  the `DEVICE` macro if it isn't `/dev/input/event0` on your
  hardware).
- `loginctl suspend` callable without an interactive auth prompt
  for the user running this (a polkit rule, or appropriate
  elogind/logind permission).
- `HandlePowerKey=ignore` set in `logind.conf`/`elogind.conf`, so
  logind doesn't also act on the power key independently.

## License

GPL-2.0-only — see [LICENSE](LICENSE)
