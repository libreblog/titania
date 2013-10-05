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

#include "BrowserWindow.h"

#include "../NodePropertiesEditor/NodePropertiesEditor.h"

namespace titania {
namespace puck {

X3DBrowserEditor::X3DBrowserEditor (const basic::uri & worldURL) :
	X3DBrowserWidget (worldURL),
	          edited (false),
	   saveConfirmed (false),
	           scene (),
	     undoHistory ()
{ }

void
X3DBrowserEditor::initialize ()
{
	X3DBrowserWidget::initialize ();

	getBrowser () -> initialized () .addInterest (this, &X3DBrowserEditor::set_initialized);
	getBrowser () -> shutdown ()    .addInterest (this, &X3DBrowserEditor::set_shutdown);
}

void
X3DBrowserEditor::restoreSession ()
{
	// Workspace
	if (getConfig () .string ("workspace") == "BROWSER")
		getBrowserMenuItem () .activate ();

	else if (getConfig () .string ("workspace") == "EDITOR")
		getEditorMenuItem () .activate ();

	X3DBrowserWidget::restoreSession ();
}

void
X3DBrowserEditor::set_initialized ()
{
	if (getBrowser () -> getExecutionContext () not_eq scene)
		setEdited (false);

	scene = getBrowser () -> getExecutionContext ();

	undoHistory .clear ();
}

void
X3DBrowserEditor::set_shutdown ()
{
	if (isSaved ())
		return;

	// Cancel shutdown

	getBrowser () -> replaceWorld (getBrowser () -> getExecutionContext ());
}

bool
X3DBrowserEditor::isSaved ()
{
	if (saveConfirmed)
		return true;

	if (getEdited ())
	{
		auto response_id = getFileSaveWarningDialog () .run ();

		getFileSaveWarningDialog () .hide ();

		switch (response_id)
		{
			case Gtk::RESPONSE_OK:
			{
				on_save ();
				saveConfirmed = not getEdited ();
				return saveConfirmed;
			}
			case Gtk::RESPONSE_CANCEL:
			case Gtk::RESPONSE_DELETE_EVENT:
			{
				return false;
			}
			default:
				break;
		}
	}

	saveConfirmed = true;
	return true;
}

void
X3DBrowserEditor::setEdited (bool value)
{
	edited        = value;
	saveConfirmed = false;
	updateTitle (value);
}

// File operations

void
X3DBrowserEditor::import (const basic::uri & worldURL)
{
	try
	{
		// Imported scene

		import (getBrowser () -> createX3DFromURL ({ worldURL .str () }));
	}
	catch (const X3D::X3DError & error)
	{
		std::clog << error .what () << std::endl;
	}
}

void
X3DBrowserEditor::import (const X3D::X3DSFNode <X3D::Scene> & scene)
{
	try
	{
		auto & rootNodes    = getBrowser () -> getExecutionContext () -> getRootNodes ();
		size_t numRootNodes = rootNodes .size ();

		// Imported scene

		getBrowser () -> getExecutionContext () -> importScene (scene);

		// Select imported nodes

		getBrowser () -> getSelection () -> clear ();

		for (const auto & rootNode : basic::adapter (rootNodes .begin () + numRootNodes, rootNodes .end ()))
			getBrowser () -> getSelection () -> addChild (rootNode);

		getBrowser () -> update ();

		setEdited (true);
	}
	catch (const X3D::X3DError & error)
	{
		std::clog << error .what () << std::endl;
	}
}

void
X3DBrowserEditor::save (const basic::uri & worldURL, bool compressed)
{
	X3DBrowserWidget::save (worldURL, compressed);

	setEdited (false);
}

bool
X3DBrowserEditor::close ()
{
	if (isSaved ())
		return X3DBrowserWindowInterface::close ();

	return true;
}

// Undo operations

// Edit operations

void
X3DBrowserEditor::addNode (const std::string & typeName, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NAME>)
{
	auto scene = getBrowser () -> getExecutionContext ();
	auto node  = scene -> createNode (typeName);

	node -> setup ();
	scene -> getRootNodes () .emplace_back (node);

	setEdited (true);
}

void
X3DBrowserEditor::cutNodes (X3D::MFNode nodes, const UndoStepPtr & undoStep)
{
	// Detach from group

	for (const auto & node : nodes)
	{
		try
		{
			detachFromGroup (node, true, undoStep);
		}
		catch (const X3D::X3DError &)
		{ }
	}

	// Set clipboard text

	Gtk::Clipboard::get () -> set_text (toString (nodes));

	// Remove nodes

	for (const auto & node : nodes)
	{
		try
		{
			removeNode (node, undoStep);
		}
		catch (const X3D::X3DError &)
		{ }
	}
}

void
X3DBrowserEditor::copyNodes (X3D::MFNode nodes)
{
	// Detach from group

	auto undoDetachFromGroup = std::make_shared <UndoStep> ("");

	for (const auto & node : nodes)
	{
		try
		{
			detachFromGroup (node, true, undoDetachFromGroup);
		}
		catch (const X3D::X3DError &)
		{ }
	}

	// Set clipboard text

	Gtk::Clipboard::get () -> set_text (toString (nodes));

	// Undo detach from group

	undoDetachFromGroup -> undo ();
}

std::string
X3DBrowserEditor::toString (X3D::MFNode & nodes) const
{
	// Find proto declarations

	std::set <X3D::X3DSFNode <X3D::X3DProto>>  protoDeclarations;

	for (auto & node : nodes)
	{
		traverse (node, [&protoDeclarations] (X3D::SFNode & node)
		          {
		             auto protoInstance = dynamic_cast <X3D::X3DPrototypeInstance*> (node .getValue ());

		             if (protoInstance)
							 protoDeclarations .insert (protoInstance -> getProtoDeclaration ());

		             return false;
					 });
	}

	// Create node index

	std::set <X3D::SFNode> nodeIndex;

	for (auto & node : nodes)
	{
		traverse (node, [&nodeIndex] (X3D::SFNode & node)
		          {
		             nodeIndex .insert (node);
		             return false;
					 });
	}

	// Find connected routes

	std::deque <X3D::Route*> routes;

	for (auto & node : nodes)
	{
		traverse (node, [&nodeIndex, &routes] (X3D::SFNode & node)
		          {
		             for (const auto & field : node -> getFieldDefinitions ())
		             {
		                for (const auto & route : field -> getOutputRoutes ())
		                {
		                   if (nodeIndex .find (route -> getDestinationNode ()) not_eq nodeIndex .end ())
									 routes .emplace_back (route);
							 }
						 }

		             return false;
					 });
	}

	// Generate text

	std::ostringstream text;

	text
		<< '#' << getBrowser () -> getExecutionContext () -> getWorldURL ()
		<< std::endl
		<< std::endl;

	X3D::Generator::CompactStyle ();
	X3D::Generator::PushContext ();

	if (not protoDeclarations .empty ())
	{
		for (const auto & protoDeclaration : protoDeclarations)
			text << protoDeclaration << std::endl;

		text << std::endl;
	}

	for (const auto & node : nodes)
		text << node << std::endl;

	if (not routes .empty ())
	{
		text << std::endl;

		for (const auto & route : routes)
			text << *route << std::endl;
	}

	X3D::Generator::PopContext ();

	return text .str ();
}

void
X3DBrowserEditor::pasteNodes (const X3D::MFNode & nodes, const UndoStepPtr & undoStep)
{
	try
	{
		Glib::RefPtr <Gtk::Clipboard> clipboard = Gtk::Clipboard::get ();

		if (clipboard -> wait_is_text_available ())
		{
			std::istringstream text (clipboard -> wait_for_text ());

			std::string whitespaces;
			X3D::Grammar::whitespaces (text, whitespaces);

			std::string worldURL;

			if (X3D::Grammar::comment (text, worldURL))
			{
				import (getBrowser () -> createX3DFromStream (worldURL, text));
			}
		}
	}
	catch (const X3D::X3DError & error)
	{
		std::clog << error .what () << std::endl;
	}
}

void
X3DBrowserEditor::removeNode (const X3D::SFNode & node, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>)
{
	if (not node)
		throw X3D::Error <X3D::INVALID_NODE> ("Node is empty.");

	removeNode (getBrowser () -> getExecutionContext (), node, undoStep);

	setEdited (true);
}

void
X3DBrowserEditor::removeNode (const X3D::X3DSFNode <X3D::Scene> & scene, X3D::SFNode node, const UndoStepPtr & undoStep)
{
	removeNode (scene .getValue (), node, undoStep);

	// Remove exported nodes

	removeExportedNodes (scene, node);

	// Delete children of node if not in scene

	std::set <X3D::SFNode> children;

	traverse (node, [&node, &children] (X3D::SFNode & child)
	          {
	             if (child not_eq node)
						 children .insert (child);

	             return false;
				 });

	for (const auto & child : children)
	{
		if (not traverse (scene, [&child] (X3D::SFNode & node)
		                  {
		                     return child == node;
								}))
		{
			removeNamedNode (scene, child);
			removeExportedNodes (scene, child);
			removeImportedNodes (scene, child);
			deleteRoutes (scene, child);
		}
	}
}

void
X3DBrowserEditor::removeExportedNodes (const X3D::X3DSFNode <X3D::Scene> & scene, const X3D::SFNode & node)
{
	// Remove exported nodes

	for (const auto & exportedNode : X3D::ExportedNodeArray (scene -> getExportedNodes ()))
	{
		if (exportedNode -> getNode () == node)
			scene -> removeExportedNode (exportedNode -> getExportedName ());
	}
}

void
X3DBrowserEditor::removeNode (X3D::X3DExecutionContext* const executionContext, const X3D::SFNode & node, const UndoStepPtr & undoStep)
{
	// Delete node in scene graph

	removeNodeFromSceneGraph (executionContext, node, undoStep);

	// Remove named node

	removeNamedNode (executionContext, node);

	// Remove nodes imported from node

	removeImportedNodes (executionContext, node);

	// Delete routes from and to node

	deleteRoutes (executionContext, node);
}

void
X3DBrowserEditor::removeNodeFromSceneGraph (X3D::X3DExecutionContext* const executionContext, const X3D::SFNode & node, const UndoStepPtr & undoStep)
{
	removeNode (executionContext, executionContext -> getRootNodes (), node, undoStep);

	// Remove node from scene graph

	traverse (executionContext, [this, &node, &undoStep] (X3D::SFNode & parent)
	          {
	             for (auto & field: parent -> getFieldDefinitions ())
	             {
	                switch (field -> getType ())
	                {
							 case X3D::X3DConstants::SFNode:
								 {
								    auto sfnode = static_cast <X3D::SFNode*> (field);

								    if (*sfnode == node)
								    {
								       undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoSetValue), this,
								                                parent,
								                                std::ref (*sfnode),
								                                node);

								       sfnode -> setValue (nullptr);
									 }

								    break;
								 }
							 case X3D::X3DConstants::MFNode:
								 {
								    auto mfnode = static_cast <X3D::MFNode*> (field);

								    removeNode (parent, *mfnode, node, undoStep);

								    break;
								 }
							 default:
								 break;
						 }
					 }

	             return false;
				 });
}

