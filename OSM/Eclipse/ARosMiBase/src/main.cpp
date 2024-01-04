#include "main.h"
#include "peripherals.h"
#include "utilities.h"
#include "Enums.h"
#include "Color.h"
#include <future>
#include <thread>
#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>

#define RPiLAB
class Path;
class Enemy;
class Player;
class TurretFire;
class TurretIce;
class TurretBolt;
class Point;

Key key;
Key keyPrev = Key::notSet;
const int Y = 450;
const int X = 600;
const int a = 50;
const int sizeX = X / a;
const int sizeY = Y / a;
View view = View::forest;
TurretId turretId = TurretId::notSet;
bool START = false;
int lvl = 0;
const int tc_lvl = 150;
int ti_lvl = tc_lvl;

std::vector<Point> points;
std::vector<Enemy> enemies;
std::vector<TurretFire> turretsFire;
std::vector<TurretIce> turretsIce;
std::vector<TurretBolt> turretsBolt;

class Draw
{
public:
	static void fillRect(int x0, int y0, int color, int b = a)
	{
		for (int y = y0; y <= y0 + b; y++)
		{
			for (int x = x0; x <= x0 + b; x++)
			{
				SetPixel(GRAPH, x, y, color);
			}
		}
	}

	static void line(int x0, int y0, int x1, int y1, int color)
	{
		double ratio = double(y1 - y0) / (x1 - x0);
		double b = y1 - x1 * ratio;

		if (x1 - x0 != 0)
		{
			int y = 0;
			if (x1 > x0)
			{
				for (int x = x0; x <= x1; x++)
				{
					for (int w = -5; w <= 5; w++)
					{
						y = x * ratio + b + w;
						SetPixel(GRAPH, x, y, color);
					}
				}
			}
			else
			{
				for (int x = x0; x >= x1; x--)
				{
					for (int w = -5; w <= 5; w++)
					{
						y = x * ratio + b + w;
						SetPixel(GRAPH, x, y, color);
					}
				}
			}
		}
		else
		{
			if (y1 > y0)
			{
				for (int y = y0; y <= y1; y++)
				{
					for (int w = -5; w <= 5; w++)
					{
						SetPixel(GRAPH, x0 + w, y, color);
					}
				}
			}
			else
			{
				for (int y = y0; y >= y1; y--)
				{
					for (int w = -5; w <= 5; w++)
					{
						SetPixel(GRAPH, x0 + w, y, color);
					}
				}
			}
		}
	}

	static void
	circle(int x0, int y0, int r, int color)
	{
		for (int y = y0 - r; y <= y0 + r; y++)
		{
			for (int x = x0 - r; x <= x0 + r; x++)
			{
				if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
				{
					SetPixel(GRAPH, x, y, color);
				}
			}
		}
	}

	static void circleNotFilled(int x0, int y0, int r, int w, int color)
	{
		for (int y = y0 - r >= 0 ? y0 - r : 0; y <= (y0 + r <= Y ? y0 + r : Y); y++)
		{
			for (int x = x0 - r >= 0 ? x0 - r : 0; x <= (x0 + r <= X ? x0 + r : X); x++)
			{
				if (((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r) &&
					((x - x0) * (x - x0) + (y - y0) * (y - y0) >= (r - w) * (r - w)))
				{
					SetPixel(GRAPH, x, y, color);
				}
			}
		}
	}

	static void turretFire(int x0, int y0)
	{
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::black);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::red);
	}

	static void turretIce(int x0, int y0)
	{
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::black);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::blue);
	}

	static void turretBolt(int x0, int y0)
	{
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::black);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::yellow);
	}

	static void turretFireShadow(int x0, int y0)
	{
		Draw::circleNotFilled(x0 + a / 2, y0 + a / 2, 2.5 * a, 3, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::bRed);
	}

	static void turretIceShadow(int x0, int y0)
	{
		Draw::circleNotFilled(x0 + a / 2, y0 + a / 2, 2.5 * a, 3, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::bBlue);
	}

	static void turretBoltShadow(int x0, int y0)
	{
		Draw::circleNotFilled(x0 + a / 2, y0 + a / 2, 2.5 * a, 3, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 2, Color::bBlack);
		Draw::circle(x0 + a / 2, y0 + a / 2, a / 4, Color::bYellow);
	}
};

struct Point
{
	int x;
	int y;
	Area area;
	Point(int x = 0, int y = 0, Area area = Area::free) : x(x), y(y), area(area) {}
};

class Path
{
public:
	int id;
	int x0, y0;
	StartMeta sm;
	Path(int id, int x0, int y0, StartMeta sm = StartMeta::notSet) : id(id), x0(x0), y0(y0), sm(sm) {}
};

