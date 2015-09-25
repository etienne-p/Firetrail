#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "Rope.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FiretrailApp : public App {
  public:
    
    static constexpr size_t NUM_ROPE_NODES = 40;
    static constexpr size_t NUM_PARTICLES = NUM_ROPE_NODES * 2;
    
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    params::InterfaceGlRef	mParams;
    gl::VboMeshRef	mVboMesh;
    gl::TextureRef	mTexture;
    float           mAttractorStrength{1.0f};
    float           mRestDist{.1f};
    vec3            mAttractorPosition{.0f};
    Rope            mRope{NUM_ROPE_NODES};
    
};

void FiretrailApp::setup()
{
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    
    mRope.setupParams(mParams);
    
    int numVertices = 1000;
    
    // Specify two planar buffers - positions are dynamic because they will be modified
    // in the update() loop. Tex Coords are static since we don't need to update them.
    vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
        gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::TEX_COORD_0, 2 ),
    };
    
    // compute texture coordinates
    vector<vec2> texCoords;
    texCoords.reserve(numVertices);
    for (auto i = 0; i < numVertices; ++i) texCoords.push_back(vec2(.0f, i % 2 == 0 ? .0f : 1.0f));
    
    mVboMesh = gl::VboMesh::create( numVertices, GL_TRIANGLE_STRIP, bufferLayout);
    mVboMesh->bufferAttrib(geom::Attrib::TEX_COORD_0, texCoords);

}

void FiretrailApp::mouseDown( MouseEvent event )
{
}

void FiretrailApp::mouseMove( MouseEvent event )
{
    mAttractorPosition = vec3(event.getX(), event.getY(), .0f);
}

void FiretrailApp::mouseDrag( MouseEvent event )
{
    mouseMove(event);
}

void FiretrailApp::update()
{
    // update normals & positions
    // write only ?
    auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::Attrib::POSITION, false );
    for( int i = 0; i < mVboMesh->getNumVertices(); i++ ) {
        //vec3 &pos = *mappedPosAttrib;
        mappedPosAttrib->y = .0f;
        ++mappedPosAttrib;
    }
    mappedPosAttrib.unmap();
}

void FiretrailApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color(Color::white());
    
    gl::setWireframeEnabled(true);
    
    // set "global" (ie common to every slice)
    // shader parameters
    
    // draw N layers, back to front
    // pass x texCoord to shader
    // pass amp to shader
    // use additive blending
    
    gl::draw(mVboMesh);
    
    gl::setWireframeEnabled(false);
    
    mRope.debugDr
}

CINDER_APP( FiretrailApp, RendererGl )
