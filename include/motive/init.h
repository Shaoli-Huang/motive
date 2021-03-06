// Copyright 2014 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MOTIVE_INIT_H_
#define MOTIVE_INIT_H_

#include "mathfu/constants.h"
#include "motive/math/range.h"
#include "motive/math/vector_converter.h"
#include "motive/util.h"

namespace motive {

class RigAnim;

enum MatrixOperationType {
  kInvalidMatrixOperation,
  kRotateAboutX,
  kRotateAboutY,
  kRotateAboutZ,
  kTranslateX,
  kTranslateY,
  kTranslateZ,
  kScaleX,
  kScaleY,
  kScaleZ,
  kScaleUniformly,
  kNumMatrixOperationTypes
};

/// Returns true if the operation is a rotate.
inline bool RotateOp(MatrixOperationType op) {
  return kRotateAboutX <= op && op <= kRotateAboutZ;
}

/// Returns true if the operation is a translate.
inline bool TranslateOp(MatrixOperationType op) {
  return kTranslateX <= op && op <= kTranslateZ;
}

/// Returns true if the operation is a scale.
inline bool ScaleOp(MatrixOperationType op) {
  return kScaleX <= op && op <= kScaleUniformly;
}

/// Returns the default value of the operation. That is, the value of the
/// operation that does nothing to the transformation. Any operation that
/// constantly returns the default value can be removed.
inline float OperationDefaultValue(MatrixOperationType op) {
  return ScaleOp(op) ? 1.0f : 0.0f;
}

/// Returns the range of the matrix operation's spline. Most ranges are just
/// the extents of the splines, but rotations we want to normalize within
/// +-pi before blending to another curve.
inline Range RangeOfOp(MatrixOperationType op) {
  return RotateOp(op) ? kAngleRange : kInvalidRange;
}

/// Return a string with the operation name. Used for debugging.
const char* MatrixOpName(const MatrixOperationType op);

/// @class SimpleInit
/// @brief Base class of Init classes for MotiveProcessors that derive from
///        SimpleProcessorTemplate.
///
/// You cannot initialize a Motivator with this class because it has no
/// MotivatorType. Instead, use one of the Init classes below that derive from
/// SimpleInit.
struct SimpleInit : public MotivatorInit {
  explicit SimpleInit(MotivatorType type)
      : MotivatorInit(type),
        start_values(nullptr),
        start_derivatives(nullptr) {}

  SimpleInit(MotivatorType type, const float* start_values,
             const float* start_derivatives = nullptr)
      : MotivatorInit(type),
        start_values(start_values),
        start_derivatives(start_derivatives) {}

  /// The starting value of each curve for each dimension. Array of length equal
  /// to the number of dimensions. This points at external values and the caller
  /// is responsible for ensuring these external values live as long as this
  /// struct.
  const float* start_values;

  /// The starting derivative of each curve for each dimension. Array of length
  /// equal to the number of dimensions. This points at external values and the
  /// caller is responsible for ensuring these external values live as long as
  /// this struct.
  const float* start_derivatives;
};

/// @class SimpleInitTemplate
/// @brief A version of SimpleInit for Motivators with kDimensions.
/// Use this class to initialize a Motivator with vector types, instead of using
/// the float arrays required by the base class SimpleInit.
/// For example, use a derivation of SimpleInit3f to initialize a Motivator3f.
template <class BaseT, class VectorConverter, MotiveDimension kDimensionsParam>
struct SimpleInitTemplate : public BaseT {
  static const MotiveDimension kDimensions = kDimensionsParam;

  typedef VectorConverter C;
  typedef typename VectorT<C, kDimensions>::type Vec;

  SimpleInitTemplate()
      : BaseT(C::ToPtr(start_values), C::ToPtr(start_derivatives)),
        start_values(0.0f),
        start_derivatives(0.0f) {}

  SimpleInitTemplate(const Vec& start_values_param,
                     const Vec& start_derivatives_param)
      : BaseT(C::ToPtr(start_values), C::ToPtr(start_derivatives)),
        start_values(start_values_param),
        start_derivatives(start_derivatives_param) {}

