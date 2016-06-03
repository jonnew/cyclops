# Welcome to Cyclops Arduino Documentation

You can take the new objects for a spin using the `examples/` directory in `Waveform` and `Task`.

I changed the class organisation a bit. `st_Waveform` is now `Source`.  
`Waveform` object has a member `Source *src` Thus a channel can switch between different `Source` objects.  
All source objects have the same interface via the `Source` class.

> @quoteThis page is sourced from `/arduino/README.md`, it supports DoxyGen style `md` [syntax](http://www.stack.nl/~dimitri/doxygen/manual/markdown.html).

