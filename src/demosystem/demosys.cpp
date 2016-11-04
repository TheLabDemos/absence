#include "demosys.h"

/////////////// Part base class implementation ///////////////

Part::Part() {
	gc = 0;

	rmode = RenderModeNormal;
	RenderTexture = 0;

	SetTimingAbs(0, 0);
	paused = false;
}

void Part::SetGraphicsContext(GraphicsContext *gc) {
	this->gc = gc;
}

GraphicsContext *Part::GetGraphicsContext() {
	return gc;
}

void Part::SetTimingAbs(dword start, dword end) {
	StartTime = start;
	EndTime = end;
	Duration = EndTime - StartTime;
}

void Part::SetTimingRel(dword start, dword dur) {
	StartTime = start;
	Duration = dur;
	EndTime = StartTime + Duration;
}

void Part::Pause() {
	if(paused) return;
	timer.Stop();
	paused = true;
}

void Part::Resume() {
	if(!paused) return;
	timer.Resume();
	paused = false;
}

dword Part::GetStartTime() const {
	return StartTime;
}

dword Part::GetEndTime() const {
	return EndTime;
}

dword Part::GetDuration() const {
	return Duration;
}

dword Part::GetTimePosition() const {
	return timer.GetMilliSec();
}

float Part::GetParametricPosition() const {
	return (float)timer.GetMilliSec() / (float)Duration;
}

void Part::SetRenderMode(RenderMode rmode) {
	this->rmode = rmode;
}

void Part::SetRenderTexture(Texture *tex) {
	RenderTexture = tex;
}

RenderMode Part::GetRenderMode() const {
	return rmode;
}

Texture *Part::GetRenderTexture() const {
	return RenderTexture;
}


void Part::Launch() {
	timer.Start();
}

void Part::ShutDown() {
	// do nothing
}

Scene *Part::GetScene() {
	return scene;
}


/////////////// main Demo System class implementation ///////////////

DemoSystem::DemoSystem(GraphicsContext *gc) {
	this->gc = gc;
	state = DemoStateStopped;
}

void DemoSystem::AddPart(Part *part) {

	if(state != DemoStateStopped) return;

	if(!part->GetGraphicsContext()) part->SetGraphicsContext(gc);

	parts.push_back(part);
	inactive.push_back(part);
}

Part *DemoSystem::GetActivePart() {
	return *active.begin();
}

//// demo flow control ////

// Run demo from the beggining
void DemoSystem::Run() {

	if(state != DemoStateStopped) return;

	state = DemoStateRunning;
	timer.Start();
}

// Stop the execution of the demo, discard any sequencing information
void DemoSystem::Stop() {

	if(state == DemoStateStopped) return;

	while(active.size()) {
		active.erase(active.begin());
	}

	inactive = parts;

	state = DemoStateStopped;
}

// Pause the demo (freeze the timers)
void DemoSystem::Pause() {

	if(state != DemoStateRunning) return;

	std::list<Part*>::iterator iter = active.begin();
	while(iter != active.end()) {
		(*iter++)->Pause();
	}	
	timer.Stop();
	
	state = DemoStatePaused;
}

void DemoSystem::Resume() {

	if(state != DemoStatePaused) return;

	std::list<Part*>::iterator iter = active.begin();
	while(iter != active.end()) {
		(*iter++)->Resume();
	}	
	timer.Resume();

	state = DemoStateRunning;
}


void DemoSystem::Update() {

	if(state != DemoStateRunning) return;

	dword time = timer.GetMilliSec();

	// Check if there are any inactive parts to launch
	std::list<Part*>::iterator iter = inactive.begin();
	while(iter != inactive.end()) {
		if(time >= (*iter)->GetStartTime()) {
			(*iter)->Launch();
			active.push_back(*iter);
			iter = inactive.erase(iter);
		} else {
			iter++;
		}
	}

	// check if there are any parts to close and close them, and run the valid ones
	iter = active.begin();
	while(iter != active.end()) {
		if(time >= (*iter)->GetEndTime()) {
			(*iter)->ShutDown();
			iter = active.erase(iter);
		} else {
			// run the part
			if((*iter)->GetRenderMode() == RenderModeTexture) {
				gc->SetRenderTarget((*iter)->GetRenderTexture(), (Texture*)0);
				gc->Clear(0);
				gc->ClearZBufferStencil(1.0f, 0);
			}

			(*iter)->MainLoop();

			if((*iter)->GetRenderMode() == RenderModeTexture) {
				gc->ResetRenderTarget();
			}

			iter++;
		}
	}
}