void
X3DBrowserEditor::removeNode (const X3D::SFNode & parent, X3D::MFNode & mfnode, const X3D::SFNode & node, const UndoStepPtr & undoStep)
{
	auto indices = mfnode .indices_of (node);

	if (not indices .empty ())
	{
		undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoEraseNode), this,
		                         parent,
		                         std::ref (mfnode),
		                         node,
		                         indices);

		mfnode .erase (std::remove (mfnode .begin (), mfnode .end (), node), mfnode .end ());
	}
}

void
X3DBrowserEditor::removeNamedNode (X3D::X3DExecutionContext* const executionContext, const X3D::SFNode & node)
{
	// Remove named node

	try
	{
		if (executionContext -> getNamedNode (node -> getName ()) == node)
			executionContext -> removeNamedNode (node -> getName ());
	}
	catch (const X3D::X3DError &)
	{ }
}

void
X3DBrowserEditor::removeImportedNodes (X3D::X3DExecutionContext* const executionContext, const X3D::SFNode & node)
{
	// Remove nodes imported from node

	X3D::X3DSFNode <X3D::Inline> inlineNode (node);

	if (inlineNode)
	{
		for (const auto & importedNode : X3D::ImportedNodeArray (executionContext -> getImportedNodes ()))
		{
			if (inlineNode == importedNode -> getInlineNode ())
			{
				deleteRoutes (executionContext, importedNode -> getExportedNode ());

				executionContext -> removeImportedNode (importedNode -> getImportedName ());
			}
		}
	}
}

