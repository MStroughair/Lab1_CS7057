#pragma once

#include "Utilities.h"

class Particle;
class Force;

class Particle{
	vec3 initialPosition;
public:
	Mesh mesh;

	vec3 position;
	vec3 velocity; 
	vec3 force;
	float mass;
	int health;
	vec3 colour;
	float scale;

	Particle(Mesh& me, vec3 p, int m) {
		scale = rand() % 10 / 10.0f;
		health = 100;
		force = vec3(0.0, 0.0, 0.0);
		velocity = vec3(rand()%30-15, 50, rand()%30-15);
		mesh = me;
		position = p;
		initialPosition = p;
		mass = m;
		mass /= 100.0;
		colour = RED;
	}
	void addForce(vec3 f)
	{
		force += f;
	}

	void resolveForce(float delta)
	{
		vec3 ac = force*mass;
		velocity += ac*delta;
		position += velocity*delta;
	}

	void reset()
	{
		force = vec3(0.0, 0.0, 0.0);
		position = initialPosition;
		velocity = vec3(rand() % 30-15, 50, rand() % 30-15);
		health = rand()%50+50;
		scale = rand() % 1000 / 1000.0f;
		colour = RED;
	}

	void evolve()
	{
		if (health > 80)
			colour = RED;
		else if (health > 60)
			colour = PURPLE;
		else if (health > 40)
			colour = GREEN;
		else if (health > 20)
			colour = YELLOW;
		else
			colour = GREY;
	}
};

class Force {
public:
	virtual void applyForce(Particle& p) = 0;
};

class Drag : public Force {
public:
	void applyForce(Particle& p) {
		GLfloat constants = 0.5 * 1.225 * 0.47 * 3.14 * (0.1*p.scale) * (0.1*p.scale);
		vec3 velocity = vec3(p.velocity.v[0] * p.velocity.v[0], p.velocity.v[1] * p.velocity.v[1], p.velocity.v[2] * p.velocity.v[2]);
		p.addForce(velocity*constants);
			//force -= velocity*constants;
	};
};

class Gravity : public Force {
public:
	void applyForce(Particle& p) {
		float force_gravity = -9.81f*p.mass;
		p.addForce(vec3(0.0, force_gravity, 0.0));
	};
};


class ParticleSystem {
public:
	vector<Particle> particles;
	//vector<Force*> forces;
	int numParticles;
	int numForces;
	Drag d;
	Gravity g;

	void init(int p, Mesh& m)
	{
		numParticles = p;
		numForces = 0;
		int length = p / 20;
		for (int i = -length/2; i < length/2; i++)
		{
			for (int j = -10; j < 10; j++)
			{
				//Particle particle(m, vec3(rand() % 60 - 30, 20.0f, rand() % 60 - 30), rand() % 400+100);
				Particle particle(m, vec3(0.0f, 20.0f, 0.0f), rand() % 400 + 100);
				if (particle.mass <= 0)
					particle.mass = 1;
				particles.push_back(particle);
			}
		}
		numForces = 2;
	}

	void applyForces(float delta)
	{
		for (int i = 0; i < numParticles; i++)
		{
			if (particles[i].health <= 0)
				particles[i].reset();
			particles[i].force = vec3(0.0, 0.0, 0.0);
			d.applyForce(particles[i]);
			g.applyForce(particles[i]);

			particles[i].resolveForce(delta);
			particles[i].health--;
		}
	}

	void checkCollisions(vec3 point, vec3 normal, float delta)
	{
		float coRest = 0.6;
		for (int i = 0; i < numParticles; i++)
		{
			if (dot((particles[i].position - point), normal) < 0.00001f && dot(particles[i].velocity, normal) < 0.00001f)
			{
				vec3 deltaX = particles[i].position - normal * 1.4 * dot((particles[i].position - point), normal);
				particles[i].position = deltaX;
				particles[i].velocity = particles[i].velocity - (normal*dot(particles[i].velocity, normal))*(1+coRest);//vec3(particles[i].velocity.v[0] * normal.v[0], particles[i].velocity.v[1] * normal.v[1], particles[i].velocity.v[2] * normal.v[2]);
			}
		}
	}
};
