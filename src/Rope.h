//
//  Rope.h
//  Firetrail
//
//  Created by Etienne on 2015-09-25.
//
//

#ifndef Firetrail_Rope_h
#define Firetrail_Rope_h

class Rope
{
    
public:
    
    float 	invFriction = 0.986f,
            linkLength = 1.0f,
            strength = .5f;
    
    glm::vec3 forces; // ie: gravity
    
    struct Particle { glm::vec3 position{.0f}, prevPosition{.0f}, normal{.0f, 1.0f, .0f}; };
    
    Rope(size_t numParticles) { reset(numParticles); }
    
    void reset(size_t numParticles)
    {
        particlesCount = numParticles;
        particles.clear();
        particles.reserve(numParticles);
        for (auto i = 0; i < numParticles; ++i) particles.push_back(Particle());
    }
    
    // you are NOT allowed to modify particles from the ouside world
    const std::vector<Particle>& getParticles() const { return particles; }
    
    // Except for the head
    Particle& getHead() { return particles[0]; }
    Particle const& getHead() const { return particles[0]; }
   
    void UpdateHeadToTail() { UpdateParticles(1, particles.size(), forces); }
    
    void UpdateTailToHead() { UpdateParticles(particles.size() - 2, -1, forces); }
    
    void setupParams(cinder::params::InterfaceGl * mParams)
    {
        mParams->addParam( "Rope Inverse Friction", &invFriction).min(.0f).max(1.0f);
        mParams->addParam( "Rope Strength", &strength).min(.0f).max(1.0f);
        mParams->addParam( "Rope Link Length", &linkLength);
        mParams->addParam( "Rope Particles Count", &particlesCount).min(3).updateFn( [this] { reset(particlesCount); } );
    }

    void drawDebug()
    {
        {
            ci::gl::ScopedColor color(ci::Color(1.0f, .0f, .0f));
            
            for (auto i = 0; i < particles.size() - 1; ++i)
            {
                ci::gl::drawLine (particles[i].position, particles[i + 1].position);
            }
        }
    }
    
    void ComputeNormals(glm::vec3 up)
    {
        for (size_t i = 1; i < particles.size() - 1; ++i)
        {
            const auto A = particles[i - 1].position - particles[i].position;
            const auto B = particles[i + 1].position - particles[i].position;
            
            // test for colinearity
            if (std::abs(glm::dot(A, B)) != (glm::length(A) * glm::length(B)))
            {
                particles[i].normal = glm::cross(A, B);
            }
        }
        particles[0].normal = particles[1].normal;
        particles[particles.size() - 1].normal = particles[particles.size() - 2].normal;
    }
    
    void ComputeNormals() { ComputeNormals(glm::vec3(.0f, 1.0f, .0f)); }
    
private:
    
    size_t particlesCount;
    
    std::vector<Particle> particles;
    
    void UpdateParticles(size_t from, size_t to, glm::vec3 forces)
    {
        const auto d = from < to ? 1 : -1;
                
        for (size_t i = from; i != to; i = i + d)
        {
            const auto tmp = particles[i].position * (1 + invFriction) - particles[i].prevPosition * invFriction + forces;
            
            particles[i].prevPosition = particles[i].position;
            
            particles[i].position = tmp;
            
            const auto delta = particles[i].position - particles[i - d].position;
            
            const auto deltaLength = glm::length(delta);
            
            const auto ratio = deltaLength > 0 ? ((deltaLength - linkLength) / deltaLength) : deltaLength;
            
            particles[i - d].position += delta * ratio * strength;
            
            particles[i].position -= delta * ratio * strength;
        }
    }
};

#endif
