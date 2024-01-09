# IMOL-File-Format
IMOL (.ioml) is a file format for 3D models.

IMOL (IGRE Model-Object Listing) is a file format designed to store a listing of 3D models, originally designed for usage with IGRE.
IGRE (Ideal Graphics Rendering Engine) is a project which I am currently working on for my Extended Project Qualification at college.

IMOL is very simplistic, being used to store textureless models comprised of polygons, each polygon coloured in a singular (r, g, b) fill.
However, its usage is not restricted to IGRE, and I have provided what should be sufficient documentation to implement IMOL in different
languages, applications, and systems.

Compiling a program using the imol.h header file should result in IMOL files that are compatible on different platforms, provided that
packing of struct memory is supported and pragmatically accessible via the compiler used on that particular system. However, the imol.h
header is not required to use IMOL, should you wish to create or use a different implementation which is compliant with the format.
