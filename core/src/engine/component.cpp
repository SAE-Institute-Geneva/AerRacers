/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include <engine/component.h>

namespace neko
{
std::string ComponentTypeToString(ComponentType type)
{
	switch (type)
	{
		case ComponentType::EMPTY: return "Empty";

			// Transform 2D
		case ComponentType::POSITION2D:  return "Position 2D";
		case ComponentType::SCALE2D:     return "Scale 2D";
		case ComponentType::ROTATION2D:  return "Rotation 2D";
		case ComponentType::TRANSFORM2D: return "Transform 2D";

			// Transform 3D
		case ComponentType::POSITION3D:  return "Position";
		case ComponentType::SCALE3D:     return "Scale";
		case ComponentType::ROTATION3D:  return "Rotation";
		case ComponentType::TRANSFORM3D: return "Transform";

			// Graphics
		case ComponentType::SPRITE2D: return "Sprite";
		case ComponentType::MODEL:    return "Model";
		case ComponentType::LIGHT:    return "Light";

			// Physics
		case ComponentType::BODY2D:             return "Body 2D";
		case ComponentType::BOX_COLLIDER2D:     return "Box Collider 2D";
		case ComponentType::CIRCLE_COLLIDER2D:  return "Circle Collider 2D";
		case ComponentType::POLYGON_COLLIDER2D: return "Polygon Collider 2D";
		case ComponentType::CONVEX_SHAPE2D:     return "Convex Shape 2D";
		case ComponentType::RIGID_STATIC:       return "Rigid Body Static";
		case ComponentType::RIGID_DYNAMIC:      return "Rigid Body Dynamic";

			// Miscellaneous
		case ComponentType::PREFAB: return "Prefab";
		case ComponentType::AUDIO_SOURCE: return "Audio Source";
		case ComponentType::OTHER_TYPE: return "Other";
	}
}
}