# 7. ASoC DAPM

## What is ASoC DAPM?

- DAPM stansd for Dynamic Audio Power Management
- DAPM tries to save power by shutting down audio routes not in use. 

- Implementing this consist of two parts:
    - Describe topology with DAPM Widgets and DAPM routes.

## DAPM Widgets

- Implementing the DAPM consist of two parts:
    - Describe topology with DAPM Widgets and DAPM routes.

- The DAPM widget represents the various components of an audio system, like inputs, outputs, mixers and amplifiers.

- We represent these components with the `snd_soc_dapm_widget`
- There are multiple macros for this.

## DAPM Routes

- Implementing the DAPM consist of two parts:
    - Describe topology with DAPM Widgets and DAPM routes.
- Routes are what connects the widgets together.

- We represent a route using an **array** of `struct snd_soc_dapm_route`.

```c
struct snd_soc_dapm_route {
	const char *sink; //where audio stream goes to 
	const char *control; //control, can be NULL
	const char *source; //audio source

	/* Note: currently only supported for links where source is a supply */
	int (*connected)(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink);

	struct snd_soc_dobj dobj;
};
```