std::vector<Path> paths =
	{
		Path(0, 7 * a, 0, StartMeta::start),
		Path(1, 7 * a, 1 * a),
		Path(2, 7 * a, 2 * a),
		Path(3, 7 * a, 3 * a),
		Path(4, 6 * a, 3 * a),
		Path(5, 5 * a, 3 * a),
		Path(6, 4 * a, 3 * a),
		Path(7, 4 * a, 4 * a),
		Path(8, 4 * a, 5 * a),
		Path(9, 4 * a, 6 * a),
		Path(10, 3 * a, 6 * a),
		Path(11, 2 * a, 6 * a),
		Path(12, 1 * a, 6 * a),
		Path(13, 0 * a, 6 * a, StartMeta::meta)};

class Player
{
public:
	Point p = Point(0, 0);
	int hp = 3;
	bool locked;
	int gold = 3;
	Player() {}

	void hpShow()
	{
		for (int i = 0; i < hp; i++)
		{
			Draw::circle(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - a + a / 2, a / 3, Color::green);
			Draw::circleNotFilled(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - a + a / 2, a / 3,1, Color::black);
		}
		for (int i = 2; i >= hp; i--)
		{
			Draw::circle(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - a + a / 2, a / 3, Color::bBlack);
			Draw::circleNotFilled(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - a + a / 2, a / 3, 1, Color::black);
		}
	}

	void goldShow()
	{
		for (int i = 0; i < gold; i++)
		{
			Draw::circle(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - 2 * a + a / 2, a / 3, Color::dYellow);
			Draw::circleNotFilled(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - 2 * a + a / 2, a / 3, 1, Color::black);
		}
		for (int i = 2; i >= gold; i--)
		{
			Draw::circle(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - 2 * a + a / 2, a / 3, Color::bBlack);
			Draw::circleNotFilled(X - 2 * a + a / 2.8 + 2 * i * a / 3, Y - 2 * a + a / 2, a / 3, 1, Color::black);
		}
	}

	void show()
	{
		locked = false;
		int eps = a * 0.05;
		for (const auto &point : points)
		{
			if (p.x == point.x && p.y == point.y)
			{
				locked = true;
				break;
			}
		}

		if (view == View::forest)
		{
			switch (key)
			{
			case Key::up:
				if (p.y + a < Y)
					p.y += a;
				break;
			case Key::down:
				if (p.y - a >= 0)
					p.y -= a;
				break;
			case Key::right:
				if (p.x + a < X - 2 * a)
					p.x += a;
				break;
			case Key::left:
				if (p.x - a >= 0)
					p.x -= a;
				break;
			case Key::changeView:
				p.x = X - 2 * a;
				p.y = 0;
				view = View::shop;
				turretId = TurretId::notSet;
				break;
			case Key::enter:
				if (turretId == TurretId::fire && !locked)
				{
					turretsFire.emplace_back(p.x, p.y);
					gold -= 1;
					turretId = TurretId::notSet;
				}
				else if (turretId == TurretId::ice && !locked)
				{
					turretsIce.emplace_back(p.x, p.y);
					gold -= 2;
					turretId = TurretId::notSet;
				}
				else if (turretId == TurretId::bolt && !locked)
				{
					turretsBolt.emplace_back(p.x, p.y);
					gold -= 3;
					turretId = TurretId::notSet;
				}
				break;
			default:
				break;
			}

			switch (turretId)
			{
			case TurretId::fire:
				Draw::turretFireShadow(p.x, p.y);
				break;
			case TurretId::ice:
				Draw::turretIceShadow(p.x, p.y);
				break;
			case TurretId::bolt:
				Draw::turretBoltShadow(p.x, p.y);
				break;
			}
		}
		else if (view == View::shop)
		{
			switch (key)
			{
			case Key::up:
				if (p.y + a < Y - 4 * a)
					p.y += a;
				break;
			case Key::down:
				if (p.y - a >= 0)
					p.y -= a;
				break;
			case Key::right:
				if (p.x + a < X)
					p.x += a;
				break;
			case Key::left:
				if (p.x - a >= X - 2 * a)
					p.x -= a;
				break;
			case Key::changeView:
				p.x = 0;
				p.y = 0;
				view = View::forest;
				break;
			case Key::enter:
				if (p.x == X - 2 * a && p.y == 0)
				{
					if (gold >= 1)
					{
						view = View::forest;
						p.x = 0;
						p.y = 0;
						turretId = TurretId::fire;
					}
				}
				if (p.x == X - 1 * a && p.y == 0)
				{
					if (gold >= 2)
					{
						view = View::forest;
						p.x = 0;
						p.y = 0;
						turretId = TurretId::ice;
					}
				}
				if (p.x == X - 1 * a && p.y == a)
				{
					if (gold >= 3)
					{
						view = View::forest;
						p.x = 0;
						p.y = 0;
						turretId = TurretId::bolt;
					}
				}
				break;
			default:
				break;
			}
		}

		for (int y = p.y; y <= p.y + a; y++)
		{
			for (int x = p.x; x <= p.x + a; x++)
			{
				if ((x >= p.x && x <= p.x + eps) ||
					(x <= p.x + a && x >= p.x + a - eps) ||
					(y >= p.y && y <= p.y + eps) ||
					(y <= p.y + a && y >= p.y + a - eps))
				{
					if (locked)
					{
						SetPixel(GRAPH, x, y, Color::red);
					}
					else
					{
						SetPixel(GRAPH, x, y, Color::yellow);
					}
				}
			}
		}
	}
};

