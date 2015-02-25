{
	'conditions': [
		['OS=="win"', {
			'variables': {
				'module_root_dir':'../../proj',		
			},
		}],
		['OS=="linux"',{ 
			'variables': {
				'module_root_dir':'../../proj',			
			},
		}]
	],
	'targets': [
    {  
		'target_name': "upgradedb",
		'type':'executable',	
		 'includes':[
			'../../deps/osconfig.gypi',
			'../../proj/configd.gypi',
		],
		'dependencies': [
			'../../deps/protobuf3.gyp:protobuf_full_do_not_use',
			'../../deps/json2pb.gyp:json2pb',
			'../../deps/jansson.gyp:jansson',		
			'../../deps/encryptstring.gyp:encryptstring',
		],
		'include_dirs': [
			'<(jansson_config_h_dir)',
			'../../deps/mysql-connector-c-6.1.5-src/include',
			'../../src/mylib',
			'../../deps/boost_1_57_0',
			'../../deps/protobuf3/src',
			'../../deps/json2pb-master',
			'../../deps/soci-3.2.2/core',
			'../../deps/soci-3.2.2/backends/mysql',
			'../../deps/encryptstring',
		],
		'sources': [ 

			'../src/upgradedb010.cc',
			'../src/upgradedb010.h',
			'../src/upgradedb.cc'
		],
		'conditions': [
			['OS=="win"', {
				'libraries': [
					'Advapi32.lib',
					'User32.lib'
				],
			}]
		],
    },
    {  
		'target_name': "testnode",
		'type':'executable',	
		 'includes':[
			'../../proj/configd.gypi',
			'../../deps/osconfig.gypi',
		],
		'dependencies': [
			'../../deps/gtest.gyp:gtest',
			'../../deps/protobuf3.gyp:protobuf_full_do_not_use',
			'../../deps/json2pb.gyp:json2pb',
			'../../deps/jansson.gyp:jansson',		
			'../../deps/snappy.gyp:snappy',	
			'../../deps/encryptstring.gyp:encryptstring',	
		],
		'include_dirs': [
			'<(jansson_config_h_dir)',
			'../../deps/mysql-connector-c-6.1.5-src/include',
			'../../src/mylib',
			'../../deps/boost_1_57_0',
			'../../deps/protobuf3/src',
			'../../deps/json2pb-master',
			'../../deps/soci-3.2.2/core',
			'../../deps/soci-3.2.2/backends/mysql',
			'../../deps/gtest/include',
			'../../deps/snappy',
			'../../deps/encryptstring',
			],
		'sources': [ 
			'../../src/mylib/cfg.h',
			'../../src/mylib/glog.h',
			'../../src/mylib/util/utiltime.h',
			'../../src/mylib/util/utillanguage.h',
			'../../src/mylib/util/codec.h',
			'../../src/dbsession/dbsession3i.h',
			'../../src/dbsession/dbsession3i.cc',
			'../../src/dbsession/dbsessionadmin.h',
			'../../src/dbsession/dbsessionadmin.cc',
			'../../src/dbsession/dbsessioncomm.h',
			'../../src/dbsession/dbsessioncomm.cc',

			'../../src/mylib/cfg.cc',
			'../../src/mylib/glog.cc',
			'../../src/mylib/util/utiltime.cc',
			'../../src/mylib/util/utillanguage.cc',
			'../../src/mylib/util/codec.cc',
			'../../src/mylib/encrypt/aes.h',
			'../../src/mylib/encrypt/aes-core.h',
			'../../src/mylib/encrypt/md5.h',	
			'../../src/mylib/encrypt/aes.c',
			'../../src/mylib/encrypt/aes-core.c',
			'../../src/mylib/encrypt/md5.cpp',
			
			'../src/Test_NodeWrap_Admin.cc',
			'../src/Test_NodeWrap_3I.cc',
			'../src/Test_NodeWrap_Comm.cc',
			'../src/Test_Encrypt.cc',
			'../src/Test_Snappy.cc',
			
			'../src/main.cc',
			'../../src/mylib/db/mysqldbutil.h',
			'../../src/mylib/db/mysqldbutil.cc',

			'../../src/gcomm/admin.pb.h',
			'../../src/gcomm/admin.pb.cc',
			'../../src/gcomm/comm.pb.h',
			'../../src/gcomm/comm.pb.cc',
			'../../src/gcomm/thirdinterface.pb.h',
			'../../src/gcomm/thirdinterface.pb.cc',
			'../../src/gcomm/session.pb.h',
			'../../src/gcomm/session.pb.cc',			


			'../../src/DataWrap.h',
			'../../src/DataWrap.cc',
			'../../src/DataWrapComm.h',
			'../../src/DataWrapComm.cc',
			'../../src/DataWrapAdmin.h',
			'../../src/DataWrapAdmin.cc',
			'../../src/DataWrap3I.h',
			'../../src/DataWrap3I.cc',
			'../../src/EncryptWrap.cc',
			'../../src/EncryptWrap.h',
			
		],
		'conditions': [
			['OS=="win"', {
				'libraries': [
					'Advapi32.lib',
					'User32.lib'
				],
			}]
		],
    },
    {  
		'target_name': "teststring",
		'type':'executable',	
		 'includes':[
			'../../proj/configd.gypi',
		],
		'dependencies': [
			'../../deps/gtest.gyp:gtest',
			'../../deps/encryptstring.gyp:encryptstring',	
		],
		'include_dirs': [
			'../../src/mylib',
			'../../deps/boost_1_57_0',
			'../../deps/gtest/include',
			'../../deps/encryptstring'
		],
		'sources': [ 
			'../../src/mylib/util/utiltime.h',
			'../../src/mylib/util/utiltime.cc',
			'../../src/mylib/encrypt/aes.h',
			'../../src/mylib/encrypt/aes-core.h',
			'../../src/mylib/encrypt/md5.h',	
			'../../src/mylib/encrypt/aes.c',
			'../../src/mylib/encrypt/aes-core.c',
			'../../src/mylib/encrypt/md5.cpp',
						
			'../../src/EncryptWrap.cc',
			'../../src/EncryptWrap.h',
			'../src/Test_Encrypt.cc',
			
			'../src/main.cc',		
			
		],
		'conditions': [
			['OS=="win"', {
				'libraries': [
					'Advapi32.lib',
					'User32.lib'
					],
			}]
		],
		
    },
    {  
		'target_name': "testsocisample",
		'type':'executable',	
		 'includes':[
			'../../proj/configd.gypi',
		],
		'include_dirs': [
			'../../deps/boost_1_57_0',
			'../../deps/soci-3.2.2/core',
			'../../deps/soci-3.2.2/backends/mysql',
			'../../deps/mysql-connector-c-6.1.5-src/include',
		],
		'sources': [ 
			'../src/sample_soci_mysql.cc',		
		],
		'conditions': [
			['OS=="win"', {
				'libraries': [
					'Advapi32.lib',
					'User32.lib'
					],
			}]
		],		
    }]   
  
}
