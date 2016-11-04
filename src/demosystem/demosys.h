#ifndef _DEMOSYS_H_
#define _DEMOSYS_H_

#include <list>
#include "typedefs.h"
#include "timing.h"

#ifdef NUC3D_API_OPENGL
#include "n3dgl/nuc3dhw.h"
#else
#include "3deng_dx8/3deng.h"
#endif

enum {TIMETYPE_ABSOLUTE, TIMETYPE_RELATIVE};
enum RenderMode {RenderModeNormal, RenderModeTexture};

// ----- Abstract Base Class Part -----
class Part {
protected:
	GraphicsContext *gc;
	Scene *scene;	// REMOVE

	dword StartTime, EndTime, Duration;		// in milliseconds
	Timer timer;		// local part timer

	Texture *RenderTexture;
	RenderMode rmode;

	bool paused;

public:

	Part();

	virtual void SetGraphicsContext(GraphicsContext *gc);
	virtual GraphicsContext *GetGraphicsContext();

	virtual void SetTimingAbs(dword start, dword end);
	virtual void SetTimingRel(dword start, dword dur);
	virtual void Pause();
	virtual void Resume();

	virtual dword GetStartTime() const;
	virtual dword GetEndTime() const;
	virtual dword GetDuration() const;
	virtual dword GetTimePosition() const;
	virtual float GetParametricPosition() const;

	virtual void SetRenderMode(RenderMode rmode);
	virtual void SetRenderTexture(Texture *tex);

	virtual RenderMode GetRenderMode() const;
	virtual Texture *GetRenderTexture() const;

	virtual void Launch();
	virtual void ShutDown();
	virtual void MainLoop() = 0;

	virtual Scene *GetScene();
};

enum DemoState {DemoStateRunning, DemoStateStopped, DemoStatePaused};

class DemoSystem {
private:
	GraphicsContext *gc;

	std::list<Part*> parts;		// list of all parts
	std::list<Part*> active;	// currently running
	std::list<Part*> inactive;	// waiting to run

	Timer timer;	// global demo timer
	DemoState state;

public:

	DemoSystem(GraphicsContext *gc);

	void AddPart(Part *part);
	Part *GetActivePart();

	void Run();
	void Pause();
	void Resume();
	void Stop();

	void Update();

	int LoadTiming(const char *filename);
};


/////////////// exceptions //////////////
class InvalidParam{};


#endif	//_DEMOSYS_H_