class Enemy
{
public:
	int x0, y0;
	int xr, yr;
	int speed = 2;
	int speedX = 0;
	int speedY = 0;
	float scale = 0.6;
	int size = a * scale;
	int pathId = 1;
	int hp = 6;
	bool slowed = false;
	int tcSlowed = 3;
	int tiSlowed = tcSlowed;
	bool died = false;
	bool meta = false;
	Player *player;

	Enemy(int x0, int y0, Player *player) : x0(x0), y0(y0), xr(x0), yr(y0), player(player)
	{
		this->x0 += (a - size) / 2;
		this->y0 += (a - size) / 2;
	}

	void setSpeed()
	{
		if (slowed && tiSlowed == tcSlowed)
		{
			speed = 1;
			tiSlowed = 0;
		}
		if (tiSlowed != tcSlowed)
		{
			tiSlowed++;
		}
		else
		{
			slowed = false;
			speed = 2;
		}

		if (abs(paths[pathId].x0 - xr) > speed)
		{
			if (paths[pathId].x0 - xr > 0)
			{
				speedX = speed;
			}
			else
			{
				speedX = -speed;
			}
		}
		else if (abs(paths[pathId].y0 - yr) > speed)
		{
			if (paths[pathId].y0 - yr > 0)
			{
				speedY = speed;
			}
			else
			{
				speedY = -speed;
			}
		}
		else
		{
			speedX = 0;
			speedY = 0;
			xr = paths[pathId].x0;
			yr = paths[pathId].y0;
			x0 = xr + (a - size) / 2;
			y0 = yr + (a - size) / 2;

			if (pathId < paths.size() - 1)
			{
				pathId++;
			}
			else
			{
				speed = 0;
				meta = true;
				player->hp--;
			}
		}
	}

	void show()
	{
		if (died || meta)
		{
			return;
		}
		if (hp <= 0)
		{
			if (player->gold < 3)
				player->gold++;
			died = true;
		}
		setSpeed();
		for (int y = y0; y <= y0 + size; y++)
		{
			for (int x = x0; x <= x0 + size; x++)
			{
				if (y <= y0 + size / 2)
				{
					if (x >= x0 + size / 2 - (y - y0) &&
						x <= x0 + size / 2 + (y - y0))
					{
						if (slowed)
						{
							SetPixel(GRAPH, x, y, Color::bBlue);
						}
						else
						{
							SetPixel(GRAPH, x, y, Color::white);
						}
					}
					if (x == x0 + size / 2 - (y - y0) ||
						x == x0 + size / 2 + (y - y0))
						SetPixel(GRAPH, x, y, Color::black);
				}
				else
				{
					if (x >= x0 + size / 2 - (size - (y - y0)) &&
						x <= x0 + size / 2 + (size - (y - y0)))
					{
						if (slowed)
						{
							SetPixel(GRAPH, x, y, Color::bBlue);
						}
						else
						{
							SetPixel(GRAPH, x, y, Color::white);
						}
					}
					if (x == x0 + size / 2 - (size - (y - y0)) ||
						x == x0 + size / 2 + (size - (y - y0)))
						SetPixel(GRAPH, x, y, Color::black);
				}
			}
		}
		if (yr + speedY <= Y && yr + speedY >= 0)
		{
			y0 += speedY;
			yr += speedY;
		}
		if (xr + speedX <= X && xr + speedX >= 0)
		{
			x0 += speedX;
			xr += speedX;
		}
	}
};

class ProjectileFire
{
public:
	ProjectileFire(int x0, int y0, int x1, int y1, Enemy *enemy) : x0(x0), y0(y0), x1(x1), y1(y1), enemy(enemy)
	{
	}
	Enemy *enemy;
	double speed = 30;
	int r = a / 4;
	int color = Color::red;
	int x0, y0, x1, y1;
	double c = sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));
	int vx = (x1 - x0) * speed / c;
	int vy = (y1 - y0) * speed / c;
	int dmg = 2;

	bool show()
	{
		for (int y = 0; y <= y0 + Y; y++)
		{
			for (int x = 0; x <= x0 + X - 2 * a; x++)
			{
				if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
					SetPixel(GRAPH, x, y, color);
			}
		}
		if (x0 == x1 && y0 == y1)
		{
			enemy->hp -= dmg;
			return false;
		}

		x0 += vx;
		y0 += vy;

		if ((x0 > x1 - speed && x0 < x1 + speed) && (y0 > y1 - speed && y0 < y1 + speed))
		{
			x0 = x1;
			y0 = y1;
		}
		return true;
	}
};

