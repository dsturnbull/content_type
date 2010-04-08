require 'mkmf'

extension_name = 'content_type'
dir_config(extension_name)

find_header('magic.h', '/opt/local/include')
find_library('magic', 'magic_open', '/opt/local/lib')

$CFLAGS += ' -Werror -Wall'

create_makefile(extension_name)
