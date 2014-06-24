You need to install the QT SDK with QT 4.7.0 or newer.
Open the project file src/QtWepApp.pro with QT Creator.

You might need to change the build directory in the project settings before
you can compile the program.

To quickly test the program, run it with argument -e. The argument -h will
display a help about the other available options.

If you like to generate a class documentation in HTML, then install doxygen
and execute the following commands:

cd src
make clean
cd ..
doxygen

The result will be placed in doc/html.
