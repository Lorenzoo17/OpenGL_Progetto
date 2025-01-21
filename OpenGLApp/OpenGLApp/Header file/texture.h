#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture2D {
public:
	unsigned int ID; // ID della texture (riferimento alla texure caricata dall'istanza della classe)
	
	// Dimensione della texture
	unsigned int Width;
	unsigned int Height;

	// Parametri della texure
	unsigned int Image_Format;
	unsigned int Internal_Format;

	// Parametri di wrap e filter decisi per l'istanza della classe contenente la texture
	unsigned int Wrap_S;
	unsigned int Wrap_T;
	unsigned int Min_Filter;
	unsigned int Mag_Filter;

	Texture2D();
	void Generate(unsigned int width, unsigned int height, unsigned char* data);
	void Bind() const;
};

#endif // TEXTURE_H
