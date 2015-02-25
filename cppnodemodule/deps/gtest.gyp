#
# Copyright (C) 2012-2013, The Particle project authors.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# The GNU General Public License is contained in the file LICENSE.
#

{

  'variables': {
    'gtest_home': 'gtest',
  },
  'targets': [
    {
      'target_name': 'gtest',
      'type': 'static_library',
	  'includes':[
		'osconfig.gypi',
	  ],
      'include_dirs': [
        '<(gtest_home)/include',
        '<(gtest_home)',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          '<(gtest_home)/include',
        ],
        'conditions': [
          ['OS == "mac"', {
            'defines': ['GTEST_LANG_CXX11=1', 'GTEST_USE_OWN_TR1_TUPLE=1'],
          }],
        ],
      },
      'sources': [
        '<(gtest_home)/src/gtest-all.cc',
        '<(gtest_home)/src/gtest-death-test.cc',
        '<(gtest_home)/src/gtest-filepath.cc',
        '<(gtest_home)/src/gtest-internal-inl.h',
        '<(gtest_home)/src/gtest-port.cc',
        '<(gtest_home)/src/gtest-printers.cc',
        '<(gtest_home)/src/gtest-test-part.cc',
        '<(gtest_home)/src/gtest-typed-test.cc',
        '<(gtest_home)/src/gtest.cc',
      ],
    },
    {
      'target_name': 'gtest_main',
      'type': 'static_library',
      'dependencies': [
        'gtest',
      ],
      'sources': [
        '<(gtest_home)/src/gtest_main.cc',
      ],
    }
  ]
}
