# TODO (November 2025)
- os/gfx module
  - [] finish input abstraction (wayland, windows)
  - [] drag and drop (wayland, windows)
  - [] opengl/egl runtime linking (wayland)
  - [] smooth resizing (wayland, windows)

- render module
  - [] render lines/curves
  - [] better sorting
  - [] more ergonomic render functions (introduce seaparate draw module?)
  - [] instancing
  - [] d3d11 backend on windows

- font module
  - [] check dwrite glyph rasterization (d3d instead of gdi?)
  
- audio module
  - [] sample rate conversion
  - [] midi (wasapi)
  - [] support changing samplerate, audiodevices (wasapi, jack)
  - [] lower latency (wasapi, jack)
  
- spectrogram example
  - [] lines instead of rectangles
  - [] visualize phase
  - [] add ui
  - [] spectra per frame, display spectrogram

# TODO (September 2025)
- os/gfx module
  - [] smooth resizing (wayland, windows)
  - [] drag and drop (wayland, windows)
  - [] finish keyboard/mouse input (wayland, windows)
  - [] generalize graphics library usage/initialization (wayland)
  - [] clean up event handling (wayland)
  
- render module
  - [] render lines/curves
  - [] more ergonomic render functions (introduce seaparate draw module?)
  - [] instancing
  - [] d3d11 backend on windows
  
- font module
  - [X] better glyph metric extraction (windows/directwrite)
  - [X] freetype on windows
  - [] Check dwrite glyph rasterization; it looks like some glyphs are off-by-one

- audio module
  - [X] route audio (wasapi)
  - [X] get sample rate from audio backend (windows/wasapi)
  - [X] fix wasapi input noise
  - [] support changing sample rate (wasapi, jack)
  - [] support changing connected audio devices (and overriding defaults) (wasapi, jack)
  - [] get/set audio device latency (wasapi, jack)
  - [] midi (wasapi)
  - [] how to support midi sysex? (jack)
  
- spectrogram example
  - [] lines instead of rectangles
  - [] visualize phase
  - [] add ui
  - [] store spectra per frame, display spectrogram
  - [X] more resilient spectrum caching

- [] logging system
- [] ui layout module
- [] plugin loading system
- [] way of denoting which functions are implemented in another layer
- [] abstract entry point
- [] input fuzzing
- [] profiler
- [] file format parsing/writing module
- [] file streaming
- [] thread pool/wavefront multithreading
- [] pseudo random number generation
- [] clean up os include situation, come up with consistent naming scheme
- [] macos port
- [] port old projects (physical models, granade)
- [] C bindings for VST3 SDK
- [] OpenGL function loading before main

# TODO (August 2025)
- os/gfx module
    - [X] create generalization layer w/ multiple backends (Wayland, X11, Windows, Mac)
	- [] make wayland implemenation graphics library agnostic
    - [] finish wayland input handling
	- [] finish wayland smooth resizing
	- [X] port to windows

- render module
    - [X] fix font glyph blending
	- [X] pull out graphics library depended calls into generalized backend
	- [X] pass pixel coordinates when calling render primatives
    - [] support lines/curves in the renderer
	- [] make calling render functions more ergonomic (separate drawing module?)
	- [] instancing
	- [X] port to windows
	
- font module
    - [X] update the font module to match other general modules (font_common files, folder per backend)
	- [X] pull out graphics library dependant calls into  generalized os/gfx or render backend
	- [X] port to windows
	
- audio module
    - [X] get sample rate from audio backend
	- [] support changing sample rate
	- [] support changing connected audio devices (and overriding defaults)
	- [] get/set audio device latency
	- [] how to support midi sysex?
	- [] port to windows

- spectrogram example
    - [X] compare against python + matplotlib
	- [] lines instead of rectangles
	- [] visualize phase
	- [] add ui
	- [] store spectra per frame, display spectrogram
	 	
- [] logging system
- [] ui layout module
- [] plugin loading system
- [] way of denoting which functions are implemented in another layer
- [] input fuzzing
- [] profiler
- [] file format parsing/writing module
- [] file streaming
- [] thread pool
- [] pseudo random number generation

## windows port
- [X] fix Assert on windows/msvc
- [X] string unicode conversion
- [] wasapi audio backend
- [X] directwrite font backend
- [X] opengl defines/loading
- [X] window opening
- [] clean up os include situation
