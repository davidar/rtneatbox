/*
* Copyright (c) 2010 David Roberts <d@vidr.cc>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "organism.h"
#include "level.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

#include <NEAT/network.h>

/**
 * Create a new organism from the given rtNEAT organism, in the given level.
 * 
 * @param organism  the rtNEAT organism
 * @param level     the level
 */
Organism::Organism(NEAT::Organism *organism, Level *level)
    : m_organism(organism), m_body(NULL), m_level(level) {
    inputs = new double[ORGANISM_NUM_INPUTS];
    inputs[ORGANISM_NUM_INPUTS-1] = 1.0; // bias
}

Organism::~Organism() {
    m_level->destroyBody(m_body);
}

/**
 * Step the organism forward by one timestep.
 * 
 * @param respawn  suppresses respawning if false
 */
void Organism::step(bool respawn) {
    if(m_body->IsFrozen()) kill();
    age(respawn);
    NEAT::Network *net = m_organism->net;
    b2Vec2 s = m_level->displacementFromGoal(position());
    b2Vec2 v = velocity();
    score = 1.0 / s.LengthSquared();
    inputs[0] = s.x;
    inputs[1] = s.y;
    inputs[2] = v.x;
    inputs[3] = v.y;
    // inputs[4] = slope (set by Level)
    inputs[5] = raycast(0.00 * b2_pi);
    inputs[6] = raycast(0.25 * b2_pi);
    inputs[7] = raycast(0.50 * b2_pi);
    inputs[8] = raycast(0.75 * b2_pi);
    inputs[9] = raycast(1.00 * b2_pi);
    inputs[10] = raycast(1.25 * b2_pi);
    inputs[11] = raycast(1.50 * b2_pi);
    inputs[12] = raycast(1.75 * b2_pi);
    net->load_sensors(inputs);
    memset(inputs, 0, sizeof(double) * ORGANISM_NUM_INPUTS);
    net->activate();
    act(net->outputs);
}

/**
 * Spawn the organism at the spawn point, removing the previous body if
 * necessary.
 */
void Organism::spawn() {
    if(m_body) m_level->destroyBody(m_body);
    score = 0;
    construct(m_level->spawnPoint
        + 3.0 * b2Vec2((double)rand()/RAND_MAX - 0.5,
                       (double)rand()/RAND_MAX - 0.5));
}

/**
 * Return the world position of the organism's body's center of mass.
 * 
 * @return  the position
 */
b2Vec2 Organism::position() {
    return m_body->GetWorldCenter();
}

/**
 * Return the linear velocity of the organism's body.
 * 
 * @return  the velocity
 */
b2Vec2 Organism::velocity() {
    return m_body->GetLinearVelocity();
}

/**
 * Return the rtNEAT organism.
 * 
 * @return  the rtNEAT organism
 */
NEAT::Organism *Organism::getNEATOrganism() {
    return m_organism;
}

/**
 * Replace the rtNEAT organism.
 * 
 * @param organism  the new rtNEAT organism
 */
void Organism::setNEATOrganism(NEAT::Organism *organism) {
    m_organism = organism;
}

/**
 * Return the organism's body.
 * 
 * @return  the body
 */
b2Body *Organism::getBody() {
    return m_body;
}

/**
 * Age the organism by one timestep, respawning if applicable.
 * 
 * @param respawn  suppresses respawning if false
 */
void Organism::age(bool respawn) {
    m_organism->time_alive++;
    if(m_organism->time_alive % NEAT::time_alive_minimum == 0) {
        m_organism->fitness = (m_organism->fitness + score)/2;
        spawn();
    }
}

/**
 * Perform a physical action with the given output signals.
 * 
 * @param outputs  the array of output signals
 */
void Organism::act(std::vector<NEAT::NNode*> outputs) {
    double forceX = 100.0 * (outputs[0]->activation - 0.5);
    double forceY = 0.0;
    m_body->ApplyForce(b2Vec2(forceX, forceY), position());
}

/**
 * Kill the organism, penalise it, and respawn it.
 */
void Organism::kill() {
    m_organism->fitness /= 2;
    spawn();
}

/**
 * Construct a body for the organism at the given position.
 * 
 * @param position  the position of the body
 */
void Organism::construct(b2Vec2 position) {
    b2BodyDef bodyDef;
    bodyDef.position = position;
    bodyDef.angularDamping = 1.0;
    m_body = m_level->createBody(&bodyDef);
    
    b2CircleDef circleDef;
    circleDef.radius = 1.0;
    circleDef.density = 1.0;
    circleDef.filter.groupIndex = -1;
    m_body->CreateShape(&circleDef);
    m_body->SetMassFromShapes();
}

/**
 * Perform a raycast from this organism in the given direction.
 * 
 * @param angle  the direction in radians
 * @param range  the length of the ray
 * @return       the distance to the intersection
 */
double Organism::raycast(double angle, double range) {
    b2Vec2 direction(cos(angle), sin(angle));
    b2Segment segment;
    segment.p1 = position();
    segment.p2 = position() + range * direction;
    return m_level->raycast(segment);
}
