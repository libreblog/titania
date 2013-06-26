/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstraße 31a, Leipzig, Germany 2011.
 *
 * All rights reserved. Holger Seelig <holger.seelig@yahoo.de>.
 *
 * THIS IS UNPUBLISHED SOURCE CODE OF create3000.
 *
 * The copyright notice above does not evidence any actual of intended
 * publication of such source code, and is an unpublished work by create3000.
 * This material contains CONFIDENTIAL INFORMATION that is the property of
 * create3000.
 *
 * No permission is granted to copy, distribute, or create derivative works from
 * the contents of this software, in whole or in part, without the prior written
 * permission of create3000.
 *
 * NON-MILITARY USE ONLY
 *
 * All create3000 software are effectively free software with a non-military use
 * restriction. It is free. Well commented source is provided. You may reuse the
 * source in any way you please with the exception anything that uses it must be
 * marked to indicate is contains 'non-military use only' components.
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
 *
 * This file is part of the Titania Project.
 *
 * Titania is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3 only, as published by the
 * Free Software Foundation.
 *
 * Titania is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License version 3 for more
 * details (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version 3
 * along with Titania.  If not, see <http://www.gnu.org/licenses/gpl.html> for a
 * copy of the GPLv3 License.
 *
 * For Silvio, Joy and Adi.
 *
 ******************************************************************************/

#ifndef __TITANIA_X3D_COMPONENTS_LAYERING_X3DLAYER_NODE_H__
#define __TITANIA_X3D_COMPONENTS_LAYERING_X3DLAYER_NODE_H__

#include "../../Execution/BindableNodeList.h"
#include "../../Execution/BindableNodeStack.h"
#include "../../Rendering/CollisionArray.h"
#include "../../Rendering/LightContainerArray.h"
#include "../../Rendering/X3DRenderer.h"
#include "../EnvironmentalEffects/LocalFog.h"
#include "../Grouping/Group.h"
#include "../Grouping/X3DGroupingNode.h"
#include "../Layering/Viewport.h"

#include <stack>

namespace titania {
namespace X3D {

typedef std::deque <X3D::X3DViewpointNode*> UserViewpointList;
typedef std::stack <LocalFog*> LocalFogStack;

class X3DLayerNode :
	virtual public X3DNode, public X3DRenderer
{
public:

	///  @name Fields

	SFBool &
	isPickable ()
	{ return *fields .isPickable; }

	const SFBool &
	isPickable () const
	{ return *fields .isPickable; }

	SFNode <X3DBaseNode> &
	viewport ()
	{ return *fields .viewport; }

	const SFNode <X3DBaseNode> &
	viewport () const
	{ return *fields .viewport; }

	MFNode &
	addChildren ()
	{ return *fields .addChildren; }

	const MFNode &
	addChildren () const
	{ return *fields .addChildren; }

	MFNode &
	removeChildren ()
	{ return *fields .removeChildren; }

	const MFNode &
	removeChildren () const
	{ return *fields .removeChildren; }

	MFNode &
	children ()
	{ return *fields .children; }

	const MFNode &
	children () const
	{ return *fields .children; }

	///  @name Children handling

	const SFNode <Group>
	getGroup () const
	{ return group; }

	///  @name Bindable node stack handling

	NavigationInfo*
	getNavigationInfo () const;

	X3DBackgroundNode*
	getBackground () const;

	X3DFogObject*
	getFog () const;

	X3DViewpointNode*
	getViewpoint () const;

	///  @name X3DBindableNode stack handling

	NavigationInfoStack &
	getNavigationInfoStack ()
	{ return navigationInfoStack; }

	const NavigationInfoStack &
	getNavigationInfoStack () const
	{ return navigationInfoStack; }

	BackgroundStack &
	getBackgroundStack ()
	{ return backgroundStack; }

	const BackgroundStack &
	getBackgroundStack () const
	{ return backgroundStack; }

	FogStack &
	getFogStack ()
	{ return fogStack; }

	const FogStack &
	getFogStack () const
	{ return fogStack; }

	ViewpointStack &
	getViewpointStack ()
	{ return viewpointStack; }

	const ViewpointStack &
	getViewpointStack () const
	{ return viewpointStack; }

	///  @name X3DBindableNode list handling

	NavigationInfoList &
	getNavigationInfos ()
	{ return navigationInfos; }

	const NavigationInfoList &
	getNavigationInfos () const
	{ return navigationInfos; }

	BackgroundList &
	getBackgrounds ()
	{ return backgrounds; }

	const BackgroundList &
	getBackgrounds () const
	{ return backgrounds; }

	FogList &
	getFogs ()
	{ return fogs; }

	const FogList &
	getFogs () const
	{ return fogs; }

	ViewpointList &
	getViewpoints ()
	{ return viewpoints; }
	
	const ViewpointList &
	getViewpoints () const
	{ return viewpoints; }
	
	UserViewpointList
	getUserViewpoints () const;

	///  @name Fog handling

	LocalFogStack &
	getLocalFogs ()
	{ return localFogs; }

	///  @name Light handling

	void
	pushLocalLight (X3DLightNode*);

	void
	popLocalLight ()
	{ localLights .pop_back (); }

	const LightContainerArray &
	getLocalLights ()
	{ return localLights; }

	void
	addGlobalLight (X3DLightNode* light)
	{ globalLights .push_back (new LightContainer (ModelViewMatrix4f (), light)); }

	const LightContainerArray &
	getGlobalLights ()
	{ return globalLights; }

	///  @name Collision handling

	CollisionArray &
	getCollisions ()
	{ return collisions; }

	///  @name ...

	Box3f
	getBBox ();

	void
	lookAt ();

	virtual
	void
	traverse (TraverseType) override;

	virtual
	void
	dispose () override;


protected:

	X3DLayerNode ();

	virtual
	void
	initialize () override;


private:

	void
	enableHeadlight ();

	void
	disableHeadlight ();

	void
	clearLights ();

	void
	set_viewport ();

	void
	pick ();

	void
	camera ();

	void
	navigation ();

	void
	collision ();

	void
	collect ();

	virtual
	void
	collect (TraverseType) override;

	struct Fields
	{
		Fields ();

		SFBool* const isPickable;
		SFNode <X3DBaseNode>* const viewport;
		MFNode* const addChildren;
		MFNode* const removeChildren;
		MFNode* const children;
	};

	Fields fields;

	SFNode <Viewport>          defaultViewport;
	SFNode <NavigationInfo>    defaultNavigationInfo;
	SFNode <X3DBackgroundNode> defaultBackground;
	SFNode <Fog>               defaultFog;
	SFNode <X3DViewpointNode>  defaultViewpoint;

	X3DViewportNode* currentViewport;

	NavigationInfoStack    navigationInfoStack;
	BackgroundStack        backgroundStack;
	FogStack               fogStack;
	ViewpointStack         viewpointStack;

	NavigationInfoList navigationInfos;
	BackgroundList     backgrounds;
	FogList            fogs;
	ViewpointList      viewpoints;

	LocalFogStack       localFogs;
	LightContainerArray localLights;
	LightContainerArray cachedLocalLights;
	LightContainerArray globalLights;
	CollisionArray      collisions;

	SFNode <Group> group;

};

} // X3D
} // titania

#endif
