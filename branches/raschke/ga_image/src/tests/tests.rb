#!/usr/bin/env ruby

require 'test/unit'

class ToolsTest < Test::Unit::TestCase
  def gi(toolname)
    "../tools/gi_#{toolname}/gi_#{toolname}"
  end
  
  def assert_exec(command)
    assert system("#{command} &> /dev/null"), `#{command}`
  end
  
  def assert_equal_images(file1, file2)
    assert_exec "#{gi 'testhelper'} --assert-equal #{file1} #{file2}"
  end

  def assert_tool_works(command, source)
    tool_name = command[/[^ ]*/].split('/')[-1]
    
    clean_command = command.gsub(/[^A-Za-z-]/, '_')
    
    output = "output/#{tool_name}_#{source}"
    reference = "reference/#{tool_name}_#{source}"
    
    assert_exec command % ["fixtures/" + source, output]
    assert_equal_images(output, reference)
  end
  
  def test_sanity_pgm
    assert_equal_images("fixtures/face.pgm", "fixtures/face.pgm")
  end
  
  def test_sanity_ppm
    assert_equal_images("fixtures/face.ppm", "fixtures/face.ppm")
  end
  
  def test_image_cpp
    assert_exec("../gaimage/test_image")
  end
  
  def test_copy_pgm
    assert_tool_works("#{gi 'testhelper'} --copy %s %s",
                      "face.pgm")
  end
  
  def test_copy_ppm
    assert_tool_works("#{gi 'testhelper'} --copy %s %s",
                      "face.ppm")
  end
  
  def test_canny
    assert_tool_works("#{gi 'canny'} %s %s 0.03 0.06", "face.ppm")
  end
end