class ProjectileBolt
{
public:
	ProjectileBolt(int x0, int y0, int x1, int y1, std::vector<Enemy *> enemies3) : x0(x0), y0(y0), x1(x1), y1(y1), enemies3(enemies3)
	{
	}
	std::vector<Enemy *> enemies3;
	int r = a / 4;
	int color = Color::yellow;
	int x0, y0, x1, y1;
	int dmg = 1;
	bool show()
	{
		Draw::line(x0, y0, x1, y1, color);
		enemies3[0]->hp -= dmg;
		for (int i = 1; i < enemies3.size(); i++)
		{
			int xtemp0 = enemies3[i - 1]->xr + a / 2;
			int ytemp0 = enemies3[i - 1]->yr + a / 2;
			int xtemp1 = enemies3[i]->xr + a / 2;
			int ytemp1 = enemies3[i]->yr + a / 2;
			Draw::line(xtemp0, ytemp0, xtemp1, ytemp1, color);
			enemies3[i]->hp -= dmg;
		}
		return false;
	}
};

class ProjectileIce
{
public:
	ProjectileIce(int x0, int y0, int x1, int y1, Enemy *enemy) : x0(x0), y0(y0), x1(x1), y1(y1), enemy(enemy)
	{
	}
	Enemy *enemy;
	int speed = 30;
	int r = a / 4;
	int color = Color::blue;
	int x0, y0, x1, y1;
	int c = sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));
	int vx = (x1 - x0) * speed / c;
	int vy = (y1 - y0) * speed / c;
	int dmg = 2;

	bool show()
	{
		for (int y = 0; y <= y0 + Y; y++)
		{
			for (int x = 0; x <= x0 + X - 2 * a; x++)
			{
				if ((x - x0) * (x - x0) + (y - y0) * (y - y0) <= r * r)
					SetPixel(GRAPH, x, y, color);
			}
		}
		if (x0 == x1 && y0 == y1)
		{
			enemy->hp -= dmg;
			enemy->slowed = true;
			return false;
		}

		x0 += vx;
		y0 += vy;

		if ((x0 > x1 - speed && x0 < x1 + speed) && (y0 > y1 - speed && y0 < y1 + speed))
		{
			x0 = x1;
			y0 = y1;
		}
		return true;
	}
};

class TurretFire
{
public:
	int x0;
	int y0;
	int x1;
	int y1;
	int tc = 20;
	int ti = 0;
	int range = 2.5 * a;
	TurretMode tm;
	std::vector<ProjectileFire> projectiles;
	TurretFire(int x0, int y0, TurretMode tm = TurretMode::idle) : x0(x0 + a / 2), y0(y0 + a / 2), tm(tm)
	{
		points.emplace_back(x0, y0, Area::locked);
	}
	void show()
	{
		Enemy *enemy;

		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			if (enemies[i].hp <= 0)
			{
				tm = TurretMode::idle;
				continue;
			}
			int xe = (enemies[i].xr + a / 2);
			int ye = (enemies[i].yr + a / 2);
			if ((xe - x0) * (xe - x0) + (ye - y0) * (ye - y0) < range * range)
			{
				enemy = &enemies[i];
				x1 = xe;
				y1 = ye;
				tm = TurretMode::attacking;
				break;
			}
			tm = TurretMode::idle;
		}

		if (tm == TurretMode::attacking)
		{
			count();
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, load());
			shoot(enemy);
		}
		if (tm == TurretMode::idle)
		{
			ti = 0;
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, Color::red);
		}
	}

	int
	shoot(Enemy *enemy)
	{
		if (ti == tc)
		{
			projectiles.emplace_back(x0, y0, x1, y1, enemy);
		}
	}

	void showProjectiles()
	{
		int del = 0;
		for (int i = 0; i < projectiles.size(); i++)
		{
			if (!projectiles[i].show())
			{
				del++;
			}
		}

		for (int j = 0; j < del; j++)
		{
			projectiles.erase(projectiles.begin());
		}
	}

	void count()
	{
		if (ti == tc)
		{
			ti = 0;
		}
		else
		{
			ti++;
		}
	}

	int load()
	{
		int R = 0;
		int G = 0;
		int B = 0;

		R = ti * 0xFF / tc; // k=0->1
		G = 0;
		B = 0;

		return R << 16 | G << 8 | B;
	}
};

class TurretIce
{
public:
	int x0;
	int y0;
	int x1;
	int y1;
	int tc = 20;
	int ti = 0;
	int range = 2.5 * a;
	TurretMode tm;
	std::vector<ProjectileIce> projectiles;
	TurretIce(int x0, int y0, TurretMode tm = TurretMode::idle) : x0(x0 + a / 2), y0(y0 + a / 2), tm(tm)
	{
		points.emplace_back(x0, y0, Area::locked);
	}
	void show()
	{
		Enemy *enemy;
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			if (enemies[i].hp <= 0)
			{
				tm = TurretMode::idle;
				continue;
			}
			int xe = (enemies[i].xr + a / 2);
			int ye = (enemies[i].yr + a / 2);
			if ((xe - x0) * (xe - x0) + (ye - y0) * (ye - y0) < range * range)
			{
				enemy = &enemies[i];
				x1 = xe;
				y1 = ye;
				tm = TurretMode::attacking;
				break;
			}
			tm = TurretMode::idle;
		}

