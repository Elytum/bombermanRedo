g++ -g -o assview -Wall -g -I ./external_librairies -I. maths/matrix.cpp -I ./maths external_librairies/asstools/assview.cpp -L./external_librairies/assimp/build/code external_librairies/assimp/libassimp.3.1.1.dylib -lz -lstdc++ -framework GLUT -framework OpenGL -framework Cocoa texturesLoading/texturesLoading.cpp scene/scene.cpp render/render.cpp
