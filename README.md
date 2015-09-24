Mini Projects in Advanced Programming Techniques ECE 8893
===================================

###Parallel Programming Projects
1. MPI: The Distributed Two–Dimensional Discrete Fourier Transform using MPI
  * Code Folder: /Parallel_Programming_Projects/Project1_MPI
  * Just make using makefile, run on a system with MPI preinstalled.
2. Pthreads: The Threaded Two–Dimensional Discrete Fourier Transform
  * Code Folder: Parallel_Programming_Projects/Project2_Pthreads
  * Just make using makefile.

###OpenGL and MBSet Projects
1. OpenGL: Used the OpenGL graphics API to create and display a three-dimensional image of a jet aircraft. The inputs are two files that describe all of the characteristics of the jet image, including the “faces” (essentially triangles), “vertices” (a single point in 3-D space), “normal vectors” which is a 3-D vector that is normal to a face, and “materials” which describes the light reflectivity of the faces. Further, the program used keyboard input to control the rotation of the jet.
  * Code Folder: /OpenGL, use given makefile to compile and run ona system with OpenGL preinstalled.
2. MBSet: USed the OpenGL graphics API, and the CUDA GPU programming environment to compute and display a visual image of the Mandelbrot Set. Once the set is displayed, project uses the mouse to select a square region from the displayed set, and recompute the set using only the selected region of the complex plane. 
  * Code Folder: /MBSet, use given makefile to compile and run ona system with OpenGL and CUDA preinstalled, aslo you will need CUDA supported grpahics card.

###Templated Vector Class Project
1. Implemented a templated class called Vector that is essentially a variable length array. The Vector can grow as necessary as new elements are added to the array. Further implemented a iterator called VectorIterator that exists to “refer to”, or “point to” an existing element in the Vector. The required API’s are specifed in the provided Vector.h file.
  * Code Folder: /Vector, use given makefile to compile and run
2. Used valgrind to check memory leaks in my implementation.

###RSA Encrytion Algorithm
1. Used the GNU multi-precision arithmetic library to implement the wellknown public-key encryption algorithm known as Rivest-Shamir-Adelman (RSA). Program chooses appropriate public/private key pairs, chooses random messages, encrypts the random message with the public key, decrypts the random messagse with the private key, and verifys that the decrypted message matches the original messages.
  * Code Folder: /RSA, use given makefile to compile and run
