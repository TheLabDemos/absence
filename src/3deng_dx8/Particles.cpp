#include "particles.h"
#include "3deng.h"
#include <cassert>

//////////////////////////////////////////////////////
//    --==( Particle class implementation )==--     //
//////////////////////////////////////////////////////
using namespace std;

Particle::Particle(int life) {
	pos = Vector3(0, 0, 0);
	vel = Vector3(0, 0, 0);
	this->life = life;
}

Particle::Particle(const Vector3 &pos, int life) {
	this->pos = pos;
	vel = Vector3(0, 0, 0);
	this->life = life;
}

Particle::Particle(const Vector3 &pos, const Vector3 &vel, int life) {
	this->pos = pos;
	this->vel = vel;
	this->life = life;
}

void Particle::Update(const Vector3 &forces, float friction) {
	if(!life) return;
	vel *= 1.0f - friction;
	pos += vel + forces;
	life--;
}

///////////////////////////////////////////////////////
//    --==( Particle System implementation )==--     //
///////////////////////////////////////////////////////

ParticleSystem::ParticleSystem(GraphicsContext *gc) {
	this->gc = gc;
	pos = prevpos = Vector3(0, 0, 0);
	size = 1.0f;
	friction = 0.0f;
	SpawnRate = 5;
	life = 100;
	EmmiterAffectsParticleTrajectory = false;
	varray = 0;
	tarray = 0;
	texture = 0;
	obj = 0;

	SetShootDirection(Vector3(0, 0, 0));
	SetMaxDispersionAngle(0.01f);
	SetInitialColor(1, 1, 1);
	SetDeathColor(0, 0, 0);

	// initialize the shape of the particles
	pquad[0] = Vertex(Vector3(-0.5f, 0.5f, 0), 0, 0, 0xffffffff);
	pquad[1] = Vertex(Vector3(0.5f, 0.5f, 1), 1, 0, 0xffffffff);
	pquad[2] = Vertex(Vector3(0.5f, -0.5f, 1), 1, 1, 0xffffffff);
	pquad[3] = Vertex(Vector3(-0.5f, -0.5f, 0), 0, 1, 0xffffffff);
	
	ptris[0] = Triangle(0, 1, 2);
	ptris[1] = Triangle(0, 2, 3);

	for(int i=0; i<4; i++) {
		pquad[i].normal = Vector3(0, 0, -1);
	}

	SetBlendingMode(BLEND_ONE, BLEND_ONE);
	SpawnDiffDispersion = 0.0f;

	FixedUpdateRate = true;
	UpdateRate = 30.0f;
	LastUpdate = -(1.0f / UpdateRate);

	SpawnRateChange = 0;
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::SetGraphicsContext(GraphicsContext *gc) {
	this->gc = gc;
}

void ParticleSystem::SetParticleSize(float psize) {
	size = psize;
}

void ParticleSystem::SetParticleLife(int life) {
	this->life = life;
}

void ParticleSystem::SetFriction(float friction) {
	this->friction = friction;
}

void ParticleSystem::SetSpawnRate(float spawnrate) {
	SpawnRate = (int)spawnrate;
}

void ParticleSystem::SetSpawnRadius(float radius) {
	SpawnRadius = radius;
}

void ParticleSystem::SetTexture(Texture *texture) {
	this->texture = texture;
}

void ParticleSystem::SetObject(Object *obj) {
	this->obj = obj;
}

void ParticleSystem::SetPosition(const Vector3 &pos) {
	this->pos = pos;
}

void ParticleSystem::SetShootDirection(const Vector3 &dir) {
	ShootDirection = dir;
}

void ParticleSystem::SetGravitualForce(float grav) {
	GravForce = grav;
}

void ParticleSystem::SetEmmiterDependence(bool dep) {
	EmmiterAffectsParticleTrajectory = dep;
}

void ParticleSystem::SetMaxDispersionAngle(float maxdisp) {
	DispRads = maxdisp;
}

void ParticleSystem::SetInitialColor(float r, float g, float b) {
	StartRed = r;
	StartGreen = g;
	StartBlue = b;
}

void ParticleSystem::SetDeathColor(float r, float g, float b) {
	EndRed = r;
	EndGreen = g;
	EndBlue = b;
}

void ParticleSystem::SetBlendingMode(BlendingFactor src, BlendingFactor dest) {
	SourceBlend = src;
	DestBlend = dest;
}

void ParticleSystem::SetSpawningDifferenceDispersion(float val) {
	SpawnDiffDispersion = val;
}

void ParticleSystem::SetSpawnRateChange(int change) {
	SpawnRateChange = change;
}

// transformation stuff
void ParticleSystem::Translate(float x, float y, float z) {
	Translation.Translate(x, y, z);
}

void ParticleSystem::Rotate(float x, float y, float z) {
	OrbitRot.Rotate(x, y, z);
}

void ParticleSystem::Rotate(const Vector3 &axis, float angle) {
	OrbitRot.Rotate(axis, angle);
}

void ParticleSystem::ResetRotation() {
	OrbitRot.ResetIdentity();
}

void ParticleSystem::ResetTranslation() {
	Translation.ResetIdentity();
}

int ParticleSystem::CountParticles() {
	ParticleCount = (int)particles.size();
	TriCount = ParticleCount << 1;
	VertexCount = ParticleCount << 2;
	IndexCount = TriCount * 3;
	
	return ParticleCount;
}

void ParticleSystem::Update(float t) {

	if(FixedUpdateRate && t-LastUpdate < 1.0f/UpdateRate) return;
	LastUpdate = t;

	// remove all particles that are dead
	list<Particle>::iterator iter = particles.begin();
	while(iter != particles.end()) {
		if(!iter->life) {
			iter = particles.erase(iter);
			// if we erase the iterator points to the next so no need to advance explicitly
		} else {
			iter++;
		}
	}

	
	// spawn the new particles according to spawn rate
	int LeftToSpawn = SpawnRate;
	Vector3 ShootDir = ShootDirection;
	Matrix4x4 dispxform;
	
	Vector3 forces = Vector3(0.0f, -GravForce, 0.0f);

	// find the velocity of the system by differenciating between the
	// last and the current position (time interval is considered constant)
	Vector3 velocity = (pos - prevpos) * 0.1f;
	
	// adjust the shoot vector to take under consideration the velocity of the system
	if(EmmiterAffectsParticleTrajectory) ShootDir += velocity;


	while(LeftToSpawn--) {
		Vector3 dir = ShootDir;
		dispxform.Rotate(frand(DispRads) - DispRads/2.0f, frand(DispRads) - DispRads/2.0f, frand(DispRads) - DispRads/2.0f);
		dir.Transform(dispxform);

		Vector3 SpawnOffset(frand(SpawnRadius) - SpawnRadius / 2.0f, frand(SpawnRadius) - SpawnRadius / 2.0f, frand(SpawnRadius) - SpawnRadius / 2.0f);
        Vector3 SpawnDisp(0.0f, 0.0f, 0.0f);
		if(SpawnDiffDispersion > 0.0f) {
			SpawnDisp = Vector3(frand(1.0f) - 0.5f, frand(1.0f) - 0.5f, frand(1.0f) - 0.5f);
			SpawnDisp.Normalize();
			SpawnDisp *= SpawnDiffDispersion;
		}
		particles.insert(particles.end(), Particle(pos + SpawnOffset, dir + SpawnDisp, life));
	}		


	//if(EmmiterAffectsParticleTrajectory) forces += velocity;

	iter = particles.begin();
	while(iter != particles.end()) {
		iter->Update(forces, friction);
		iter++;
	}

	SpawnRate += SpawnRateChange;
	if(SpawnRate < 0) SpawnRate = 0;
}

inline dword FtoDW(float f) { return *((dword*)&f); }

void ParticleSystem::Render() {

	CountParticles();
	if(!ParticleCount) return;

	list<Particle>::iterator iter = particles.begin();

	if(!obj) {

		// ----- Render Billboarded Textured Quads -----

        VertexBuffer *vb;
		gc->CreateVertexBuffer(ParticleCount, UsageStatic, &vb);
		Vertex *vbptr;
		Lock(vb, &vbptr);

		for(int i=0; i<ParticleCount; i++) {
			vbptr[i].pos = iter->pos;
	
			float t = 1.0f - (float)iter->life / (float)life;
			float red = StartRed + (EndRed - StartRed) * t;
			float green = StartGreen + (EndGreen - StartGreen) * t;
			float blue = StartBlue + (EndBlue - StartBlue) * t;
			
			vbptr[i].color = Color(red, green, blue).GetPacked32();

			iter++;
		}
		Unlock(vb);

		gc->SetWorldMatrix(Matrix4x4());
		gc->SetLighting(false);
		gc->SetZWrite(false);
		gc->SetTexture(0, texture);
		gc->SetTextureStageColor(0, TexBlendModulate, TexArgCurrent, TexArgTexture);
		gc->SetAlphaBlending(true);
		gc->SetBlendFunc(SourceBlend, DestBlend);
		gc->SetVertexProgram(FixedFunction);

		gc->SetColorVertex(true);

		gc->D3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
		gc->D3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);
		gc->D3DDevice->SetRenderState(D3DRS_POINTSIZE, FtoDW(size));
		gc->D3DDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.0f));
		gc->D3DDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.0f));
		gc->D3DDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.0f));

		gc->D3DDevice->SetStreamSource(0, vb, sizeof(Vertex));
		gc->D3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, ParticleCount);

		gc->D3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
		gc->D3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	
		gc->SetColorVertex(false);

		gc->SetLighting(true);
		gc->SetZWrite(true);
		gc->SetAlphaBlending(false);

		vb->Release();
	} else {

		// ---- Render Mesh Objects ----
		for(int i=0; i<ParticleCount; i++) {
			obj->ResetTranslation();
			obj->Translate(iter->pos.x, iter->pos.y, iter->pos.z);
			obj->Render();
			iter++;
		}
	}


}