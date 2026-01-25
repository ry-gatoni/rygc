# TODO (January 2025)
- render
  - [ ] it should be possible to use the render layer without using the font layer

# TODO (December 2025)
- base layer
  - [ ] logging
    - [ ] make logging system more ergonomic
    - [ ] support different destinations (console, file, ui, popup)
  - [ ] profiler
    - [ ] figure out how to implement the `ProfileFunction` macro
    - [ ] put profile stuff in its own log scope (or stop using the logging system for profile data)
  - [ ] add streams
  - [ ] replace crt math
  - [ ] add printf-like function attributes to relevent functions
  - [ ] abstract entry point
  - [ ] multi-core by default
  - [ ] pseudo-random number generation
  - [ ] os
    - [ ] (windows) read/write files at position
    - [ ] run-time linking/loading abstraction

- gfx
  - [ ] (linux/wayland) opengl & egl runtime linking
  - [ ] drag and drop support
  - [ ] smooth resizing
  - [ ] set cursor shape
  - [ ] (windows) implement keyboard & mouse event handling

- render
  - [ ] render curves
  - [ ] better sorting (not with z buffer)
  - [ ] (windows) d3d11 backend

- font
  - [ ] (windows/dwrite) check dwrite rasterization (replace gdi with d3d?)

- audio
  - [ ] sample rate conversion
  - [ ] get/set client latency
  - [ ] midi sysex
  - [ ] device selection
  - [ ] set client sample rate
  - [ ] expose more granular interface, allowing manual audio thread creation (instead of callback)
  - [ ] reusable audio logging system (hook into existing logging system somehow?)

- spectrogram example
  - [ ] lines instead of rectangles
  - [ ] visualize phase
  - [ ] add ui
  - [x] spectra per frame, display spectrogram

- misc
  - [ ] clang on windows
  - [ ] mac support
  - [ ] static executable on linux (with detour-like ld-linux api loading)
  - [ ] vst3, audio unit, aax, clap, and vcv rack targets
  - [ ] build configuration via script arguments
  - [ ] self-image parsing
  - [ ] input fuzzing

# TODO (November 2025)
- base layer
  - [] finish text logging system
  - [] improve audio logging system
  - [] add profiler
  - [] replace crt math usage
  - [] printf-like function attributes

- os/core module
  - [X] read/write files at position (linux)
  - [] read/write files at position (windows)

- os/gfx module
  - [X] support key/button presses in input abstraction
  - [] implement keyboard/mouse event handling (windows)
  - [] drag and drop (wayland, windows)
  - [] opengl/egl runtime linking (wayland)
  - [] smooth resizing (wayland, windows)

- render module
  - [X] render lines
  - [] render curves
  - [] better sorting
  - [] more ergonomic render functions (introduce seaparate draw module?)
  - [X] instancing
  - [] d3d11 backend on windows

- font module
  - [] check dwrite glyph rasterization (d3d instead of gdi?)

- audio module
  - [] sample rate conversion
  - [X] midi (wasapi)
  - [] midi sysex (wasapi, jack)
  - [] support changing samplerate, audiodevices (wasapi, jack)
  - [] support configuring latency (wasapi, jack)
  - [] expose more granular interface that allows manual audio thread creation (instead of callback)

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
  - [] more ergonomic render functions (introduce separate draw module?)
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