		if (tm == TurretMode::attacking)
		{
			count();
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, load());
			shoot(enemy);
		}
		if (tm == TurretMode::idle)
		{
			ti = 0;
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, Color::blue);
		}
	}

	int
	shoot(Enemy *enemy)
	{
		if (ti == tc)
		{
			projectiles.emplace_back(x0, y0, x1, y1, enemy);
		}
	}

	void showProjectiles()
	{
		int del = 0;
		for (int i = 0; i < projectiles.size(); i++)
		{
			if (!projectiles[i].show())
			{
				del++;
			}
		}

		for (int j = 0; j < del; j++)
		{
			projectiles.erase(projectiles.begin());
		}
	}

	void count()
	{
		if (ti == tc)
		{
			ti = 0;
		}
		else
		{
			ti++;
		}
	}

	int load()
	{
		int R = 0;
		int G = 0;
		int B = 0;

		R = 0; // k=0->1
		G = 0;
		B = ti * 0xFF / tc;

		return R << 16 | G << 8 | B;
	}
};

class TurretBolt
{
public:
	int x0;
	int y0;
	int x1;
	int y1;
	int tc = 20;
	int ti = 0;
	int range = 2.5 * a;
	TurretMode tm;
	std::vector<ProjectileBolt> projectiles;
	TurretBolt(int x0, int y0, TurretMode tm = TurretMode::idle) : x0(x0 + a / 2), y0(y0 + a / 2), tm(tm)
	{
		points.emplace_back(x0, y0, Area::locked);
	}
	void show()
	{
		std::vector<Enemy *> enemies3;
		int i3 = 0;
		for (int i = enemies.size() - 1; i >= 0; i--)
		{
			if (enemies[i].hp <= 0)
			{
				continue;
			}
			int xe = (enemies[i].xr + a / 2);
			int ye = (enemies[i].yr + a / 2);
			if ((xe - x0) * (xe - x0) + (ye - y0) * (ye - y0) < range * range)
			{
				enemies3.emplace_back(&enemies[i]);
				x1 = xe;
				y1 = ye;
				tm = TurretMode::attacking;
				if (++i3 == 3)
					break;
			}
		}
		if (!i3)
			tm = TurretMode::idle;

		if (tm == TurretMode::attacking)
		{
			count();
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, load());
			shoot(enemies3);
		}
		if (tm == TurretMode::idle)
		{
			ti = 0;
			Draw::circle(x0, y0, a / 2, Color::black);
			Draw::circle(x0, y0, a / 4, Color::yellow);
		}
	}

	int
	shoot(std::vector<Enemy *> enemies3)
	{
		if (ti == tc)
		{
			projectiles.emplace_back(x0, y0, x1, y1, enemies3);
		}
	}

	void showProjectiles()
	{

		int del = 0;
		for (int i = 0; i < projectiles.size(); i++)
		{
			if (!projectiles[i].show())
			{
				del++;
			}
		}

		for (int j = 0; j < del; j++)
		{
			projectiles.erase(projectiles.begin());
		}
	}

	void count()
	{
		if (ti == tc)
		{
			ti = 0;
		}
		else
		{
			ti++;
		}
	}

	int load()
	{
		int R = 0;
		int G = 0;
		int B = 0;

		R = ti * 0xFF / tc; // k=0->1
		G = ti * 0xFF / tc;
		B = 0;

		return R << 16 | G << 8 | B;
	}
};

class Collider
{
};

class PathCreator
{
public:
	void show()
	{
		for (const auto &p : paths)
		{
			Draw::fillRect(p.x0, p.y0, Color::brown);
			points.emplace_back(p.x0, p.y0, Area::locked);
		}
	}
};

class EnemySpawner
{
public:
	int x0 = paths[0].x0;
	int y0 = paths[0].y0;
	int ti = 0;
	int tc;
	int tc_a = 50;
	int tc_b = 70;
	Player *player;

	EnemySpawner(Player *player) : player(player)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(tc_a, tc_b);
		tc = distr(gen);
	}
	void show()
	{
		count();
		if (ti == tc)
		{
			enemies.emplace_back(Enemy(x0, y0, player));
		}
	}
	void count()
	{
		if (ti == tc)
		{
			ti = 0;
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> distr(tc_a - lvl * 10, tc_b - lvl * 10);
			tc = distr(gen);
			if (tc < 5)
				tc = 5;
		}
		else
		{
			ti++;
		}
	}
};

class Map
{
public:
	Map() {}
	void gridForest()
	{
		for (int y = 0; y <= Y; y++)
		{
			for (int x = 0; x < X - 2 * a; x++)
			{
				if (!(x % a))
					SetPixel(GRAPH, x, y, Color::white);
			}
		}

		for (int x = 0; x < X - 2 * a; x++)
		{
			for (int y = 0; y <= Y; y++)
			{
				if (!(y % a))
					SetPixel(GRAPH, x, y, Color::white);
			}
		}
	}

