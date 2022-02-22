#pragma once

struct Texture;

void initOverlayTexture();
void maybeDrawRect(int val, int x, int y, int w, int h);

void drawPopDensity();
void drawCrimeMap();
void drawTrafficMap();

const Texture& crimeOverlayTexture();
const Texture& populationDensityTexture();
const Texture& trafficDensityTexture();
