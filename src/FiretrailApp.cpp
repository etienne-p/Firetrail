#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"


#include "Rope.h"
#include "Spline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FiretrailApp : public App {
  public:
    
    static constexpr size_t NUM_ROPE_NODES = 40;
    static constexpr size_t NUM_PARTICLES = NUM_ROPE_NODES * 2;
    
	void setup() override;
    void resize() override;
	void mouseDown( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    params::InterfaceGlRef	mParams;
    CameraPersp     mCamera;
    gl::VboMeshRef	mVboMesh;
    gl::TextureRef	mTexture;
    float           mAttractorStrength{1.0f};
    float           mRestDist{.1f};
    float           mAttractorFactor{.2f};
    vec3            mAttractorPosition{.0f};
    Rope            mRope{NUM_ROPE_NODES};
    Spline          mSpline;
};

void FiretrailApp::setup()
{
    mCamera.lookAt(vec3(.0f, .0f, -1.0f), vec3(.0f));
    
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    mParams->addParam( "Attractor Factor", &mAttractorFactor).min(.0f).max(1.0f);
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
    
    resize();
}

void FiretrailApp::resize()
{
    mCamera.setPerspective(60, getWindowAspectRatio(), 1, 20);
}

void FiretrailApp::mouseDown( MouseEvent event )
{
}

void FiretrailApp::mouseMove( MouseEvent event )
{
    const auto ray = mCamera.generateRay(event.getPos(), getWindowSize());
    float result = .0f;
    ray.calcPlaneIntersection(vec3(.0f, .0f, 5.0f), vec3(.0f, .0f, 1.0f), &result);
    mAttractorPosition = ray.calcPosition(result);
    cout << "x: " << mAttractorPosition.x << " y: " << mAttractorPosition.y << " z: " << mAttractorPosition.z <<endl;
    
    mSpline.pushPoint(mAttractorPosition);
}

void FiretrailApp::mouseDrag( MouseEvent event )
{
    mouseMove(event);
}

void FiretrailApp::update()
{
    mRope.getHead().position = mAttractorPosition;//(mAttractorPosition - mRope.getHead().position) * mAttractorFactor;
    mRope.UpdateHeadToTail();
    
    // update normals & positions
    // write only ?
    /*
    auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::Attrib::POSITION, false );
    for( int i = 0; i < mVboMesh->getNumVertices(); i++ ) {
        //vec3 &pos = *mappedPosAttrib;
        mappedPosAttrib->y = .0f;
        ++mappedPosAttrib;
    }
    mappedPosAttrib.unmap();*/
}

void FiretrailApp::draw()
{
    const gl::ScopedViewport scopedVPort(vec2(0,0), getWindowSize());
    const gl::ScopedMatrices scopedMatrices;
    gl::setMatrices( mCamera );
    
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::color(Color::white());
    
    const auto length = mSpline.getLength();
    
    if (length > .0f)
    {
        const auto d = min(1.0f, length / 10);
        for (auto i = 0; i < 10; ++i)
        {
            gl::drawSphere(mSpline.positionAtLength(d * i), .2f);
        }
    }
    
    /*
    gl::setWireframeEnabled(true);
    
    // set "global" (ie common to every slice)
    // shader parameters
    
    // draw N layers, back to front
    // pass x texCoord to shader
    // pass amp to shader
    // use additive blending
    
    gl::draw(mVboMesh);
    
    gl::setWireframeEnabled(false);*/
    
    mRope.drawDebug();
    
    mParams->draw();
}

CINDER_APP( FiretrailApp, RendererGl )
