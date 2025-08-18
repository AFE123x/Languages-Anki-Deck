# 4. CODEC driver

## codec driver - registering

- The CODEC driver is represented via `snd_soc_component_driver`
    - to register, you use the register function listed below, where you pass in the `snd_soc_component_driver` and `snd_soc_dai_driver`
```c
// to register device.
int snd_soc_register_component(
    struct device *dev,
    const struct snd_soc_component_driver *component_driver,
    struct snd_soc_dai_driver *dai_drv,
    int num_dai);

int devm_snd_soc_register_component(
    struct device *dev,
    const struct snd_soc_component_driver *component_driver,
    struct snd_soc_dai_driver *dai_drv,
    int num_dai);
```

## `struct snd_soc_component_driver` - introduction

```c
struct snd_soc_component_driver {
const char *name;
/* Default control and setup, added after probe() is run */
const struct snd_kcontrol_new *controls;
unsigned int num_controls;
const struct snd_soc_dapm_widget *dapm_widgets;
unsigned int num_dapm_widgets;
const struct snd_soc_dapm_route *dapm_routes;
unsigned int num_dapm_routes;
int (*probe)(struct snd_soc_component *component);
void (*remove)(struct snd_soc_component *component);
int (*suspend)(struct snd_soc_component *component);
int (*resume)(struct snd_soc_component *component);
// [...]
};
```

- `struct snd_kcontrol_new` is an array of controls (volume, mixing, etc.)
- `struct snd_soc_dapm_widget` is an array of power management controls so ASoC can turn off routes not being used.
- `struct snd_soc_dapm_route` describes the routes between CPU codec and other codec.

## `struct snd_soc_component_driver` - changing settings

- There are various functions you can use here
```c
int (*set_sysclk)(
    struct snd_soc_component *component,
    int clk_id, 
    int source, 
    unsigned int freq, 
    int dir);

int (*set_pll)(
    struct snd_soc_component *component, 
    int pll_id,
    int source, 
    unsigned int freq_in, 
    unsigned int freq_out);

// [...]
int (*hw_params)(
    struct snd_soc_component *component,
    struct snd_pcm_substream *substream,
    struct snd_pcm_hw_params *params
    );
```
- `set_sysclk` lets you adjust the input clock of the component
- `set_pll` configures the PLL
- `hw_params` is a callbock on PCM stream setup, lets you configure hardware.

## `struct snd_soc_dai_driver`

- This structure represents the Digital Audio Interface Driver

```c
struct snd_soc_dai_driver {
/* DAI description */
const char *name;
/* [...] */

/* ops */
const struct snd_soc_dai_ops *ops;
const struct snd_soc_cdai_ops *cops;

/* DAI capabilities */
struct snd_soc_pcm_stream capture; //defines format of stream
struct snd_soc_pcm_stream playback;
/* [...] */
};
```

## `struct snd_soc_pcm_stream`

- This struct provides information about the SoC PCM stream.
    - example, sample rates, channels, etc.
```c
/* SoC PCM stream information */
struct snd_soc_pcm_stream {
	const char *stream_name;
	u64 formats;			/* SNDRV_PCM_FMTBIT_* */
	u32 subformats;			/* for S32_LE format, SNDRV_PCM_SUBFMTBIT_* */
	unsigned int rates;		/* SNDRV_PCM_RATE_* */
	unsigned int rate_min;		/* min rate */
	unsigned int rate_max;		/* max rate */
	unsigned int channels_min;	/* min channels */
	unsigned int channels_max;	/* max channels */
	unsigned int sig_bits;		/* number of bits of content */
};
```
