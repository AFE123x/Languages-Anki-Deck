/**
 * this is my full analysis of the code
 */

// many include files
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio/consumer.h>

#define CS4270_FORMATS (SNDRV_PCM_FMTBIT_S8      | SNDRV_PCM_FMTBIT_S16_LE  | \
			SNDRV_PCM_FMTBIT_S18_3LE | SNDRV_PCM_FMTBIT_S20_3LE | \
			SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_LE)

#define CS4270_CHIPID	0x01	
#define CS4270_PWRCTL	0x02	
#define CS4270_MODE	0x03	
#define CS4270_FORMAT	0x04	
#define CS4270_TRANS	0x05	
#define CS4270_MUTE	0x06	
#define CS4270_VOLA	0x07	
#define CS4270_VOLB	0x08	

#define CS4270_FIRSTREG	0x01
#define CS4270_LASTREG	0x08
#define CS4270_NUMREGS	(CS4270_LASTREG - CS4270_FIRSTREG + 1)
#define CS4270_I2C_INCR	0x80

#define CS4270_CHIPID_ID	0xF0
#define CS4270_CHIPID_REV	0x0F
#define CS4270_PWRCTL_FREEZE	0x80
#define CS4270_PWRCTL_PDN_ADC	0x20
#define CS4270_PWRCTL_PDN_DAC	0x02
#define CS4270_PWRCTL_PDN	0x01
#define CS4270_PWRCTL_PDN_ALL	\
	(CS4270_PWRCTL_PDN_ADC | CS4270_PWRCTL_PDN_DAC | CS4270_PWRCTL_PDN)
#define CS4270_MODE_SPEED_MASK	0x30
#define CS4270_MODE_1X		0x00
#define CS4270_MODE_2X		0x10
#define CS4270_MODE_4X		0x20
#define CS4270_MODE_SLAVE	0x30
#define CS4270_MODE_DIV_MASK	0x0E
#define CS4270_MODE_DIV1	0x00
#define CS4270_MODE_DIV15	0x02
#define CS4270_MODE_DIV2	0x04
#define CS4270_MODE_DIV3	0x06
#define CS4270_MODE_DIV4	0x08
#define CS4270_MODE_POPGUARD	0x01
#define CS4270_FORMAT_FREEZE_A	0x80
#define CS4270_FORMAT_FREEZE_B	0x40
#define CS4270_FORMAT_LOOPBACK	0x20
#define CS4270_FORMAT_DAC_MASK	0x18
#define CS4270_FORMAT_DAC_LJ	0x00
#define CS4270_FORMAT_DAC_I2S	0x08
#define CS4270_FORMAT_DAC_RJ16	0x18
#define CS4270_FORMAT_DAC_RJ24	0x10
#define CS4270_FORMAT_ADC_MASK	0x01
#define CS4270_FORMAT_ADC_LJ	0x00
#define CS4270_FORMAT_ADC_I2S	0x01
#define CS4270_TRANS_ONE_VOL	0x80
#define CS4270_TRANS_SOFT	0x40
#define CS4270_TRANS_ZERO	0x20
#define CS4270_TRANS_INV_ADC_A	0x08
#define CS4270_TRANS_INV_ADC_B	0x10
#define CS4270_TRANS_INV_DAC_A	0x02
#define CS4270_TRANS_INV_DAC_B	0x04
#define CS4270_TRANS_DEEMPH	0x01
#define CS4270_MUTE_AUTO	0x20
#define CS4270_MUTE_ADC_A	0x08
#define CS4270_MUTE_ADC_B	0x10
#define CS4270_MUTE_POLARITY	0x04
#define CS4270_MUTE_DAC_A	0x01
#define CS4270_MUTE_DAC_B	0x02


static const struct reg_default cs4270_reg_defaults[] = {
	{ 2, 0x00 },
	{ 3, 0x30 },
	{ 4, 0x00 },
	{ 5, 0x60 },
	{ 6, 0x20 },
	{ 7, 0x00 },
	{ 8, 0x00 },
};

static const char *supply_names[] = {
	"va", "vd", "vlc"
};
/**
 * private meta data for the codec
 */
struct cs4270_private {
	struct regmap *regmap; //for i2c interraction
	unsigned int mclk; //the master clock frequency
	unsigned int mode; //ifferent controls/modes (I2S, left/right justified, etc.)
	unsigned int slave_mode; //if in slave mode, it'll listen to master for clock signal.
	unsigned int manual_mute; //manually mute sound (not automatically.)

	struct regulator_bulk_data supplies[ARRAY_SIZE(supply_names)]; //lets driver talk to regulator core. controls power rails

	struct gpio_desc *reset_gpio; //gpio for reset pin on codec
};

/**
 * This is our snd_soc_dapm_widget array
 * snd_soc_dapm_widget is a kernel level descriptore used by ASoC
 * Here, we define the elements in the codec (input, out, mixer, etc.)
 */
