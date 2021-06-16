// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "esp/bindings/bindings.h"
#include "esp/physics/PhysicsObjectBase.h"
#include "esp/physics/RigidBase.h"
#include "esp/physics/RigidObject.h"
#include "esp/physics/RigidStage.h"
#include "esp/physics/bullet/objectWrappers/ManagedBulletArticulatedObject.h"
#include "esp/physics/bullet/objectWrappers/ManagedBulletRigidObject.h"
#include "esp/physics/objectWrappers/ManagedArticulatedObject.h"
#include "esp/physics/objectWrappers/ManagedPhysicsObjectBase.h"
#include "esp/physics/objectWrappers/ManagedRigidBase.h"
#include "esp/physics/objectWrappers/ManagedRigidObject.h"

namespace py = pybind11;
using py::literals::operator""_a;

namespace PhysWraps = esp::physics;
using PhysWraps::ManagedArticulatedObject;
using PhysWraps::ManagedRigidObject;

namespace esp {
namespace physics {

template <class T>
void declareBasePhysicsObjectWrapper(py::module& m,
                                     const std::string& objType,
                                     const std::string& classStrPrefix) {
  using PhysObjWrapper = AbstractManagedPhysicsObject<T>;
  std::string pyclass_name =
      classStrPrefix + std::string("_PhysicsObjectWrapper");
  // ==== AbstractManagedPhysicsObject ====
  py::class_<PhysObjWrapper, std::shared_ptr<PhysObjWrapper>>(
      m, pyclass_name.c_str())
      .def_property_readonly("handle", &PhysObjWrapper::getHandle,
                             ("Name of this " + objType).c_str())
      .def_property(
          "motion_type", &PhysObjWrapper::getMotionType,
          &PhysObjWrapper::setMotionType,
          ("Get or set the MotionType of this " + objType +
           ". Changing MotionType will override any custom collision group.")
              .c_str())
      .def_property_readonly(
          "object_id", &PhysObjWrapper::getID,
          ("System-generated ID for this " + objType +
           " construct.  Will be unique among " + objType + "s.")
              .c_str())
      .def_property_readonly(
          "is_alive", &PhysObjWrapper::isAlive,
          ("Whether this " + objType + " still exists and is still valid.")
              .c_str())
      .def_property_readonly("template_class", &PhysObjWrapper::getClassKey,
                             ("Class name of this " + objType).c_str())
      .def_property(
          "transformation", &PhysObjWrapper::getTransformation,
          &PhysObjWrapper::setTransformation,
          ("Get or set the transformation matrix of this " + objType +
           "'s root SceneNode. If modified, sim state will be updated.")
              .c_str())
      .def_property(
          "translation", &PhysObjWrapper::getTranslation,
          &PhysObjWrapper::setTranslation,
          ("Get or set the translation vector of this " + objType +
           "'s root SceneNode. If modified, sim state will be updated.")
              .c_str())
      .def_property(
          "rotation", &PhysObjWrapper::getRotation,
          &PhysObjWrapper::setRotation,
          ("Get or set the rotation quaternion of this " + objType +
           "'s root SceneNode. If modified, sim state will be updated.")
              .c_str())
      .def_property("rigid_state", &PhysObjWrapper::getRigidState,
                    &PhysObjWrapper::setRigidState,
                    ("Get or set this " + objType +
                     "'s transformation as a Rigid State (i.e. vector, "
                     "quaternion). If modified, sim state will be updated.")
                        .c_str())
      .def_property_readonly("root_scene_node", &PhysObjWrapper::getSceneNode,
                             ("Get a reference to the root SceneNode of this " +
                              objType + "'s  SceneGraph subtree.")
                                 .c_str())

      .def("set_light_setup", &PhysObjWrapper::setLightSetup,
           ("Set this " + objType +
            "'s light setup using passed light_setup_key.")
               .c_str(),
           "light_setup_key"_a)
      .def_property("awake", &PhysObjWrapper::isActive,
                    &PhysObjWrapper::setActive,
                    ("Get or set whether this " + objType +
                     " is actively being simulated, or is sleeping.")
                        .c_str())
      .def("contact_test", &PhysObjWrapper::contactTest,
           ("Discrete collision check for contact between an object and the "
            "collision world."))
      .def("override_collision_group", &PhysObjWrapper::overrideCollisionGroup,
           "group"_a,
           ("Manually set the collision group for an object. Setting a new "
            "MotionType will override this change."))
      .def(
          "translate", &PhysObjWrapper::translate, "vector"_a,
          ("Move this " + objType + " using passed translation vector").c_str())
      .def(
          "rotate",
          [](PhysObjWrapper& self, Mn::Radd angle, Mn::Vector3& normAxis) {
            self.rotate(Mn::Rad(angle), normAxis);
          },
          "angle_in_rad"_a, "norm_axis"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around passed 3-element normalized "
           "norm_axis.")
              .c_str())
      .def(
          "rotate_local",
          [](PhysObjWrapper& self, Mn::Radd angle, Mn::Vector3& normAxis) {
            self.rotateLocal(Mn::Rad(angle), normAxis);
          },
          "angle_in_rad"_a, "norm_axis"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around passed 3-element normalized "
           "norm_axis in the local frame.")
              .c_str())
      .def(
          "rotate_x",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateX(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the x-axis in global frame.")
              .c_str())
      .def(
          "rotate_x_local",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateXLocal(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the x-axis in local frame.")
              .c_str())
      .def(
          "rotate_y",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateY(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the y-axis in global frame.")
              .c_str())
      .def(
          "rotate_y_local",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateYLocal(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the y-axis in local frame.")
              .c_str())
      .def(
          "rotate_z",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateZ(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the z-axis in global frame.")
              .c_str())
      .def(
          "rotate_z_local",
          [](PhysObjWrapper& self, Mn::Radd angle) {
            self.rotateZLocal(Mn::Rad(angle));
          },
          "angle_in_rad"_a,
          ("Rotate this " + objType +
           " by passed angle_in_rad around the z-axis in local frame.")
              .c_str())
      .def_property_readonly(
          "visual_scene_nodes", &PhysObjWrapper::getVisualSceneNodes,
          ("Get a list of references to the SceneNodes with this " + objType +
           "' render assets attached. Use this to manipulate this " + objType +
           "'s visual state. Changes to these nodes will not affect physics "
           "simulation.")
              .c_str())
      .def_property_readonly(
          "user_attributes", &PhysObjWrapper::getUserAttributes,
          ("User-defined " + objType +
           " attributes.  These are not used internally by Habitat in any "
           "capacity, but are available for a user to consume how they wish.")
              .c_str());
}  // declareBasePhysicsObjectWrapper

template <class T>
void declareRigidBaseWrapper(py::module& m,
                             const std::string& objType,
                             const std::string& classStrPrefix) {
  using RigidBaseWrapper = AbstractManagedRigidBase<T>;
  std::string pyclass_name = classStrPrefix + std::string("_RigidBaseWrapper");
  // ==== AbstractManagedRigidBase ====
  py::class_<RigidBaseWrapper, AbstractManagedPhysicsObject<T>,
             std::shared_ptr<RigidBaseWrapper>>(m, pyclass_name.c_str())

      /* --- Geometry & Transformations --- */

      .def_property_readonly("scale", &RigidBaseWrapper::getScale,
                             ("Get the scale of the " + objType).c_str())

      /* --- Physics Properties and Functions --- */
      .def("apply_force", &RigidBaseWrapper::applyForce, "force"_a,
           "relative_position"_a,
           ("Apply an external force to this " + objType +
            " at a specific point relative to the " + objType +
            "'s center of mass in global coordinates. Only applies to "
            "MotionType::DYNAMIC objects.")
               .c_str())
      .def("apply_impulse", &RigidBaseWrapper::applyImpulse, "impulse"_a,
           "relative_position"_a,
           ("Apply an external impulse to this " + objType +
            " at a specific point relative to the " + objType +
            "'s center of mass in global coordinates. Only applies to "
            "MotionType::DYNAMIC objects.")
               .c_str())
      .def("apply_torque", &RigidBaseWrapper::applyTorque, "torque"_a,
           ("Apply torque to this " + objType +
            ". Only applies to MotionType::DYNAMIC objects.")
               .c_str())
      .def("apply_impulse_torque", &RigidBaseWrapper::applyImpulseTorque,
           "impulse"_a,
           ("Apply torque impulse to this " + objType +
            ". Only applies to MotionType::DYNAMIC objects.")
               .c_str())
      .def_property("angular_damping", &RigidBaseWrapper::getAngularDamping,
                    &RigidBaseWrapper::setAngularDamping,
                    ("Get or set this " + objType +
                     "'s scalar angular damping coefficient. Only applies "
                     "to MotionType::DYNAMIC objects.")
                        .c_str())
      .def_property("angular_velocity", &RigidBaseWrapper::getAngularVelocity,
                    &RigidBaseWrapper::setAngularVelocity,
                    ("Get or set this " + objType +
                     "'s scalar angular velocity vector. Only applies to "
                     "MotionType::DYNAMIC objects.")
                        .c_str())

      .def_property(
          "collidable", &RigidBaseWrapper::getCollidable,
          &RigidBaseWrapper::setCollidable,
          ("Get or set whether this " + objType + " has collisions enabled.")
              .c_str())
      .def_property("com", &RigidBaseWrapper::getCOM, &RigidBaseWrapper::setCOM,
                    ("Get or set this " + objType +
                     "'s center of mass (COM) in global coordinate frame.")
                        .c_str())
      .def_property("friction_coefficient",
                    &RigidBaseWrapper::getFrictionCoefficient,
                    &RigidBaseWrapper::setFrictionCoefficient,
                    ("Get or set this " + objType +
                     "'s scalar coefficient of friction. Only applies to "
                     "MotionType::DYNAMIC objects.")
                        .c_str())
      .def_property(
          "intertia_diagonal", &RigidBaseWrapper::getInertiaVector,
          &RigidBaseWrapper::setInertiaVector,
          ("Get or set the inertia matrix's diagonal for this " + objType +
           ". If an object is aligned with its principle axii of inertia, "
           "the 3x3 inertia matrix can be reduced to a diagonal. Only "
           "applies to MotionType::DYNAMIC objects.")
              .c_str())
      .def_property_readonly("inertia_matrix",
                             &RigidBaseWrapper::getInertiaMatrix,
                             ("Get the inertia matrix for this " + objType +
                              ".  To change the values, use the object's "
                              "'intertia_diagonal' property.")
                                 .c_str())
      .def_property("linear_damping", &RigidBaseWrapper::getLinearDamping,
                    &RigidBaseWrapper::setLinearDamping,
                    ("Get or set this " + objType +
                     "'s scalar linear damping coefficient. Only applies to "
                     "MotionType::DYNAMIC objects.")
                        .c_str())
      .def_property("linear_velocity", &RigidBaseWrapper::getLinearVelocity,
                    &RigidBaseWrapper::setLinearVelocity,
                    ("Get or set this " + objType +
                     "'s vector linear velocity. Only applies to "
                     "MotionType::DYNAMIC objects.")
                        .c_str())
      .def_property("mass", &RigidBaseWrapper::getMass,
                    &RigidBaseWrapper::setMass,
                    ("Get or set this " + objType +
                     "'s mass. Only applies to MotionType::DYNAMIC objects.")
                        .c_str())
      .def_property("restitution_coefficient",
                    &RigidBaseWrapper::getRestitutionCoefficient,
                    &RigidBaseWrapper::setRestitutionCoefficient,
                    ("Get or set this " + objType +
                     "'s scalar coefficient of restitution. Only applies to "
                     "MotionType::DYNAMIC objects.")
                        .c_str())

      /* --- Miscellaneous --- */
      .def_property("semantic_id", &RigidBaseWrapper::getSemanticId,
                    &RigidBaseWrapper::setSemanticId,
                    ("Get or set this " + objType + "'s semantic ID.").c_str());

}  // declareRigidBaseWrapper

void declareRigidObjectWrapper(py::module& m,
                               const std::string& objType,
                               const std::string& classStrPrefix) {
  // ==== ManagedRigidObject ====
  py::class_<ManagedRigidObject, AbstractManagedRigidBase<RigidObject>,
             std::shared_ptr<ManagedRigidObject>>(m, classStrPrefix.c_str())
      .def_property_readonly(
          "creation_attributes",
          &ManagedRigidObject::getInitializationAttributes,
          ("Get a copy of the attributes used to create this " + objType + ".")
              .c_str())
      .def_property_readonly(
          "velocity_control", &ManagedRigidObject::getVelocityControl,
          ("Retrieves a reference to the VelocityControl struct for this " +
           objType + ".")
              .c_str());

}  // declareRigidObjectTemplateWrapper

void declareArticulatedObjectWrapper(py::module& m,
                                     const std::string& objType,
                                     const std::string& classStrPrefix) {
  // ==== ManagedArticulatedObject ====
  py::class_<ManagedArticulatedObject,
             AbstractManagedPhysicsObject<ArticulatedObject>,
             std::shared_ptr<ManagedArticulatedObject>>(m,
                                                        classStrPrefix.c_str())
      .def("get_link_scene_node", &ManagedArticulatedObject::getLinkSceneNode,
           ("Get the scene node for this " + objType +
            "'s articulated link specified by the passed "
            "link_id. Use link_id==-1 to get the base link.")
               .c_str(),
           "link_id"_a)
      .def("get_link_visual_nodes",
           &ManagedArticulatedObject::getLinkVisualSceneNodes,
           ("Get a list of the visual scene nodes from this " + objType +
            "'s articulated link specified by the passed "
            "link_id. Use link_id==-1 to get the base link.")
               .c_str(),
           "link_id"_a)
      .def("get_link", &ManagedArticulatedObject::getLink,
           ("Get this " + objType +
            "'s articulated link specified by the passed "
            "link_id. Use link_id==-1 to get the base link.")
               .c_str(),
           "link_id"_a)
      .def(
          "get_link_ids", &ManagedArticulatedObject::getLinkIds,
          ("Get a list of this " + objType + "'s individual link ids.").c_str())
      .def_property_readonly(
          "num_links", &ManagedArticulatedObject::getNumLinks,
          ("Get the number of links this " + objType + " holds.").c_str())
      .def_property(
          "root_linear_velocity",
          &ManagedArticulatedObject::getRootLinearVelocity,
          &ManagedArticulatedObject::setRootLinearVelocity,
          ("The linear velocity of the " + objType + "'s root.").c_str())
      .def_property(
          "root_angular_velocity",
          &ManagedArticulatedObject::getRootAngularVelocity,
          &ManagedArticulatedObject::setRootAngularVelocity,
          ("The angular velocity (omega) of the " + objType + "'s root.")
              .c_str())
      .def_property("joint_forces", &ManagedArticulatedObject::getJointForces,
                    &ManagedArticulatedObject::setJointForces,
                    ("Get or set the joint forces/torques (indexed by DoF id) "
                     "currently acting on this " +
                     objType + ".")
                        .c_str())
      .def("add_joint_forces", &ManagedArticulatedObject::addJointForces,
           ("Add joint forces/torques (indexed by DoF id) to this " + objType +
            ".")
               .c_str(),
           "forces"_a)
      .def_property("joint_velocities",
                    &ManagedArticulatedObject::getJointVelocities,
                    &ManagedArticulatedObject::setJointVelocities,
                    ("Get or set this " + objType +
                     "'s joint velocities, indexed by DOF id.")
                        .c_str())
      .def_property("joint_positions",
                    &ManagedArticulatedObject::getJointPositions,
                    &ManagedArticulatedObject::setJointPositions,
                    ("Get or set this " + objType +
                     "'s joint positions. For link to index mapping see "
                     "get_link_joint_pos_offset and get_link_num_joint_pos.")
                        .c_str())
      .def("get_joint_position_limits",
           &ManagedArticulatedObject::getJointPositionLimits,
           ("Get a list of this " + objType +
            "'s joint limits, either upper limits or lower limits, depending "
            "on the supplied boolean value for upper_limits.")
               .c_str(),
           "upper_limits"_a)
      .def("get_link_dof_offset", &ManagedArticulatedObject::getLinkDoFOffset,
           ("Get the index of this " + objType +
            "'s link's first DoF in the global DoF array. Link specified by "
            "the given link_id.")
               .c_str(),
           "link_id"_a)
      .def("get_link_num_dofs", &ManagedArticulatedObject::getLinkNumDoFs,
           ("Get the number of DoFs for the parent joint of this " + objType +
            "'s link specified by the given link_id.")
               .c_str(),
           "link_id"_a)
      .def("get_link_joint_pos_offset",
           &ManagedArticulatedObject::getLinkJointPosOffset,
           ("Get the index of this " + objType +
            "'s link's first position in the global joint positions array. "
            "Link specified by the given link_id.")
               .c_str(),
           "link_id"_a)
      .def("get_link_num_joint_pos",
           &ManagedArticulatedObject::getLinkNumJointPos,
           ("Get the number of position variables for the parent joint of "
            "this " +
            objType + "'s link specified by the given link_id.")
               .c_str(),
           "link_id"_a)
      .def("get_link_joint_type", &ManagedArticulatedObject::getLinkJointType,
           ("Get the type of the parent joint for this " + objType +
            "'s link specified by the given link_id.")
               .c_str(),
           "link_id"_a)
      .def("add_link_force", &ManagedArticulatedObject::addArticulatedLinkForce,
           ("Apply the given force to this " + objType +
            "'s link specified by the given link_id")
               .c_str(),
           "link_id"_a, "force"_a)
      .def("get_link_friction",
           &ManagedArticulatedObject::getArticulatedLinkFriction,
           ("Get the link friction from this " + objType +
            "'s link specified by the provided link_id")
               .c_str(),
           "link_id"_a)
      .def("set_link_friction",
           &ManagedArticulatedObject::setArticulatedLinkFriction,
           ("Set the link friction for this " + objType +
            "'s link specified by the provided link_id to the provided "
            "friction value.")
               .c_str(),
           "link_id"_a, "friction"_a)
      .def("clear_joint_states", &ManagedArticulatedObject::reset,
           ("Clear this " + objType +
            "'s joint state by zeroing forces, torques, positions and "
            "velocities. Does not change root state.")
               .c_str())
      .def_property_readonly(
          "can_sleep", &ManagedArticulatedObject::getCanSleep,
          ("Whether or not this " + objType + " can be put to sleep").c_str())
      .def_property(
          "auto_clamp_joint_limits",
          &ManagedArticulatedObject::getAutoClampJointLimits,
          &ManagedArticulatedObject::setAutoClampJointLimits,
          ("Get or set whether this " + objType +
           "'s joints should be autoclamped to specified joint limits.")
              .c_str())
      .def("clamp_joint_limits", &ManagedArticulatedObject::clampJointLimits,
           ("Clamp this " + objType +
            "'s current pose to specified joint limits.")
               .c_str());
}  // declareArticulatedObjectWrapper

template <class T>
void declareBaseObjectWrappers(py::module& m,
                               const std::string& objType,
                               const std::string& classStrPrefix) {
  declareBasePhysicsObjectWrapper<T>(m, objType, classStrPrefix);
  declareRigidBaseWrapper<T>(m, objType, classStrPrefix);
}

void initPhysicsObjectBindings(py::module& m) {
  // create Rigid Object base wrapper bindings
  declareBaseObjectWrappers<RigidObject>(m, "Rigid Object",
                                         "ManagedRigidObject");

  // ==== ManagedRigidObject ====
  declareRigidObjectWrapper(m, "Rigid Object", "ManagedRigidObject");

  // ==== ManagedBulletRigidObject ====
  py::class_<ManagedBulletRigidObject, ManagedRigidObject,
             std::shared_ptr<ManagedBulletRigidObject>>(
      m, "ManagedBulletRigidObject")
      .def_property(
          "margin", &ManagedBulletRigidObject::getMargin,
          &ManagedBulletRigidObject::setMargin,
          R"(REQUIRES BULLET TO BE INSTALLED. Get or set this object's collision margin.)")
      .def_property_readonly(
          "collision_shape_aabb",
          &ManagedBulletRigidObject::getCollisionShapeAabb,
          R"(REQUIRES BULLET TO BE INSTALLED. The bounds of the axis-aligned bounding box from Bullet Physics, in its local coordinate frame.)");

  // create bindings for ArticulatedObjects
  // physics object base instance for articulated object
  declareBasePhysicsObjectWrapper<ArticulatedObject>(m, "Articulated Object",
                                                     "ArticulatedObject");

  // ==== ManagedArticulatedObject ====
  declareArticulatedObjectWrapper(m, "Articulated Object",
                                  "ManagedArticulatedObject");

  // ==== ManagedBulletArticulatedObject ====
  py::class_<ManagedBulletArticulatedObject, ManagedArticulatedObject,
             std::shared_ptr<ManagedBulletArticulatedObject>>(
      m, "ManagedBulletArticulatedObject")
      .def(
          "contact_test", &ManagedBulletArticulatedObject::contactTest,
          R"(REQUIRES BULLET TO BE INSTALLED. Returns the result of a discrete collision test between this object and the world.)");

}  // initPhysicsObjectBindings

}  // namespace physics
}  // namespace esp