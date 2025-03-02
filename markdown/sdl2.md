# SDL2 In Rust

## What is SDL2

- SDL2 is a framework that provides:
    - Window Management
    - 2D graphics
    - Input event systems
    - Timing management
    - Audio processing
    - File IO
    - Threading
    - 3D graphics

## How do we initialize SDL2

```rs
let sdl2_result = sdl2::init()?;
```

- This function initializes all subsystems of sdl2


## Creating a window

- Your program needs a window for multimedia input and output.
- to create a window, you need to first initialize the video subsystem.
    - This is done by calling the window function in the sdl2 structure.
- After initializing the subsystem, you can create the window by using the ```window``` function. 
```rs
    let sdl2_result = sdl2::init()?;
    let sdl2_window = sdl2_result.video(); //initialized subsystem.
    let window = video_subsystem
        .window("rust-sdl2 demo: Video", 800, 600) //creates window
        .build()?; //builds windowe
```

## Creating the Canvas

- The canvas types owns the surface or window.
    - The canvas lets you modify the window and surface.

```rs
    // ...
    let window = video_subsystem
        .window("rust-sdl2 demo: Video", 800, 600) //creates window
        .build()?; //builds windowe
    let mut canvas = window.into_canvas().build()?;
    canvas.set_draw_color(Color::RGB(255,0,0));
    canvas.clear(); 
    canvas.present();

```

- `canvas.set_draw_color(Color::RGB)` will specify the color all operations use.
- `canvas.clear()` will clear the canvas to it's default color.
- `canvas.present()` will render the frame.

## SDL2 - loops

```rs
    // ...
    let mut event_pump = sdl_context.event_pump()?;
    'running: loop {
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit { .. }
                | Event::KeyDown {
                    keycode: Some(Keycode::Escape),
                    ..
                } => break 'running,
                _ => {}
            }
        }

        canvas.clear();
        canvas.present();
        std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 30));
        // The rest of the game loop goes here...
    }
```
- The event_pump holds SDL Event related functions (event listeners).
    - we get a stream of events using `.poll_iter()`, and we use pattern matching.
    - We use Pattern matching for the KeyDown event.
        - KeyDown is an enum struct which has a keycode, which is an option enum holding a keycode.
- Event is an enum containing things like: Quit, KeyDown, KeyUp, etc.

## Textures

- Textures are the GPU Rendering equivalent of surfaces.
    - Textures are created from texture_creator, which are created from canvases

```rs
let texture = canvas.texture_creator();
let mut texture = canvas.create_texture_target(PixelFormatEnum::RGB24, 256, 240);
```
- `canvas.texture_creator()` returns the texture creator.
- `create_texture_tarpet` returns a texture.
    - The PixelFormatEnum is the pixel coloring format
    - 256 is the width
    - 240 is the height.

## Updating textures 

- We can update textures using texture.update, which passes in a u8 buffer:

```rs
texture.update(None, &frame.data, 256 *2 * 3).unwrap(); example
pub fn update<R>( &mut self, rect: R, pixel_data: &[u8], pitch: usize)
// returns return enum
```
- This function updates the texture, or to a particular section if a Rectangle is passed in.
- Pitch specifies how many bytes are in a row. 


## Copying texture to canvas

- To copy a texture to a canvas, we can use the `copy()` method.

```rs
texture.update(None, &frame.data, 256 *2 * 3).unwrap();
canvas.copy(&texture, None, None).unwrap();
pub fn copy<R1, R2>(&mut self, texture: &Texture<'_>, src: R1, dst: R2) //results enum.
```
- This copies the texture to the canvas.
- The first argument is the texture to copy.
- src is a rectangle that chooses what part of the texture to copy from.
    - If none, it copies the whole texture.
- dest is a rectangle that specifies where to copy the texture to.
    - If none, it stritches the teture to fill the canvas.