static const struct snd_soc_dapm_widget cs4270_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("AINL"), //names left analog input, AINL
SND_SOC_DAPM_INPUT("AINR"), //names right analog input, AINR

SND_SOC_DAPM_OUTPUT("AOUTL"), //name left analog output, AOUTL
SND_SOC_DAPM_OUTPUT("AOUTR"), //name right analog output, AOUTR
};

/**
 * Defines the dapm routes
 * snd_soc_dapm_route Defines an audio route originating at source via 
 * control and finishing at sink.
 */
static const struct snd_soc_dapm_route cs4270_dapm_routes[] = {
	{ "Capture", NULL, "AINL" }, //source is AINL, NULL is control, Capture is sink.
	{ "Capture", NULL, "AINR" },

	{ "AOUTL", NULL, "Playback" }, //Playback is the source, NULL is the control, AOUTR is the sink.
	{ "AOUTR", NULL, "Playback" },
};


struct cs4270_mode_ratios {
	unsigned int ratio; //how many times the external MCLK is faster than the audio sample rate.
	u8 speed_mode; //multiply the divisor of mclk
	u8 mclk; //clock divider, dividing the clock signal
};

/**
 * This is a Clock Ratio table
 * @param ratio is the ratio of MCLK to the sample rate
 * @param speed_mode the Speed Mode bits to set in the Mode Control register for this ratio
 * @param mclk is the Ratio Select bits to set in the Mode Control register for this ratio
 * 
 * https://statics.cirrus.com/pubs/proDatasheet/CS4270_DS686F2.pdf
 * 
 * |     7    |     6    |  5  |  4  |     3      |     2      |    1      |   0  |
 * | reserved | reserved | fm1 | fm0 | mclk_freq2 | mclk_freq1 |mclk_freq0 | popg |
 * 
 * | fm1 | fm0 |           mode           |       macro       |
 * |-----|-----|--------------------------|-------------------|
 * |  0  |  0  | single speed master mode |  CS4270_MODE_1X   |
 * |  0  |  1  | double speed master mode |  CS4270_MODE_1X   |
 * |  1  |  0  |  quad-speed master mode  |  CS4270_MODE_1X   |
 * |  1  |  1  |        slave mode        | CS4270_MODE_SLAVE |
 * 
 * | MCLK_FREQ2 | MCLK_FREQ1 | MCLK_FREQ0 |           Mode          |
 * |------------|------------|------------|-------------------------|
 * |     0      |     0      |     0      |  divide by 1 (default)  |
 * |     0      |     0      |     1      | divide by 1.5 (default) | this is buggy
 * |     0      |     1      |     0      |  divide by 2 (default)  |
 * |     0      |     1      |     1      |  divide by 3 (default)  |
 * |     1      |     0      |     0      |  divide by 4 (default)  |
 * 
 * the internal clock = MCLK / (speed_mode * divide factor)
 */
static struct cs4270_mode_ratios cs4270_mode_ratios[] = {
	{64, CS4270_MODE_4X, CS4270_MODE_DIV1},
#ifndef CONFIG_SND_SOC_CS4270_VD33_ERRATA
	{96, CS4270_MODE_4X, CS4270_MODE_DIV15},
#endif
	{128, CS4270_MODE_2X, CS4270_MODE_DIV1},
	{192, CS4270_MODE_4X, CS4270_MODE_DIV3},
	{256, CS4270_MODE_1X, CS4270_MODE_DIV1},
	{384, CS4270_MODE_2X, CS4270_MODE_DIV3},
	{512, CS4270_MODE_1X, CS4270_MODE_DIV2},
	{768, CS4270_MODE_1X, CS4270_MODE_DIV3},
	{1024, CS4270_MODE_1X, CS4270_MODE_DIV4}
};

#define NUM_MCLK_RATIOS		ARRAY_SIZE(cs4270_mode_ratios) //macro for length of array

// simply checks if the register is readable, and valid.
static bool cs4270_reg_is_readable(struct device *dev, unsigned int reg)
{
	return (reg >= CS4270_FIRSTREG) && (reg <= CS4270_LASTREG);
}
/**
 * ALSA SoC sound system maintains a "register cache", so you don't need 
 * to read from a register every time.
 * There are certain registers that can't be cached:
 * - write-only, reading would return garbage data.
 * - register value changes independently of what the register wrote.
 * 
 * If the function returns true, ALSA will not cache the register.
 */
static bool cs4270_reg_is_volatile(struct device *dev, unsigned int reg)
{

	if ((reg < CS4270_FIRSTREG) || (reg > CS4270_LASTREG))
		return true;

	return reg == CS4270_CHIPID;
}

/**
 * This function determines the codec's sample rate.
 * change the driver only data, not actually interract
 * with the hardware.
 */
