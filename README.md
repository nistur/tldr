TINY LITTLE DOM READER
======================

tldr is, strictly speaking, a lie. At this stage, it doesn't do much DOM reading at all. It does have a simple interface with DOM-esque accessors. It's written in C and C++ and designed to be easy for me to use to save arbitrary data into binary and potentially human readable formats.

Principles of tldr
------------------

This library was written after, and based on the design of `tlmm`, my Tiny Little Maths Machine. Many of the same principles carry across, but the main things to take into account are:

* The API should be simple, clean and easy to use (in both C and C++)
* The library should be small
* Accessing the library should be quick

This has basically lead both libraries to have a particularly slimline API, favouring a concise design over a feature rich, but more confusing one. Using tldr as an example, you can have hierarchical keys, but you cannot have multiple child keys with the same name. If you use a more standardised library, you can have multiple (say, xml) nodes with the same name at every level. This might limit some of the usage of tldr, but at the same time it removes the need for things such as searching and filtering for all nodes with a given name.

Using tldr
----------

Using tldr within your program should be relatively straightforward. At this stage, no C++ wrapper has been provided, but will be added shortly.


~~~~~~ C
       tldrContext* context = tldrInitContext();
       tldrWriteValue(context, "some.key", "some value");

       // save/load
       tldrSaveBinary(context, "filename.tldr");
       tldrLoadBinary(context, "filename.tldr", true);
       char value[TLDR_STRING_MAX];
       tldrReadValue(context, "some.key", value);

       tldrTerminateContext(&context);
~~~~~~

Notes
-----

At this stage, no wrappers for other types than a c-string are supported, I cannot think of a simple method of supporting this without losing the clean API.

The code within tldr.cpp is written on a couple of train journeys, and is still pretty rough. The main aim was to ensure that it worked and had a nice interface, tidying the library and documenting fully will come later. It also uses a mixture of C and C++ internally. For the most part I have attempted to stick with C to reduce the mandatory need for the C++ overhead (both in terms of library footprint and runtime usage) however, some things were just much simpler to write in a C++ style.

I will be also creating a -lite version of the lib at some point, which will not have internal file I/O. Also, the fatter version will potentially have a human readable output (/input?) file format. I have yet to decide if this will be XML, yaml, json, or a home rolled specification.