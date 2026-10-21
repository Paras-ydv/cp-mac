# Security

cp-mac's installer is usually run with `curl | sh` and may ask for your password, so security problems matter.

## Reporting a problem

Please **don't** open a public issue. Report it privately instead:
[github.com/Paras-ydv/cp-mac/security/advisories/new](https://github.com/Paras-ydv/cp-mac/security/advisories/new)

Include what an attacker could do, and the steps or code to reproduce it. You'll get a reply within a few days, and a fix will be released as soon as possible, with credit to you unless you'd rather not.

## Supported versions

Only the latest release gets fixes. Update with `cp-mac update` (or `brew upgrade cp-mac`).

## What the installer does

So you can check before running it:

- `install.sh` downloads the source of a tagged release from `github.com/Paras-ydv/cp-mac`, unpacks it in a temporary folder and checks that it looks like cp-mac before changing anything.
- It copies headers into `<prefix>/include` and the `cp-mac` / `cprun` commands into `<prefix>/bin` (default prefix `/usr/local`), and records every file in `<prefix>/include/cp-mac/MANIFEST`. Uninstalling removes exactly those files.
- It uses `sudo` only for folders you can't write to, and says so first.
- `cp-mac` contacts GitHub only to look up the latest release (at most once a day; turn off with `CPMAC_NO_UPDATE_CHECK=1`) and when you run `cp-mac update`. Automatic updates are off unless you run `cp-mac autoupdate on`.
