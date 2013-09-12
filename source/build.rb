
compile_library 'DreamGraphics' do
	def source_files(environment)
		FileList[root, 'Dream/**/*.cpp']
	end
end

copy_headers do
	def source_files(environment)
		FileList[root, 'Dream/**/*.h']
	end
end
