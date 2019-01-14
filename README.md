# Path Tracing
Implementation based on Peter Shirley's Ray Tracing minibooks.
 
![Final Scene - Book 1](output1.png)

- Final scene of "Ray Tracing: In a Weekend", with a 1920x1080 resolution and 50 samples per pixel.
 
![Final Scene - Book 2](output2.png)

- Final scene of "Ray Tracing: The Next Week", with a 500x500 resolution and 10k samples per pixel.
 
 ## Current Features:
- Multithreaded implementation;
- Supports PNG output, through [STB Image Write](http://nothings.org/stb/stb_image_write.h);
- Triangle intersection code based on [Morgan McGuire's 'The Graphics Codex'](http://graphicscodex.com);
- Supports OBJ models with (partial) MTL material support, through [Bly7's OBJ Loader](https://github.com/Bly7/OBJ-Loader);
- [XorShift32 random generator by Aras Pranckevicius](https://github.com/aras-p/ToyPathTracer/blob/master/Cpp/Source/Maths.cpp#L5-L18).

## TODO:
- Code & documentation clean up;

Further developments will probably be done to my [CUDA](https://github.com/joaovbs96/CUDA-Path-Tracer) or [OptiX Path Tracers](https://github.com/joaovbs96/OptiX-Path-Tracer/), rather than this project.

All credits go to Peter Shirley. For more info and how to get the books: https://twitter.com/Peter_shirley/status/1029342221139509249

Implemented during an undergraduate research project mentored by prof. Hélio Pedrini at the Institute of Computing - Unicamp.
