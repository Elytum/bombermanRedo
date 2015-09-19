#ifndef TEXTURESLOADING_HPP
# define TEXTURESLOADING_HPP

void initchecker(void);
void lowerstring(char *s);
unsigned int loadtexture(char *filename);
unsigned int loadmaterial(struct aiMaterial *material, char *basedir);

#endif