void
X3DBrowserEditor::deleteRoutes (X3D::X3DExecutionContext* const executionContext, const X3D::SFNode & node)
{
	// Delete routes from and to node

	for (const auto & route : X3D::RouteArray (executionContext -> getRoutes ()))
	{
		if (route -> getSourceNode () == node or route -> getDestinationNode () == node)
			executionContext -> deleteRoute (route);
	}
}

X3D::SFNode
X3DBrowserEditor::groupNodes (const X3D::MFNode & nodes, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>)
{
	if (nodes .empty ())
		throw X3D::Error <X3D::INVALID_NODE> ("Nodes are empty.");

	auto scene = getBrowser () -> getExecutionContext ();

	X3D::X3DSFNode <X3D::X3DGroupingNode> group (scene -> createNode ("Transform"));

	for (const auto & child : nodes)
	{
		if (not child)
			continue;

		// Adjust transformation
		X3D::Matrix4f                          childModelViewMatrix = findModelViewMatrix (child);
		X3D::X3DSFNode <X3D::X3DTransformNode> transform (child);

		if (transform)
		{
			childModelViewMatrix .multLeft (transform -> getMatrix ());
			transform -> setMatrix (childModelViewMatrix);
		}

		// Remove child from scene graph

		removeNodeFromSceneGraph (getBrowser () -> getExecutionContext (), child, undoStep);

		// Add to group

		group -> children () .emplace_back (child);
	}

	group -> setup ();
	scene -> getRootNodes () .emplace_back (group);

	setEdited (true);

	return X3D::SFNode (group);
}

