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
end