static int cs4270_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				 int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_component *component = codec_dai->component; //retrieve the snd_soc_component from snd_soc_dai
	
	/* retrieve the driver metadata from the snd_soc_component, returns void pointer */
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);

	cs4270->mclk = freq; //sets sample rate of this metadata
	return 0;
}
/**
 * this function will setup the codec for the particular audio format
 * it passes in the dai, and a format integer.
 * - You use a bitwise and extract certain bits to get the specific data
 * 
 * https://statics.cirrus.com/pubs/proDatasheet/CS4270_DS686F2.pdf
 * 
 * DAC Digital Interface Format (bit 4:3) of ADC and DAC control
 * |        7      |        6      |      5     |     4    |     3    |     2    |     1    |     0    |
 * | ADC_HPF_FRZ_A | ADC_HPF_FRZ_B | DIG_LOOPBK | DAC_DIF1 | DAC_DIF0 | reserved | reserved | ADC_DIF0 |
 * 
 * | DAC_DIF1 | DAC_DIF0 |           description          |                              notes                             |
 * |    ---   |    ---   |               ---              |                               ---                              |
 * |     0    |     0    | left justified (up to 24 bits) |                               ---                              |
 * |     0    |     1    |       i2s, up to 24 bits       |                               ---                              |
 * |     1    |     1    |    right justified, 16 bits    | ASoc doesn't support different formats for playback vs. record |
 * |     1    |     0    |    right justified, 24 bits    |                          same reason ^                         |
 */
static int cs4270_set_dai_fmt(struct snd_soc_dai *codec_dai,
			      unsigned int format)
{
	struct snd_soc_component *component = codec_dai->component;
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component); //get specific driver data

	switch (format & SND_SOC_DAIFMT_FORMAT_MASK) { //select the format (i2s, left/right justified)
	case SND_SOC_DAIFMT_I2S: //macro is 1, aligns with table above
	case SND_SOC_DAIFMT_LEFT_J: //mode is 3 (doesn't make sense???)
		cs4270->mode = format & SND_SOC_DAIFMT_FORMAT_MASK; //set the mode in struct to format
		break;
	default: //if you get here, it's an invalid format number
		dev_err(component->dev, "invalid dai format\n");
		return -EINVAL;
	}

	switch (format & SND_SOC_DAIFMT_MASTER_MASK) { //setup master/slave audio interface
	case SND_SOC_DAIFMT_CBC_CFC: //codec clock consumer, frame consumer
		cs4270->slave_mode = 1; //we enable slave mode on drvdata
		break;
	case SND_SOC_DAIFMT_CBP_CFP: //here, codec clock is the producer, frame clock is producer.
		cs4270->slave_mode = 0; //disable slave mode on drvdata
		break;
	default: //state doesn't exist

		dev_err(component->dev, "Unknown master/slave configuration\n");
		return -EINVAL;
	}

	return 0;
}
/**
 * This function actually interracts with the hardware. 
 * @param substream is the audio stream
 * @param params the hardware paraments
 * @param dai is the SOC DAI
 */
static int cs4270_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component); //acquire driver data
	int ret;
	unsigned int i;
	unsigned int rate;
	unsigned int ratio;
	int reg;

	rate = params_rate(params);	//acquire the sample rate from hw params
	ratio = cs4270->mclk / rate; //gets ratio between master clock and sample rate

	for (i = 0; i < NUM_MCLK_RATIOS; i++) {
		/**
		 * this will go through the ratio table for the correct ratio.
		 */
		if (cs4270_mode_ratios[i].ratio == ratio)
			break;
	}

	if (i == NUM_MCLK_RATIOS) { //check ratio doesn't exist, it's an error

		dev_err(component->dev, "could not find matching ratio\n");
		return -EINVAL;
	}
	
	reg = snd_soc_component_read(component, CS4270_MODE); //read from Mode Control register
	/**
	 * |     7    |     6    |  5  |  4  |     3      |     2      |    1      |   0  |
 	 * | reserved | reserved | fm1 | fm0 | mclk_freq2 | mclk_freq1 |mclk_freq0 | popg |
	 * our 'reg' value
	 */
	
	reg &= ~(CS4270_MODE_SPEED_MASK | CS4270_MODE_DIV_MASK); //clear MCLK and SPEED mask bits
	reg |= cs4270_mode_ratios[i].mclk; //set the master clock (mclk_freq*)

	if (cs4270->slave_mode)
		reg |= CS4270_MODE_SLAVE; //set fm1 and fm0 to 11 (3)
	else
		reg |= cs4270_mode_ratios[i].speed_mode; //set speed to the speedmode in ratio array

	ret = snd_soc_component_write(component, CS4270_MODE, reg); //write the i2c register
	if (ret < 0) {
		dev_err(component->dev, "i2c write failed\n");
		return ret;
	}

	reg = snd_soc_component_read(component, CS4270_FORMAT); //retrieve the DAC AND ADC control register
	/**
	 *  DAC Digital Interface Format (bit 4:3) of ADC and DAC control, contents
     * |        7      |        6      |      5     |     4    |     3    |     2    |     1    |     0    |
     * | ADC_HPF_FRZ_A | ADC_HPF_FRZ_B | DIG_LOOPBK | DAC_DIF1 | DAC_DIF0 | reserved | reserved | ADC_DIF0 |
	 */
	reg &= ~(CS4270_FORMAT_DAC_MASK | CS4270_FORMAT_ADC_MASK); //clear the ADC_DIF0, DAC_DIF0 and DAC_DIF1 bits

	switch (cs4270->mode) { //checks the mode set in the set_dai_fmt() function
	case SND_SOC_DAIFMT_I2S:
		reg |= CS4270_FORMAT_DAC_I2S | CS4270_FORMAT_ADC_I2S; //enable ADC_DIF0, meaning i2s. sets DAC_DIF0/1 to 0 and 1 respectfully
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		reg |= CS4270_FORMAT_DAC_LJ | CS4270_FORMAT_ADC_LJ; //sets ADC_DIF and DAC_DIF* bits to 0 (for left justified)
		break;
	default: //dai format is unknown.
		dev_err(component->dev, "unknown dai format\n");
		return -EINVAL;
	}

	ret = snd_soc_component_write(component, CS4270_FORMAT, reg); //write the register to the ADC and DAC control register
	if (ret < 0) {
		dev_err(component->dev, "i2c write failed\n");
		return ret;
	}

	return ret;
}
/**
 * This function will mute/unmute the audio.
 */