  const Vec start_values;
  const Vec start_derivatives;
};

/// @class ConstInit
/// @brief Initialize a MotivatorNf that holds values and velocities that
///        never change.
///
/// All calls to SetTarget functions are ignored.
struct ConstInit : public SimpleInit {
  MOTIVE_INTERFACE();
  ConstInit() : SimpleInit(kType) {}
  explicit ConstInit(const float* start_values,
                     const float* start_derivatives = nullptr)
      : SimpleInit(kType, start_values, start_derivatives) {}
};

/// Use these types to initialize their corresponding MotivatorXfs using vector
/// types instead of float arrays.
typedef SimpleInitTemplate<ConstInit, MathFuVectorConverter, 1>
    ConstInit1f;
typedef SimpleInitTemplate<ConstInit, MathFuVectorConverter, 2>
    ConstInit2f;
typedef SimpleInitTemplate<ConstInit, MathFuVectorConverter, 3>
    ConstInit3f;
typedef SimpleInitTemplate<ConstInit, MathFuVectorConverter, 4>
    ConstInit4f;

/// @class EaseInEaseOutInit
/// @brief Initialize a MotivatorNf move towards target using ease-in
///        ease-out math.
///
/// Call @ref MotivatorNf::SetTargetWithShape to set the target the
/// curve moves towards.
struct EaseInEaseOutInit : public SimpleInit {
  MOTIVE_INTERFACE();
  EaseInEaseOutInit() : SimpleInit(kType) {}
  explicit EaseInEaseOutInit(const float* start_values,
                             const float* start_derivatives = nullptr)
      : SimpleInit(kType, start_values, start_derivatives) {}
};

/// Use these types to initialize their corresponding MotivatorXfs using vector
/// types instead of float arrays.
typedef SimpleInitTemplate<EaseInEaseOutInit, MathFuVectorConverter, 1>
    EaseInEaseOutInit1f;
typedef SimpleInitTemplate<EaseInEaseOutInit, MathFuVectorConverter, 2>
    EaseInEaseOutInit2f;
typedef SimpleInitTemplate<EaseInEaseOutInit, MathFuVectorConverter, 3>
    EaseInEaseOutInit3f;
typedef SimpleInitTemplate<EaseInEaseOutInit, MathFuVectorConverter, 4>
    EaseInEaseOutInit4f;

/// @class SpringInit
/// @brief Initialize a MotivatorNf move oscillate over a target.
///
/// Call @ref MotivatorNf::SetTargetWithShape to set the target the
/// curve moves towards.
struct SpringInit : public SimpleInit {
  MOTIVE_INTERFACE();
  SpringInit() : SimpleInit(kType) {}
  explicit SpringInit(const float* start_values,
                      const float* start_derivatives = nullptr)
      : SimpleInit(kType, start_values, start_derivatives) {}
};

/// Use these types to initialize their corresponding MotivatorXfs using vector
/// types instead of float arrays.
typedef SimpleInitTemplate<SpringInit, MathFuVectorConverter, 1>
    SpringInit1f;
typedef SimpleInitTemplate<SpringInit, MathFuVectorConverter, 2>
    SpringInit2f;
typedef SimpleInitTemplate<SpringInit, MathFuVectorConverter, 3>
    SpringInit3f;
typedef SimpleInitTemplate<SpringInit, MathFuVectorConverter, 4>
    SpringInit4f;

/// @class OvershootInit
/// @brief Initialize a MotivatorNf move towards a target using spring physics.
///
/// Call MotivatorNf::SetTargets() to set the target that we swing towards.
/// The name comes from the movement overshooting the target then coming
/// back, the way a dampened oscillator overshoots its resting point.
class OvershootInit : public MotivatorInit {
 public:
  MOTIVE_INTERFACE();

  OvershootInit()
      : MotivatorInit(kType),
        range_(Range::Full()),
        modular_(false),
        max_velocity_(0.0f),
        accel_per_difference_(0.0f),
        wrong_direction_multiplier_(0.0f),
        max_delta_time_(0) {}

  /// Ensure velocity is within the reasonable limits.
  float ClampVelocity(float velocity) const {
    return mathfu::Clamp(velocity, -max_velocity_, max_velocity_);
  }

