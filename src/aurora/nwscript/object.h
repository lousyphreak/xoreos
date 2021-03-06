/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file aurora/nwscript/object.h
 *  An NWScript object.
 */

#ifndef AURORA_NWSCRIPT_OBJECT_H
#define AURORA_NWSCRIPT_OBJECT_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/nwscript/variablecontainer.h"

namespace Aurora {

namespace NWScript {

class ObjectContainer;

typedef std::map<uint32, class Object *> ObjectIDMap;
typedef std::multimap<Common::UString, class Object *> ObjectTagMap;

class Object : public VariableContainer {
public:
	Object();
	virtual ~Object();

	uint32 getID() const;

	const Common::UString &getTag() const;

protected:
	uint32 _id;

	Common::UString _tag;

	void removeContainer();

private:
	ObjectContainer *_objectContainer;
	ObjectTagMap::iterator _objectContainerTag;

	friend class ObjectContainer;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_OBJECT_H
