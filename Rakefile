require 'rake'
require 'jeweler'
require 'rspec'
require 'rspec/core/rake_task'

task :default => [:clean, :make, :spec]

task :make do
  Dir.chdir 'ext' do
    puts `ruby extconf.rb`
    puts `make`
  end
end

RSpec::Core::RakeTask.new(:spec) do |t|
  t.rspec_opts = %w(-fs -c)
  t.pattern = FileList['spec/*_spec.rb']
end

task :clean do
  `rm -r ext/*.o ext/*.so ext/*.bundle ext/conftest.dSYM 2>/dev/null`
end

Jeweler::Tasks.new do |s|
  s.name = 'content_type'
  s.summary = 'libmagic bindings to quickly determine content type of files'
  s.email = 'dsturnbull@me.com'
  s.homepage = 'http://github.com/dsturnbull/content_type'
  s.description =<<-eod
      Provides ContentType#content_type, File#content_type, File::content_type
      and String#content_type methods to determine mime type
  eod
  s.executables = []
  s.authors = ['David Turnbull']
  s.files = ['Rakefile', 'ext/content_type.c', 'ext/extconf.rb']
end
