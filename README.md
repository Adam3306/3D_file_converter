# 3D_file_converter

## The project is created in Visual Studio 2017

Write a general 3D file converter, and create a concrete implementation for an OBJ reader and an STL writer.
Create a software architecture, that allows you to read, convert, and write 3D mesh file formats. The architecture should make it possible to easily extend the implementation with new file formats.
Implement a OBJ->STL conversion (only one direction! From OBJ to binary STL). The OBJ file support should be limited to v, vt, vn and f elements. STL export should be binary STL, not the text version. Note that OBJ supports non triangular faces. You can assume that these are always convex planar faces.
For the OBJ format use http://paulbourke.net/dataformats/obj/ as a reference, and for STL use https://en.wikipedia.org/wiki/STL_(file_format) since neither has a standard reference so we have to choose one to one to avoid inconsistencies among the solutions.
The code you submit should be production ready, whatever that means to you. 

These are absolutely not must haves, but lots of fun to implement:
+1. Allow the user to set a transformation (any combination of rotation, scale, translation) in the converter.
+1. Calculate the surface area of the model
+1. Calculate the volume of the model. You can assume that the model is watertight.
+1. Implement an algorithm that will decide from a 3d point if it is inside or outside the model. You can assume that the model is watertight.
