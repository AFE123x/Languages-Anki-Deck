# 8. CPU DAI Drivers

## What is a CPU DAI Driver? What should it do?

- The CPU DAI Driver is a component driver.
- it's responsible for handling IRQs, taking care of pinmuxing, clocks and DMA.

## CPU DAI Driver - Handling DMA

- DMA in alsa done through the `dmaengine_pcm`

- You register the DMA with the `devm_snd_dmaengine_pcm_register()`
    - this will parse the device tree if needed.

- THe DMA engine is configured using `snd_soc_dai_init_dma_data()`, which takes the DMA configuration for playback and capture.