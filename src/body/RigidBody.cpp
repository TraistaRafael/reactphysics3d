/********************************************************************************
* ReactPhysics3D physics library, http://www.reactphysics3d.com                 *
* Copyright (c) 2010-2018 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

// Libraries
#include "RigidBody.h"
#include "constraint/Joint.h"
#include "collision/shapes/CollisionShape.h"
#include "engine/DynamicsWorld.h"
#include "utils/Profiler.h"

// We want to use the ReactPhysics3D namespace
using namespace reactphysics3d;

// Constructor
/**
* @param transform The transformation of the body
* @param world The world where the body has been added
* @param id The ID of the body
*/
RigidBody::RigidBody(const Transform& transform, CollisionWorld& world, Entity entity, bodyindex id)
          : CollisionBody(world, entity, id), mArrayIndex(0), mInitMass(decimal(1.0)),
            mCenterOfMassLocal(0, 0, 0), mCenterOfMassWorld(transform.getPosition()),
            mIsGravityEnabled(true), mMaterial(world.mConfig), mLinearDamping(decimal(0.0)), mAngularDamping(decimal(0.0)),
            mJointsList(nullptr), mIsCenterOfMassSetByUser(false), mIsInertiaTensorSetByUser(false) {

    // Compute the inverse mass
    mMassInverse = decimal(1.0) / mInitMass;

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();
}

// Destructor
RigidBody::~RigidBody() {
    assert(mJointsList == nullptr);
}

// Set the type of the body
/// The type of the body can either STATIC, KINEMATIC or DYNAMIC as described bellow:
/// STATIC : A static body has infinite mass, zero velocity but the position can be
///          changed manually. A static body does not collide with other static or kinematic bodies.
/// KINEMATIC : A kinematic body has infinite mass, the velocity can be changed manually and its
///             position is computed by the physics engine. A kinematic body does not collide with
///             other static or kinematic bodies.
/// DYNAMIC : A dynamic body has non-zero mass, non-zero velocity determined by forces and its
///           position is determined by the physics engine. A dynamic body can collide with other
///           dynamic, static or kinematic bodies.
/**
 * @param type The type of the body (STATIC, KINEMATIC, DYNAMIC)
 */
void RigidBody::setType(BodyType type) {

    if (mType == type) return;

    CollisionBody::setType(type);

    // Recompute the total mass, center of mass and inertia tensor
    recomputeMassInformation();

    // If it is a static body
    if (mType == BodyType::STATIC) {

        // Reset the velocity to zero
        mWorld.mDynamicsComponents.setLinearVelocity(mEntity, Vector3::zero());
        mWorld.mDynamicsComponents.setAngularVelocity(mEntity, Vector3::zero());
    }

    // If it is a static or a kinematic body
    if (mType == BodyType::STATIC || mType == BodyType::KINEMATIC) {

        // Reset the inverse mass and inverse inertia tensor to zero
        mMassInverse = decimal(0.0);
        mInertiaTensorLocalInverse.setToZero();
        mInertiaTensorInverseWorld.setToZero();
    }
    else {  // If it is a dynamic body
        mMassInverse = decimal(1.0) / mInitMass;

        if (mIsInertiaTensorSetByUser) {
            mInertiaTensorLocalInverse = mUserInertiaTensorLocalInverse;
        }
    }

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();

    // Awake the body
    setIsSleeping(false);

    // Remove all the contacts with this body
    resetContactManifoldsList();

    // Ask the broad-phase to test again the collision shapes of the body for collision
    // detection (as if the body has moved)
    askForBroadPhaseCollisionCheck();

    // Reset the force and torque on the body
    mExternalForce.setToZero();
    mExternalTorque.setToZero();
}

// Set the local inertia tensor of the body (in local-space coordinates)
/// If the inertia tensor is set with this method, it will not be computed
/// using the collision shapes of the body.
/**
 * @param inertiaTensorLocal The 3x3 inertia tensor matrix of the body in local-space
 *                           coordinates
 */
void RigidBody::setInertiaTensorLocal(const Matrix3x3& inertiaTensorLocal) {

    mUserInertiaTensorLocalInverse = inertiaTensorLocal.getInverse();
    mIsInertiaTensorSetByUser = true;

    if (mType != BodyType::DYNAMIC) return;

    // Compute the inverse local inertia tensor
    mInertiaTensorLocalInverse = mUserInertiaTensorLocalInverse;

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set inertiaTensorLocal=" + inertiaTensorLocal.to_string());
}

