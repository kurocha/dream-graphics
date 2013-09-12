//
//  Graphics/Graphics.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/12/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Graphics.h"

namespace Dream
{
	namespace Graphics
	{
		void check_graphics_error()
		{
#ifdef DREAM_DEBUG
			GLenum error = GL_NO_ERROR;

			while ((error = glGetError()) != GL_NO_ERROR) {
				logger()->log(LOG_ERROR, LogBuffer() << "OpenGL Error #" << error);

				// Abort due to error
				DREAM_ASSERT(error == GL_NO_ERROR);
			}

#endif
		}
	}
}
