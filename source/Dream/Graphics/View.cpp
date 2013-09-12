//
//  Graphics/View.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 19/06/07.
//  Copyright 2009 Orion Transfer Ltd. All rights reserved.
//

#include "View.h"
#include "WireframeRenderer.h"
#include <Dream/Display/Scene.h>

#include <Euclid/Numerics/Vector.IO.h>

namespace Dream
{
	namespace Graphics
	{
		View::Controller::Controller () {
			_static_focused_view = NULL;
			_dynamic_focused_view = NULL;
		}

		View::Controller::~Controller () {
		}

		void View::Controller::set_principal_view (Ptr<View> view) {
			_static_focused_view = NULL;
			_dynamic_focused_view = NULL;

			_principal = view;
		}

		bool View::Controller::resize(const ResizeInput &input) {
			_principal->resize(input);
			return true;
		}

		bool View::Controller::button(const ButtonInput &input) {
			if (_static_focused_view)
				return _static_focused_view->button(input);

			return false;
		}

		bool View::Controller::motion(const MotionInput &input) {
			bool result = false;

			View * current = dynamic_focused_view();
			std::vector<View*> views;

			views_to_point(input.current_position(), views);

			View * next = NULL;

			if (views.size()) {
				// The mouse is over some views, grab the last child
				next = views.back();

				if (current != next) {
					// The dynamic focus needs to be updated
					set_dynamic_focused_view(next);
				}
			} else {
				// Set null view
				set_dynamic_focused_view(NULL);
			}

			if (input.state() == Pressed) {
				// next is NULL if clicked outside a bounding view
				set_static_focused_view(next);
			}

			if (next) {
				views.clear();
				views_to_point(input.previous_position().reduce(), views);
				View *prev = NULL;

				if (views.size()) {
					prev = views.back();

					if (prev->is_enabled())
						prev->motion(input);
				}

				if (next->is_enabled())
					result = next->motion(input);
			}

			return result;
		}

		void View::Controller::set_dynamic_focused_view(Ptr<View> view) {
			_dynamic_focused_view = view;
		}

		void View::Controller::set_static_focused_view(Ptr<View> view) {
			_static_focused_view = view;
		}

		void View::Controller::views_to_point (const Vec2 &point, std::vector<View*> &views) {
			_principal->views_to_point(point, views);
		}

		void View::Controller::did_become_current (ISceneManager * manager, IScene * scene) {
			DREAM_ASSERT(_principal);

			// Fire the initial resize event:
			ResizeInput initial_layout_size(manager->display_context()->size());
			process(initial_layout_size);

			_principal->did_become_current(manager, scene);
		}

		void View::Controller::will_revoke_current (ISceneManager * manager, IScene * scene) {
			_principal->will_revoke_current(manager, scene);
		}

		void View::Controller::dump_structure (std::ostream & outp) {
			outp << "View Controller " << this << std::endl;
			_principal->dump_structure(outp, 1);
		}

		void View::init () {
			_bounds = AlignedBox2(ZERO);
			_rotation = 0;
			_offset = 0;
			_orientation = 0;
			_scale = 0;
			_size = 0;
		}

		void View::dump_structure (std::ostream & outp, unsigned indent) {
			using namespace std;

			string space(indent, '\t');

			outp << space << "View" << endl;
			//outp << space << "Enabled = " << _enabled << " Bounds = " << _bounds << " Rotation = " << _rotation << " Subviews = " << _subviews.size() << endl;
			outp << space << "Size = " << _size << " Scale = " << _scale << " Offset = " << _offset << " Orientation = " << _orientation << endl;

			for (auto subview : _subviews) {
				subview->dump_structure(outp, indent + 1);
			}
		}

		View::View (Ptr<View> parent) : _enabled(true), _parent(parent)
		{
			// This is a subview of _parent
			_parent->register_child_view(this);
			_controller = _parent->controller();
			_enabled = _parent->is_enabled();

			init();
		}

		View::View (Ptr<Controller> controller) : _enabled(true), _controller(controller)
		{
			_controller->set_principal_view(this);
			init();
		}

		View::View (Ptr<Controller> controller, const AlignedBox<2> &bounds) : _enabled(true), _controller(controller), _bounds(bounds)
		{
			_controller->set_principal_view(this);
			init();
		}

		View::~View() {
		}

		void View::set_size_and_scale (const Vec2 &size, const Vec2 &scale) {
			_size = size;
			_scale = scale;
		}

		void View::set_offset_and_orientation (const Vec2 &offset, const Vec2 &orientation) {
			_offset = offset;
			_orientation = orientation;
		}

		void View::set_rotation (const RealT rotation) {
			_rotation = rotation;
		}

		void View::register_child_view (Ref<View> child) {
			_subviews.push_back(child);
		}

		bool View::is_principal_view () const {
			return !_parent;
		}

		bool View::intersects_with (const Vec2 &point) {
			return _bounds.contains_point(point);
		}

