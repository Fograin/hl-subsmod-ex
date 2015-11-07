//=============================================================//
//	Half-Life Update MOD
//	https://github.com/Fograin/hl-subsmod-ex
//	
//	This product contains software technology licensed from:
//	The Battle Grounds Team and Contributors.
//
//	Before using any parts of this code, read licence.txt file 
//=============================================================//
#include "hud.h"
#include "particle_header.h"
#include <windows.h>
#include <gl/gl.h>
#include <gl/glaux.h>

// Set this really high, so we don't have to worry about indexes interacting.

#define BASE_EXT_TEXTURE_ID		(1<<25) // dont use zero
int current_ext_texture_id = BASE_EXT_TEXTURE_ID;
//extern int current_ext_texture_id;

tga_header *pTgaHeader;
tga *pTga;

// the different tga headers
unsigned char uTgaHeader[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};
unsigned char cTgaHeader[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

// loads a texture to the pointer
particle_texture_s * LoadTGA(particle_texture_s *pTexture = NULL, char* filename = "")
{
	// perform a texture lookup.  return the cached entry or create a new texture
	pTexture = pParticleManager->HasTexture(filename);
	if(pTexture != NULL) {
		return pTexture;
	}
	else {
		pTexture = new particle_texture_s;
	}

	// get the full path from our mod dir
	char szFilename[1024];
	_snprintf(szFilename, sizeof(szFilename) - 1, "%s/%s\0", gEngfuncs.pfnGetGameDirectory(), filename);

	// load the file
	FILE* fTga = NULL;
	fTga = fopen(szFilename, "rb");

	// check for file exists
	if(fTga == NULL)
	{
		_snprintf(szFilename, sizeof(szFilename) - 1, "Could not open: %s",  filename);

		MessageBox(NULL, szFilename, "ERROR", MB_OK);
		return NULL;
	}

	// allocate the memory for these
	pTgaHeader = new tga_header;
	pTga = new tga;

	// read the header in
	if(fread(pTgaHeader, sizeof(tga_header), 1, fTga) == 0)
	{
		MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);
		fclose(fTga);

		delete pTgaHeader;
		pTgaHeader = NULL;
		delete pTga;
		pTga = NULL;
		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	// check for uncompressed tga header
	if(memcmp(uTgaHeader, pTgaHeader, sizeof(pTgaHeader)) == 0)
		LoadUncompressedTGA(pTexture, fTga);
	// check for commpressed tga header
	else if(memcmp(cTgaHeader, pTgaHeader, sizeof(pTgaHeader)) == 0)
		LoadCompressedTGA(pTexture, fTga);
	// this file isn't a tga that we can deal with
	else
	{
		MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);
		fclose(fTga);
		delete pTgaHeader;
		pTgaHeader = NULL;
		delete pTga;
		pTga = NULL;
		delete [] pTexture->imageData;
		pTexture->imageData = NULL;

		delete pTexture;
		pTexture = NULL;
		return NULL;
	}

	// make sure we free any existing texture on this surface
	if(pTexture->iID) {
		pTexture->iID = 0;
	}

	// load the texture to an opengl surface
	glEnable(GL_TEXTURE_2D);
	pTexture->iID = current_ext_texture_id;
	glBindTexture(GL_TEXTURE_2D, pTexture->iID);
	glTexImage2D(GL_TEXTURE_2D, 0, pTexture->iBpp / 8, pTexture->iWidth, pTexture->iHeight, 0, pTexture->iType, GL_UNSIGNED_BYTE, pTexture->imageData);

	// set mipmap properties
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	
	// create mip maps 
	if (pTexture->iBpp == 32)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, pTexture->iWidth, pTexture->iHeight, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->imageData);

	if (pTexture->iBpp == 24)
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pTexture->iWidth, pTexture->iHeight, GL_RGB, GL_UNSIGNED_BYTE, pTexture->imageData);

	// cache this texture for the rest of the maps life
	pParticleManager->AddTexture(filename, pTexture);

	delete pTgaHeader;
	pTgaHeader = NULL;
	delete pTga;
	pTga = NULL;

	current_ext_texture_id++;
	return pTexture;
}

