# TODO (August 2025)
- [] os/gfx module
    - [X] create generalization layer w/ multiple backends (Wayland, X11, Windows, Mac)
    - [] finish wayland input handling
	- [] finish wayland smooth resizing
	- [?] should contain graphics library generalizations for creating textures, to be used in font module
- [] pull OpenGL structs/functions out from examples into a separate module, for use in other modules

- [] render module
    - [X] fix font glyph blending
    - [] support lines/curves in the renderer
	- [] pull out graphics library depended calls into generalized backend
	- [] make calling render functions more ergonomic (separate drawing module?)
	
- [] font module
    - [X] update the font module to match other general modules (font_common files, folder per backend)
	- [] pull out graphics library dependant calls into  generalized os/gfx or render backend
	
- [] audio module
    - [X] get sample rate from audio backend
	- [] support changing sample rate
	- [] support changing connected audio devices (and overriding defaults)
	- [] get/set audio device latency
	- [] how to support midi sysex?

- [] finish spectrogram example
    - [X] compare against python + matplotlib
	- [] lines instead of rectangles
	- [] visualize phase
	- [] add ui
	- [] store spectra per frame, display spectrogram
	
- [] ui layout module
- [] file format parsing/writing module
- [] file streaming
- [] plugin loading system
- [] logging system
