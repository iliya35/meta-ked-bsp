# Build

## Clone the BSP Repository

First of all you need a local copy of the BSP repository. This can be either
`meta-ked-bsp` or a custom layer. This guide will use the public `meta-ked-bsp`
BSP repository to build images for our standard and demo hardware.

```
cd ~
git clone https://git.kontron-electronics.de/sw/ked/meta-ked-bsp.git
cd meta-ked-bsp
```

## Use the Menu to Configure (Recommended)

!!! note "`kas` command"

    In the following we will use the `kas-container` script to build inside of a
    container. If you do native builds, you can use the `kas` command directly
    instead.

Run the `menu` command:

```
kas-container menu
```

Navigate through the menu using ++arrow-up++++arrow-down++ and select the
options you need using ++space++.

Afterwards use ++tab++ to navigate to the button menu, select "Save & Build",
"Save & Exit" or "Exit" using ++arrow-left++++arrow-right++ and confirm with
++return++.

The configuration is saved to `.config.yaml` which is excluded from being
tracked in Git.

The configuration in `.config.yaml` will mainly consist of references to other
configuration files in the `kas` subdirectory.

## Run the Build

If you selected "Save & Build" in the configuration menu, a build will be
started automatically. Otherwise you can start a build with:

```
kas-container build .config.yaml
```

Now `kas` will fetch all repos specified in the configuration, initialize the
build environment and launch `bitbake`.

## Select Configuration Files Manually

Instead of using the `menu` command and creating a `.config.yaml` you can also
select the existing configuration file(s) manually.

The following command produces the same result as when selecting the options
"Kontron Electronics SL/BL/OSM i.MX8MM" and "Yocto Kirkstone" via the menu.

```
kas-container build kas/ked-mx8mm.yml:kas/series/kirkstone.yml
```

## Using the `shell` Command

Instead of using the `build` command to start `bitbake`, you can also use the
`shell` command to instruct `kas` to drop you to a shell inside the build
environment.

```
kas-container shell .config.yaml
```

Once inside the shell, you can run any `bitbake` command you like.
