# TODO (August 2025)
- os/gfx module
    - [X] create generalization layer w/ multiple backends (Wayland, X11, Windows, Mac)
	- [] make wayland implemenation graphics library agnostic
    - [] finish wayland input handling
	- [] finish wayland smooth resizing

- render module
    - [X] fix font glyph blending
	- [X] pull out graphics library depended calls into generalized backend
	- [X] pass pixel coordinates when calling render primatives
    - [] support lines/curves in the renderer
	- [] make calling render functions more ergonomic (separate drawing module?)
	- [] instancing
	
- font module
    - [X] update the font module to match other general modules (font_common files, folder per backend)
	- [X] pull out graphics library dependant calls into  generalized os/gfx or render backend
	
- audio module
    - [X] get sample rate from audio backend
	- [] support changing sample rate
	- [] support changing connected audio devices (and overriding defaults)
	- [] get/set audio device latency
	- [] how to support midi sysex?

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