X3D::MFNode
X3DBrowserEditor::ungroupNode (const X3D::SFNode & group, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>,
       X3D::Error <X3D::INVALID_NAME>)
{
	if (not group)
		throw X3D::Error <X3D::INVALID_NODE> ("Group node is emtpy.");

	auto groupingField = getGroupingField (group);

	X3D::MFNode children;

	for (const auto & child : *groupingField)
	{
		if (not child)
			continue;

		// Adjust transformation

		X3D::Matrix4f                          childModelViewMatrix = findModelViewMatrix (child);
		X3D::X3DSFNode <X3D::X3DTransformNode> transform (child);

		if (transform)
		{
			childModelViewMatrix .multLeft (transform -> getMatrix ());
			transform -> setMatrix (childModelViewMatrix);
		}

		// Add to layer

		getBrowser () -> getExecutionContext () -> getRootNodes () .emplace_back (child);

		children .emplace_back (child);
	}

	// Remove group from scene

	removeNode (getBrowser () -> getExecutionContext (), group, undoStep);

	setEdited (true);

	return children;
}

void
X3DBrowserEditor::addToGroup (const X3D::SFNode & group, const X3D::SFNode & child, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>,
       X3D::Error <X3D::INVALID_NAME>)
{
	if (not group)
		throw X3D::Error <X3D::INVALID_NODE> ("Group node is emtpy.");

	if (not child)
		throw X3D::Error <X3D::INVALID_NODE> ("Child node is emtpy.");

	if (child == group)
		throw X3D::Error <X3D::INVALID_NODE> ("Group and child node are equal.");

	auto containerField = getContainerField (group, child);

	// Get group modelview matrix

	X3D::Matrix4f                          groupModelViewMatrix = findModelViewMatrix (group);
	X3D::X3DSFNode <X3D::X3DTransformNode> transform (group);

	if (transform)
		groupModelViewMatrix .multLeft (transform -> getMatrix ());

	// Adjust child transformation

	{
		X3D::Matrix4f                          childModelViewMatrix = findModelViewMatrix (child);
		X3D::X3DSFNode <X3D::X3DTransformNode> transform (child);

		if (transform)
		{
			try
			{
				childModelViewMatrix .multLeft (transform -> getMatrix ());
				childModelViewMatrix .multRight (~groupModelViewMatrix);

				transform -> setMatrix (childModelViewMatrix);
			}
			catch (const std::domain_error & error)
			{ }
		}
	}

	// Remove child from scene graph

	removeNodeFromSceneGraph (getBrowser () -> getExecutionContext (), child, undoStep);

	// Add child to group

	auto sfnode = dynamic_cast <X3D::SFNode*> (containerField);

	if (sfnode)
		sfnode -> setValue (child);                                                                                                                                                                                                                                                                                          // XXX Remove previous child completely from scene if not in scene anymore

	else
	{
		auto mfnode = dynamic_cast <X3D::MFNode*> (containerField);

		if (mfnode)
			mfnode -> emplace_back (child);
	}

	setEdited (true);
}

