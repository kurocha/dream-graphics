//
//  VertexArray.h
//  Dream
//
//  Created by Samuel Williams on 16/02/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_VERTEX_ARRAY_H
#define _DREAM_CLIENT_GRAPHICS_VERTEX_ARRAY_H

#include "Buffer.h"

namespace Dream
{
	namespace Graphics
	{
		template <typename ElementT>
		struct BasicVertex {
			ElementT element;
		};

		class VertexArray : private NonCopyable {
		protected:
			GLuint _handle;

			void bind();
			void unbind();

		public:
			VertexArray();
			~VertexArray();

			GLuint handle() const { return _handle; }

			class Binding;

			class Attributes {
			protected:
				Binding & _binding;

			public:
				Attributes(Binding & binding) : _binding(binding) {
				}

				struct Location {
					Attributes & attributes;
					GLuint index;

					template <class T, typename U>
					void operator=(U T::* member) {
						attributes.associate(index, member);
					}
				};

				Location operator[](GLuint index) {
					return (Location){*this, index};
				}

				template <class T, typename U>
				void associate(GLuint index, U T::* member, bool normalized = false);
			};

			class Binding : private NonCopyable {
			protected:
				VertexArray * _vertex_array;

			public:
				Binding(VertexArray * vertex_array) : _vertex_array(vertex_array) {
					_vertex_array->bind();
				}

				Binding(Binding && other) : _vertex_array(other._vertex_array) {
					other._vertex_array = NULL;
				}

				~Binding() {
					if (_vertex_array)
						_vertex_array->unbind();
				}

				void set_attribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, std::ptrdiff_t offset);

				void enable(GLuint index);
				void disable(GLuint index);

				// These functions facilitate canonical usage where data is stored in vertex buffers.
				void draw_elements(GLenum mode, GLsizei count, GLenum type);
				void draw_arrays(GLenum mode, GLint first, GLsizei count);

				void attach(BufferHandle<GL_ELEMENT_ARRAY_BUFFER> & buffer) {
					buffer.bind();
				}

				Attributes attach(BufferHandle<GL_ARRAY_BUFFER> & buffer) {
					buffer.bind();

					Attributes attributes(*this);

					return attributes;
				}
			};

			Binding binding() {
				Binding binding(this);

				// Is this approach really efficient?
				return std::move(binding);
			}
		};

		template <class T, typename U>
		void VertexArray::Attributes::associate(GLuint index, U T::* member, bool normalized) {
			_binding.set_attribute(index, std::tuple_size<typename U::array>::value, GLTypeTraits<typename std::tuple_element<0, typename U::array>::type>::TYPE, normalized, sizeof(T), member_offset(member));

			// We assume that the attributes are enabled by default:
			_binding.enable(index);
		}
	}
}

#endif