	void gridShop()
	{
		for (int y = 0; y <= Y - 7 * a; y++)
		{
			for (int x = X - 2 * a; x <= X; x++)
			{
				if (!(x % a))
					SetPixel(GRAPH, x, y, Color::black);
			}
		}

		for (int x = X - 2 * a; x <= X; x++)
		{
			for (int y = 0; y <= Y - 7 * a; y++)
			{
				if (!(y % a))
					SetPixel(GRAPH, x, y, Color::black);
			}
		}
	}

	void shop()
	{
		for (int y = 0; y <= Y; y++)
		{
			for (int x = X - 2 * a; x <= X; x++)
			{
				SetPixel(GRAPH, x, y, Color::white);
			}
		}

		Draw::turretFire(X - 2 * a, 0);
		Draw::turretIce(X - a, 0);
		Draw::turretBolt(X - a, a);
		for (int i = 2; i >= 0; i--)
		{
			Draw::circle(X - 0.2 * a - 2 * i * a / 7,
						 a + a * 4 / 5, a / 7, Color::dYellow);
			Draw::circleNotFilled(X - 0.2 * a - 2 * i * a / 7,
								  a + a * 4 / 5, a / 7, 1, Color::black);
		}

		for (int i = 2; i >= 1; i--)
		{
			Draw::circle(X - 0.2 * a - 2 * i * a / 7,
						 a * 4 / 5, a / 7, Color::dYellow);
			Draw::circleNotFilled(X - 0.2 * a - 2 * i * a / 7,
								  a * 4 / 5, a / 7, 1, Color::black);
		}

		for (int i = 2; i >= 2; i--)
		{
			Draw::circle(X - 1.2 * a - 2 * i * a / 7,
						 a * 4 / 5, a / 7, Color::dYellow);
			Draw::circleNotFilled(X - 1.2 * a - 2 * i * a / 7,
								  a * 4 / 5, a / 7, 1, Color::black);
		}
	}

	void forest()
	{
		for (int y = 0; y <= Y; y++)
		{
			for (int x = 0; x < X - 2 * a; x++)
			{
				SetPixel(GRAPH, x, y, Color::green);
			}
		}
	}

