#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <list>
#include "n3dmath.h"
#include "3dgeom.h"
#include "objects.h"

enum BlendingFactor;

class Particle {
public:
	Vector3 pos;
	Vector3 vel;
	unsigned int life;

	Particle(int life=0);
	Particle(const Vector3 &pos, int life=0);
	Particle(const Vector3 &pos, const Vector3 &vel, int life=0);

	void Update(const Vector3 &forces, float friction=0);
};

class ParticleSystem {
private:
	GraphicsContext *gc;
	bool FixedUpdateRate;
	float UpdateRate;
	float LastUpdate;

	BlendingFactor SourceBlend, DestBlend;

	Vector3 pos, prevpos;			// position of the emmiter (world space)
	Vector3 ShootDirection;			// an initial velocity vector for the particles
	std::list<Particle> particles;	// a list of particles
	Vertex pquad[4];				// the basic particle quad vertices
	Triangle ptris[2];				// the basic particle quad triangles
	float size;						// size of the particles
	float friction;					// friction impeding particle movement
	int SpawnRate;					// rate of particle generation
	float SpawnRadius;				// spawning radius around the emmiter
	float GravForce;				// Gravitual force
	float DispRads;
	int life;						// particle life
	bool EmmiterAffectsParticleTrajectory;	// ehm ... yeah
	float SpawnDiffDispersion;
	int SpawnRateChange;			// if 0 then spawn rate constant

	float StartRed, StartGreen, StartBlue;
	float EndRed, EndGreen, EndBlue;

	int VertsToRender, TrianglesToRender;
	int vbsize, ibsize, maxprimitives;
	Texture *texture;				// the particles' texture
	Object *obj;					// the particles' mesh object (if present don't render quads)
	Vertex *varray;					// secondary vertex array (ease of development)
	Triangle *tarray;				// the triangles

	int VertexCount, IndexCount, TriCount, ParticleCount;

public:
	Matrix4x4 Translation, OrbitRot;

	ParticleSystem(GraphicsContext *gc);
	~ParticleSystem();

	void SetGraphicsContext(GraphicsContext *gc);
	void SetParticleSize(float psize);
	void SetParticleLife(int life);
	void SetFriction(float friction);
	void SetSpawnRate(float spawnrate);
	void SetSpawnRadius(float radius);
	void SetTexture(Texture *texture);
	void SetObject(Object *obj);
	void SetPosition(const Vector3 &pos);
	void SetShootDirection(const Vector3 &dir);
	void SetGravitualForce(float grav);
	void SetEmmiterDependence(bool dep);
	void SetMaxDispersionAngle(float maxdisp);
	void SetInitialColor(float r, float g, float b);
	void SetDeathColor(float r, float g, float b);
	void SetBlendingMode(BlendingFactor src, BlendingFactor dest);
	void SetSpawningDifferenceDispersion(float val);
	void SetSpawnRateChange(int change);

	void Translate(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3 &axis, float angle);
	void ResetTranslation();
	void ResetRotation();	

	int CountParticles();
			
	void Update(float t = 0.0f);
	void Render();
};
	

#endif	// _PARTICLES_H_