void
X3DBrowserEditor::detachFromGroup (X3D::SFNode node, bool detachToLayer0, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>)
{
	X3D::X3DSFNode <X3D::X3DNode> child (node);

	if (not child)
		throw X3D::Error <X3D::INVALID_NODE> ("Node is empty.");

	auto layers = child -> getLayers ();

	// Adjust transformation

	X3D::X3DSFNode <X3D::X3DTransformNode> transform (child);

	if (transform)
	{
		undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoSetMatrix), this,
		                         transform,
		                         transform -> getMatrix ());

		X3D::Matrix4f childModelViewMatrix = findModelViewMatrix (child);

		childModelViewMatrix .multLeft (transform -> getMatrix ());
		transform -> setMatrix (childModelViewMatrix);
	}

	// Remove child from scene graph

	removeNodeFromSceneGraph (getBrowser () -> getExecutionContext (), node, undoStep);

	// Add to layers

	if (detachToLayer0)
	{
		undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoInsertNode), this,
		                         X3D::SFNode (getBrowser () -> getExecutionContext ()),
		                         std::ref (getBrowser () -> getExecutionContext () -> getRootNodes ()),
		                         getBrowser () -> getExecutionContext () -> getRootNodes () .size (),
		                         node);

		getBrowser () -> getExecutionContext () -> getRootNodes () .emplace_back (child);
	}
	else
	{
		for (const auto & layer : layers)
		{
			if (layer -> isLayer0 ())
			{
				undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoInsertNode), this,
				                         X3D::SFNode (getBrowser () -> getExecutionContext ()),
				                         std::ref (getBrowser () -> getExecutionContext () -> getRootNodes ()),
				                         getBrowser () -> getExecutionContext () -> getRootNodes () .size (),
				                         node);

				getBrowser () -> getExecutionContext () -> getRootNodes () .emplace_back (child);
			}
			else
			{
				undoStep -> addFunction (std::mem_fn (&X3DBrowserEditor::undoInsertNode), this,
				                         X3D::SFNode (layer),
				                         std::ref (layer -> children ()),
				                         layer -> children () .size (),
				                         node);

				layer -> children () .emplace_back (child);
			}
		}
	}

	setEdited (true);
}

