//
//  Graphics/View.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 23/07/09.
//  Copyright 2009 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_VIEW_H
#define _DREAM_CLIENT_GRAPHICS_VIEW_H

#include "Graphics.h"
#include <Dream/Text/Font.h>
#include <Dream/Text/TextBuffer.h>

#include "Renderer.h"
#include "ImageRenderer.h"
#include <Dream/Imaging/Image.h>

#include <Euclid/Geometry/AlignedBox.h>

namespace Dream
{
	namespace Graphics
	{
		using namespace Euclid::Numerics::Constants;
		using Euclid::Geometry::AlignedBox;
		using Imaging::Image;

		/** Implements the basic structure of user interface layout and display.
		*/
		class View : public Object, implements INode, implements ILayer {
		public:
			/** The top level user interface controller.
			*/
			class Controller : public Object, implements ILayer {
			protected:
				Ref<View> _principal;
				Ref<View> _static_focused_view, _dynamic_focused_view;

				void views_to_point (const Vec2 &point, std::vector<View*> &views);
				void set_principal_view (Ptr<View> view);

				friend class View;
			public:
				Controller ();
				virtual ~Controller ();

				virtual bool resize (const ResizeInput &);
				virtual bool button (const ButtonInput &);
				virtual bool motion (const MotionInput &);

				/// Returns the top level view.
				Ref<View> principal_view () { return _principal; }
				const Ref<View> principal_view () const { return _principal; }

				/// Set the current dynamically focused view.
				/// This is normally determined by the location of the mouse or pointer device.
				void set_dynamic_focused_view(Ptr<View> view);

				/// Get the currently dynamically focused view.
				Ptr<View> dynamic_focused_view() const { return _dynamic_focused_view; }

				// Set the current statically focused view.
				/// This is normally determined by a click of the mouse or keyboard.
				void set_static_focused_view(Ptr<View> view);

				/// Get the current statically focused view.
				Ptr<View> static_focused_view() const { return _static_focused_view; }

				virtual void did_become_current (ISceneManager * manager, IScene * scene);
				virtual void will_revoke_current (ISceneManager * manager, IScene * scene);

				virtual void dump_structure (std::ostream & output);
			};

		private:
			// This is used for setting up the parent <=> child relationship
			void register_child_view (Ref<View> child);
			void views_to_point (const Vec2 &point, std::vector<View*> &views, bool first = true);

		protected:
			bool _enabled;
			Ptr<Controller> _controller;
			Ptr<View> _parent;

			std::vector<Ref<View>> _subviews;

			Vec2 _size, _scale;
			Vec2 _offset, _orientation;
			StringT _tag;

			AlignedBox<2> _bounds;
			RealT _rotation;

			virtual bool intersects_with (const Vec2 & point);

		private:
			void init ();

		public:
			/// Initialize the view inside a parent view.
			View (Ptr<View> parent);

			/// Initialize the view as the top level view in a controller.
			View (Ptr<Controller> controller);

			/// Initialize the view as a top level view and set its bounding box.
			View (Ptr<Controller> controller, const AlignedBox<2> &bounds);

			virtual ~View();

			/// Return the top level controller.
			virtual Ptr<Controller> controller () { return _controller; }
			virtual const Ptr<Controller> controller () const { return _controller; }

			/// @returns true if the view is the top level view in the heirarchy.
			bool is_principal_view () const;

			/// Set the size and scale of the view.
			/// The size is measured in units (typically pixels), while scale is specified using
			/// a percentage of the containing view.
			void set_size_and_scale (const Vec2 &size, const Vec2 &scale);

			/// Set the offset and orientation of the view.
			/// The offset is measured in units (typically pixels), while the orientation is
			/// specified as a percentage location in the containing view.
			/// @sa AlignedBox::alignWithinSuperBox
			void set_offset_and_orientation (const Vec2 &offset, const Vec2 &orientation);

			/// Set the rotation of the view box relative to the containing view.
			void set_rotation (const RealT rotation);

			virtual const Ptr<View> parent_view () const { return _parent; }
			virtual Ptr<View> parent_view () { return _parent; }

			const std::vector<Ref<View>> & children_views () const { return _subviews; }
			std::vector<Ref<View>> & children_views () { return _subviews; }

			// Viewport in normalized coordinates 0.0 ... 1.0
			virtual const AlignedBox<2> & bounds () const { return _bounds; }
			virtual RealT rotation () const { return _rotation; }

			// Input Processing
			virtual bool resize (const ResizeInput &);
			virtual bool button (const ButtonInput &);
			virtual bool motion (const MotionInput &);

			virtual void did_become_current (ISceneManager * manager, IScene * scene);
			virtual void will_revoke_current (ISceneManager * manager, IScene * scene);

			bool has_dynamic_focus () const;
			bool has_static_focus () const;

			// Enabled / Disabled - whether it will respond to events and propagate events
			bool is_enabled () const;
			virtual void enable ();
			virtual void disable ();

			void set_tag (const StringT & tag) { _tag = tag; }
			StringT tag () const { return _tag; }

			virtual void dump_structure (std::ostream & outp, unsigned indent = 0);
		};

		class ImageView : public View {
		protected:
			Ref<Image> _default_image;
			Ref<Image> _dynamic_focus_image;
			Ref<Image> _static_focus_image;

		public:
			ImageView (Ptr<View> parent);
			virtual ~ImageView ();

			void set_default_image (Ref<Image> image);
			void set_dynamic_focus_image (Ref<Image> image);
			void set_static_focus_image (Ref<Image> image);

			Ref<Image> current_image() const;

			virtual bool motion (const MotionInput &);

			typedef std::function<bool (const Input &)> OnActivateT;
			OnActivateT on_activate;
		};

		using Text::Font;
		using Text::TextBuffer;

		class TextView : public View {
		protected:
			Ref<Text::TextBuffer> _text_buffer;

			bool _editable;
			unsigned int _offset;

		public:
			TextView (Ptr<View> parent, Ptr<Font> font);
			virtual ~TextView ();

			Ref<TextBuffer> text_buffer ();

			Ref<Image> current_image() const;

			void set_editable (bool flag = true);
			bool editable ();

			virtual bool button (const ButtonInput &);
			virtual bool motion (const MotionInput &);
		};
	}
}

#endif