// Set the inverse local inertia tensor of the body (in local-space coordinates)
/// If the inverse inertia tensor is set with this method, it will not be computed
/// using the collision shapes of the body.
/**
 * @param inverseInertiaTensorLocal The 3x3 inverse inertia tensor matrix of the body in local-space
 *                           		coordinates
 */
void RigidBody::setInverseInertiaTensorLocal(const Matrix3x3& inverseInertiaTensorLocal) {

    mUserInertiaTensorLocalInverse = inverseInertiaTensorLocal;
    mIsInertiaTensorSetByUser = true;

    if (mType != BodyType::DYNAMIC) return;

    // Compute the inverse local inertia tensor
    mInertiaTensorLocalInverse = mUserInertiaTensorLocalInverse;

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set inverseInertiaTensorLocal=" + inverseInertiaTensorLocal.to_string());
}

// Set the local center of mass of the body (in local-space coordinates)
/// If you set the center of mass with the method, it will not be computed
/// automatically using collision shapes.
/**
 * @param centerOfMassLocal The center of mass of the body in local-space
 *                          coordinates
 */
void RigidBody::setCenterOfMassLocal(const Vector3& centerOfMassLocal) {

    if (mType != BodyType::DYNAMIC) return;

    mIsCenterOfMassSetByUser = true;

    const Vector3 oldCenterOfMass = mCenterOfMassWorld;
    mCenterOfMassLocal = centerOfMassLocal;

    // Compute the center of mass in world-space coordinates
    mCenterOfMassWorld = mWorld.mTransformComponents.getTransform(mEntity) * mCenterOfMassLocal;

    // Update the linear velocity of the center of mass
    Vector3 linearVelocity = mWorld.mDynamicsComponents.getAngularVelocity(mEntity);
    const Vector3& angularVelocity = mWorld.mDynamicsComponents.getAngularVelocity(mEntity);
    linearVelocity += angularVelocity.cross(mCenterOfMassWorld - oldCenterOfMass);
    mWorld.mDynamicsComponents.setLinearVelocity(mEntity, linearVelocity);

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set centerOfMassLocal=" + centerOfMassLocal.to_string());
}

// Set the mass of the rigid body
/**
 * @param mass The mass (in kilograms) of the body
 */
void RigidBody::setMass(decimal mass) {

    if (mType != BodyType::DYNAMIC) return;

    mInitMass = mass;

    if (mInitMass > decimal(0.0)) {
        mMassInverse = decimal(1.0) / mInitMass;
    }
    else {
        mInitMass = decimal(1.0);
        mMassInverse = decimal(1.0);
    }

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set mass=" + std::to_string(mass));
}

// Remove a joint from the joints list
void RigidBody::removeJointFromJointsList(MemoryManager& memoryManager, const Joint* joint) {

    assert(joint != nullptr);
    assert(mJointsList != nullptr);

    // Remove the joint from the linked list of the joints of the first body
    if (mJointsList->joint == joint) {   // If the first element is the one to remove
        JointListElement* elementToRemove = mJointsList;
        mJointsList = elementToRemove->next;
        elementToRemove->~JointListElement();
        memoryManager.release(MemoryManager::AllocationType::Pool,
                              elementToRemove, sizeof(JointListElement));
    }
    else {  // If the element to remove is not the first one in the list
        JointListElement* currentElement = mJointsList;
        while (currentElement->next != nullptr) {
            if (currentElement->next->joint == joint) {
                JointListElement* elementToRemove = currentElement->next;
                currentElement->next = elementToRemove->next;
                elementToRemove->~JointListElement();
                memoryManager.release(MemoryManager::AllocationType::Pool,
                                      elementToRemove, sizeof(JointListElement));
                break;
            }
            currentElement = currentElement->next;
        }
    }
}

// Update the world inverse inertia tensor of the body
/// The inertia tensor I_w in world coordinates is computed with the
/// local inverse inertia tensor I_b^-1 in body coordinates
/// by I_w = R * I_b^-1 * R^T
/// where R is the rotation matrix (and R^T its transpose) of the
/// current orientation quaternion of the body
void RigidBody::updateInertiaTensorInverseWorld() {

    // TODO : Make sure we do this in a system

    Matrix3x3 orientation = mWorld.mTransformComponents.getTransform(mEntity).getOrientation().getMatrix();
    mInertiaTensorInverseWorld = orientation * mInertiaTensorLocalInverse * orientation.getTranspose();
}

