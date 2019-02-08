//
//  AnimPose.cpp
//
//  Created by Anthony J. Thibault on 10/14/15.
//  Copyright (c) 2015 High Fidelity, Inc. All rights reserved.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "AnimPose.h"
#include <GLMHelpers.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "AnimUtil.h"

const AnimPose AnimPose::identity = AnimPose(1.0f, glm::quat(), glm::vec3(0.0f));

AnimPose::AnimPose(const glm::mat4& mat) {
    static const float EPSILON = 0.0001f;
    glm::vec3 scale = extractScale(mat);
    // quat_cast doesn't work so well with scaled matrices, so cancel it out.
    glm::mat4 tmp = glm::scale(mat, 1.0f / scale);
    _scale = extractUniformScale(scale);
    _rot = glm::quat_cast(tmp);
    float lengthSquared = glm::length2(_rot);
    if (glm::abs(lengthSquared - 1.0f) > EPSILON) {
        float oneOverLength = 1.0f / sqrtf(lengthSquared);
        _rot = glm::quat(_rot.w * oneOverLength, _rot.x * oneOverLength, _rot.y * oneOverLength, _rot.z * oneOverLength);
    }
    _trans = extractTranslation(mat);
}

glm::vec3 AnimPose::operator*(const glm::vec3& rhs) const {
    return _trans + (_rot * (_scale * rhs));
}

glm::vec3 AnimPose::xformPoint(const glm::vec3& rhs) const {
    return *this * rhs;
}

glm::vec3 AnimPose::xformVector(const glm::vec3& rhs) const {
    return _rot * (_scale * rhs);
}

AnimPose AnimPose::operator*(const AnimPose& rhs) const {
    float scale = _scale * rhs._scale;
    glm::quat rot = _rot * rhs._rot;
    glm::vec3 trans = _trans + (_rot * (_scale * rhs._trans));
    return AnimPose(scale, rot, trans);
}

AnimPose AnimPose::inverse() const {
    float invScale = 1.0f / _scale;
    glm::quat invRot = glm::inverse(_rot);
    glm::vec3 invTrans = invScale * (invRot * -_trans);
    return AnimPose(invScale, invRot, invTrans);
}

// mirror about x-axis without applying negative scale.
AnimPose AnimPose::mirror() const {
    return AnimPose(_scale, glm::quat(_rot.w, _rot.x, -_rot.y, -_rot.z), glm::vec3(-_trans.x, _trans.y, _trans.z));
}

AnimPose::operator glm::mat4() const {
    glm::vec3 xAxis = _rot * glm::vec3(_scale, 0.0f, 0.0f);
    glm::vec3 yAxis = _rot * glm::vec3(0.0f, _scale, 0.0f);
    glm::vec3 zAxis = _rot * glm::vec3(0.0f, 0.0f, _scale);
    return glm::mat4(glm::vec4(xAxis, 0.0f), glm::vec4(yAxis, 0.0f), glm::vec4(zAxis, 0.0f), glm::vec4(_trans, 1.0f));
}

void AnimPose::blend(const AnimPose& srcPose, float alpha) {
    _scale = lerp(srcPose._scale, _scale, alpha);
    _rot = safeLerp(srcPose._rot, _rot, alpha);
    _trans = lerp(srcPose._trans, _trans, alpha);
}
