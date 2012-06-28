srcdir = '.'
blddir = 'build'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  conf.env['CXX']      = 'g++-4.6'
  conf.env['CXXFLAGS'] = '-std=c++0x'
  conf.env['LIB']      = ['icuio', 'boost_regex']

def build(bld):
  obj          = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target   = 'icu'
  obj.source   = 'export_to_nodejs.cpp'