// Add a collision shape to the body.
/// When you add a collision shape to the body, an internal copy of this
/// collision shape will be created internally. Therefore, you can delete it
/// right after calling this method or use it later to add it to another body.
/// This method will return a pointer to a new proxy shape. A proxy shape is
/// an object that links a collision shape and a given body. You can use the
/// returned proxy shape to get and set information about the corresponding
/// collision shape for that body.
/**
 * @param collisionShape The collision shape you want to add to the body
 * @param transform The transformation of the collision shape that transforms the
 *        local-space of the collision shape into the local-space of the body
 * @param mass Mass (in kilograms) of the collision shape you want to add
 * @return A pointer to the proxy shape that has been created to link the body to
 *         the new collision shape you have added.
 */
ProxyShape* RigidBody::addCollisionShape(CollisionShape* collisionShape,
                                         const Transform& transform,
                                         decimal mass) {

    // Create a new entity for the proxy-shape
    Entity proxyShapeEntity = mWorld.mEntityManager.createEntity();

    // Create a new proxy collision shape to attach the collision shape to the body
    ProxyShape* proxyShape = new (mWorld.mMemoryManager.allocate(MemoryManager::AllocationType::Pool,
                                      sizeof(ProxyShape))) ProxyShape(proxyShapeEntity, this, mWorld.mMemoryManager);

    // Add the proxy-shape component to the entity of the body
    Vector3 localBoundsMin;
    Vector3 localBoundsMax;
    // TODO : Maybe this method can directly returns an AABB
    collisionShape->getLocalBounds(localBoundsMin, localBoundsMax);

    ProxyShapeComponents::ProxyShapeComponent proxyShapeComponent(mEntity, proxyShape, -1,
                                                                   AABB(localBoundsMin, localBoundsMax),
                                                                   transform, collisionShape, mass, 0x0001, 0xFFFF);
    mWorld.mProxyShapesComponents.addComponent(proxyShapeEntity, mIsSleeping, proxyShapeComponent);

    mWorld.mBodyComponents.addProxyShapeToBody(mEntity, proxyShapeEntity);

#ifdef IS_PROFILING_ACTIVE

	// Set the profiler
	proxyShape->setProfiler(mProfiler);

#endif

#ifdef IS_LOGGING_ACTIVE

    // Set the logger
    proxyShape->setLogger(mLogger);

#endif

    // Compute the world-space AABB of the new collision shape
    AABB aabb;
    collisionShape->computeAABB(aabb, mWorld.mTransformComponents.getTransform(mEntity) * transform);

    // Notify the collision detection about this new collision shape
    mWorld.mCollisionDetection.addProxyCollisionShape(proxyShape, aabb);

    // Recompute the center of mass, total mass and inertia tensor of the body with the new
    // collision shape
    recomputeMassInformation();

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Proxy shape " + std::to_string(proxyShape->getBroadPhaseId()) + " added to body");

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::ProxyShape,
             "ProxyShape " + std::to_string(proxyShape->getBroadPhaseId()) + ":  collisionShape=" +
             proxyShape->getCollisionShape()->to_string());

    // Return a pointer to the proxy collision shape
    return proxyShape;
}

// Remove a collision shape from the body
/// To remove a collision shape, you need to specify the pointer to the proxy
/// shape that has been returned when you have added the collision shape to the
/// body
/**
 * @param proxyShape The pointer of the proxy shape you want to remove
 */
void RigidBody::removeCollisionShape(ProxyShape* proxyShape) {

    // Remove the collision shape
    CollisionBody::removeCollisionShape(proxyShape);

    // Recompute the total mass, center of mass and inertia tensor
    recomputeMassInformation();
}

// Set the variable to know if the gravity is applied to this rigid body
/**
 * @param isEnabled True if you want the gravity to be applied to this body
 */
void RigidBody::enableGravity(bool isEnabled) {
    mIsGravityEnabled = isEnabled;

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set isGravityEnabled=" +
             (mIsGravityEnabled ? "true" : "false"));
}

// Set the linear damping factor. This is the ratio of the linear velocity
// that the body will lose every at seconds of simulation.
/**
 * @param linearDamping The linear damping factor of this body
 */
void RigidBody::setLinearDamping(decimal linearDamping) {
    assert(linearDamping >= decimal(0.0));
    mLinearDamping = linearDamping;

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set linearDamping=" + std::to_string(mLinearDamping));
}

// Set the angular damping factor. This is the ratio of the angular velocity
// that the body will lose at every seconds of simulation.
/**
 * @param angularDamping The angular damping factor of this body
 */
void RigidBody::setAngularDamping(decimal angularDamping) {
    assert(angularDamping >= decimal(0.0));
    mAngularDamping = angularDamping;

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set angularDamping=" + std::to_string(mAngularDamping));
}

