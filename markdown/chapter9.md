# 9. Regmap API - A Register Map Abstraction

## why do we need the Regmap API

- One way of developing I2C and SPI drivers is dealing with the SPI and I2C core
- this led to redundant code, hence it lead to the standalone regmap api

- The regmap API provides an abstraction over the I2C and SPI subsystem, to reduce repetitive code. 

## regmap API - Structs

- There are only a few structs needed:
    - `struct regmap_config` for configuring regmap
    - `struct regmap` is the regmap instance itself.

## `regmap_config` struct

- this structure stores the configuration of the regmap during the driver's lifetime.

- here, you store information about:
    - the register addresses.
    - functions the kernel uses to check if a particular register is readable or writable
    - functions where the kernel needs to be read from the regmap cache, 'volatile_reg'.
        - 



## initializing/removing regmap

- there are two functions for initializing, and one function for exiting from the regmap

```c
regmap_init_i2c(struct i2c_device* spi, const struct regmap_config);
struct regmap* regmap_init_spi(struct spi_device* spi, const struct regmap_config);
void regmap_exit(struct regmap* map;)
```

## device access

- there are a bunch

```c
int regmap_read(struct regmap* map, unsigned int reg, unsigned int* val);
int regmap_write(struct regmap* map, unsigned int reg, unsigned int val);
regmap_update_bits(struct regmap* map, unsigned int reg, unsigned int mask, unsigned int val);
int regmap_multi_reg_write(struct regmap *map, const struct reg_sequence *regs, int num_regs);
int regmap_bulk_read(struct regmap* map, unsigned int reg, void* val, size_t val_count);
int regmap_bulk_write(struct regmap* map, unsigned int reg, const void* val, size_t val_count)
```

- `regmap_write` writes data to device
- `regmap_read` reads data from device
- `regmap_update_bits` will read data, modify, then write back to the register.
- `regmap_multi_reg_write()` will write to multiple registers of the device
- `regmap_bulk_(read|write)` will write and read bytes of data to the device at a time

## regmap and caches

- regmap supports caching, which you can use by setting the value of the cache_type in the `regmap_config`

