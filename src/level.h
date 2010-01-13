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

#ifndef LEVEL_H
#define LEVEL_H

#include "debugdraw.h"

#include <map>

#include <Box2D.h>

#define FRAME_RATE 60
#define FRAME_PERIOD (1000/FRAME_RATE)

class Population;

class Level : b2ContactListener {
public:
    /** The position where organisms spawn */
    b2Vec2 spawnPoint;
    
    Level(const char *filename);
    ~Level();
    void step();
    b2Vec2 displacementFromGoal(b2Vec2 position);
    double raycast(const b2Segment &segment);
    void repositionBody(b2Body *body, b2Vec2 position);
    b2Body *createBody(const b2BodyDef *def);
    void destroyBody(b2Body *body);
    
    // b2ContactListener
    void Add(const b2ContactPoint *point);
    void Persist(const b2ContactPoint *point);
    void Remove(const b2ContactPoint *point);
    
protected:
    /** The world used by this level */
    b2World *m_world;
    /** The body comprising any floors and walls */
    b2Body *m_ground;
    /** The point for the organisms to aim for */
    b2Vec2 m_goal;
    /** The population for the level */
    Population *m_population;
    /** Renderer */
    DebugDraw m_debugDraw;
    /** Number of ticks elapsed */
    int m_time;
    /** When and where to reposition the goal */
    std::map<int, b2Vec2> m_goalChanges;
    
    void contactPoint(const b2ContactPoint *point, bool persist);
};

#endif