/// Update the transform of the body after a change of the center of mass
void RigidBody::updateTransformWithCenterOfMass() {

    // TODO : Make sure we compute this in a system

    // Translate the body according to the translation of the center of mass position
    Transform& transform = mWorld.mTransformComponents.getTransform(mEntity);
    transform.setPosition(mCenterOfMassWorld - transform.getOrientation() * mCenterOfMassLocal);
}

// Set a new material for this rigid body
/**
 * @param material The material you want to set to the body
 */
void RigidBody::setMaterial(const Material& material) {
    mMaterial = material;

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set Material" + mMaterial.to_string());
}

// Set the linear velocity of the rigid body.
/**
 * @param linearVelocity Linear velocity vector of the body
 */
void RigidBody::setLinearVelocity(const Vector3& linearVelocity) {

    // If it is a static body, we do nothing
    if (mType == BodyType::STATIC) return;

    // Update the linear velocity of the current body state
    mWorld.mDynamicsComponents.setLinearVelocity(mEntity, linearVelocity);

    // If the linear velocity is not zero, awake the body
    if (linearVelocity.lengthSquare() > decimal(0.0)) {
        setIsSleeping(false);
    }

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set linearVelocity=" + linearVelocity.to_string());
}

// Set the angular velocity.
/**
* @param angularVelocity The angular velocity vector of the body
*/
void RigidBody::setAngularVelocity(const Vector3& angularVelocity) {

    // TODO : Make sure this method is not called from the internal physics engine

    // If it is a static body, we do nothing
    if (mType == BodyType::STATIC) return;

    // Set the angular velocity
    mWorld.mDynamicsComponents.setAngularVelocity(mEntity, angularVelocity);

    // If the velocity is not zero, awake the body
    if (angularVelocity.lengthSquare() > decimal(0.0)) {
        setIsSleeping(false);
    }

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set angularVelocity=" + angularVelocity.to_string());
}

// Set the current position and orientation
/**
 * @param transform The transformation of the body that transforms the local-space
 *                  of the body into world-space
 */
void RigidBody::setTransform(const Transform& transform) {

    // Update the transform of the body
    mWorld.mTransformComponents.setTransform(mEntity, transform);

    const Vector3 oldCenterOfMass = mCenterOfMassWorld;

    // Compute the new center of mass in world-space coordinates
    mCenterOfMassWorld = transform * mCenterOfMassLocal;

    // Update the linear velocity of the center of mass
    Vector3 linearVelocity = mWorld.mDynamicsComponents.getLinearVelocity(mEntity);
    const Vector3& angularVelocity = mWorld.mDynamicsComponents.getAngularVelocity(mEntity);
    linearVelocity += angularVelocity.cross(mCenterOfMassWorld - oldCenterOfMass);
    mWorld.mDynamicsComponents.setLinearVelocity(mEntity, linearVelocity);

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();

    // Update the broad-phase state of the body
    updateBroadPhaseState();

    RP3D_LOG(mLogger, Logger::Level::Information, Logger::Category::Body,
             "Body " + std::to_string(mID) + ": Set transform=" + transform.to_string());
}