// load a type 2 ( uncompressed ) tga
bool LoadUncompressedTGA(particle_texture_s *pTexture, FILE * fTga)
{
	// read the tga header
	if(fread(pTga->cHeader, sizeof(pTga->cHeader), 1, fTga) == 0)
	{										
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		fclose(fTga);
		return false;
	}	

	// determine the width and height from the header ( highbyte * 256 + the low byte)
	pTexture->iWidth = pTga->cHeader[1] * 256 + pTga->cHeader[0];
	pTexture->iHeight = pTga->cHeader[3] * 256 + pTga->cHeader[2];

	// load the bpp and then fill the tga with the correct values
	pTexture->iBpp = pTga->cHeader[4];
	pTga->iWidth = pTexture->iWidth;
	pTga->iHeight = pTexture->iHeight;
	pTga->iBpp = pTexture->iBpp;

	// check for image bounds and validity
	if((pTexture->iWidth <= 0) || (pTexture->iHeight <= 0) ||
		((pTexture->iBpp != 24) && (pTexture->iBpp !=32)))
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);
		fclose(fTga);
		return false;
	}

	// sets the ogl texture type
	if(pTexture->iBpp == 24)
		pTexture->iType	= GL_RGB;
	else
		pTexture->iType	= GL_RGBA;

	// calculate and allocate the correct byte size of the image
	pTga->iBytespp	= (pTga->iBpp / 8);
	pTga->iImageSize = (pTga->iBytespp * pTga->iHeight * pTga->iWidth);
	pTexture->imageData = new GLubyte[pTga->iImageSize];

	// check that we've actually allocated this memory
	if(pTexture->imageData == NULL)
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);
		fclose(fTga);
		return false;
	}

	// check that we've allocated all memory required
	if(fread(pTexture->imageData, pTga->iImageSize, 1, fTga) == 0)
	{
		MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);
		if(pTexture->imageData != NULL) {
			delete [] pTexture->imageData;
			pTexture->imageData = NULL;
		}

		fclose(fTga);
		return false;
	}

	// Byte Swapping Optimized By Steve Thomas
	for(GLuint cswap = 0; cswap < (int)pTga->iImageSize; cswap += pTga->iBytespp)
	{
		pTexture->imageData[cswap] ^= pTexture->imageData[cswap+2] ^=
		pTexture->imageData[cswap] ^= pTexture->imageData[cswap+2];
	}

	fclose(fTga);
	return true;
}

