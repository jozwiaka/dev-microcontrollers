#pragma once

enum Key
{
	up = 80,
	down = 72,
	left = 75,
	right = 77,
	enter = 76,
	changeView = 71,
	notSet = 0,
	start = 81
};

enum class Area
{
	free,
	locked
};

enum class View
{
	forest,
	shop
};

enum class TurretId
{
	fire,
	ice,
	bolt,
	notSet

};

enum class TurretMode
{
	attacking,
	idle
};

enum class StartMeta
{
	start,
	meta,
	notSet
};