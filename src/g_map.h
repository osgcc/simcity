#pragma once

struct Texture;

void initOverlayTexture();
void maybeDrawRect(int val, int x, int y, int w, int h);

void drawPopDensity();
void drawCrimeMap();
void drawTrafficMap();
void drawPollutionMap();
void drawLandMap();
void drawPoliceRadius();
void drawFireRadius();

const Texture& crimeOverlayTexture();
const Texture& populationDensityTexture();
const Texture& trafficDensityTexture();
const Texture& pollutionTexture();
const Texture& landValueTexture();
const Texture& policeRadiusTexture();
const Texture& fireRadiusTexture();