  /// Ensure the Motivator's 'value' doesn't increment by more than 'max_delta'.
  /// This is different from ClampVelocity because it is independent of time.
  /// No matter how big the timestep, the delta will not be too great.
  float ClampDelta(float delta) const {
    return mathfu::Clamp(delta, -max_delta_, max_delta_);
  }

  /// Return true if we're close to the target and almost stopped.
  /// The definition of "close to" and "almost stopped" are given by the
  /// "at_target" member.
  bool AtTarget(float dist, float velocity) const {
    return at_target_.Settled(dist, velocity);
  }

  const Range& range() const { return range_; }
  void set_range(const Range& r) { range_ = r; }
  bool modular() const { return modular_; }
  void set_modular(bool modular) { modular_ = modular; }
  float max_velocity() const { return max_velocity_; }
  float max_delta() const { return max_delta_; }
  const Settled1f& at_target() const { return at_target_; }
  Settled1f& at_target() { return at_target_; }
  float accel_per_difference() const { return accel_per_difference_; }
  float wrong_direction_multiplier() const {
    return wrong_direction_multiplier_;
  }
  MotiveTime max_delta_time() const { return max_delta_time_; }

  void set_max_velocity(float max_velocity) { max_velocity_ = max_velocity; }
  void set_max_delta(float max_delta) { max_delta_ = max_delta; }
  void set_at_target(const Settled1f& at_target) { at_target_ = at_target; }
  void set_accel_per_difference(float accel_per_difference) {
    accel_per_difference_ = accel_per_difference;
  }
  void set_wrong_direction_multiplier(float wrong_direction_multiplier) {
    wrong_direction_multiplier_ = wrong_direction_multiplier;
  }
  void set_max_delta_time(MotiveTime max_delta_time) {
    max_delta_time_ = max_delta_time;
  }

 private:
  /// Minimum and maximum values for Motivator::Value().
  /// Clamp (if modular_ is false) or wrap-around (if modular_ is true) when
  /// we reach these boundaries.
  Range range_;

  /// A modular value wraps around from min to max. For example, an angle
  /// is modular, where -pi is equivalent to +pi. Setting this to true ensures
  /// that arithmetic wraps around instead of clamping to min/max.
  bool modular_;

  /// Maximum speed at which the value can change. That is, maximum value for
  /// the Motivator::Velocity(). In units/tick.
  /// For example, if the value is an angle, then this is the max angular
  /// velocity, and the units are radians/tick.
  float max_velocity_;

  /// Maximum that Motivator::Value() can be altered on a single call to
  /// MotiveEngine::AdvanceFrame(), regardless of velocity or delta_time.
  float max_delta_;

  /// Cutoff to determine if the Motivator's current state has settled on the
  /// target. Once it has settled, Value() is set to TargetValue() and
  /// Velocity() is set to zero.
  Settled1f at_target_;

  /// Acceleration is a multiple of abs('state_.position' - 'target_.position').
  /// Bigger differences cause faster acceleration.
  float accel_per_difference_;

  /// When accelerating away from the target, we multiply our acceleration by
  /// this amount. We need counter-acceleration to be stronger so that the
  /// amplitude eventually dies down; otherwise, we'd just have a pendulum.
  float wrong_direction_multiplier_;

  /// The algorithm is iterative. When the iteration step gets too big, the
  /// behavior becomes erratic. This value clamps the iteration step.
  MotiveTime max_delta_time_;
};

/// @class SplineInit
/// @brief Initialize a MotivatorNf to follow a spline.
///
/// Call MotivatorNf::SetSplines() to follow predefined splines,
/// or call MotivatorNf::SetTargets() to dynamically generate a spline that
/// travels through several key points.
class SplineInit : public MotivatorInit {
 public:
  MOTIVE_INTERFACE();

  SplineInit() : MotivatorInit(kType) {}

  /// @param range If using modular arithmetic, the normalized range.
  ///              If not using modular arithmetic, pass in an invalid range
  ///              such as Range().
  explicit SplineInit(const Range& range) : MotivatorInit(kType), range_(range) {}

