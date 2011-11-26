/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2010, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef GLTOOLS_H
#define GLTOOLS_H

#include <QGLFunctions>
#include <QStack>
#include <GL/gl.h>
#include <GL/glext.h>

#include "dds/dds_api.h"
#include "dds/DirectDrawSurface.h" // unused? check if upstream has cleaner or d
#include "../niftypes.h"

class AlphaProperty;
class BSShaderLightingProperty;
class StencilProperty;
class VertexColorProperty;
class WireframeProperty;
class SpecularProperty;
class MaterialProperty;
class TextureProperty;
class TexturingProperty;
class ZBufferProperty;
class Scene;

//! \file gltools.h BoundSphere, VertexWeight, BoneWeights, SkinPartition

//! A bounding sphere for an object, typically a Mesh
class BoundSphere
{
public:
	BoundSphere();
	BoundSphere( const BoundSphere & );
	BoundSphere( const Vector3 & center, float radius );
	BoundSphere( const QVector<Vector3> & vertices );
	
	Vector3	center;
	float	radius;
	
	BoundSphere & operator=( const BoundSphere & );
	BoundSphere & operator|=( const BoundSphere & );
	
	BoundSphere operator|( const BoundSphere & o );
	
	BoundSphere & apply( const Transform & t );
	BoundSphere & applyInv( const Transform & t );
	
	friend BoundSphere operator*( const Transform & t, const BoundSphere & s );
};

//! A vertex, weight pair
class VertexWeight
{
public:
	VertexWeight()
	{ vertex = 0; weight = 0.0; }
	VertexWeight( int v, float w )
	{ vertex = v; weight = w; }
	
	int vertex;
	float weight;
};

//! A set of vertices weighted to a bone
class BoneWeights
{
public:
	BoneWeights() { bone = 0; }
	BoneWeights( const NifModel * nif, const QModelIndex & index, int b );
	
	Transform trans;
	Vector3 center; float radius;
	Vector3 tcenter;
	int bone;
	QVector<VertexWeight> weights;
};

//! A skin partition
class SkinPartition
{
public:
	SkinPartition() { numWeightsPerVertex = 0; }
	SkinPartition( const NifModel * nif, const QModelIndex & index );
	
	QVector<int> boneMap;
	QVector<int> vertexMap;
	
	int numWeightsPerVertex;
	QVector< QPair< int, float > > weights;
	
	QVector< Triangle > triangles;
	QList< QVector< quint16 > > tristrips;
};

