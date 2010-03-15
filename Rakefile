require 'rake'
require 'spec'
require 'spec/rake/spectask'

require 'vendor/gems/environment'

task :default => [:clean, :make, :spec]

task :make do
  puts `ruby extconf.rb`
  puts `make`
end

Spec::Rake::SpecTask.new(:spec) do |t|
  t.spec_opts = %w(-fs -c)
  t.spec_files = FileList['spec/*_spec.rb']
end

task :clean do
  `rm -r *.o *.so *.bundle conftest.dSYM 2>/dev/null`
end

require 'jeweler'
Jeweler::Tasks.new do |s|
  s.name = 'content_type'
  s.summary = 'libmagic bindings to quickly determine content type of files'
  s.email = 'dsturnbull@me.com'
  s.homepage = 'http://github.com/dsturnbull/content_type'
  s.description =<<-eod
      Provides ContentType#content_type, File#content_type and
      File::content_type methods to determine mime type
  eod
  s.authors = ['David Turnbull']
  s.files = ['Rakefile', 'content_type.c', 'extconf.rb']
end
