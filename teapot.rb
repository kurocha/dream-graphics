
#
#  This file is part of the "Teapot" project, and is released under the MIT license.
#

teapot_version "0.8.0"

define_project "Dream Graphics" do |project|
	project.add_author "Samuel Williams"
	project.license = "MIT License"
	
	project.version = "0.1.0"
end

define_target "dream-graphics" do |target|
	target.build do |environment|
		build_directory(package.path, 'source', environment)
	end
	
	target.depends "Language/C++11"
	target.depends "Library/Dream/Display"
	target.depends "Library/Dream/Imaging"
	target.depends "Library/Dream/Text"

	target.depends "Library/OpenGL"

	target.depends "Library/Euclid"
	
	target.provides "Library/Dream/Graphics" do
		append linkflags "-lDreamGraphics"
	end
end

define_target "dream-graphics-tests" do |target|
	target.build do |environment|
		build_directory(package.path, 'test', environment)
	end
	
	target.run do |environment|
		run_executable("bin/dream-graphics-test-runner", environment)
	end
	
	target.depends "Library/Dream/Graphics"
	target.depends "Library/UnitTest"
	
	target.provides "Test/Dream/Graphics"
end

define_configuration "dream-graphics" do |configuration|
	configuration.public!
	
	configuration.require "dream"
	configuration.require "dream-imaging"
	configuration.require "dream-text"
	configuration.require "dream-display"
end

define_configuration "travis" do |configuration|
	configuration[:source] = "https://github.com/dream-framework"
	
	configuration.require "platforms"
	configuration.import "dream-graphics"
	
	configuration[:run] = ["Test/Dream/Graphics"]
end
