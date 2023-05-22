This project skeleton was created by Matthew Chidlow and completed by Josef Papp and Thomas Rigby. 

To run this program one can download these files and then follow these steps which have been outlined by Matthew Chidlow. As we have completed this project on a MAC device, these steps will be Macintosh specific. 

### Command Line

Make sure to `cd` into the 'cits3003_project'

Then to generate build files, run (these only needs to be re-run if you update CMakeLists.txt)

`cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug`

`cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release`

Then to build the debug profile, run:

`cmake --build cmake-build-debug`

Then to build the release profile, run:

`cmake --build cmake-build-release`

Then to run the last built profile, run:

`./cits3003_project`