static int cs4270_dai_mute(struct snd_soc_dai *dai, int mute, int direction)
{
	struct snd_soc_component *component = dai->component;
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);
	int reg6;

	reg6 = snd_soc_component_read(component, CS4270_MUTE); //the Mute Control register
	/** content of Mute Control Register
	 * |     7    |     6    |     5     |       4      |       3      |     2    |       1      |       0      |
	 * | reserved | reserved | AUTO_MUTE | MUTE_ADC_CHB | MUTE_ADC_CHA | MUTE_POL | MUTE_DAC_CHB | MUTE_DAC_CHA | 
	 */
	if (mute) //checks the mute parameter to see whether to mute
		reg6 |= CS4270_MUTE_DAC_A | CS4270_MUTE_DAC_B; //mute both channels a and b (both set to 1)
	else {
		reg6 &= ~(CS4270_MUTE_DAC_A | CS4270_MUTE_DAC_B); //will clear the bits to unmute
		reg6 |= cs4270->manual_mute; //set in the cs4270_soc_put_mute() function for manual mute.
	}

	return snd_soc_component_write(component, CS4270_MUTE, reg6); //write to the mute control register
}

/**
 * cs4270_soc_put_mute(): puts callback for the "Master Playback switch" alsa control
 * @param kcontrol is the mixer control
 * @param ucontrol is the control element information
 */
static int cs4270_soc_put_mute(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component); //retrieve driver data
	int left = !ucontrol->value.integer.value[0]; //get the volume from ucontrol (left channel)
	int right = !ucontrol->value.integer.value[1]; //get the volume from ucontroller (right channel)

	cs4270->manual_mute = (left ? CS4270_MUTE_DAC_A : 0) |
			      (right ? CS4270_MUTE_DAC_B : 0); //retrieve bits based on left and right variable

	/**
	 * It reads the current Mute Control register,
     * Applies the new mute bits,
	 * Writes the register back,
	 * Updates the internal cache.
	 */
	return snd_soc_put_volsw(kcontrol, ucontrol); //it will store the value from ucontral.
}

/**
 * An array of snd_kcontrol_new struct
 * 
 * contains a list of non-DAPM controls the CS4270 supports.
 * you can check out the values via amixer -c <cardnum> scontents
 */
static const struct snd_kcontrol_new cs4270_snd_controls[] = {
	SOC_DOUBLE_R("Master Playback Volume", //double channel with separate registers
		CS4270_VOLA, CS4270_VOLB, 0, 0xFF, 1), //we have two registers here.
	SOC_SINGLE("Digital Sidetone Switch", CS4270_FORMAT, 5, 1, 0), //single channel control
	SOC_SINGLE("Soft Ramp Switch", CS4270_TRANS, 6, 1, 0),
	SOC_SINGLE("Zero Cross Switch", CS4270_TRANS, 5, 1, 0), //the i2c register values
	SOC_SINGLE("De-emphasis filter", CS4270_TRANS, 0, 1, 0),
	SOC_SINGLE("Popguard Switch", CS4270_MODE, 0, 1, 1),
	SOC_SINGLE("Auto-Mute Switch", CS4270_MUTE, 5, 1, 0),
	SOC_DOUBLE("Master Capture Switch", CS4270_MUTE, 3, 4, 1, 1),
	SOC_DOUBLE_EXT("Master Playback Switch", CS4270_MUTE, 0, 1, 1, 1,
		snd_soc_get_volsw, cs4270_soc_put_mute),
};

