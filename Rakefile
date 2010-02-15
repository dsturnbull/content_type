require 'rake'
require 'spec'
require 'spec/rake/spectask'

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
  `rm *.o *.so *.bundle 2>/dev/null`
end