// loads a compressed ( type 10 ) tga
bool LoadCompressedTGA(particle_texture_s *pTexture, FILE * fTga)
{ 
	// read the header
	if(fread(pTga->cHeader, sizeof(pTga->cHeader), 1, fTga) == 0)
	{
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);
		fclose(fTga);

		return false;
	}

	// determine the width and height from the header ( highbyte * 256 + the low byte)
	pTexture->iWidth = pTga->cHeader[1] * 256 + pTga->cHeader[0];
	pTexture->iHeight = pTga->cHeader[3] * 256 + pTga->cHeader[2];

	// load the bpp and then fill the tga with the correct values
	pTexture->iBpp = pTga->cHeader[4];
	pTga->iWidth = pTexture->iWidth;
	pTga->iHeight = pTexture->iHeight;
	pTga->iBpp = pTexture->iBpp;

	// check for image bounds and validity
	if((pTexture->iWidth <= 0) || (pTexture->iHeight <= 0) ||
		((pTexture->iBpp != 24) && (pTexture->iBpp !=32)))
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);
		fclose(fTga);
		return false;
	}

	// calculate and allocate the correct byte size of the image
	pTga->iBytespp	= (pTga->iBpp / 8);
	pTga->iImageSize = (pTga->iBytespp * pTga->iHeight * pTga->iWidth);
	pTexture->imageData = new GLubyte[pTga->iImageSize];

	// check that we've actually allocated this memory
	if(pTexture->imageData == NULL)
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);
		fclose(fTga);
		return false;
	}

	// create variables to hold the size of this image, the current pixel and the actual colour infomation for this pixel
	GLuint iPixelCount = pTga->iHeight * pTga->iWidth;
	GLuint iCurrentPixel = 0;
	GLuint iCurrentByte = 0;
	GLubyte *pColorBuffer = (GLubyte*)malloc(pTga->iBytespp);

	do
	{
		GLubyte chunkHeader = 0;

		// read in the rle ( compression ) header byte
		if(fread(&chunkHeader, sizeof(GLubyte), 1, fTga) == 0)
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);
			fclose(fTga);

			if(pTexture->imageData != NULL) {
				delete [] pTexture->imageData;
				pTexture->imageData = NULL;
			}

			return false;
		}

		// If the header is < 128, it means the that is the number of RAW color packets
		// minus 1 that follow the header
		if(chunkHeader < 128)												
		{				
			// add 1 to get number of following color values
			chunkHeader++;
			// Read RAW color values
			for(short iCounter = 0; iCounter < chunkHeader; iCounter++)
			{
				// try to read in the image data,
				if(fread(pColorBuffer, pTga->iBytespp, 1, fTga) == 0)
				{
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);

					fclose(fTga);
					if(pColorBuffer != NULL) {
						free(pColorBuffer);
					}
					if(pTexture->imageData != NULL) {
						delete [] pTexture->imageData;
						pTexture->imageData = NULL;
					}
					return false;
				}

				// copy in the pixel data, swapping the r and b values
				pTexture->imageData[iCurrentByte] = pColorBuffer[2];
				pTexture->imageData[iCurrentByte + 1] = pColorBuffer[1];
				pTexture->imageData[iCurrentByte + 2] = pColorBuffer[0];

				// copy the alpha value if it is available
				if(pTga->iBytespp == 4)
				{
					pTexture->imageData[iCurrentByte + 3] = pColorBuffer[3];
				}

				// increment our counters to see how far in we are
				iCurrentByte += pTga->iBytespp;
				iCurrentPixel++;

				// check we haven't read past the end of image
				if(iCurrentPixel > iPixelCount)
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);

					fclose(fTga);
					if(pColorBuffer != NULL) {
						free(pColorBuffer);
					}
					if(pTexture->imageData != NULL) {
						delete [] pTexture->imageData;
					pTexture->imageData = NULL;
					}
					return false;
				}
			}
		}
		// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		else																			
		{
			// Subteact 127 to get rid of the ID bit
			chunkHeader -= 127;		
			// lets read some colour values
			if(fread(pColorBuffer, pTga->iBytespp, 1, fTga) == 0)
			{	
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);

				fclose(fTga);
				if(pColorBuffer != NULL) {
					free(pColorBuffer);
				}
				if(pTexture->imageData != NULL) {
					delete [] pTexture->imageData;
					pTexture->imageData = NULL;
				}
				return false;
			}

			for(short iCounter = 0; iCounter < chunkHeader; iCounter++)
			{
				// copy in the pixel data, swapping the r and b values
				pTexture->imageData[iCurrentByte] = pColorBuffer[2];
				pTexture->imageData[iCurrentByte + 1] = pColorBuffer[1];
				pTexture->imageData[iCurrentByte + 2] = pColorBuffer[0];

				// copy the alpha value if it is available
				if(pTga->iBytespp == 4)
				{
					pTexture->imageData[iCurrentByte + 3] = pColorBuffer[3];
				}

				// increment our counters to see how far in we are
				iCurrentByte += pTga->iBytespp;
				iCurrentPixel++;

				// check we haven't read past the end of image
				if(iCurrentPixel > iPixelCount)
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);

					fclose(fTga);
					if(pColorBuffer != NULL) {
						free(pColorBuffer);
					}
					if(pTexture->imageData != NULL) {
						delete [] pTexture->imageData;
						pTexture->imageData = NULL;
					}
					return false;
				}
			}
		}
	}
	while(iCurrentPixel < iPixelCount);

	free(pColorBuffer);
	fclose(fTga);
	return true;
}