  const Range& range() const { return range_; }
  void set_range(const Range& r) { range_ = r; }

 private:
  /// If using modular arithmetic, the normalized range.
  /// For example, for angles, the normalized range can be (pi, +pi].
  /// Whenever a new spline segment is started, the internal logic resets
  /// the value to the normalized range. Note, however, that it is possible
  /// for the value to escape the normalized range. That is,
  /// MotivatorNf::Value() may be outside of `range_`, though it will always
  /// be close enough to normalize efficiently with
  /// Range::NormalizeCloseValue().
  ///
  /// If not using modular arithmetic, set to an invalid range and ignored.
  Range range_;
};

/// @class MatrixOperationInit
/// @brief Init params for a basic operation on a matrix.
struct MatrixOperationInit {
  enum UnionType {
    kUnionEmpty,
    kUnionInitialValue,
    kUnionTarget,
    kUnionSpline
  };

  /// Matrix operation never changes. Always use 'const_value'.
  MatrixOperationInit(MatrixOpId id, MatrixOperationType type,
                      float const_value)
      : init(nullptr),
        id(id),
        type(type),
        union_type(kUnionInitialValue),
        initial_value(const_value) {}

  /// Matrix operation is driven by Motivator defined by 'init'.
  MatrixOperationInit(MatrixOpId id, MatrixOperationType type,
                      const MotivatorInit& init)
      : init(&init), id(id), type(type), union_type(kUnionEmpty) {}

  /// Matrix operation is driven by Motivator defined by 'init'. Specify initial
  /// value as well.
  MatrixOperationInit(MatrixOpId id, MatrixOperationType type,
                      const MotivatorInit& init, float initial_value)
      : init(&init),
        id(id),
        type(type),
        union_type(kUnionInitialValue),
        initial_value(initial_value) {}

  MatrixOperationInit(MatrixOpId id, MatrixOperationType type,
                      const MotivatorInit& init, const MotiveTarget1f& target)
      : init(&init),
        id(id),
        type(type),
        union_type(kUnionTarget),
        target(&target) {}

  MatrixOperationInit(MatrixOpId id, MatrixOperationType type,
                      const MotivatorInit& init, const CompactSpline& spline)
      : init(&init),
        id(id),
        type(type),
        union_type(kUnionSpline),
        spline(&spline) {}

  const MotivatorInit* init;
  MatrixOpId id;
  MatrixOperationType type;
  UnionType union_type;
  union {
    float initial_value;
    const MotiveTarget1f* target;
    const CompactSpline* spline;
  };
};

/// @class MatrixInit
/// @brief Initialize a MatrixMotivator4f to generate its matrix from
///        a series of operations.
///
/// Initialize a MatrixMotivator4f with these initialization parameters to
/// create a motivator that generates a 4x4 matrix from a series of basic
/// matrix operations. The basic matrix operations are driven by one dimensional
/// motivators.
///
/// The series of operations can transform an object from the coordinate space
/// in which it was authored, to world (or local) space. For example, if you
/// have a penguin that is authored at (0,0,0) facing up the x-axis, you can
/// move it to it's target position with four operations:
///
///      kScaleUniformly --> to make penguin the correct size
///      kRotateAboutY --> to make penguin face the correct direction
///      kTranslateX } --> to move penguin along to ground to target position
///      kTranslateZ }
class MatrixOpArray {
 public:
  typedef std::vector<MatrixOperationInit> OpVector;

  // Guess at the number of operations we'll have. Better to high-ball a little
  // so that we don't have to reallocate the `ops_` vector.
  static const int kDefaultExpectedNumOps = 8;

  /// By default expect a relatively high number of ops. Cost for allocating
  /// a bit too much temporary memory is small compared to cost of reallocating
  /// that memory.
  explicit MatrixOpArray(int expected_num_ops = kDefaultExpectedNumOps) {
    ops_.reserve(expected_num_ops);
  }

  /// Remove all matrix operations from the sequence.
  void Clear(int expected_num_ops = kDefaultExpectedNumOps) {
    ops_.clear();
    ops_.reserve(expected_num_ops);
  }