/**
 * All the operations for our sound device, for dai operations
 * - .hw_params is the function that actually talks to device
 * - everything else just configures the driver data.
 */
static const struct snd_soc_dai_ops cs4270_dai_ops = { //all the functions to actually modify our driver data
	.hw_params	= cs4270_hw_params,
	.set_sysclk	= cs4270_set_dai_sysclk,
	.set_fmt	= cs4270_set_dai_fmt,
	.mute_stream	= cs4270_dai_mute,
	.no_capture_mute = 1,
};

/**
 * Describes the Digital Audio Interface in terms of its ALSA, DAI and AC97
 * operations and capabilities. Has the playback and capture specs
 */
static struct snd_soc_dai_driver cs4270_dai = {
	.name = "cs4270-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_CONTINUOUS,
		.rate_min = 4000,
		.rate_max = 216000,
		.formats = CS4270_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_CONTINUOUS,
		.rate_min = 4000,
		.rate_max = 216000,
		.formats = CS4270_FORMATS,
	},
	.ops = &cs4270_dai_ops,
};
/**
 * Our probe function, part of the platform driver.
 * It's called when the kernel matches a platform device with it's driver
 */
static int cs4270_probe(struct snd_soc_component *component)
{
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);
	int ret;

	/**
	 * This will perform read/modify/write cycle
	 * @param component, the component we want to update
	 * @param reg the register to update (CS4270_MUTE here)
	 * @param mask the particular bits we want to update
	 * @param val the value for the bits
	 */
	ret = snd_soc_component_update_bits(component, CS4270_MUTE, CS4270_MUTE_AUTO, 0);
	if (ret < 0) {
		dev_err(component->dev, "i2c write failed\n");
		return ret;
	}

	/**
	 * Same as above, but updates the TRANS_SOFT and TRANS_ZERO bits in the CS4270_TRANS register
	 */
	ret = snd_soc_component_update_bits(component, CS4270_TRANS,
		CS4270_TRANS_SOFT | CS4270_TRANS_ZERO, 0);
	if (ret < 0) {
		dev_err(component->dev, "i2c write failed\n");
		return ret;
	}
	/**
	 * Will enable multiple regulator consumers?
	 * lets you enable multiple regulators
	 */
	ret = regulator_bulk_enable(ARRAY_SIZE(cs4270->supplies),
				    cs4270->supplies);

	return ret;
}
/**
 * Will clean resources when unloading modules
 */
static void cs4270_remove(struct snd_soc_component *component)
{
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);

	//disable regulators.
	regulator_bulk_disable(ARRAY_SIZE(cs4270->supplies), cs4270->supplies);
};

#ifdef CONFIG_PM //this is included if, from makemenu, PM is enalbled

/*
config PM
	bool "Power Management support"
	help
	  Support processor power management modes
*/

static int cs4270_soc_suspend(struct snd_soc_component *component)
{
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);
	int reg, ret;
	/**
	 * Power Control Register at address 0x02
	 * |    7   |     6    |    5    |    4     |     3    |     2    |    1    |  0  |
	 * |    -   |     -    |    -    |     -    |     -    |     -    |    -    |  -  |
	 * | freeze | reserved | PDN_ADC | reserved | reserved | reserved | PDN_DAC | PDN |
	 * 
     * - when PDN_ADC and PDN_DAC are set, the ADC and DAC portion of the device enters a low power state.
	 * - When the PDN bit is set, the device enters a low power state.
	 */
	
	/* This will read the content from the Power control register and combine it with the PDN_ADC, PDN_DAC and PDN register */
	reg = snd_soc_component_read(component, CS4270_PWRCTL) | CS4270_PWRCTL_PDN_ALL;
	if (reg < 0) // this is error handling. Should usually be handled before performing bit manipulation
		return reg;
	//we write the resulting register value back to the power control register.
	ret = snd_soc_component_write(component, CS4270_PWRCTL, reg);
	if (ret < 0)
		return ret;
	//disable the regulator system.
	regulator_bulk_disable(ARRAY_SIZE(cs4270->supplies),
			       cs4270->supplies);

	return 0;
}