X3D::MFNode
X3DBrowserEditor::createParentGroup (const X3D::SFNode & child, const UndoStepPtr & undoStep)
throw (X3D::Error <X3D::INVALID_NODE>)
{
	if (not child)
		throw X3D::Error <X3D::INVALID_NODE> ("Node is empty.");

	X3D::MFNode groups;

	auto scene = getBrowser () -> getExecutionContext ();

	traverse (scene, [&scene, &child, &groups] (X3D::SFNode & node)
	          {
	             if (child not_eq node)
						 return false;

	             // Replace node with Transform

	             X3D::X3DSFNode <X3D::X3DGroupingNode> group (scene -> createNode ("Transform"));

	             group -> children () .emplace_back (child);
	             group -> setup ();

	             node = group;

	             groups .emplace_back (group);

	             return false;
				 });

	setEdited (true);

	return groups;
}

void
X3DBrowserEditor::openNodePropertiesEditor (const X3D::SFNode & node)
{
	addDialog (new NodePropertiesEditor (getBrowserWindow (), node));
}

// Scene opeations

bool
X3DBrowserEditor::traverse (X3D::X3DExecutionContext* const executionContext, const TraverseCallback & callback) const
{
	X3D::ChildObjectSet seen;

	for (auto & rootNode : executionContext -> getRootNodes ())
	{
		if (traverse (rootNode, callback, seen))
			return true;
	}

	return false;
}

bool
X3DBrowserEditor::traverse (X3D::SFNode & node, const TraverseCallback & callback) const
{
	X3D::ChildObjectSet seen;

	return traverse (node, callback, seen);
}

bool
X3DBrowserEditor::traverse (X3D::SFNode & node, const TraverseCallback & callback, X3D::ChildObjectSet & seen) const
{
	if (not node)
		return false;

	if (not seen .insert (node) .second)
		return false;

	if (callback (node))
		return true;

	for (const auto & field : node -> getFieldDefinitions ())
	{
		switch (field -> getType ())
		{
			case X3D::X3DConstants::SFNode :
				{
					auto sfnode = static_cast <X3D::SFNode*> (field);

					if (traverse (*sfnode, callback, seen))
						return true;

					break;
				}
			case X3D::X3DConstants::MFNode:
			{
				auto mfnode = static_cast <X3D::MFNode*> (field);

				for (auto & value : *mfnode)
				{
					if (traverse (value, callback, seen))
						return true;
				}

				break;
			}
			default:
				break;
		}
	}

	return false;
}

X3D::Matrix4f
X3DBrowserEditor::findModelViewMatrix (X3D::X3DBaseNode* const node) const
{
	X3D::Matrix4f modelViewMatix;

	std::set <X3D::X3DBaseNode*> seen;

	for (const auto & parentNode : getParentNodes (node))
	{
		if (findModelViewMatrix (parentNode, modelViewMatix, seen))
			break;
	}

	return modelViewMatix;
}

bool
X3DBrowserEditor::findModelViewMatrix (X3D::X3DBaseNode* const node, X3D::Matrix4f & modelViewMatix, std::set <X3D::X3DBaseNode*> & seen) const
{
	if (not seen .insert (node) .second)
		return false;

	// Do we found the layer?

	if (dynamic_cast <X3D::X3DLayerNode*> (node))
		return true;

	// Bad nodes

	if (dynamic_cast <X3D::X3DBrowser*> (node))
		return false;

	if (dynamic_cast <X3D::Scene*> (node))
		return false;

	if (dynamic_cast <X3D::X3DScriptNode*> (node))
		return false;

	if (dynamic_cast <X3D::X3DProgrammableShaderObject*> (node))
		return false;

	// Iterate over parents

	for (const auto & parentNode : getParentNodes (node))
	{
		if (findModelViewMatrix (parentNode, modelViewMatix, seen))
		{
			auto transform = dynamic_cast <X3D::X3DTransformNode*> (node);

			if (transform)
				modelViewMatix .multLeft (transform -> getMatrix ());

			return true;
		}
	}

	return false;
}

