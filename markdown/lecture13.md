# 13. Advanced topics

## `BR2_EXTERNAL` variable

- sometimes, storing your packages, custom configs and defconfigs in the buildroot tree might not be the most practical.
- the `BR2_EXTERNAL` lets you store your package recipes, defconfigs and other things outside the buildroot source tree.

- **note**: this can only be used to add new packages, not to overwrite existing ones.

- you'd pass this into make as a environment variable, as a colon separated list

```bash
make BR2_EXTERNAL=/path/to/external1:/path/to/external2
```
- you only need to do this once, as it'll save in a `.br2-external.mk` file

## `BR2_EXTERNAL`: important files in external directory

- each external directory must contain:
    - `external.desc` which provides a name and description
    - `Config.in`, configuration options that will be included in menuconfig
    - `external.mk` is included in the make logic

## `BR2_EXTERNAL` - external.desc file

- this file gives metadata about an external tree

```desc
name: FOOBAR
desc: Foobar Company
```

## `BR2_EXTERNAL`: main Config.in

- this provides the config options to buidroot.
- you can use `$BR2_EXTERNAL_<NAME>_PATH`, where name is defined in the extern.desc fie

```Config
source "$BR2_EXTERNAL_<NAME>_PATH/package/package1/Config.in"
source "$BR2_EXTERNAL_<NAME>_PATH/package/package2/Config.in"
```

## BR2_EXTERNAL: external.mk

- this only includes make logic
- only meant to include the package .mk files

```Makefile
include $(sort $(wildcard $(BR2_EXTERNAL_<NAME>_PATH)/package/*/*.mk))
```

