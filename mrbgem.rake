MRuby::Gem::Specification.new('mruby-sdl2-ttf') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Moon4u'

  spec.add_dependency('mruby-sdl2')
  spec.cc.flags << '`sdl2-config --cflags`'
  spec.linker.flags_before_libraries << '`sdl2-config --libs` -lSDL2_ttf'
end