// Recompute the center of mass, total mass and inertia tensor of the body using all
// the collision shapes attached to the body.
void RigidBody::recomputeMassInformation() {

    mInitMass = decimal(0.0);
    mMassInverse = decimal(0.0);
    if (!mIsInertiaTensorSetByUser) mInertiaTensorLocalInverse.setToZero();
    if (!mIsInertiaTensorSetByUser) mInertiaTensorInverseWorld.setToZero();
    if (!mIsCenterOfMassSetByUser) mCenterOfMassLocal.setToZero();
    Matrix3x3 inertiaTensorLocal;
    inertiaTensorLocal.setToZero();

    const Transform& transform = mWorld.mTransformComponents.getTransform(mEntity);

    // If it is a STATIC or a KINEMATIC body
    if (mType == BodyType::STATIC || mType == BodyType::KINEMATIC) {
        mCenterOfMassWorld = transform.getPosition();
        return;
    }

    assert(mType == BodyType::DYNAMIC);

    // Compute the total mass of the body
    const List<Entity>& proxyShapesEntities = mWorld.mBodyComponents.getProxyShapes(mEntity);
    for (uint i=0; i < proxyShapesEntities.size(); i++) {
        ProxyShape* proxyShape = mWorld.mProxyShapesComponents.getProxyShape(proxyShapesEntities[i]);
        mInitMass += proxyShape->getMass();

        if (!mIsCenterOfMassSetByUser) {
            mCenterOfMassLocal += proxyShape->getLocalToBodyTransform().getPosition() * proxyShape->getMass();
        }
    }

    if (mInitMass > decimal(0.0)) {
        mMassInverse = decimal(1.0) / mInitMass;
    }
    else {
        mCenterOfMassWorld = transform.getPosition();
        return;
    }

    // Compute the center of mass
    const Vector3 oldCenterOfMass = mCenterOfMassWorld;

    if (!mIsCenterOfMassSetByUser) {
        mCenterOfMassLocal *= mMassInverse;
    }

    mCenterOfMassWorld = transform * mCenterOfMassLocal;

    if (!mIsInertiaTensorSetByUser) {

        // Compute the inertia tensor using all the collision shapes
        const List<Entity>& proxyShapesEntities = mWorld.mBodyComponents.getProxyShapes(mEntity);
        for (uint i=0; i < proxyShapesEntities.size(); i++) {

            ProxyShape* proxyShape = mWorld.mProxyShapesComponents.getProxyShape(proxyShapesEntities[i]);

            // Get the inertia tensor of the collision shape in its local-space
            Matrix3x3 inertiaTensor;
            proxyShape->getCollisionShape()->computeLocalInertiaTensor(inertiaTensor, proxyShape->getMass());

            // Convert the collision shape inertia tensor into the local-space of the body
            const Transform& shapeTransform = proxyShape->getLocalToBodyTransform();
            Matrix3x3 rotationMatrix = shapeTransform.getOrientation().getMatrix();
            inertiaTensor = rotationMatrix * inertiaTensor * rotationMatrix.getTranspose();

            // Use the parallel axis theorem to convert the inertia tensor w.r.t the collision shape
            // center into a inertia tensor w.r.t to the body origin.
            Vector3 offset = shapeTransform.getPosition() - mCenterOfMassLocal;
            decimal offsetSquare = offset.lengthSquare();
            Matrix3x3 offsetMatrix;
            offsetMatrix[0].setAllValues(offsetSquare, decimal(0.0), decimal(0.0));
            offsetMatrix[1].setAllValues(decimal(0.0), offsetSquare, decimal(0.0));
            offsetMatrix[2].setAllValues(decimal(0.0), decimal(0.0), offsetSquare);
            offsetMatrix[0] += offset * (-offset.x);
            offsetMatrix[1] += offset * (-offset.y);
            offsetMatrix[2] += offset * (-offset.z);
            offsetMatrix *= proxyShape->getMass();

            inertiaTensorLocal += inertiaTensor + offsetMatrix;
        }

        // Compute the local inverse inertia tensor
        mInertiaTensorLocalInverse = inertiaTensorLocal.getInverse();
    }

    // Update the world inverse inertia tensor
    updateInertiaTensorInverseWorld();

    // Update the linear velocity of the center of mass
    Vector3 linearVelocity = mWorld.mDynamicsComponents.getLinearVelocity(mEntity);
    Vector3 angularVelocity = mWorld.mDynamicsComponents.getAngularVelocity(mEntity);
    linearVelocity += angularVelocity.cross(mCenterOfMassWorld - oldCenterOfMass);
    mWorld.mDynamicsComponents.setLinearVelocity(mEntity, linearVelocity);
}

// Return the linear velocity
/**
 * @return The linear velocity vector of the body
 */
Vector3 RigidBody::getLinearVelocity() const {
    return mWorld.mDynamicsComponents.getLinearVelocity(mEntity);
}

// Return the angular velocity of the body
/**
 * @return The angular velocity vector of the body
 */
Vector3 RigidBody::getAngularVelocity() const {
    return mWorld.mDynamicsComponents.getAngularVelocity(mEntity);
}

// Set the variable to know whether or not the body is sleeping
void RigidBody::setIsSleeping(bool isSleeping) {

    if (isSleeping) {
        mWorld.mDynamicsComponents.setLinearVelocity(mEntity, Vector3::zero());
        mWorld.mDynamicsComponents.setAngularVelocity(mEntity, Vector3::zero());
        mExternalForce.setToZero();
        mExternalTorque.setToZero();
    }

    CollisionBody::setIsSleeping(isSleeping);
}

#ifdef IS_PROFILING_ACTIVE

// Set the profiler
void RigidBody::setProfiler(Profiler* profiler) {

	CollisionBody::setProfiler(profiler);

	// Set the profiler for each proxy shape
    const List<Entity>& proxyShapesEntities = mWorld.mBodyComponents.getProxyShapes(mEntity);
    for (uint i=0; i < proxyShapesEntities.size(); i++) {

        ProxyShape* proxyShape = mWorld.mProxyShapesComponents.getProxyShape(proxyShapesEntities[i]);

		proxyShape->setProfiler(profiler);
	}
}

#endif
