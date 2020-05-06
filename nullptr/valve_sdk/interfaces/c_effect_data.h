#pragma once

struct c_effect_data
{
	Vector origin;
	Vector start;
	Vector normal;
	Vector angles;
	int flags;
	int entity;
	float scale;
	float magnitude;
	float radius;
	int attachmentIndex;
	short surfaceProp;
	int material;
	int damageType;
	int hitBox;
	int otherEntIndex;
	unsigned char color;
	int effectName;
};

typedef void(*client_effect_callback)(const c_effect_data& data);

struct c_client_effect_registration
{
	c_client_effect_registration(const char* pEffectName, client_effect_callback fn);
	const char* effectName;
	client_effect_callback function;
	c_client_effect_registration* next;
};