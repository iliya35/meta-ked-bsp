# Customization

If you develop your own BSP using `meta-kontron` as a base, it is recommended to
setup your own Git repository to track all changes.

!!! note "Layer Naming"

    We will use `meta-custom` as name for the custom layer. Use a name that
    matches your project, product or whatever you want to describe in the layers
    data.

```
cd ~
mkdir meta-custom
cd meta-custom
git init
```

## Custom `kas` Configuration

The first part of you new custom BSP repository should be a `kas` configuration
file. A minimal example for a BSP based on `meta-kontron` looks like this:

```yaml title="custom.yaml"
header:
  version: 12
  includes:
    - repo: meta-kontron
      file: kas/inc/imx.yml
    - repo: meta-kontron
      file: kas/series/kirkstone.yml

repos:
  meta-kontron:
    url: https://git.kontron-electronics.de/sw/ked/meta-kontron.git
    refspec: 6.0.0
    path: layers/meta-kontron

machine: kontron-mx8mm
target:
  - image-ktn-minimal
```

## Custom Yocto Layer

In order to provide custom configuration and metadata to Yocto, you need to
create your own layer. You can use the `bitbake-layers` utility to do this.

First drop to a `bitbake` shell:

```
kas-container shell custom.yaml
```

Then create your new layer:

```
bitbake-layers create-layer /repo/layers/meta-custom
```

!!! info "Accessing the Repo from within the Container"

    If you use `kas-container` the current base repository gets mounted to
    `/repo` inside the container automatically.

The `bitbake-layers create-layer` command will tell you to use `bitbake-layers
add-layer` to add an entry for your new layer in `bblayers.conf`. As we are
letting `kas` manage the `bblayers-conf`, we will skip this step and instead
edit our `kas` configuration.

```diff
 repos:
   meta-kontron:
     url: https://git.kontron-electronics.de/sw/ked/meta-kontron.git
     refspec: 6.0.0
     path: layers/meta-kontron
+  meta-custom:
+    layers:
+      layers/meta-custom:
```

Now with your custom layer setup you can add your customizations such as machine
or distro configs, image recipes or any other Yocto/OE metadata to the new layer
in `layers/meta-custom`.
