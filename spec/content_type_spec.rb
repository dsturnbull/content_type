require 'rubygems'
require 'should_be_faster'
require 'tempfile'
require 'ext/content_type'

describe ContentType do
  before do
    @img = 'spec/fixtures/grindewald.jpg'
    @pdf = 'spec/fixtures/pdftest.pdf'
    @lzm = 'spec/fixtures/compressed.jpg.lz'
    @dcx = 'spec/fixtures/wordtest.docx'
    @dot = 'spec/fixtures/.supercabanafuntimekgozzzzzzzzzzzzzzzzzzzz'
    @pdf_with_charset = 'spec/fixtures/bash.pdf'
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

  context 'file ext overrides' do
    it 'should detect docx files' do
      ct = ContentType.new(@dcx)
      ct.content_type.should == 'application/vnd.openxmlformats-officedocument.wordprocessingml.document'
    end

    it 'should handle dot files' do
      ct = ContentType.new(@dot)
      ct.content_type.should == 'text/plain'
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

    it 'should detect mime types with charsets by ignoring the charset' do
      ct = ContentType.new(@pdf_with_charset)
      ct.content_type.should == 'application/pdf'
    end

    it 'should detect lzma files' do
      ct = ContentType.new(@lzm)
      ct.content_type.should == 'application/x-lzip'
    end
  end

  context 'comparing with shelling out' do
    it 'should be much faster' do
      shell = lambda { `file -i #{@img}` }
      ext   = lambda { ContentType.new(@img).content_type }
      ext.should be_at_least(5).times.faster_than(shell)
    end

    it 'should slower to reinstantiate' do
      @ct = ContentType.new(@img)
      memo = lambda { @ct.content_type }
      kgo  = lambda { ContentType.new(@img).content_type }
      kgo.should be_at_least(5).times.slower_than(memo, :iterations => 1000)
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
