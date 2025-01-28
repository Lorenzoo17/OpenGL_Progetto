#include <iostream>

#include "texture.h"

Texture2D::Texture2D() {
	this->Width = 0;
	this->Height = 0;
	this->Image_Format = GL_RGB;
	this->Internal_Format = GL_RGB;
	this->Wrap_S = GL_REPEAT;
	this->Wrap_T = GL_REPEAT;
	this->Min_Filter = GL_LINEAR;
	this->Mag_Filter = GL_LINEAR;

	glGenTextures(1, &this->ID); // Genero la texture e uso come riferimento ad essa il campo ID
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // Si fa il bind della texture (da qui in poi con GL_TEXTURE_2D ci si riferisce sempre alla texture con ID di questa istanza)
    glBindTexture(GL_TEXTURE_2D, this->ID);
    // Si crea la texture
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
    // Si impostano wrap e filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Min_Filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Mag_Filter);
    // Si fa l'unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Per poter fare il bind dall'esterno
void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}