		void View::views_to_point (const Vec2 &point, std::vector<View*> &views, bool first) {
			if (first && intersects_with(point))
				views.push_back(this);

			for (auto subview : _subviews) {
				if (subview->intersects_with(point)) {
					views.push_back(subview.get());

					// Descend into child view
					subview->views_to_point(point, views, false);

					break;
				}
			}
		}

		void View::did_become_current (ISceneManager * manager, IScene * scene) {
			for (auto subview : _subviews) {
				subview->did_become_current(manager, scene);
			}
		}

		void View::will_revoke_current (ISceneManager * manager, IScene * scene) {
			for (auto subview : _subviews) {
				subview->will_revoke_current(manager, scene);
			}
		}

		bool View::resize(const ResizeInput &input) {
			if (is_principal_view()) {
				// We need to resize the entire box.
				_bounds.min() = 0;

				//LogBuffer buffer;
				//buffer << "Resizing principal from " << _bounds.origin() << "; " << _bounds.size();

				_bounds.set_size_from_origin(input.new_size());

				//buffer << " to " << _bounds.origin() << "; " << _bounds.size() << "(" << input.new_size() << ")";
				//logger()->log(LOG_DEBUG, buffer);
			} else {
				//std::cout << "Resizing view from " << _bounds.origin() << "; " << _bounds.size();
				_bounds.set_size_from_origin(_parent->bounds().size() * _scale + _size);
				_bounds.align_within_super_box(_parent->bounds(), _orientation);
				_bounds.translate_by(_offset);
				//std::cout << " to " << _bounds.origin() << "; " << _bounds.size() << std::endl;
			}

			bool result = false;

			// Propagate resize input down
			for (auto subview : _subviews) {
				result = result || subview->resize(input);
			}

			return result;
		}

		bool View::button(const ButtonInput &input) {
			if (!_enabled) return false;

			bool handled = false;

			for (auto subview : _subviews) {
				handled = handled || subview->button(input);
			}

			return handled;
		}

		bool View::motion(const MotionInput &input) {
			return false;
		}

		bool View::has_dynamic_focus () const {
			return controller()->dynamic_focused_view().get() == this;
		}

		bool View::has_static_focus () const {
			return controller()->static_focused_view().get() == this;
		}

		bool View::is_enabled () const {
			return _enabled;
		}

		void View::enable () {
			_enabled = true;

			for (auto subview : _subviews) {
				subview->enable();
			}
		}

		void View::disable () {
			_enabled = false;

			for (auto subview : _subviews) {
				subview->disable();
			}
		}

// MARK: -

		/*


		ButtonView::ButtonView (Ptr<View> parent)
		    : View(parent)
		{

		}

		ButtonView::~ButtonView ()
		{

		}

		bool ButtonView::motion (const MotionInput & input)
		{
		    if (onActivate && hasStaticFocus()) {
		        return onActivate(input);
		    }

		    return false;
		}
		*/

// MARK: -



		ImageView::ImageView (Ptr<View> parent) : View(parent) {
		}

		ImageView::~ImageView () {
		}

		void ImageView::set_default_image (Ref<Image> image) {
			_default_image = image;
		}

		void ImageView::set_dynamic_focus_image (Ref<Image> image) {
			_dynamic_focus_image = image;
		}

		void ImageView::set_static_focus_image (Ref<Image> image) {
			_static_focus_image = image;
		}

		Ref<Image> ImageView::current_image () const {
			Ref<Image> image = _default_image;

			if (has_static_focus() && _static_focus_image) {
				image = _static_focus_image;
			} else if (has_dynamic_focus() && _dynamic_focus_image) {
				image = _dynamic_focus_image;
			}

			return image;
		}

		bool ImageView::motion (const MotionInput & input) {
			if (on_activate && has_static_focus()) {
				return on_activate(input);
			}

			return false;
		}

// MARK: -

		TextView::TextView (Ptr<View> parent, Ptr<Font> font) : View(parent), _offset(0) {
			_text_buffer = new Text::TextBuffer(font);
		}

		TextView::~TextView () {
		}

		Ref<TextBuffer> TextView::text_buffer () {
			return _text_buffer;
		}

		void TextView::set_editable (bool flag) {
			_editable = flag;
		}

		bool TextView::editable () {
			return _editable;
		}

		Ref<Image> TextView::current_image() const {
			bool updated = false;
			Ref<Image> text_image = _text_buffer->render_text(updated);

			//if (updated) {
			//	AlignedBox<2> text_bounds = _bounds;
			//	text_bounds.set_center_and_size(text_bounds.center(), text_pixel_buffer->size().reduce());
			//}

			return text_image;
		}

		bool TextView::button (const ButtonInput & input) {
			if (_editable && has_static_focus()) {
				_text_buffer->insert_character_at_offset(_offset, input.key().button());
				_offset += 1;

				return true;
			}

			return false;
		}

		bool TextView::motion (const MotionInput & input) {
			return View::motion(input);
		}
	}
}
