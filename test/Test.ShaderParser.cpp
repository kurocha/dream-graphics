
#include <UnitTest/UnitTest.h>

#include <Dream/Graphics/ShaderParser.h>

namespace Dream
{
	namespace Graphics
	{
		const char * BasicShader =
			"@shader\n"
			"#version 330\n"
			"@vertex\n"
			"void main() { vertex(); }\n"
			"@fragment\n"
			"void main() { fragment(); }\n";
		
		UnitTest::Suite ShaderParserTestSuite {
			"Dream::Graphics::ShaderParser",

			{"Parsing",
				[](UnitTest::Examiner & examiner) {
					StaticBuffer buffer = StaticBuffer::for_cstring(BasicShader);
					
					ShaderParser parser(buffer);
					
					auto & source_buffers = parser.source_buffers();
					
					examiner << "Both vertex and fragment shader extracted" << std::endl;
					examiner.check_equal(source_buffers.size(), 3);
				}
			},
		};
	}
}
