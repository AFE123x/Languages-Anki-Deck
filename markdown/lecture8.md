# 8. Download infrastructure in buildroot

## Objective of buildroot - fetching projects
- Buildroot fetches source code from third party projects.
- It'll download via HTTP, FPT, Git, etc.

## bulidroot download infrastructure - download location

- each buildroot package will indicate in its `.mk` file which files need to be downloaded.
    - They can be tar files, patches or binary files.
- when downloading a file, buildroot will try the following:
    - local `$(DL_DIR)` directory, where downloaded files are kept
    - primary site, indicated by `BR2_PRIMARY_SITE`
    - original site, from the `.mk` file for package
    - backup buildroot mirror, indicated by `BR2_BACKUP_SITE`

## buildroot - `DL_DIR` variable

- When buildroot downloads a file, it cache's the file in the directory pointed to by `$(DL_DIR)`.
- you can change this with the `BR2_DL_DIR` config.
- files are only added, never removed.

## buildroot - primary site

- `BR2_PRIMARY_SITE` lets you define the location of a HTTP/FTP server.
- this will let you do a local mirror within your company.
- if `BR2_PRIMARY_SITE` will only use the primary site, not the backup or original site.

## backup buildroot mirror

- Here, if the upstream locations become temporarily unavailable, having a backup server will be useful.
- Backup service is configured with `BR2_BACKUP_SITE`

- by default, it's `http://sources.buildroot.net


## Bulidroot - downloading with VCS

- when we use source code from Git, or another VCS, buildroot cannot directly download a tarball.
- it'll use a VCS specific approach to fetch the specified version of the source from the VCS repository.
- buildroot will checkout and clone the repository, then create the tarball containing the source code (not the version control metadata).

## buildroot - vendoring

- Vendoring is the approach where language specific package managers download dependencies themselves.
    - For example, Cargo and go.
- buildroot supports vendering depencies for cargo and go, and will invoke the correct tools, then bundle the dependencies in the tarball.

## buildroot file integrity checking

- Buildroot packages can provide a `.hash` file to provide hashes for the downloaded file.
- buildroot's download infrastructure will use the hash file to check the integrity of the downloaded file.
- hashes will always be used on downloaded files, even if it's already cached
- if the hash fails, the build fails.


## buildroot - downloading related make targets

- `make source`: will download all the files needed to build the current configuration
- `make external-deps` wil ist the files the `$(DL_DIR)` that are needed for the current configuration to build.
    - it's not guaranteed al the files are in `$(DL_DIR)`, and make source is required.

## make - phony targets

- phony targets are make recipes that aren't a file

```Makefile
.PHONY: clean # usually good practice to keep
clean:
    rm -rf $(TARGET_DIR) $(BINARIES_DIR) $(HOST_DIR) \
    $(BUILD_DIR) $(BASE_DIR)/staging \
    $(LEGAL_INFO_DIR)
distclean: clean
    # [...]
    rm -rf $(BR2_CONFIG) $(CONFIG_DIR)/.config.old \
    $(CONFIG_DIR)/.auto.deps
```

- We aren't expecting a file name `clean`, but we're doing something.
- What if `clean` file existed? `make clean` would never run, so you'd use `.PHONY: clean` to clarify that you alway swant it to run.

## make - defining and referencing variables

- there are many ways to define variables:

| syntax | meaning |
| --- | --- |
| `FOOBAR = value` | expanded at time of use |
| `FOOBAR := value` | expanded at time of assignment |
| `FOOBAR += value` | append to variable, with separating space. defaults to expanded at time of use |
| `FOOBAR ?= value` | defined only if not already defined. |
| `define NAME <multiline stuff between> endef` | multi line variables are described|

## make - conditions

- there are conditional statements in two forms:
    - `ifeq` or `ifneq`
    - `$(if ...)`
```Makefile
ifeq($(BR2_CACHE),y)  #if BR2_CACHE == y (yes)
    CCACHE := $(HOST_DIR)/bin/ccache
endif

HOSTAPD_LIBS += $(if $(BR2_STATIC_LIBS),-lcrypto -lz) # if BR2_STATIC_IBS is defined, include these two libraries.
```

## make - functions

you can define functions similar to a variable

```Makefile
# one line
MESSAGE = echo "$(TERM_BOLD)>>> $($(PKG)_NAME) $($(PKG)_VERSION) $(call qstrip,$(1))$(TERM_RESET)"

# multi line
define legal-license-header # pkg, license-file, {HOST|TARGET}
    printf "$(LEGAL_INFO_SEPARATOR)\n\t$(1):\
        $(2)\n$(LEGAL_INFO_SEPARATOR)\n\n\n" >>$(LEGAL_LICENSES_TXT_$(3))
endef

$(BUILD_DIR)/%/.stamp_extracted:
#     [...]
    @$(call MESSAGE,"Extracting")
    define legal-license-nofiles # pkg, {HOST|TARGET}
    $(call legal-license-header,$(1),unknown license file(s),$(2))
    endef
```
- Arguments are accessed by `$(1),$(2),etc.`
- you call the function using `$(call func,arg1,arg2,...)`

## useful make functions

- `subst` and `patsubst` replaces text.
```Makefile
ICU_SOURCE = icu4c-$(subst .,_,$(ICE_VERSION))-src.tgz
```

- `filter` and `filter-out` filters entries
- `foreach` implements loop

- EVEN MORE

## packages - writing recipes

- recipes are just shell commands
- each line must be indented with one tab
- each line of shell command is independent from others
    - variables aren't shared
- need to do single line, but you can use `\` for complex commands
- shell variables are referenced using `$$name`

```Makefile
define PPPD_INSTALL_RADIUS # defines PPPD_INSTALL_RADIUS
#     ...
    for m in $(PPPD_RADIUS_CONF); do \ # loop
    $(INSTALL) -m 644 -D $(PPPD_DIR)/pppd/plugins/radius/etc/$$m \ #command
    $(TARGET_DIR)/etc/ppp/radius/$$m; \
    done
#     ...
endef
```
