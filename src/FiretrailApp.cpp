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
    
    static constexpr size_t NUM_SPLINE_NODES = 24;
    static constexpr size_t NUM_SUBDIVISIONS = 4;
    
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
    vec3            mHeadPosition{.0f};
    Spline          mSpline{200};
};

void FiretrailApp::setup()
{
    mCamera.lookAt(vec3(.0f, .0f, -1.0f), vec3(.0f));
    
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    
    const auto numVertices = NUM_SPLINE_NODES * NUM_SUBDIVISIONS;
    const auto numIndices = (NUM_SPLINE_NODES - 1) * (NUM_SUBDIVISIONS - 1) * 6;
    
    // Specify two planar buffers - positions are dynamic because they will be modified
    // in the update() loop. Tex Coords are static since we don't need to update them.
    vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::Attrib::POSITION, 3 ),
        //gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::TEX_COORD_0, 2 ),
    };
    
    // compute texture coordinates
    vector<vec2> texCoords;
    texCoords.resize(numVertices);
    
    int k = -1;
    
    for (int i = 0; i < NUM_SPLINE_NODES; ++i)
    {
        for (int j = 0; j < NUM_SUBDIVISIONS; ++j)
        {
            texCoords[++k] = vec2((float)i / (float)(NUM_SPLINE_NODES - 1),
                                  (float)j / (float)(NUM_SUBDIVISIONS - 1));
        }
    }
    
    // Compute indices
    vector<uint32_t> indices( numIndices );
    indices.resize(numIndices);
    
    k = -1;
    
    for (int i = 0; i < NUM_SPLINE_NODES - 1; ++i)
    {
        for (int j = 0; j < NUM_SUBDIVISIONS - 1; ++j)
        {
            // tri 1
            indices[++k] = (j + 0) + (i + 0) * NUM_SUBDIVISIONS;
            indices[++k] = (j + 0) + (i + 1) * NUM_SUBDIVISIONS;
            indices[++k] = (j + 1) + (i + 0) * NUM_SUBDIVISIONS;
            
            // tri 2
            indices[++k] = (j + 0) + (i + 1) * NUM_SUBDIVISIONS;
            indices[++k] = (j + 1) + (i + 1) * NUM_SUBDIVISIONS;
            indices[++k] = (j + 1) + (i + 0) * NUM_SUBDIVISIONS;
        }
    }
    
    const auto indicesVbo = gl::Vbo::create<uint32_t>( GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW );
    
    mVboMesh = gl::VboMesh::create( numVertices, GL_TRIANGLES, bufferLayout,
                                    numIndices, GL_UNSIGNED_SHORT, indicesVbo);
    
    //mVboMesh->bufferAttrib(geom::Attrib::TEX_COORD_0, texCoords);
    
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
    //mHeadPosition += (mAttractorPosition - mHeadPosition) * .2f;
    //mSpline.pushPoint(mHeadPosition);
    
    const auto length = mSpline.getLength();
    if (length <= .0f) return;
    
    // update normals & positions
    // write only ?
    
    auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::Attrib::POSITION );
    
    const auto d = min(20.0f, length / (float)NUM_SPLINE_NODES);
        
    for (int i = 0; i < NUM_SPLINE_NODES; ++i)
    {
        const auto splinePos = mSpline.positionAtLength(d * i);
        
        for (int j = 0; j < NUM_SUBDIVISIONS; ++j)
        {
            vec3 p = splinePos + vec3(.0f, j * .1f, .0f);
            mappedPosAttrib->x = p.x;
            mappedPosAttrib->y = p.y;
            mappedPosAttrib->z = p.z;
            mappedPosAttrib++;
        }
    }
    
    mappedPosAttrib.unmap();
}

void FiretrailApp::draw()
{
    const gl::ScopedViewport scopedVPort(vec2(0,0), getWindowSize());
    const gl::ScopedMatrices scopedMatrices;
    gl::setMatrices( mCamera );
    
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
    
    //mParams->draw();
}

CINDER_APP( FiretrailApp, RendererGl )
