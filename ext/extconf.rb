require 'mkmf'
RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

extension_name = 'content_type'
dir_config(extension_name)

brew_magic = Dir['/usr/local/Cellar/libmagic/**/magic.h']
if brew_magic
  brew_dir = File.dirname(File.dirname(brew_magic[0]))
  find_header('magic.h', File.join(brew_dir, 'include'))
  find_library('magic', 'magic_open', File.join(brew_dir, 'lib'))
else
  find_header('magic.h', '/opt/local/include')
  find_library('magic', 'magic_open', '/opt/local/lib')
end

$CFLAGS += ' -Werror -Wall'

create_makefile(extension_name)
