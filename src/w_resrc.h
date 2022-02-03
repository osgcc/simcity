#pragma once


struct Resource
{
	char* buf{ nullptr };
	long size{ 0 };
	char name[4]{};
	long id{ 0 };
	struct Resource* next{ nullptr };
};


typedef char** Handle;

Handle GetResource(char* name, long id);