	void endScreen()
		{
			for (int y = 0; y <= Y; y++)
			{
				for (int x = 0; x <= X; x++)
				{
					SetPixel(GRAPH, x, y, Color::green);
				}
			}

			int b = a/2;
			int x0 = 3*b;
			int y0 = 4*b;
			Draw::fillRect(1*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(1*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(1*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(1*b+x0, 5*b+y0, Color::brown,b);
						Draw::fillRect(1*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(2*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(2*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(2*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(3*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(3*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(3*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(4*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(3*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(4*b+x0, 6*b+y0, Color::brown,b);
						//N
						Draw::fillRect(6*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(6*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(6*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(6*b+x0, 5*b+y0, Color::brown,b);
						Draw::fillRect(6*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(7*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(8*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(9*b+x0, 5*b+y0, Color::brown,b);

						Draw::fillRect(10*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(10*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(10*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(10*b+x0, 5*b+y0, Color::brown,b);
						Draw::fillRect(10*b+x0, 6*b+y0, Color::brown,b);

						//D
						Draw::fillRect(12*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(12*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(12*b+x0, 4*b+y0, Color::brown,b);
						Draw::fillRect(12*b+x0, 5*b+y0, Color::brown,b);
						Draw::fillRect(12*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(13*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(13*b+x0, 6*b+y0, Color::brown,b);
						Draw::fillRect(14*b+x0, 2*b+y0, Color::brown,b);
						Draw::fillRect(14*b+x0, 6*b+y0, Color::brown,b);

						Draw::fillRect(15*b+x0, 3*b+y0, Color::brown,b);
						Draw::fillRect(15*b+x0, 5*b+y0, Color::brown,b);

						Draw::fillRect(16*b+x0, 4*b+y0, Color::brown,b);
		}
};

int main(int argc, char *argv[])
{
	Map map;
	Player player;
	EnemySpawner enemySpawner(&player);
	PathCreator pathCreator;

	SystemInit();
	DataPrepare();
	while (1)
	{


		if (static_cast<Key>(getKey()) != keyPrev)
		{
			key = static_cast<Key>(getKey());
			keyPrev = key;
		}
		else
		{
			key = Key::notSet;
		}

		UpdateIO();
		ClearScreen();
		if (player.hp > 0)
		{
		map.forest();
		map.shop();
		pathCreator.show();
		map.gridForest();
		map.gridShop();
		player.show();

		for (auto &e : enemies)
		{
			e.show();
		}
		for (auto &t : turretsFire)
		{
			t.show();
		}

		for (auto &t : turretsIce)
		{
			t.show();
		}

		for (auto &t : turretsBolt)
		{
			t.show();
		}

		for (auto &t : turretsFire)
		{
			t.showProjectiles();
		}

		for (auto &t : turretsIce)
		{
			t.showProjectiles();
		}

		for (auto &t : turretsBolt)
		{
			t.showProjectiles();
		}

		if (START)
		{
			if (!ti_lvl)
			{
				lvl++;
				ti_lvl = tc_lvl;
			}
			enemySpawner.show();
			ti_lvl--;
		}

		if (key == Key::start)
			START = !START;
		player.hpShow();
		player.goldShow();
		}
		else
		{
				map.endScreen();
			if (key == Key::enter)
			{
				player.hp=3;
				player.gold=3;
				START=false;
				lvl=0;
				turretsFire.clear();
				turretsIce.clear();
				turretsBolt.clear();
				enemies.clear();
			}
		}
		usleep(10000);
	}
}

void *TimerThread(void *arguments)
{
	struct timespec destTime;
	clock_gettime(CLOCK_MONOTONIC, &destTime);
	while (1)
	{
		destTime.tv_nsec += 1000000;
		if (destTime.tv_nsec >= 1E9)
		{
			destTime.tv_nsec -= 1E9;
			destTime.tv_sec++;
		}
		globalTimer_ms++;
		if (!(globalTimer_ms % 1000))
			flash_ligth = 0xFF;
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &destTime, NULL);
	}
	return NULL;
}

void SystemInit()
{
	FrameBufferFD = open("/dev/fb0", O_RDWR);
	ioctl(FrameBufferFD, FBIOGET_FSCREENINFO, &fix_info);
	framebuffer = mmap(NULL, fix_info.line_length * 480, PROT_READ | PROT_WRITE, MAP_SHARED, FrameBufferFD, 0);
	inputEventsFD = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
#ifdef RPiLAB
	inputJoyFD = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
#else
	inputJoyFD = 0;
#endif
	pthread_create(&tID, NULL, TimerThread, NULL);
	struct timeval tTime;
	gettimeofday(&tTime, NULL);
	startTime_ms = tTime.tv_sec * 1000 + tTime.tv_usec / 1000;
}

void PrintDiagnosticInfo()
{
	struct timeval tTime;
	gettimeofday(&tTime, NULL);
	long TimeDiff = (tTime.tv_sec * 1000 + tTime.tv_usec / 1000) - startTime_ms;
	long actTime = globalTimer_ms;
	if ((frame_count % 20) == 0)
		printf("%i:: %li:%li=>%li KEYS: %i, %i, %i, %i, %x\n", frame_count, actTime, TimeDiff, actTime - TimeDiff, getKey(), Jx, Jy, JRz, JoYAct.ButtonStates);
}

void UpdateIO()
{

	struct input_event ev;
	while (read(inputEventsFD, &ev, sizeof ev) > 0)
	{
		if (ev.type == EV_KEY)
		{
			switch (ev.value)
			{
			case 1:
				gRawKeys[0] = ev.code;
				break;
			case 0:
				gRawKeys[0] = 0;
				break;
			default:
				break;
			}
#ifndef RPiLAB
			switch (ev.code)
			{
			case KEY_KP4:
				JoYAct.AxisX = -127 * (ev.value & 1);
				break;
			case KEY_KP6:
				JoYAct.AxisX = 127 * (ev.value & 1);
				break;
			case KEY_KP8:
				JoYAct.AxisY = -127 * (ev.value & 1);
				break;
			case KEY_KP2:
				JoYAct.AxisY = 127 * (ev.value & 1);
				break;
			case KEY_RIGHT:
				if (ev.value == 1)
					JoYAct.ButtonStates |= BUTTON_2;
				if (ev.value == 0)
					JoYAct.ButtonStates &= ~BUTTON_2;
				break;
			case KEY_LEFT:
				if (ev.value == 1)
					JoYAct.ButtonStates |= BUTTON_4;
				if (ev.value == 0)
					JoYAct.ButtonStates &= ~BUTTON_4;
				break;
			case KEY_UP:
				if (ev.value == 1)
					JoYAct.ButtonStates |= BUTTON_1;
				if (ev.value == 0)
					JoYAct.ButtonStates &= ~BUTTON_1;
				break;
			case KEY_DOWN:
				if (ev.value == 1)
					JoYAct.ButtonStates |= BUTTON_3;
				if (ev.value == 0)
					JoYAct.ButtonStates &= ~BUTTON_3;
				break;
			default:
				break;
			}
#endif
		}
	}
#ifdef RPiLAB
	struct js_event ev2;
	while (read(inputJoyFD, &ev2, sizeof ev2) > 0)
	{
		if (ev2.type & JS_EVENT_AXIS)
		{
			switch (ev2.number)
			{

			case 1:
				JoYAct.AxisY = ev2.value >> 8;
				break;
			case 2:
				JoYAct.AxisX = ev2.value >> 8;
				break;
			case 3:
				JoYAct.AxisZ = ev2.value >> 8;
				break;
			case 4:
				JoYAct.AxisR = ev2.value >> 8;
				break;
			default:
				break;
			}
		}
		if (ev2.type & JS_EVENT_BUTTON)
		{
			if (ev2.value)
				JoYAct.ButtonStates |= (1 << ev2.number);
			else
				JoYAct.ButtonStates &= ~(1 << ev2.number);
		}
	}
#endif
}

void ClearScreen()
{

	unsigned int bckgmask = getKey() >> 2;

	int stage_max = -JRz / 20;
	if (stage_max < 1)
		stage_max = 1;
	if (stage_max > 9)
		stage_max = 9;

	if (JoYAct.ButtonStates & BUTTON_SELECT)
		stage_max = 24;

	stage++;
	for (int a = stage % stage_max; a < (640 * 480); a += stage_max)
	{
		((u_int32_t *)framebuffer)[fix_info.line_length / 4 * (a / 640) + a % 640] = GRAPH[a];
		GRAPH[a] = ((GRAPH[a] & 0xfefefe) >> 1) | bckgmask;
	}
}
void DrawObjects()
{
	int get_flash_light = flash_ligth;
	unsigned int bckgmask = (get_flash_light << 16) |
							(get_flash_light << 8) | (get_flash_light);
	if (stage % 2)
		flash_ligth >>= 1;

	frame_count++;
	Jx = (Jx * 93 + 7 * JoYAct.AxisX) / 100;
	Jy = (Jy * 93 + 7 * JoYAct.AxisY) / 100;
#ifdef RPiLAB
	Jz = (Jz * 93 + 7 * JoYAct.AxisZ) / 100;
	JRz = (JRz * 93 + 7 * JoYAct.AxisR) / 100;
#else
	Jz = (Jz * 93 + 7 * ((JoYAct.ButtonStates & BUTTON_2 ? -128 : 0) + (JoYAct.ButtonStates & BUTTON_4 ? 127 : 0))) / 100;
	JRz = (JRz * 93 + 7 * ((JoYAct.ButtonStates & BUTTON_1 ? -128 : 0) + (JoYAct.ButtonStates & BUTTON_3 ? 127 : 0))) / 100;
#endif
	if (JRz > 40)
		JRz = 40;
	ActMaxObj += 15;
	ActMaxObj %= MaxObj << 1;
	int RenderCnt = ActMaxObj - MaxObj;
	RenderCnt = (RenderCnt > 0) ? RenderCnt : -RenderCnt;
	if (RenderCnt < 2500)
		RenderCnt = 2500;

	for (int c = 0; c < RenderCnt; c++)
	{
		int srt = abs(dy[c]) + abs(dx[c]);
		for (int g = 0; g < srt / 10 + 1; g++)
		{
			int ddf = 20;
			ddf += -(JRz) / (srt + 1);
			if (ddf < 8)
				ddf = 8;
			y[c] += (ddf * dy[c]) / 30 + (Jyt - Jy) * srt / 20;
			x[c] += (ddf * dx[c]) / 30 + (Jxt - Jx) * srt / 20;
			y[c] -= ((xt[c] - x[c]) * Jz) / 200;
			x[c] += ((yt[c] - y[c]) * Jz) / 200;
			xt[c] = x[c];
			yt[c] = y[c];
			if (x[c] < 0 || y[c] < 0 || x[c] > 639 * 16 - s[c] || y[c] > 479 * 16 - s[c])
			{
				do
				{
					s[c] = randr(0x1f) + 1;
					xt[c] = x[c] = randr(519 * 16 - s[c]) + 60 * 16;
					yt[c] = y[c] = randr(379 * 16 - s[c]) + 50 * 16;
					int spd = randr(15) + 7;
					dx[c] = (x[c] - 320 * 16 - 16 * Jx - (Jx - Jxt) / 2) * spd;
					dy[c] = (y[c] - 240 * 16 - 16 * Jy - (Jy - Jyt) / 2) * spd;

					while (abs(dx[c]) > (-JRz / 18 + 9) || abs(dy[c]) > (-JRz / 18 + 9))
					{
						dx[c] /= 3;
						dy[c] /= 3;
					}
				} while ((abs(dy[c]) + abs(dx[c])) <= 4);
			}
			int size = s[c];

			int col = ((y[c] * 255 / (480 << 4)) + 0xf0000 * size) | bckgmask;

			for (int b = y[c] >> 4; b < (y[c] + size) >> 4; b++)
				for (int a = x[c] >> 4; a < (x[c] + size) >> 4; a++)
					SetPixel(GRAPH, a, b, col);
		}
	}
	Jxt = Jx;
	Jyt = Jy;
}
void DataPrepare()
{
	for (unsigned char a = 0; a < sizeof(struct JoY); a++)
		((char *)&JoYAct)[a] = 0;
	ActMaxObj = MaxObj;

	for (int a = 0; a < MaxObj; a++)
	{
		do
		{
			s[a] = randr(0x1f) + 1;
			xt[a] = x[a] = randr(619 * 16 - s[a]);
			yt[a] = y[a] = randr(479 * 16 - s[a]);
			int spd = randr(15) + 7;
			dx[a] = (x[a] - 320 * 16) * spd;
			dy[a] = (y[a] - 240 * 16) * spd;

			while (abs(dx[a]) > (9) || abs(dy[a]) > (9))
			{
				dx[a] /= 3;
				dy[a] /= 3;
			}
		} while ((abs(dy[a]) + abs(dx[a])) <= 4);
	}
}