std::deque <X3D::X3DBaseNode*>
X3DBrowserEditor::getParentNodes (X3D::X3DBaseNode* const node) const
{
	std::deque <X3D::X3DBaseNode*> parentNodes;

	for (const auto & parent : node -> getParents ())
	{
		auto sfnode = dynamic_cast <X3D::SFNode*> (parent);

		if (sfnode)
		{
			for (const auto & secondParent : sfnode -> getParents ())
			{
				auto mfnode = dynamic_cast <X3D::MFNode*> (secondParent);

				if (mfnode)
				{
					for (const auto & thirdParent : mfnode -> getParents ())
					{
						auto baseNode = dynamic_cast <X3D::X3DBaseNode*> (thirdParent);

						if (baseNode)
							parentNodes .emplace_back (baseNode);
					}

					continue;
				}

				auto baseNode = dynamic_cast <X3D::X3DBaseNode*> (secondParent);

				if (baseNode)
				{
					parentNodes .emplace_back (baseNode);
					continue;
				}
			}
		}
	}

	return parentNodes;
}

X3D::X3DFieldDefinition*
X3DBrowserEditor::getContainerField (const X3D::SFNode & parent, const X3D::SFNode & child) const
throw (X3D::Error <X3D::INVALID_NAME>)
{
	X3D::X3DFieldDefinition* field = nullptr;

	try
	{
		field = parent -> getField (child -> getLocalNode () -> getContainerField ());

		if (field -> getType () not_eq X3D::X3DConstants::SFNode and field -> getType () not_eq X3D::X3DConstants::MFNode)
			throw X3D::Error <X3D::INVALID_NAME> ("No appropriate container field found.");
	}
	catch (const X3D::Error <X3D::INVALID_NAME> &)
	{
		try
		{
			field = getGroupingField (parent);
		}
		catch (const X3D::Error <X3D::INVALID_NAME> &)
		{
			field = parent -> getField ("shape");

			if (field -> getType () not_eq X3D::X3DConstants::SFNode)
				throw X3D::Error <X3D::INVALID_NAME> ("No appropriate container field found.");
		}
	}

	return field;
}

X3D::MFNode*
X3DBrowserEditor::getGroupingField (const X3D::SFNode & node) const
throw (X3D::Error <X3D::INVALID_NAME>)
{
	X3D::X3DFieldDefinition* field = nullptr;

	try
	{
		field = node -> getField ("children");

		if (field -> getType () not_eq X3D::X3DConstants::MFNode)
			throw X3D::Error <X3D::INVALID_NAME> ("No appropriate container field found.");
	}
	catch (const X3D::Error <X3D::INVALID_NAME> &)
	{
		field = node -> getField ("layers");

		if (field -> getType () not_eq X3D::X3DConstants::MFNode)
			throw X3D::Error <X3D::INVALID_NAME> ("No appropriate container field found.");
	}

	return dynamic_cast <X3D::MFNode*> (field);
}

// Undo functions

void
X3DBrowserEditor::undoInsertNode (const X3D::SFNode & parent, X3D::MFNode & field, size_t index, const X3D::SFNode & node)
{
	if (index < field .size () and field [index] == node)
	{
		field .erase (field .begin () + index);
	}
	else
	{
		// There has something changed, clear history.
		__LOG__ << std::endl;
	}
}

void
X3DBrowserEditor::undoEraseNode (const X3D::SFNode & parent, X3D::MFNode & field, const X3D::SFNode & value, const std::vector <size_t> & indices)
{
	for (const auto & index : indices)
	{
		if (index <= field .size ())
		{
			field .insert (field .begin () + index, value);
		}
		else
		{
			// There is has something changed, clear history
			__LOG__ << std::endl;
		}
	}
}

void
X3DBrowserEditor::undoSetValue (const X3D::SFNode & parent, X3D::SFNode & field, const X3D::SFNode & value)
{
	field = value;
}

void
X3DBrowserEditor::undoSetMatrix (const X3D::X3DSFNode <X3D::X3DTransformNode> & transform, const X3D::Matrix4f & matrix)
{
	transform -> setMatrix (matrix);
}

} // puck
} // titania
