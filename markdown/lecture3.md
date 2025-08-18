# 3. ASoC

## What is ASoC?

- `ASoC`: **ALSA System on Chip**
- **ASoC** is a Linux kernel subsystem for ALSA support on system-on-chip and portable audio devices.
- Enables **reuse of codec drivers** across multiple architectures.
- Provides an **API for writing codec drivers**.
- Provides an **API for writing SoC audio interface drivers**.

## ASoC - Components

- **Codec class drivers**: define the codec's capabilities (audio interface, audio control, analog input/output).
- **platform class drivers**: defines the SoC audio interface (Called CPU DAI), setting up DMA when applicable.
- **Codec to platform integration**: done in the device tree instead of writing driver

## ALSA - `simple-audio-card`, populate device tree

- nowadays, we don't need to write separate drivers for sound cards.

- instead, we fill out the device tree

- `simple-sound-card` is a generic sound card driver that works.

### populating device tree

step 1: enable SAI and codec

- let's say we're using the ADAU1372 codec

```cs
&sai2 { //enables serial audio interface
    pinctrl-names = "default";
    pinctrl-0 = <&pinctrl_sai2>;
    status = "okay";
};
&i2c1 { //define i2c interface codec uses
    adau1372: codec@3c {
        #sound-dai-cells = <0>;
        compatible = "adi,adau1372";
        reg = <0x3c>;
        clock-names = "mclk";
        clocks = <&adau1372z_xtal>;
    };
};
/ { //define the clock speed of codec.
    adau1372z_xtal: adau1372z_xtal {
        compatible = "fixed-clock";
        #clock-cells = <0>;
        clock-frequency = <12288000>;
    };
};
```

step 2: describe soundcard itself.

```c
sound {
    compatible = "simple-audio-card"; //define driver
    simple-audio-card,name = "imx6ul-adau1372";
    simple-audio-card,dai-link@0 {
            format = "i2s"; //define format
            bitclock-master = <&adau1372_dai>;
            frame-master = <&adau1372_dai>;
            sai2_dai: cpu {
            sound-dai = <&sai2>;
        };
        adau1372_dai: codec {
            sound-dai = <&adau1372>;
        };
    };
};
```

## simple-card: routing

- you can route audio from the codec to board using the `simple-audio-card.routing` node.
    - by **routing**, routing defines how audio streams flow between different components inside the audio hardware

```cs
simple-audio-card,routing =
    "AIN0", "Line0", //line0 sends it's signal to AIN0
    "AIN1", "Line0",
    "AIN2", "Line1",
    "AIN3", "Line1",
    "Headphone Jack", "HPOUTL",
    "Headphone Jack", "HPOUTR",
    "<sink widget>", "<source widget>",
```

## Machine driver - sound card structure

- The machine driver registers a `struct snd_soc_card`

```c
struct snd_soc_card {
    const char *name;
    const char *long_name;
    const char *driver_name;
    struct device *dev;
    struct snd_card *snd_card;

    // [...]

    /* CPU <--> Codec DAI links */
    struct snd_soc_dai_link *dai_link; /* predefined links only */
    int num_links; /* predefined links only */
    struct list_head dai_link_list; /* all links */
    int num_dai_links;

    // [...]
};

/* functions to register */
int snd_soc_register_card(struct snd_soc_card *card);
int snd_soc_unregister_card(struct snd_soc_card *card);
int devm_snd_soc_register_card(struct device *dev, struct snd_soc_card *card);
```

## Machine driver - Routing linking

- to link the CPU DAI and the codec DAI, we use the `snd_soc_dai_link` structure

```c
struct snd_soc_dai_link {
    /* config - must be set by machine driver */
    const char *name; /* Codec name */
    const char *stream_name; /* Stream name */
    struct snd_soc_dai_link_component *cpus; //cpu codec
    unsigned int num_cpus;

    struct snd_soc_dai_link_component *codecs;
    unsigned int num_codecs;
    unsigned int dai_fmt; /* format to set on init */
}
```

- This structure above will link the codec driver to the SoC DAI Driver.
- We also need to define what are the codec outputs, and inputs that are statically used on the board.

- There are two approaches with this:
    - statically, using `.dapm_routes` and `.num_dapm_routes` member of `struct snd_soc_card`
    - from the device tree, using `snd_soc_of_parse_audio_routing(struct snd_soc_card* card, const char* propname)`


## Machine Driver - Routing: board connectors

- You define the board connectors in the machine driver.
- The connectors are represented with the `struct snd_soc_dapm_widget` part of the `struct snd_soc_card`.

```c
static const struct snd_soc_dapm_widget atmel_asoc_wm8904_dapm_widgets[] = {
    SND_SOC_DAPM_HP("Headphone Jack", NULL),
    SND_SOC_DAPM_MIC("Mic", NULL),
    SND_SOC_DAPM_LINE("Line In Jack", NULL),
}
```

## Machine Driver - Clocking: producer/consumer

- The producer/consumer relationship defines who generates the clock signal and who receives it.
- With the ASoC system, you define the relation using the `.dai_fmt` field of the `struct snd_soc_dai_link` type.

- here, you define who's the bit clock and frame clock master.

## `struct snd_soc_dai_link` callback functions

- You can define callback functions for the snd_soc_dai_link using the `struct snd_soc_ops` type.

```c
/* SoC audio ops */
struct snd_soc_ops {
    int (*startup)(struct snd_pcm_substream *);
    void (*shutdown)(struct snd_pcm_substream *);
    int (*hw_params)(struct snd_pcm_substream *, struct snd_pcm_hw_params *);
    int (*hw_free)(struct snd_pcm_substream *);
    int (*prepare)(struct snd_pcm_substream *);
    int (*trigger)(struct snd_pcm_substream *, int);
};
```

- `.hw_params` is called when setting up the audio stream.
    - the `struct snd_pcm_hw_param` type contains the audio characteristics.
    - there are various functions you can use to extract said characteristics

## extracting values from `struct snd_pcm_hw_param`

```c
static inline unsigned int params_rate(const struct snd_pcm_hw_params *p); //gets sample rate
static inline unsigned int params_channels(const struct snd_pcm_hw_params *p); //gets number of channels
static inline snd_pcm_format_t params_format(const struct snd_pcm_hw_params *p); //gets format (including bit depth)
int snd_soc_params_to_bclk(const struct snd_pcm_hw_params *parms); //calculates the bit clock
int snd_soc_dai_set_sysclk(struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir); //sets the clock rate and direction for the DAI
int snd_soc_dai_set_clkdiv(struct snd_soc_dai *dai,
int div_id, int div); //configure clock divisor
int snd_soc_dai_set_pll(struct snd_soc_dai *dai,
int pll_id, int source, unsigned int freq_in, unsigned int freq_out); //configure PLL
```