  /// Operation is constant. For example, use to put something flat on the
  /// ground, with 'type' = kRotateAboutX and 'const_value' = pi/2.
  void AddOp(MatrixOpId id, MatrixOperationType type, float const_value) {
    ops_.push_back(MatrixOperationInit(id, type, const_value));
  }

  /// Operation is driven by a one dimensional motivator. For example, you can
  /// control the face angle of a standing object with 'type' = kRotateAboutY
  /// and 'init' a curve specified by SplineInit.
  void AddOp(MatrixOpId id, MatrixOperationType type,
             const MotivatorInit& init) {
    ops_.push_back(MatrixOperationInit(id, type, init));
  }

  /// Operation is driven by a one dimensional motivator, and initial value
  /// is specified.
  void AddOp(MatrixOpId id, MatrixOperationType type, const MotivatorInit& init,
             float initial_value) {
    ops_.push_back(MatrixOperationInit(id, type, init, initial_value));
  }

  /// Operation is driven by a one dimensional motivator, which is initialized
  /// to traverse the key points specified in `target`.
  void AddOp(MatrixOpId id, MatrixOperationType type, const MotivatorInit& init,
             const MotiveTarget1f& target) {
    ops_.push_back(MatrixOperationInit(id, type, init, target));
  }

  /// Operation is driven by a one dimensional motivator, which is initialized
  /// to follow the predefined curve specified in `spline`.
  void AddOp(MatrixOpId id, MatrixOperationType type, const MotivatorInit& init,
             const CompactSpline& spline) {
    ops_.push_back(MatrixOperationInit(id, type, init, spline));
  }

  // Maximum duration of any of the splines.
  MotiveTime EndTime() const {
    MotiveTime end_time = 0;
    for (size_t i = 0; i < ops_.size(); ++i) {
      const MatrixOperationInit& op = ops_[i];
      if (op.union_type == MatrixOperationInit::kUnionSpline) {
        end_time =
            std::max(end_time, static_cast<MotiveTime>(op.spline->EndX()));
      }
    }
    return end_time;
  }

  const OpVector& ops() const { return ops_; }

 private:
  OpVector ops_;
};

class MatrixInit : public MotivatorInit {
 public:
  MOTIVE_INTERFACE();
  typedef std::vector<MatrixOperationInit> OpVector;

  explicit MatrixInit(const MatrixOpArray& ops)
      : MotivatorInit(kType), ops_(&ops) {}

  const OpVector& ops() const { return ops_->ops(); }

 private:
  /// Reference to the union of all operations that this matrix will be able
  /// to execute. Later calls to MotivatorMatrix4f::BlendToOps() must provide
  /// operations that are a subset of those in `ops_`.
  /// In `RigAnim`, these represent operations in the defining anim.
  const MatrixOpArray* ops_;
};

class RigInit : public MotivatorInit {
 public:
  MOTIVE_INTERFACE();

  RigInit(const RigAnim& defining_anim, const BoneIndex* bone_parents,
          BoneIndex num_bones);
  const RigAnim& defining_anim() const { return *defining_anim_; }
  const mathfu::AffineTransform* bone_transforms() const {
    return bone_transforms_;
  }

  // Utility functions. Ensure that animations are compatible with rigs.
  static bool MatchesHierarchy(const BoneIndex* parents_a, BoneIndex len_a,
                               const BoneIndex* parents_b, BoneIndex len_b);
  static bool MatchesHierarchy(const RigAnim& anim, const BoneIndex* parents_b,
                               BoneIndex len_b);
  static bool MatchesHierarchy(const RigAnim& anim_a, const RigAnim& anim_b);

 private:
  /// Animation defining hierarchy and the union of matrix ops (across all
  /// animations).
  const RigAnim* defining_anim_;

  /// Array defining default pose. That is, the transformation from a bone to
  /// its parent. With just these, you can reconstruct the model in the pose
  /// it was exported in (i.e. its default pose).
  /// These transforms are used as the `start_transform_`s of the underlying
  /// `MatrixInit`s. All the matrix operations are applied from the origin of
  /// the bone they're animating.
  const mathfu::AffineTransform* bone_transforms_;
};

}  // namespace motive

#endif  // MOTIVE_INIT_H_