class GLTools : protected QGLFunctions {
private:
//! Number of texture units
GLint num_texture_units;
//! Maximum anisotropy
float max_anisotropy;

public:
void drawAxes( Vector3 c, float axis ) const;
void drawBox( Vector3 a, Vector3 b ) const;
void drawCircle( Vector3 c, Vector3 n, float r, int sd = 16 ) const;
void drawArc( Vector3 c, Vector3 x, Vector3 y, float an, float ax, int sd = 8 ) const;
void drawSolidArc( Vector3 c, Vector3 n, Vector3 x, Vector3 y, float an, float ax, float r, int sd = 8 ) const;
void drawCone( Vector3 c, Vector3 n, float a, int sd = 16 ) const;
void drawRagdollCone( Vector3 pivot, Vector3 twist, Vector3 plane, float coneAngle, float minPlaneAngle, float maxPlaneAngle, int sd = 16 ) const;
void drawSphere( Vector3 c, float r, int sd = 8 ) const;
void drawCapsule( Vector3 a, Vector3 b, float r, int sd = 5 ) const;
void drawDashLine( Vector3 a, Vector3 b, int sd = 15 ) const;
void drawConvexHull( QVector<Vector4> vertices, QVector<Vector4> normals ) const;
void drawSpring( Vector3 a, Vector3 b, float stiffness, int sd = 16, bool solid = false ) const;
void drawRail( const Vector3 &a, const Vector3 &b ) const;

inline void glTranslate( const Vector3 & v ) const
{
	glTranslatef( v[0], v[1], v[2] );
}

inline void glScale( const Vector3 & v ) const
{
	glScalef( v[0], v[1], v[2] );
}

inline void glVertex( const Vector2 & v ) const
{
	glVertex2fv( v.data() );
}

inline void glVertex( const Vector3 & v ) const
{
	glVertex3fv( v.data() );
}

inline void glVertex( const Vector4 & v ) const
{
	glVertex3fv( v.data() );
}

inline void glNormal( const Vector3 & v ) const
{
	glNormal3fv( v.data() );
}

inline void glTexCoord( const Vector2 & v ) const
{
	glTexCoord2fv( v.data() );
}

inline void glColor( const Color3 & c ) const
{
	glColor3fv( c.data() );
}

inline void glColor( const Color4 & c ) const
{
	glColor4fv( c.data() );
}

inline void glMaterial( GLenum x, GLenum y, const Color4 & c ) const
{
	glMaterialfv( x, y, c.data() );
}

inline void glLoadMatrix( const Matrix4 & m ) const
{
	glLoadMatrixf( m.data() );
}

inline void glMultMatrix( const Matrix4 & m ) const
{
	glMultMatrixf( m.data() );
}

inline void glLoadMatrix( const Transform & t ) const
{
	glLoadMatrix( t.toMatrix4() );
}

inline void glMultMatrix( const Transform & t ) const
{
	glMultMatrix( t.toMatrix4() );
}


inline GLuint glClosestMatch( GLuint * buffer, GLint hits ) const
{	// a little helper function, returns the closest matching hit from the name buffer
	GLuint	choose = buffer[ 3 ];
	GLuint	depth = buffer[ 1 ];
	for ( int loop = 1; loop < hits; loop++ )
	{
		if ( buffer[ loop * 4 + 1 ] < depth )
		{
			choose = buffer[ loop * 4 + 3 ];
			depth = buffer[ loop * 4 + 1 ];
		}       
	}
	return choose;
}

void renderText(double x, double y, double z, const QString & str) const;
void renderText(const Vector3& c, const QString & str) const;
void DrawVertexSelection( QVector<Vector3> &verts, int i ) const;
void DrawTriangleSelection( QVector<Vector3> const &verts, Triangle const &tri ) const;
void DrawTriangleIndex( QVector<Vector3> const &verts, Triangle const &tri, int index) const;
void drawHvkConstraint( const NifModel * nif, const QModelIndex & iConstraint, const Scene * scene ) const;
void drawFurnitureMarker( const NifModel *nif, const QModelIndex &iPosition ) const;
void drawHvkShape( const NifModel * nif, const QModelIndex & iShape, QStack<QModelIndex> & stack, const Scene * scene, const float origin_color3fv[3] ) const;

void glProperty( AlphaProperty * p ) const;
void glProperty( ZBufferProperty * p ) const;
void glProperty( TexturingProperty * p ) const;
void glProperty( TextureProperty * p ) const;
void glProperty( MaterialProperty * p, SpecularProperty * s ) const;
void glProperty( WireframeProperty * p ) const;
void glProperty( VertexColorProperty * p, bool vertexcolors ) const;
void glProperty( StencilProperty * p ) const;
void glProperty( BSShaderLightingProperty * p ) const;

float get_max_anisotropy() const;
void initializeTextureUnits();
bool activateTextureUnit( int x );
void resetTextureUnits();

//! A function for loading textures.
/*!
 * Loads a texture pointed to by filepath.
 * Returns true on success, and throws a QString otherwise.
 * The parameters format, width, height and mipmaps will be filled with information about
 * the loaded texture.
 *
 * \param filepath The full path to the texture that must be loaded.
 * \param format Contain the format, for instance "DDS (DXT3)" or "TGA", on successful load.
 * \param width Contains the texture width on successful load.
 * \param height Contains the texture height on successful load.
 * \param mipmaps Contains the number of mipmaps on successful load.
 * \return true if the load was successful, false otherwise.
 */
bool texLoad( const QString & filepath, QString & format, GLuint & width, GLuint & height, GLuint & mipmaps );

//! A function for loading textures.
/*!
* Loads a texture pointed to by model index.
* Returns true on success, and throws a QString otherwise.
* The parameters format, width, height and mipmaps will be filled with information about
* the loaded texture.
*
* \param iData Reference to pixel data block
* \param format Contain the format, for instance "DDS (DXT3)" or "TGA", on successful load.
* \param width Contains the texture width on successful load.
* \param height Contains the texture height on successful load.
* \param mipmaps Contains the number of mipmaps on successful load.
* \return true if the load was successful, false otherwise.
*/
bool texLoad( const QModelIndex & iData, QString & format, GLuint & width, GLuint & height, GLuint & mipmaps );

//! A function which checks whether the given file can be loaded.
/*!
 * The function checks whether the file exists, is readable, and whether its extension
 * is that of a supported file format (dds, tga, or bmp).
 *
 * \param filepath The full path to the texture that must be checked.
 */
bool texCanLoad( const QString & filepath );

//! Save pixel data to a DDS file
/*!
 * \param index Reference to pixel data
 * \param filepath The filepath to write
 * \param width The width of the texture
 * \param height The height of the texture
 * \param mipmaps The number of mipmaps present
 * \return true if the save was successful, false otherwise
 */
bool texSaveDDS( const QModelIndex & index, const QString & filepath, GLuint & width, GLuint & height, GLuint & mipmaps );

//! Save pixel data to a TGA file
/*!
 * \param index Reference to pixel data
 * \param filepath The filepath to write
 * \param width The width of the texture
 * \param height The height of the texture
 * \return true if the save was successful, false otherwise
 */
bool texSaveTGA( const QModelIndex & index, const QString & filepath, GLuint & width, GLuint & height );

//! Save a file to pixel data
/*!
 * \param filepath The source texture to convert
 * \param iData The pixel data to write
 */
bool texSaveNIF( class NifModel * nif, const QString & filepath, QModelIndex & iData );

int generateMipMaps( int m );
int texLoadRaw( QIODevice & f, int width, int height, int num_mipmaps, int bpp, int bytespp, const quint32 mask[], bool flipV = false, bool flipH = false, bool rle = false );
void flipDXT(GLenum, int, int, unsigned char*);
GLuint texLoadDXT(QIODevice&, GLenum, int, quint32, quint32, quint32, bool);
GLuint texLoadDXT( DDSFormat &hdr, const quint8 *pixels, uint size );
GLuint texLoadDDS(QIODevice&, QString&);
GLuint texLoadTGA( QIODevice & f, QString & texformat );
GLuint texLoadBMP( QIODevice & f, QString & texformat );
GLuint texLoadNIF( QIODevice & f, QString & texformat );
int texLoadPal( QIODevice & f, int width, int height, int num_mipmaps, int bpp, int bytespp, const quint32 colormap[], bool flipV, bool flipH, bool rle );
};

#define ID2COLORKEY(id) (id + 1)
#define COLORKEY2ID(id) (id - 1)

#endif
