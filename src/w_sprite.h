#pragma once


struct SimSprite
{
	struct SimSprite* next{ nullptr };
	char* name{ nullptr };
	int type{ 0 };
	int frame{ 0 };
	int x{ 0 }, y{ 0 };
	int width{ 0 }, height{ 0 };
	int x_offset{ 0 }, y_offset{ 0 };
	int x_hot{ 0 }, y_hot{ 0 };
	int orig_x{ 0 }, orig_y{ 0 };
	int dest_x{ 0 }, dest_y{ 0 };
	int count{ 0 }, sound_count{ 0 };
	int dir{ 0 }, new_dir{ 0 };
	int step{ 0 }, flag{ 0 }, control{ 0 };
	int turn{ 0 };
	int accel{ 0 };
	int speed{ 0 };
};


void MoveObjects();
void DoTrainSprite(SimSprite* sprite);
