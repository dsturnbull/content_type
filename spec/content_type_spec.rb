require 'tempfile'
require 'benchmark'
require 'content_type'

describe ContentType do
  before do
    @img = 'spec/fixtures/grindewald.jpg'
    @pdf = 'spec/fixtures/pdftest.pdf'
    @lzm = 'spec/fixtures/compressed.jpg.lz'
  end

  context 'initialising' do
    it 'should take a file path' do
      path = @img
      ct = ContentType.new(path)
      ct.filepath.should == path
    end

    it 'should check that the file exists' do
      lambda {
        ct = ContentType.new('poopstain')
      }.should raise_error ArgumentError
    end

    it 'should memoize the content type' do
      ct = ContentType.new(@img)
      lambda {
        ct.content_type.should == 'image/jpeg'
      }.should change(ct, :processed).to(true)
    end
  end

  context 'detecting mime type' do
    it 'should detect images' do
      ct = ContentType.new(@img)
      ct.content_type.should == 'image/jpeg'
    end

    it 'should detect pdfs' do
      ct = ContentType.new(@pdf)
      ct.content_type.should == 'application/pdf'
    end

    it 'should detect lzma files' do
      ct = ContentType.new(@lzm)
      ct.content_type.should == 'application/x-lzip'
    end
  end

  context 'comparing with shelling out' do
    it 'should be much faster' do
      Benchmark.bm(5) do |x|
        x.report('shell') do
          100.times do
            `file -i #{@img}`
          end
        end

        x.report('ext') do
          100.times do
            ContentType.new(@img).content_type
          end
        end
      end
    end
  end

  context 'when using the file class methods' do
    it 'should have File.content_type(path)' do
      File.content_type(@img).should == 'image/jpeg'
    end

    it 'should have File#content_type' do
      File.open(@img).content_type.should == 'image/jpeg'
    end

    it 'should make it into tempfile' do
      t = Tempfile.new('hi')
      t.content_type.should == 'application/x-empty'
      t << File.read(@img)
      t.content_type.should == 'image/jpeg'
      File.unlink(t.path)
    end
  end
end
