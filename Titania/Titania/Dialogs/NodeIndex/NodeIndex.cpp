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

#include "NodeIndex.h"

#include "../../Base/AdjustmentObject.h"
#include "../../Browser/X3DBrowserWindow.h"
#include "../../Configuration/config.h"

#include <Titania/X3D/Bits/Traverse.h>
#include <Titania/X3D/Components/Networking/Inline.h>
#include <Titania/X3D/Execution/NamedNode.h>
#include <Titania/X3D/Execution/ImportedNode.h>
#include <Titania/X3D/Execution/ExportedNode.h>

#include <Titania/OS.h>
#include <Titania/String.h>

namespace titania {
namespace puck {

namespace Search {

static constexpr int TYPE_NAME = 0;

};

namespace Columns {

static constexpr int TYPE_NAME      = 0;
static constexpr int NAME           = 1;
static constexpr int IMPORTED_NODES = 2;
static constexpr int EXPORTED_NODES = 3;

};

NodeIndex::NodeIndex (X3DBrowserWindow* const browserWindow) :
	     X3DBaseInterface (browserWindow, browserWindow -> getBrowser ()),
	X3DNodeIndexInterface (get_ui ("Dialogs/NodeIndex.xml"), gconf_dir ()),
	     executionContext (),
	                index (NAMED_NODES_INDEX),
	                types (),
	                nodes (),
	                 node (),
	            nodeTypes (),
	          hadjustment (new AdjustmentObject ()),
	          vadjustment (new AdjustmentObject ())
{
	addChildren (node);
	setup ();
}

void
NodeIndex::initialize ()
{
	X3DNodeIndexInterface::initialize ();

	getExecutionContext () .addInterest (this, &NodeIndex::set_executionContext);

	set_executionContext ();

	// Initialize SearchEntryCompletion:

	for (const auto & node : getBrowser () -> getSupportedNodes ())
	{
		const auto row = getSearchListStore () -> append ();
		row -> set_value (Search::TYPE_NAME, node -> getTypeName ());

		nodeTypes .emplace (node -> getTypeName (), node -> getType () .back ());
	}
  
	const auto cellrenderer = Gtk::manage (new Gtk::CellRendererText ());
	getSearchEntryCompletion () -> pack_start (*cellrenderer, false);
	getSearchEntryCompletion () -> add_attribute (*cellrenderer, "text", Search::TYPE_NAME);
}

void
NodeIndex::refresh ()
{
	switch (index)
	{
		case NAMED_NODES_INDEX:
		{
			setNodes (getNodes ());
			break;
		}
		case TYPE_INDEX:
		{
			setNodes (getNodes (types));
			break;
		}
		case ANIMATION_INDEX:
		{
			X3D::MFNode animations;
			
			for (const auto & basenode : getNodes ({ X3D::X3DConstants::Group }))
			{
				try
				{
					X3D::X3DPtr <X3D::X3DNode> node (basenode);
					node -> getMetaData <X3D::MFInt32> ("/Animation/duration", false);
					animations .emplace_back (basenode);
				}
				catch (const X3D::X3DError &)
				{ }
			}

			setNodes (std::move (animations));
			break;
		}
	}
}

void
NodeIndex::setNamedNodes ()
{
	executionContext -> sceneGraph_changed () .removeInterest (this, &NodeIndex::refresh);

	index = NAMED_NODES_INDEX;
	setNodes (getNodes ());
}

void
NodeIndex::setTypes (const std::set <X3D::X3DConstants::NodeType> & value)
{
	executionContext -> sceneGraph_changed () .addInterest (this, &NodeIndex::refresh);

	index = TYPE_INDEX;
	types = value;
	setNodes (getNodes (types));
}

void
NodeIndex::setAnimations ()
{
	executionContext -> sceneGraph_changed () .addInterest (this, &NodeIndex::refresh);

	index = ANIMATION_INDEX;
	refresh ();
}

void
NodeIndex::setNodes (X3D::MFNode && value)
{
	static const std::string empty_string;
	static const std::string document_import ("document-import");
	static const std::string document_export ("document-export");

	nodes = std::move (value);

	hadjustment -> preserve (getTreeView () .get_hadjustment ());
	vadjustment -> preserve (getTreeView () .get_vadjustment ());

	getTreeView () .unset_model ();
	getListStore () -> clear ();

	const auto importingInlines = getImportingInlines ();
	const auto exportedNodes    = getExportedNodes ();

	for (const auto & node : nodes)
	{
		const auto row = getListStore () -> append ();
		row -> set_value (Columns::TYPE_NAME,      node -> getTypeName ());
		row -> set_value (Columns::NAME,           node -> getName ());
		row -> set_value (Columns::IMPORTED_NODES, importingInlines .count (node) ? document_import : empty_string);
		row -> set_value (Columns::EXPORTED_NODES, exportedNodes .count (node)    ? document_export : empty_string);
	}

	getTreeView () .set_model (getListStore ());
	getTreeView () .set_search_column (Columns::NAME);
}

/***
 *  Returns a list of all nodes where type is @a types.
 */
X3D::MFNode
NodeIndex::getNodes (const std::set <X3D::X3DConstants::NodeType> & types)
{
	// Find nodes

	X3D::MFNode nodes;
	
	if (inPrototypeInstance ())
		return nodes;

	X3D::traverse (getExecutionContext () -> getRootNodes (), [&] (X3D::SFNode & node)
	               {
	                  if (node -> getExecutionContext () not_eq getExecutionContext ())
								return true;

	                  for (const auto & type: basic::make_reverse_range (node -> getType ()))
	                  {
	                     if (types .count (type))
	                     {
	                        nodes .emplace_back (node);
	                        break;
								}
							}

	                  return true;
						});

	std::sort (nodes .begin (), nodes .end (), [ ] (const X3D::SFNode & lhs, const X3D::SFNode & rhs)
	           {
	              return basic::naturally_compare (lhs -> getName (), rhs -> getName ());
				  });

	return nodes;
}

/***
 *  Returns a list of all named nodes.
 */
X3D::MFNode
NodeIndex::getNodes ()
{
	// Find nodes

	X3D::MFNode nodes;
	
	if (inPrototypeInstance ())
		return nodes;

	for (const auto pair : getExecutionContext () -> getNamedNodes ())
	{
		try
		{
			nodes .emplace_back (pair .second -> getLocalNode ());
		}
		catch (...)
		{ }
	}

	std::sort (nodes .begin (), nodes .end (), [ ] (const X3D::SFNode & lhs, const X3D::SFNode & rhs)
	           {
	              return basic::naturally_compare (lhs -> getName (), rhs -> getName ());
				  });

	return nodes;
}

std::set <X3D::SFNode>
NodeIndex::getImportingInlines () const
{
	std::set <X3D::SFNode> importingInlines;

	for (const auto & pair : getExecutionContext () -> getImportedNodes ())
	{
		try
		{
			importingInlines .emplace (pair .second -> getInlineNode ());
		}
		catch (...)
		{ }
	}

	return importingInlines;
}

std::set <X3D::SFNode>
NodeIndex::getExportedNodes () const
{
	std::set <X3D::SFNode> exportedNodes;

	X3D::X3DScenePtr scene (getExecutionContext ());

	if (not scene)
		return exportedNodes;

	for (const auto & pair : scene -> getExportedNodes ())
	{
		try
		{
			exportedNodes .emplace (pair .second -> getLocalNode ());
		}
		catch (...)
		{ }
	}

	return exportedNodes;
}

void
NodeIndex::set_executionContext ()
{
	if (executionContext)
	{
		executionContext -> namedNodes_changed ()    .removeInterest (this, &NodeIndex::refresh);
		executionContext -> importedNodes_changed () .removeInterest (this, &NodeIndex::refresh);
		executionContext -> sceneGraph_changed ()    .removeInterest (this, &NodeIndex::refresh);
	}

	X3D::X3DScenePtr scene (executionContext);

	if (scene)
		scene -> exportedNodes_changed () .removeInterest (this, &NodeIndex::refresh);

	executionContext = getExecutionContext ();
	executionContext -> namedNodes_changed ()    .addInterest (this, &NodeIndex::refresh);
	executionContext -> importedNodes_changed () .addInterest (this, &NodeIndex::refresh);

	if (index == TYPE_INDEX)
		executionContext -> sceneGraph_changed () .addInterest (this, &NodeIndex::refresh);

	scene = executionContext;

	if (scene)
		scene -> exportedNodes_changed () .addInterest (this, &NodeIndex::refresh);

	refresh ();
}

bool
NodeIndex::on_search_entry_key_press_event (GdkEventKey* event)
{
	switch (event -> keyval)
	{
		case GDK_KEY_Return:
		case GDK_KEY_KP_Enter:
		{
			try
			{
				setTypes ({ nodeTypes .at (getSearchEntry () .get_text ()) });
			}
			catch (const std::out_of_range &)
			{
			   setTypes ({ });
			}

			return true;
		}
		case GDK_KEY_Escape:
		{
			getSearchEntry () .set_text ("");
			getTreeView () .grab_focus ();
			return true;
		}
		default:
			break;
	}

	return false;
}

bool
NodeIndex::on_search_entry_match_selected (const Gtk::TreeModel::iterator & iter)
{
	try
	{
	   Glib::ustring typeName;

	   iter -> get_value (Search::TYPE_NAME, typeName);

		setTypes ({ nodeTypes .at (typeName) });
	}
	catch (const std::out_of_range &)
	{
	   setTypes ({ });
	}

	return false;
}

void
NodeIndex::on_row_activated (const Gtk::TreeModel::Path & path, Gtk::TreeViewColumn*)
{
	node = nodes [path .front ()];

	// Select node.

	switch (index)
	{
		case NAMED_NODES_INDEX:
		case TYPE_INDEX:
		{
			const X3D::MFNode selection = { node };

			node -> getBrowser () -> getSelection () -> setChildren (selection);
			getBrowserWindow () -> expandNodes (selection);
			break;
		}
		default:
			break;
	}
}

void
NodeIndex::set_adjustment (const Glib::RefPtr <Gtk::Adjustment> & adjustment, const double value)
{
	adjustment -> set_value (value);
}

NodeIndex::~NodeIndex ()
{
	dispose ();
}

} // puck
} // titania