static int cs4270_soc_resume(struct snd_soc_component *component)
{
	struct cs4270_private *cs4270 = snd_soc_component_get_drvdata(component);
	int reg, ret;
	//re-enable the regulators
	ret = regulator_bulk_enable(ARRAY_SIZE(cs4270->supplies),
				    cs4270->supplies);
	if (ret != 0) //error handling
		return ret;

	ndelay(500);

	regcache_sync(cs4270->regmap); //sync the regmap cache (i need to read up on this)
	/**
	 * Power Control Register at address 0x02
	 * |    7   |     6    |    5    |    4     |     3    |     2    |    1    |  0  |
	 * |    -   |     -    |    -    |     -    |     -    |     -    |    -    |  -  |
	 * | freeze | reserved | PDN_ADC | reserved | reserved | reserved | PDN_DAC | PDN |
	 * 
     * - when PDN_ADC and PDN_DAC are set, the ADC and DAC portion of the device enters a low power state.
	 * - When the PDN bit is set, the device enters a low power state.
	 */
	reg = snd_soc_component_read(component, CS4270_PWRCTL); //read the power control register
	reg &= ~CS4270_PWRCTL_PDN_ALL; //sets the power related bits back to 0 setting the ADC, DAC and device in high power state.

	return snd_soc_component_write(component, CS4270_PWRCTL, reg); //writing back to register.
}
#else
#define cs4270_soc_suspend	NULL
#define cs4270_soc_resume	NULL
#endif 

/**
 * Our actual codec driver, putting together all the components we need
 */
static const struct snd_soc_component_driver soc_component_device_cs4270 = {
	.probe			= cs4270_probe,
	.remove			= cs4270_remove,
	.suspend		= cs4270_soc_suspend,
	.resume			= cs4270_soc_resume,
	.controls		= cs4270_snd_controls, //the array of controls
	.num_controls		= ARRAY_SIZE(cs4270_snd_controls),
	.dapm_widgets		= cs4270_dapm_widgets, //the inputs and outputs
	.num_dapm_widgets	= ARRAY_SIZE(cs4270_dapm_widgets),
	.dapm_routes		= cs4270_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(cs4270_dapm_routes), //the actual connections between input/output
	.idle_bias_on		= 1, //bit field stuff
	.use_pmdown_time	= 1,
	.endianness		= 1, //defines little ndian and big endian
};

static const struct of_device_id cs4270_of_match[] = {
	{ .compatible = "cirrus,cs4270", },
	{ }
};
MODULE_DEVICE_TABLE(of, cs4270_of_match); //our device table, for components.

//our regmap configuration, where we configure i2c related things
static const struct regmap_config cs4270_regmap = {
	.reg_bits =		8,
	.val_bits =		8,
	.max_register =		CS4270_LASTREG,
	.reg_defaults =		cs4270_reg_defaults,
	.num_reg_defaults =	ARRAY_SIZE(cs4270_reg_defaults),
	.cache_type =		REGCACHE_MAPLE,
	.write_flag_mask =	CS4270_I2C_INCR,

	.readable_reg =		cs4270_reg_is_readable, //checks if register is readable
	.volatile_reg =		cs4270_reg_is_volatile, //checks if register should be cahced.
};


static void cs4270_i2c_remove(struct i2c_client *i2c_client)
{
	struct cs4270_private *cs4270 = i2c_get_clientdata(i2c_client); //retrieve client metadata

	/**
	 * Reset (Input) - Input for resetting all internal 
	 * registers to their default settings and 
	 * for placing the device in a low-power mode.
	 */
	gpiod_set_value_cansleep(cs4270->reset_gpio, 0); //set reset to low.
}
/**
 * We initialize the driver, allocating the metadata for the device, and initializing the regmap for i2c.
 */
static int cs4270_i2c_probe(struct i2c_client *i2c_client)
{

	struct cs4270_private *cs4270; //this is private metadata for the codec
	unsigned int val;
	int ret, i;

	/* devm_kzalloc, will allocate zeroed memory, attached to the device. 
	it's automatically freed automatically when the remove function is called. */
	cs4270 = devm_kzalloc(&i2c_client->dev, sizeof(struct cs4270_private),
			      GFP_KERNEL);
	if (!cs4270)
		return -ENOMEM;

	for (i = 0; i < ARRAY_SIZE(supply_names); i++)
		cs4270->supplies[i].supply = supply_names[i]; //will fill supply with the names

	//this function will retrieve the multiple regulator consumers.
	ret = devm_regulator_bulk_get(&i2c_client->dev,
				      ARRAY_SIZE(cs4270->supplies),
				      cs4270->supplies);
	if (ret < 0)
		return ret;

	cs4270->reset_gpio = devm_gpiod_get_optional(&i2c_client->dev, "reset",
						     GPIOD_OUT_LOW);
	if (IS_ERR(cs4270->reset_gpio)) {
		dev_dbg(&i2c_client->dev, "Error getting CS4270 reset GPIO\n");
		return PTR_ERR(cs4270->reset_gpio);
	}

	if (cs4270->reset_gpio) {
		dev_dbg(&i2c_client->dev, "Found reset GPIO\n");
		gpiod_set_value_cansleep(cs4270->reset_gpio, 1); //sets the value to high (takes codec out of reset mode.)
	}

	ndelay(500);

	cs4270->regmap = devm_regmap_init_i2c(i2c_client, &cs4270_regmap); //initialize the regmap device (interracts with I2C)
	if (IS_ERR(cs4270->regmap))
		return PTR_ERR(cs4270->regmap);

	ret = regmap_read(cs4270->regmap, CS4270_CHIPID, &val); //this will read the CHIP_ID register
	if (ret < 0) {
		dev_err(&i2c_client->dev, "failed to read i2c at addr %X\n",
		       i2c_client->addr);
		return ret;
	}

	if ((val & 0xF0) != 0xC0) { //will check if chip ID is correct (Same as expected from the datasheet)
		dev_err(&i2c_client->dev, "device at addr %X is not a CS4270\n",
		       i2c_client->addr);
		return -ENODEV;
	}

	dev_info(&i2c_client->dev, "found device at i2c address %X\n",
		i2c_client->addr);
	dev_info(&i2c_client->dev, "hardware revision %X\n", val & 0xF);

	i2c_set_clientdata(i2c_client, cs4270);

	ret = devm_snd_soc_register_component(&i2c_client->dev,
			&soc_component_device_cs4270, &cs4270_dai, 1); //we register the card here, links the i2c_clinte device with the alsa structs (componont and dai.)
	return ret;
}

