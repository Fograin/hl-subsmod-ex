/*
    Copyright 2001 to 2004. The Battle Grounds Team and Contributors

    This file is part of the Battle Grounds Modification for Half-Life.

    The Battle Grounds Modification for Half-Life is free software;
    you can redistribute it and/or modify it under the terms of the
    GNU Lesser General Public License as published by the Free
    Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    The Battle Grounds Modification for Half-Life is distributed in
    the hope that it will be useful, but WITHOUT ANY WARRANTY; without
    even the implied warranty of MERCHANTABILITY or FITNESS FOR A
    PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with The Battle Grounds Modification for Half-Life;
    if not, write to the Free Software Foundation, Inc., 59 Temple Place,
    Suite 330, Boston, MA  02111-1307  USA

    You must obey the GNU Lesser General Public License in all respects for
    all of the code used other than code distributed with the Half-Life
    SDK developed by Valve.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.
*/


#ifndef PARTICLE_TEXTURE_H
#define PARTICLE_TEXTURE_H

#ifdef CLIENT_DLL
#include <windows.h>
#include "gl/gl.h"
#include "gl/glaux.h"
#include "r_studioint.h"

// our texture
struct particle_texture_s								
{
	particle_texture_s() { iID = NULL; imageData = NULL; }

	GLubyte	*imageData;
	unsigned int	iBpp;
	unsigned int	iWidth;
	unsigned int	iHeight;
	int				iID;
	unsigned int	iType;
	int	m_iTexture;
};	

// our tga file's header
struct tga_header
{
	unsigned char	cHeader[12];
};

// our tga file definition
struct tga
{
	unsigned char	cHeader[6];
	unsigned int	iImageSize;
	unsigned int	iTemp;
	unsigned int	iType;
	unsigned int	iWidth;
	unsigned int	iHeight;
	unsigned int	iBpp;
	unsigned int	iBytespp;
} ;

extern bool LoadUncompressedTGA(particle_texture_s *, FILE *);
extern bool LoadCompressedTGA(particle_texture_s *, FILE *);


// Global engine <-> studio model rendering code interface
extern engine_studio_api_t IEngineStudio;		

#endif
#endif

  