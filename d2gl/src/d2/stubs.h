#pragma once

namespace d2gl::d2 {

uint32_t __stdcall mpqLoadStub(const char* dll, const char* mpqfile, const char* mpqname, int v4, int v5);

void gameDrawBeginStub();

void automapDrawBeginStub();
void automapDrawEndStub();

void uiDrawBeginStub();
void uiDrawEndStub();

void drawUnitStubStack();
void drawUnitStubESI();
void drawMissileStub();
void drawWeatherParticlesStub();
void drawWeatherParticlesStub114d();

// void ObjectHoverBeginStub();
void rectangledTextBeginStub();
void rectangledTextEndStub();
void drawRectFrameStub();
void drawRectFrameStubECX();
// void DrawFramedBoxStub();
void unitHoverTextStub();
void altItemsTextStub();
void altItemsTextStub110f();
void altItemsTextStub109d();

void loadUIImageStub();
void loadUIImageStubECX();
void drawSubTextAStub();
void drawSubTextBStub();
void drawSubTextCStub();

void levelEntryTextBeginStub();
void levelEntryTextEndStub();

}