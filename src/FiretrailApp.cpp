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
	void update() override;
	void draw() override;
    
    params::InterfaceGlRef	mParams;
    CameraPersp     mCamera;
    gl::VboMeshRef	mVboMesh;
    gl::TextureRef	mFireTex, mNoiseTex;
    gl::GlslProgRef mGlsl;
    gl::BatchRef    mBatch;
    float           mAttractorStrength{1.0f};
    float           mRestDist{.1f};
    float           mAttractorFactor{.2f};
    float           mLacunarity{2.0f};
    float           mGain{.2f};
    float           mNoiseScale{5.0f};
    float           mMagnitude{1.0f};
    float           mTimeFactor{1.0f};
    float           mFragMul{.1f};
    float           mMaxDSlice{.02f};
    float           mFps{.0f};
    float           mLayerOffset{-.1f};
    vec3            mHeadPosition{.0f};
    Spline          mSpline{256};
};

void FiretrailApp::setup()
{
    mCamera.lookAt(vec3(.0f, .0f, -1.0f), vec3(.0f));
    
    mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( ivec2( 200, 300 ) ) );
    mParams->addParam("FPS", &mFps);
    mParams->addParam("Max D Slice", &mMaxDSlice);
    mParams->addParam("Time Factor", &mTimeFactor);
    mParams->addParam("Gain",           &mGain      ).updateFn( [this] { mGlsl->uniform("gain", mGain);} );
    mParams->addParam("Lacunarity",     &mLacunarity).updateFn( [this] { mGlsl->uniform("lacunarity", mLacunarity);} );
    mParams->addParam("Magnitude",      &mMagnitude ).updateFn( [this] { mGlsl->uniform("magnitude", mMagnitude);} );
    mParams->addParam("Frag Mul",       &mFragMul   ).updateFn( [this] { mGlsl->uniform("fragMul", mFragMul);} );
    mParams->addParam("Noise Scale",    &mNoiseScale).updateFn( [this] { mGlsl->uniform("noiseScale", mNoiseScale);} );
    mParams->addParam("Layer Offset",    &mLayerOffset).updateFn( [this] { mGlsl->uniform("layerOffset", mLayerOffset);} );
    
    
    
    gl::Texture::Format mTexFormat;
    mTexFormat.magFilter( GL_LINEAR ).minFilter( GL_LINEAR ).internalFormat( GL_RGBA );//.wrap(GL_REPEAT);
    mFireTex = gl::Texture::create( loadImage( loadAsset( "flame5.png" ) ), mTexFormat );
    
    mTexFormat.wrap(GL_REPEAT);
    mNoiseTex = gl::Texture::create( loadImage( loadAsset( "nzw.png" ) ), mTexFormat );
    
    mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
                                 .vertex( loadAsset( "fire.vert" ) )
                                 .fragment( loadAsset( "fire.frag" ) )
                                 .geometry( loadAsset( "fire.geom" )).attrib( geom::CUSTOM_0, "vSize" ) );

    mGlsl->uniform("fireTex", 0);
    mGlsl->uniform("noiseTex", 1);
    
    mGlsl->uniform("fragMul", mFragMul);
    mGlsl->uniform("gain", mGain);
    mGlsl->uniform("magnitude", mMagnitude);
    mGlsl->uniform("lacunarity", mLacunarity);
    mGlsl->uniform("noiseScale", mNoiseScale);
    mGlsl->uniform("layerOffset", mLayerOffset);

    // compute texture coordinates
    vector<float> amp( NUM_SPLINE_NODES );
    amp.resize(NUM_SPLINE_NODES);
    
    for (size_t i = 0; i < NUM_SPLINE_NODES; ++i)
    {
        const auto t = (float)i / (float)(NUM_SPLINE_NODES - 1);
        amp[i] = 1.0f - easeInOutSine(t);
    }
    
    mVboMesh = gl::VboMesh::create( NUM_SPLINE_NODES, GL_POINTS, {
        gl::VboMesh::Layout().usage( GL_DYNAMIC_DRAW ).attrib( geom::POSITION, 3 ),
        gl::VboMesh::Layout().usage( GL_STATIC_DRAW ).attrib( geom::Attrib::CUSTOM_0, 1 ),
    });
    
    mVboMesh->bufferAttrib(geom::Attrib::CUSTOM_0, amp);
    
    mBatch = gl::Batch::create(mVboMesh, mGlsl);
    
    resize();
}

void FiretrailApp::resize()
{
    mCamera.setPerspective(60, getWindowAspectRatio(), 1, 50);
}

void FiretrailApp::update()
{
    const auto ray = mCamera.generateRay(getMousePos() - getWindowPos(), getWindowSize());
    float result = .0f;
    ray.calcPlaneIntersection(vec3(.0f, .0f, 5.0f), vec3(.0f, -2.0f, 1.0f), &result);
    
    mHeadPosition += (ray.calcPosition(result) - mHeadPosition) * .8f;
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
    
    mFps = getAverageFps();
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
    gl::ScopedTextureBind noiseFire( mNoiseTex, 1);
    
    mGlsl->uniform("time", (float)getElapsedSeconds() * mTimeFactor);
    
    mBatch->draw();
    
    mParams->draw();
}

CINDER_APP( FiretrailApp, RendererGl )
