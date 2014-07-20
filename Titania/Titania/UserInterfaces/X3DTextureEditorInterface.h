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
#ifndef __TMP_GLAD2CPP_TEXTURE_EDITOR_H__
#define __TMP_GLAD2CPP_TEXTURE_EDITOR_H__

#include "../Base/X3DEditorInterface.h"
#include <gtkmm.h>
#include <string>

namespace titania {
namespace puck {

using namespace Gtk;

class X3DTextureEditorInterface :
	public X3DEditorInterface
{
public:

	template <class ... Arguments>
	X3DTextureEditorInterface (const std::string & filename, const Arguments & ... arguments) :
		X3DEditorInterface (m_widgetName, arguments ...),
		          filename (filename)
	{ create (filename); }

	const Glib::RefPtr <Gtk::Builder> &
	getBuilder () const { return m_builder; }

	const std::string &
	getWidgetName () const { return m_widgetName; }

	void
	updateWidget (const Glib::ustring & name) const
	{ getBuilder () -> add_from_file (filename, name); }

	void
	updateWidgets (const std::vector <Glib::ustring> & names) const
	{ getBuilder () -> add_from_file (filename, names); }

	template <class Type>
	Type*
	getWidget (const std::string & name) const
	{
		Type* widget = nullptr;

		m_builder -> get_widget (name, widget);
		widget -> set_name (name);
		return widget;
	}

	const Glib::RefPtr <Gtk::Adjustment> &
	getTexturePropertiesAnisotropicDegreeAdjustment () const
	{ return m_TexturePropertiesAnisotropicDegreeAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTexturePropertiesBorderWidthAdjustment () const
	{ return m_TexturePropertiesBorderWidthAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTexturePropertiesTexturePriorityAdjustment () const
	{ return m_TexturePropertiesTexturePriorityAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformCenterXAdjustment () const
	{ return m_TextureTransformCenterXAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformCenterYAdjustment () const
	{ return m_TextureTransformCenterYAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformRotationAdjustment () const
	{ return m_TextureTransformRotationAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformScaleXAdjustment () const
	{ return m_TextureTransformScaleXAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformScaleYAdjustment () const
	{ return m_TextureTransformScaleYAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformTranslationXAdjustment () const
	{ return m_TextureTransformTranslationXAdjustment; }

	const Glib::RefPtr <Gtk::Adjustment> &
	getTextureTransformTranslationYAdjustment () const
	{ return m_TextureTransformTranslationYAdjustment; }

	Gtk::Window &
	getWindow () const
	{ return *m_Window; }

	Gtk::Box &
	getWidget () const
	{ return *m_Widget; }

	Gtk::Expander &
	getTextureExpander () const
	{ return *m_TextureExpander; }

	Gtk::ComboBoxText &
	getTextureButton () const
	{ return *m_TextureButton; }

	Gtk::Button &
	getTextureUnlinkButton () const
	{ return *m_TextureUnlinkButton; }

	Gtk::Notebook &
	getTextureNotebook () const
	{ return *m_TextureNotebook; }

	Gtk::Label &
	getTextureLabel () const
	{ return *m_TextureLabel; }

	Gtk::Box &
	getImageTextureBox () const
	{ return *m_ImageTextureBox; }

	Gtk::Grid &
	getTexture2DBox () const
	{ return *m_Texture2DBox; }

	Gtk::CheckButton &
	getTexture2DNodeRepeatSCheckButton () const
	{ return *m_Texture2DNodeRepeatSCheckButton; }

	Gtk::CheckButton &
	getTexture2DNodeRepeatTCheckButton () const
	{ return *m_Texture2DNodeRepeatTCheckButton; }

	Gtk::Expander &
	getTexturePropertiesExpander () const
	{ return *m_TexturePropertiesExpander; }

	Gtk::CheckButton &
	getTexturePropertiesCheckButton () const
	{ return *m_TexturePropertiesCheckButton; }

	Gtk::Button &
	getTexturePropertiesUnlinkButton () const
	{ return *m_TexturePropertiesUnlinkButton; }

	Gtk::Grid &
	getTexturePropertiesBox () const
	{ return *m_TexturePropertiesBox; }

	Gtk::Expander &
	getTextureTransformExpander () const
	{ return *m_TextureTransformExpander; }

	Gtk::ComboBoxText &
	getTextureTransformButton () const
	{ return *m_TextureTransformButton; }

	Gtk::Button &
	getTextureTransformUnlinkButton () const
	{ return *m_TextureTransformUnlinkButton; }

	Gtk::Notebook &
	getTextureTransformNotebook () const
	{ return *m_TextureTransformNotebook; }

	Gtk::Label &
	getTextureTransformLabel () const
	{ return *m_TextureTransformLabel; }

	Gtk::Grid &
	getTextureTransformBox () const
	{ return *m_TextureTransformBox; }

	Gtk::SpinButton &
	getTextureTransformTranslationXSpinButton () const
	{ return *m_TextureTransformTranslationXSpinButton; }

	Gtk::SpinButton &
	getTextureTransformTranslationYSpinButton () const
	{ return *m_TextureTransformTranslationYSpinButton; }

	Gtk::SpinButton &
	getTextureTransformScaleXSpinButton () const
	{ return *m_TextureTransformScaleXSpinButton; }

	Gtk::SpinButton &
	getTextureTransformScaleYSpinButton () const
	{ return *m_TextureTransformScaleYSpinButton; }

	Gtk::SpinButton &
	getTextureTransformCenterXSpinButton () const
	{ return *m_TextureTransformCenterXSpinButton; }

	Gtk::SpinButton &
	getTextureTransformCenterYSpinButton () const
	{ return *m_TextureTransformCenterYSpinButton; }

	Gtk::SpinButton &
	getTextureTransformRotationSpinButton () const
	{ return *m_TextureTransformRotationSpinButton; }

	Gtk::Expander &
	getTextureCoordinateGeneratorExpander () const
	{ return *m_TextureCoordinateGeneratorExpander; }

	Gtk::Box &
	getTextureCoordinateGeneratorMainBox () const
	{ return *m_TextureCoordinateGeneratorMainBox; }

	Gtk::CheckButton &
	getTextureCoordinateGeneratorCheckButton () const
	{ return *m_TextureCoordinateGeneratorCheckButton; }

	Gtk::Button &
	getTextureCoordinateGeneratorUnlinkButton () const
	{ return *m_TextureCoordinateGeneratorUnlinkButton; }

	Gtk::Grid &
	getTextureCoordinateGeneratorBox () const
	{ return *m_TextureCoordinateGeneratorBox; }

	Gtk::ComboBoxText &
	getTextureCoordinateGeneratorModeButton () const
	{ return *m_TextureCoordinateGeneratorModeButton; }

	virtual
	void
	on_textureTransform_center_changed () = 0;

	virtual
	void
	on_textureTransform_rotation_changed () = 0;

	virtual
	void
	on_textureTransform_scale_changed () = 0;

	virtual
	void
	on_textureTransform_translation_changed () = 0;

	virtual
	void
	on_texture_changed () = 0;

	virtual
	void
	on_texture_unlink_clicked () = 0;

	virtual
	void
	on_texture2DNode_repeatS_toggled () = 0;

	virtual
	void
	on_texture2DNode_repeatT_toggled () = 0;

	virtual
	void
	on_textureProperties_toggled () = 0;

	virtual
	void
	on_textureProperties_unlink_clicked () = 0;

	virtual
	void
	on_textureTransform_changed () = 0;

	virtual
	void
	on_textureTransform_unlink_clicked () = 0;

	virtual
	void
	on_textureCoordinateGenerator_toggled () = 0;

	virtual
	void
	on_textureCoordinateGenerator_unlink_clicked () = 0;

	virtual
	void
	on_textureCoordinateGenerator_mode_changed () = 0;

	virtual
	~X3DTextureEditorInterface ();


private:

	void
	create (const std::string &);

	static const std::string m_widgetName;

	std::string                    filename;
	Glib::RefPtr <Gtk::Builder>    m_builder;
	Glib::RefPtr <Gtk::Adjustment> m_TexturePropertiesAnisotropicDegreeAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TexturePropertiesBorderWidthAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TexturePropertiesTexturePriorityAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformCenterXAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformCenterYAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformRotationAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformScaleXAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformScaleYAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformTranslationXAdjustment;
	Glib::RefPtr <Gtk::Adjustment> m_TextureTransformTranslationYAdjustment;
	Gtk::Window*                   m_Window;
	Gtk::Box*                      m_Widget;
	Gtk::Expander*                 m_TextureExpander;
	Gtk::ComboBoxText*             m_TextureButton;
	Gtk::Button*                   m_TextureUnlinkButton;
	Gtk::Notebook*                 m_TextureNotebook;
	Gtk::Label*                    m_TextureLabel;
	Gtk::Box*                      m_ImageTextureBox;
	Gtk::Grid*                     m_Texture2DBox;
	Gtk::CheckButton*              m_Texture2DNodeRepeatSCheckButton;
	Gtk::CheckButton*              m_Texture2DNodeRepeatTCheckButton;
	Gtk::Expander*                 m_TexturePropertiesExpander;
	Gtk::CheckButton*              m_TexturePropertiesCheckButton;
	Gtk::Button*                   m_TexturePropertiesUnlinkButton;
	Gtk::Grid*                     m_TexturePropertiesBox;
	Gtk::Expander*                 m_TextureTransformExpander;
	Gtk::ComboBoxText*             m_TextureTransformButton;
	Gtk::Button*                   m_TextureTransformUnlinkButton;
	Gtk::Notebook*                 m_TextureTransformNotebook;
	Gtk::Label*                    m_TextureTransformLabel;
	Gtk::Grid*                     m_TextureTransformBox;
	Gtk::SpinButton*               m_TextureTransformTranslationXSpinButton;
	Gtk::SpinButton*               m_TextureTransformTranslationYSpinButton;
	Gtk::SpinButton*               m_TextureTransformScaleXSpinButton;
	Gtk::SpinButton*               m_TextureTransformScaleYSpinButton;
	Gtk::SpinButton*               m_TextureTransformCenterXSpinButton;
	Gtk::SpinButton*               m_TextureTransformCenterYSpinButton;
	Gtk::SpinButton*               m_TextureTransformRotationSpinButton;
	Gtk::Expander*                 m_TextureCoordinateGeneratorExpander;
	Gtk::Box*                      m_TextureCoordinateGeneratorMainBox;
	Gtk::CheckButton*              m_TextureCoordinateGeneratorCheckButton;
	Gtk::Button*                   m_TextureCoordinateGeneratorUnlinkButton;
	Gtk::Grid*                     m_TextureCoordinateGeneratorBox;
	Gtk::ComboBoxText*             m_TextureCoordinateGeneratorModeButton;

};

} // puck
} // titania

#endif