static const struct i2c_device_id cs4270_id[] = {
	{"cs4270"},
	{}
};

//replace the need for module_init and module_exit
MODULE_DEVICE_TABLE(i2c, cs4270_id);


/**
 * Our structure for the i2c driver.
 * contains things, like the of table, etc.
 */
static struct i2c_driver cs4270_i2c_driver = {
	.driver = {
		.name = "cs4270",
		.of_match_table = cs4270_of_match,
	},
	.id_table = cs4270_id,
	.probe = cs4270_i2c_probe,
	.remove = cs4270_i2c_remove,
};

module_i2c_driver(cs4270_i2c_driver);

MODULE_AUTHOR("Timur Tabi <timur@freescale.com>");
MODULE_DESCRIPTION("Cirrus Logic CS4270 ALSA SoC Codec Driver");
MODULE_LICENSE("GPL");

// ⣿⣿⣿⡟⠀⠀⠀⠀⢠⠂⠀⠀⠀⠀⠀⠀⢀⣀⠀⠀⠀⠀⠀⣀⡀⢸⡀⠀⠀⢀⠀⠀⠀⠀⣧⢀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
// ⣿⣿⡟⠀⠀⠀⠀⣰⡗⢢⡶⠋⠀⠀⠀⠀⠉⠀⠀⠀⠀⠀⠀⠉⠉⣿⣿⡁⠈⠻⡇⠹⣷⡘⣟⠀⠋⠙⢆⠘⢶⢲⡄⢳⡀⠀⠀⠀⠀⠀
// ⣿⣿⠁⠀⠀⠀⢀⡟⠀⡟⠇⠀⠀⠀⠀⢀⠀⠀⠀⠀⠀⢀⣠⣴⣄⣹⣿⣧⠀⠀⢳⠀⢻⣇⢹⡌⠀⠀⠈⠀⠰⠸⡇⠈⣷⠀⠀⠀⠀⠀
// ⣿⡏⠀⠀⢠⠀⣼⠂⣰⣓⡠⠤⠖⠃⠀⢸⡀⠀⢰⠀⠀⠀⠀⠀⠀⠙⡿⣇⠉⠀⠉⠁⠚⣟⠈⠇⠓⠠⣀⣦⡀⡂⣿⡀⢻⡆⠀⠀⠀⠀
// ⠘⠃⠀⠀⢾⣾⡟⠈⠀⡏⠀⠀⠀⠀⢀⣿⡇⠀⠘⠀⠀⠀⠀⠀⠀⠀⡇⢻⡀⠀⠀⠀⠀⢻⠀⢣⡧⠀⠀⠀⠉⠙⣿⣾⢸⠃⠀⠀⠀⠀
// ⠀⠀⠀⠀⠈⠙⣧⠀⠐⠃⠀⠀⠀⠀⢸⢻⡇⠀⠀⠀⠀⠀⠀⠀⠀⢠⡇⠀⢷⠀⠀⠀⠀⢸⠀⢸⡇⠀⠀⠀⠀⠀⢹⣿⡆⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⣿⠀⢐⠀⠀⠀⠀⠀⡜⠀⡇⠀⠀⠀⠀⠀⠀⠀⢀⣿⣀⣀⣈⣧⠀⠀⠀⢸⡄⢰⠁⠀⠀⠀⠀⠀⢸⣿⣿⠀⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠠⣿⠀⡎⠀⠀⠀⠀⣠⠃⠀⡇⠀⡀⠀⠀⠀⠀⠀⡏⡏⠀⠀⠀⠘⣆⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⡆⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⠀⣸⢠⠇⠀⠀⠀⢰⡏⠀⠀⡇⢸⠃⠀⠀⠀⠀⢸⣹⠡⠀⠀⠀⠀⠘⣆⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣷⠀⠀⠀⠀
// ⠀⠀⠀⠀⠀⣿⡇⢸⠀⠀⠀⣰⠋⠁⠀⠀⣿⡞⠀⠀⠀⠀⢀⣿⡏⠀⠀⢀⣀⣤⣀⣈⣦⠀⠇⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⠀⠀⠀⠀
// ⠀⠀⠠⣾⠟⢹⣧⡇⠀⠀⣰⣿⣯⣖⡀⠀⣿⠁⠀⠀⠀⠀⣼⡟⠀⣰⡾⠿⣻⣿⣟⡛⠿⣦⣄⠀⠀⠀⠀⠀⠀⢰⠋⢀⠙⢿⡀⠀⠀⠀
// ⣀⠀⠀⣿⡆⢸⡟⢀⣴⣾⣿⣯⡙⡙⠇⠀⡏⠀⠀⠀⠀⣰⡟⠀⠜⠉⢠⣿⣿⣿⣄⠁⡄⠀⢹⡀⡀⠀⠀⠀⠀⢸⣔⠁⠀⠀⢷⠀⠀⠀
// ⡿⣿⢶⣟⠀⢸⡿⢯⠸⣿⡿⠿⠷⡇⠀⣸⠁⠀⠀⠀⣰⠟⠀⠀⠀⠀⢸⣿⡿⠛⠛⢷⡇⠀⣸⡃⡏⠀⠀⠀⠀⡞⠀⠀⠱⡀⢸⡆⠀⠀
// ⠀⠈⡿⠻⡄⢾⡆⠈⠣⣙⡷⠤⠞⡁⢠⠃⠀⠀⠀⡰⠋⠀⠀⠀⠀⠀⠀⠙⠷⠶⠶⢋⣠⠔⢹⠁⡇⠀⠀⠀⢠⡇⡄⠀⠀⡇⣸⠇⠀⠀
// ⢸⣶⣧⣠⡿⢾⡇⠀⠀⠀⠀⠀⠀⢠⠃⠀⠀⡠⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡏⠀⡇⠀⠀⠀⢸⠁⡇⠀⠀⢡⣿⠀⠀⠀
// ⡁⠙⣶⠿⣆⠘⣿⠀⠀⠀⠀⠀⡰⠃⠀⡠⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⠁⣰⡇⠀⠀⠀⣸⡞⠀⣠⢦⣿⣿⡆⠀⠀
// ⢲⣦⣏⣀⣾⠆⣿⡄⠀⠀⢠⠞⣁⡴⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡏⣰⠋⡇⠀⠀⢠⡯⠖⠉⣠⣾⣿⣿⡇⠀⠀
// ⠈⠻⣧⣹⣇⠀⣿⣧⣠⡶⠟⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣴⠃⢸⠃⠀⠀⢸⣶⣾⣿⣿⣿⣿⣿⣇⠀⠀
// ⣆⡀⣨⡇⢨⣧⢸⣿⡅⠀⠀⠀⠀⠀⠀⠀⠀⢠⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⠿⠋⠀⣿⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀
// ⠸⣿⣿⡟⣾⠁⠈⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠸⣿⡟⠉⠳⡄⠀⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⣰⡿⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀
// ⠀⣷⢈⣿⠉⣣⠀⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⢿⠋⠉⠙⣳⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠋⡇⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀
// ⢸⣿⣿⣧⣤⡟⠀⢹⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠙⠒⠚⠉⠀⠀⠀⠀⠀⠀⢀⡤⠚⠁⠀⢠⠇⠀⠀⣸⣿⣿⡿⢿⣿⣿⣿⣿⣿⡆⠀
// ⣨⣧⣿⣿⣏⣽⠄⠈⣿⣿⣿⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠴⠚⠁⠀⠀⠀⢀⣸⠀⠀⢰⠋⠁⢤⠀⠈⠛⣿⣿⣿⣿⡇⠀
// ⠀⣿⣿⣿⣿⠀⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣷⣦⣀⡀⣀⣠⡤⠚⠉⠀⠀⠀⢀⣤⣶⣿⠛⡞⠀⢀⠏⠁⠀⠀⠀⠀⠀⠈⢿⣿⣿⣇⠀
// ⠀⣿⣿⣿⣿⣇⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⠟⠛⠁⠠⠉⠀⡀⠀⢀⣀⣐⠚⠁⠀⣈⣁⠤⠗⠒⠛⠁⠀⠀⠀⠀⠀⠀⡀⠀⠈⠉⠛⠆