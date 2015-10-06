#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"
#include "cinder/Easing.h"

#include "Rope.h"
#include "Spline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FiretrailApp : public App {
  public:
    
    static constexpr size_t NUM_SPLINE_NODES = 1024;
    
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
    gl::TextureRef	mFireTex;
    gl::GlslProgRef mGlsl;
    float           mAttractorStrength{1.0f};
    float           mRestDist{.1f};
    float           mAttractorFactor{.2f};
    float           mOctavePow{2.2f};
    float           mNoiseScale{.4f};
    float           mNoiseGain{.24f};
    float           mTimeFactor{1.0f};
    float           mFragMul{.24f};
    float           mMaxDSlice{.1f};
    vec3            mAttractorPosition{.0f};
    vec3            mHeadPosition{.0f};
    Spline          mSpline{256};
};

void FiretrailApp::setup()
{
    mCamera.lookAt(vec3(.0f, .0f, -1.0f), vec3(.0f));
    
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    mParams->addParam("Noise Scale", &mNoiseScale);
    mParams->addParam("Noise Gain", &mNoiseGain);
    mParams->addParam("Octave Pow", &mOctavePow);
    mParams->addParam("Time Factor", &mTimeFactor);
    mParams->addParam("Frag Mul", &mFragMul);
    mParams->addParam("Max D Slice", &mMaxDSlice);
    
    // load fire texture
    gl::Texture::Format mTexFormat;
    mTexFormat.magFilter( GL_LINEAR ).minFilter( GL_LINEAR ).internalFormat( GL_RGBA );//.wrap(GL_REPEAT);
    mFireTex = gl::Texture::create( loadImage( loadAsset( "flame2_blur.png" ) ), mTexFormat );
    
    // load shader
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format().vertex( loadAsset( "fire.vert" ) )
                                 .fragment( loadAsset( "fire.frag" ) )
                                 .geometry( loadAsset( "fire.geom" )).attrib( geom::CUSTOM_0, "vSize" ) );

    // compute texture coordinates
    vector<float> amp( NUM_SPLINE_NODES );
    amp.resize(NUM_SPLINE_NODES);
    
    constexpr auto head = .1f;
    
    for (size_t i = 0; i < NUM_SPLINE_NODES; ++i)
    {
        const auto t = (float)i / (float)(NUM_SPLINE_NODES - 1);
        if (t < head) amp[i] = easeOutQuad(t / head);
        else amp[i] = 1.0f - easeInOutSine((t - head) / (1.0f - head));
    }
    
    vector<gl::VboMesh::Layout> bufferLayout = {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::POSITION, 3 ),
        gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::CUSTOM_0, 1 ),
    };
    
    mVboMesh = gl::VboMesh::create( NUM_SPLINE_NODES, GL_POINTS, bufferLayout);
    
    mVboMesh->bufferAttrib(geom::Attrib::CUSTOM_0, amp);
    
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
    ray.calcPlaneIntersection(vec3(.0f, .0f, 5.0f), vec3(.0f, -1.0f, 1.0f), &result);
    mAttractorPosition = ray.calcPosition(result);
}

void FiretrailApp::mouseDrag( MouseEvent event )
{
    mouseMove(event);
}

void FiretrailApp::update()
{
    mHeadPosition += (mAttractorPosition - mHeadPosition) * .2f;
    mSpline.pushPoint(mHeadPosition);
    
    const auto length = mSpline.getLength();
    if (length <= .0f) return;
    
    auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::POSITION );
    
    const auto d = min(mMaxDSlice, length / (float)NUM_SPLINE_NODES);
    
    for (size_t i = 0; i < NUM_SPLINE_NODES; ++i)
    {
        *mappedPosAttrib++ = mSpline.positionAtLength(d * i);
    }
    
    mappedPosAttrib.unmap();
}

void FiretrailApp::draw()
{
    const gl::ScopedViewport scopedVPort(vec2(0,0), getWindowSize());
    const gl::ScopedMatrices scopedMatrices;
    gl::setMatrices( mCamera );
    
	gl::clear();
    
    gl::color(Color::white());

    gl::ScopedBlendAdditive scopedBlend;
    gl::ScopedTextureBind texFire( mFireTex, 0);
    gl::ScopedGlslProg scpGlsl( mGlsl );
    
    mGlsl->uniform("fireTex", 0);
    mGlsl->uniform("time", (float)getElapsedSeconds() * mTimeFactor);
    mGlsl->uniform("noiseScale", mNoiseScale);
    mGlsl->uniform("noiseGain", mNoiseGain);
    mGlsl->uniform("octavePow", mOctavePow);
    mGlsl->uniform("fragMul", mFragMul);

    
    gl::draw(mVboMesh);
    
    mParams->draw();
}

CINDER_APP( FiretrailApp, RendererGl )
