---
date: "2023-10-12"
---
## Changes from 6.1.0 to Dev (Work In Progress)

### Integrated Components

* Yocto 4.0 LTS "Kirkstone" (4.0.12). Layers compatible with 3.1 LTS "Dunfell"
* Linux 6.1 LTS (6.1.45) with KED backports and patches
* U-Boot 2023.04 with KED backports and patches

### Supported Hardware

| Product Numbers | Product Name | Product Type | Status |
| --------------- | ------------ | ------------ | ------ |
| 40099 122 | SL i.MX6 UL | SoM | Not Supported (Unverified) |
| 40099 145 | SL i.MX6 ULL | SoM | Not Supported (Unverified) |
| 40099 167 | SL STM32 MP157 | SoM | Not Supported (Planned) |
| 40099 175<br>40099 185<br>40099 242<br>40099 212 | SL i.MX8M Mini Quad | SoM | Supported |
| 40099 227<br>40099 229<br>40099 239<br>40099 231 | OSM-S i.MX8M Mini Quad | SoM | Supported |
| 41099 248 | OSM-S i.MX8M Plus | SoM | Supported |
| 40099 148 | BL i.MX6 ULL | Board | Not Supported (Unverified) |
| 40099 176 | BL STM32 MP157 | Board | Not Supported (Planned) |
| 40099 187<br>40099 220 | BL i.MX8M-Mini Quad | Board | Supported (Basic[^1]) |
| 40099 265 | BL i.MX8M-Plus Quad | Board | Supported (Basic[^1]) |

### Core/Distro Changes (all HW Platforms)

* Add CI jobs for test builds and test execution
* Add support for libubootenv YAML config in SWUpdate (backport)
* Upgrade libubootenv to v0.3.5 to support common implementation of `libuboot_namespace_from_dt()``
* Set provider for `u-boot-fw-utils` to `libubootenv-bin` on distro level
* Install `u-boot-fw-utils` by default in `image-ked*` images

### Changes for i.MX Platforms

#### Changes for i.MX8 Platform

* Fix access to UART4 from A53 domain on i.MX8MM hardware

#### Changes for i.MX6 Platform

* none

### Changes for STM32MP1 Platform

* none

### Breaking Changes

* none

### Known Issues

* The onboard USB hub on the BL i.MX8MM doesn't work in U-Boot.
* Devices connected to the USB-C connector of the BL i.MX8MP before booting up
  the board are not detected and need to be reconnected after booting.

[^1]: Basic support doesn't include all features of the board. Especially
      advanced peripherals like display, graphics, hardware acceleration units,
      camera interfaces, etc. might not be supported or tested yet.
