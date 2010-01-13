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

#include "level.h"
#include "debugdraw.h"
#include "organism.h"
#include "population.h"

#include <fstream>
#include <cstdio>

#define DO_SLEEP 1

const b2Vec2 GRAVITY(0.0, -10.0);

/**
 * Load a level from the given file.
 * 
 * @param filename  the name of the file describing the level
 */
Level::Level(const char *filename)
    : m_time(0) {
    std::ifstream fin(filename);
    while(true) {
        std::string key; fin >> key;
        if(key == "end") {
            break;
        } else if(key == "worldAABB") {
            double x, y;
            b2AABB worldAABB;
            fin >> x >> y; worldAABB.lowerBound.Set(x, y);
            fin >> x >> y; worldAABB.upperBound.Set(x, y);
            m_world = new b2World(worldAABB, GRAVITY, DO_SLEEP);
            b2BodyDef bodyDef; m_ground = createBody(&bodyDef);
        } else if(key == "ground") {
            double l, x, y, q; fin >> l >> x >> y >> q;
            b2PolygonDef polygonDef;
            polygonDef.SetAsBox(l, 1.0, b2Vec2(x, y), q * b2_pi / 180);
            m_ground->CreateShape(&polygonDef);
        } else if(key == "goal") {
            int t; double x, y; fin >> t >> x >> y;
            if(t == 0) m_goal.Set(x, y);
            else       m_goalChanges[t] = b2Vec2(x, y);
        } else if(key == "spawnPoint") {
            double x, y; fin >> x >> y;
            spawnPoint.Set(x, y);
        } else if(key == "lifetime") {
            double t; fin >> t;
            m_population = new Population(this, (int) (t * FRAME_RATE));
            m_population->evolve = true;
            m_population->spawn();
        }
    }
    fin.close();
    
    m_world->SetContactListener(this);
    m_world->SetDebugDraw(&m_debugDraw);
    m_debugDraw.SetFlags(b2DebugDraw::e_shapeBit);
}

Level::~Level() {
    delete m_world;
}

/**
 * Step the level forward by one timestep.
 */
void Level::step() {
    if(m_time % FRAME_RATE == 0 && m_goalChanges.count(m_time / FRAME_RATE))
        m_goal = m_goalChanges[m_time / FRAME_RATE];
    m_time++;
    m_population->step();
    m_world->Step(1.0 / FRAME_RATE, 10);
    m_debugDraw.DrawSolidCircle(
        m_goal, 5.0, b2Vec2_zero, b2Color(0.0, 0.5, 1.0));
}

/**
 * Return the displacement of the given position from the goal.
 * 
 * @param position  the position to measure
 * @return          the displacement
 */
b2Vec2 Level::displacementFromGoal(b2Vec2 position) {
    return position - m_goal;
}

/**
 * Return the location of the first intersection between the given segment and
 * the ground.
 * 
 * @param segment  the segment
 * @return         the distance to the intersection, where 1.0 is the end point
 *                 of the segment
 */
double Level::raycast(const b2Segment &segment) {
    float lambda, bestLambda = 1.0;
    b2Vec2 normal;
    for (b2Shape* s = m_ground->GetShapeList(); s; s = s->GetNext())
        if(s->TestSegment(m_ground->GetXForm(), &lambda, &normal, segment, 1.0)
           && lambda < bestLambda)
            bestLambda = lambda;
    return bestLambda;
}

/**
 * Reposition the given body.
 * 
 * @param body      the body to be repositioned
 * @param position  the new position
 */
void Level::repositionBody(b2Body *body, b2Vec2 position) {
    body->SetXForm(position, body->GetAngle());
    body->WakeUp();
}

/**
 * Create a new body according to the given body definition.
 * 
 * @param def  the body definition
 * @return     the new body
 */
b2Body *Level::createBody(const b2BodyDef *def) {
    return m_world->CreateBody(def);
}

/**
 * Destroy the given body.
 * 
 * @param body  the body to be destroyed
 */
void Level::destroyBody(b2Body *body) {
    m_world->DestroyBody(body);
}

void Level::Add(const b2ContactPoint *point) {
    contactPoint(point, false);
}

void Level::Persist(const b2ContactPoint *point) {
    contactPoint(point, true);
}

void Level::Remove(const b2ContactPoint *point) {
    (void) point;
}

/**
 * Notify of a contact point.
 * 
 * @param point    the contact point
 * @param persist  indicates whether the point has persisted
 */
void Level::contactPoint(const b2ContactPoint *point, bool persist) {
    Organism *organism;
    bool reverse;
    if((organism = m_population->find(point->shape1->GetBody()))
       && point->shape2->GetBody() == m_ground)
        reverse = false;
    else if((organism = m_population->find(point->shape2->GetBody()))
            && point->shape1->GetBody() == m_ground)
        reverse = true;
    else
        return;
    double slope = point->normal.x / point->normal.y;
    if(reverse) slope = -slope;
    organism->inputs[4] = slope;
}
