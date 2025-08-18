# 9. Userspace ALSA

## What is `alsa-lib`

- ALSA lib is a library letting you interact with alsa devices from userspace.
- the library itself is usually libasound

## alsa-lib - opening/closing pcm, PCM name

```c
int snd_pcm_open(
    snd_pcm** pcmp, //array of pcms
    const char* name,  //name of pcm to be opened
    snd_pcm_stream_t stream, //either SND_PCM_STREAM_PLAYBACK or SND_PCM_STREAM_CAPTURE
    int mode //SND_PCM_NONBLOCK or SND_PCM_ASYNC (or both)
    );
int snd_pcm_close(snd_pcm_t* pcm);
```


- The name consist of three arguments:
    - CARD: name of the hardware.
    - DEV: device number
    - SUBDEV: subdevice number
- You can use aplay's device_list to see all the physical cards.

## alsa-lib, setting pcm parameters

- you can set the hardware parameters for the pcm:

```c
int snd_pcm_hw_params_set_channels( //defines number of channels
    snd_pcm_t* pcm, 
    snd_pcm_hw_params_t* params,
    unsigned int val);

int snd_pcm_hw_params_set_rate_near( //sets the sample rate
    snd_pcm_t* pcm,
    snd_pcm_hw_params_t* params,
    unsigned int* val,
    int* dir);

int snd_pcm_hw_params_set_period( //set number of periods
    snd_pcm_t* pcm, 
    snd_pcm_hw_params_t* params,
    unsigned int val,
    int dir);

int snd_pcm_hw_params_set_period_size( //set period size in buffer
    snd_pcm_t *pcm, 
    snd_pcm_hw_params_t *params,
    snd_pcm_uframes_t val, int dir);

int snd_pcm_hw_params_set_buffer_size( //set size of buffer
    snd_pcm_t *pcm, 
    snd_pcm_hw_params_t *params,
    snd_pcm_uframes_t val);

int snd_pcm_hw_params( //install parameters and prepare stream
    snd_pcm_t* pcm,
    snd_pcm_hw_params_t* params);

int snd_pcm_hw_params_free(snd_pcm_hw_params_t* obj); //free param
int snd_pcm_prepare(snd_pcm_t* pcm); //prepares stream
int snd_pcm_wait(snd_pcm_t* pcm, int timeout); //waits for PCM to be ready. 
```


## alsa-lib api - PCM writing/reading stream

```c
/* write/read from interleaved or non-interleaved buffer */
snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t *pcm, const void *buffer, snd_pcm_uframes_t size)
snd_pcm_sframes_t snd_pcm_readi(snd_pcm_t *pcm, void *buffer, snd_pcm_uframes_t size)
snd_pcm_sframes_t snd_pcm_writen(snd_pcm_t *pcm, void **bufs, snd_pcm_uframes_t size)
snd_pcm_sframes_t snd_pcm_readn(snd_pcm_t *pcm, void **bufs, snd_pcm_uframes_t size)

/* write/read from interleaved or non-interleaved mmap buffer. */
int snd_pcm_mmap_begin(
    snd_pcm_t *pcm, 
    const snd_pcm_channel_area_t **areas,
    snd_pcm_uframes_t *offset, 
    snd_pcm_uframes_t *frames)

snd_pcm_sframes_t snd_pcm_mmap_commit(
    snd_pcm_t *pcm, 
    snd_pcm_uframes_t offset,
    snd_pcm_uframes_t frames)

snd_pcm_sframes_t snd_pcm_mmap_writei(
    snd_pcm_t *pcm, 
    const void *buffer,
    snd_pcm_uframes_t size)

snd_pcm_sframes_t snd_pcm_mmap_readi(
    snd_pcm_t *pcm, 
    void *buffer, 
    snd_pcm_uframes_t size)

snd_pcm_sframes_t snd_pcm_mmap_writen(
    snd_pcm_t *pcm, 
    void **bufs, 
    snd_pcm_uframes_t size)

snd_pcm_sframes_t snd_pcm_mmap_readn(
    snd_pcm_t *pcm, 
    void **bufs, 
    snd_pcm_uframes_t size)
```

## alsa-lib: setting controls

- You set the controls with the `snd_ctl_t` type

```c
/* procedure */

/* step one: open sound card to be controlled */
snd_ctl_t* handle;
int snd_ctl_open(snd_ctl_t** ctl, const char* name, int mode);

/* step two open allocate a particular control, and value to set for the contrlol*/
snd_ctl_eleme_id_t* id;
snd_ctl_elem_id_alloca(id);
snd_ctl_elem_value_t* value;
snd_ctl_elem_value_alloca(ptr);

/* step three: set interface and name of control to be set */
void snd_ctl_elem_id_set_interface(snd_ctl_elem_id_t *obj, snd_ctl_elem_iface_t val);
void snd_ctl_elem_id_set_name(snd_ctl_elem_id_t *obj, const char *val);

/* step four: link value with control id */
void snd_ctl_elem_value_set_id(snd_ctl_elem_value_t *obj, const snd_ctl_elem_id_t *ptr);

void snd_ctl_elem_value_set_boolean(
    snd_ctl_elem_value_t *obj, 
    unsigned int idx, 
    long val)

void snd_ctl_elem_value_set_integer(
    snd_ctl_elem_value_t *obj, 
    unsigned int idx, 
    long val)

void snd_ctl_elem_value_set_integer64(
    snd_ctl_elem_value_t *obj, 
    unsigned int idx,
    long long val)

void snd_ctl_elem_value_set_enumerated(
    snd_ctl_elem_value_t *obj, 
    unsigned int idx,
    unsigned int val)

void snd_ctl_elem_value_set_byte(
    snd_ctl_elem_value_t *obj, 
    unsigned int idx,
    unsigned char val)

void snd_ctl_elem_set_bytes(
    snd_ctl_elem_value_t *obj, 
    void *data, 
    size_t size)

int snd_ctl_elem_write(snd_ctl_t* ctl, snd_ctl_elem_value_t* data);
```

## Alsa-utils

- alsa-utils provides a set of tools to interact with ALSA devices.