/*
 * =====================================================================================
 *
 *       Filename:  Physics.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2011 14:51:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _GAME_PHYSICS_H
#define _GAME_PHYSICS_H

#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletCollision/btBulletCollisionCommon.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <Ogre.h>
#include "MotionState.h"

/*************************************************************************/
/* Define a singleton class to drive our physics simulation for the game.
 * This way it is accessible by any game object as needed.
 *************************************************************************/
class GamePhysics
{
    //Constants
    const btVector3 gravity_; 
    const double worldSize_;
    const btScalar shipMass_;
    
    static boost::shared_ptr<GamePhysics> instance_;

    //Member vars
    btBroadphaseInterface *broadphase_; //for collisions
    btDefaultCollisionConfiguration *collisionConfig_;
    btCollisionDispatcher *dispatcher_;
    btSequentialImpulseConstraintSolver *solver_;
    btDiscreteDynamicsWorld *dynamicsWorld_; //world simulation

    //Ground vars
    btCollisionShape *ground_;
    btDefaultMotionState *groundMotionState_;
    btRigidBody *groundBody_;

    btTransform worldTransform_; //transform defining the world

    btAlignedObjectArray<btCollisionShape *> collisionShapes_; //store collision shapes in this vector for easy access

    //Ensure that nothing else is able to instantiate a new GamePhysics object
    protected:
        GamePhysics();

    public:
        //Generate our publicly accessible pointer
        static GamePhysics *instance();
        ~GamePhysics();

        //Register a rigid dynamics body using a scene node
        void setupRigidDynamicsBody(Ogre::SceneNode *node);

        //Should be called after stepSimulation has been run
        void performCollisionDetection();


        //Accessors
        btTransform &getWorldTransform()
        { return worldTransform_; }

        btDiscreteDynamicsWorld *getDynamicsWorld()
        { return dynamicsWorld_; }
};
#endif
