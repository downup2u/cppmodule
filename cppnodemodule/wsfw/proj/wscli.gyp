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
		'target_name': "wscli",
		'type':'executable',	
		 'includes':[
			'../../deps/osconfig.gypi',
			'../../proj/configd.gypi',
		],
		'dependencies': [
			'../../deps/protobuf3.gyp:protobuf_full_do_not_use',
			'../../deps/json2pb.gyp:json2pb',
			'../../deps/jansson.gyp:jansson',		
			'../../deps/glog.gyp:glog',
		],
		'include_dirs': [
			'../../deps/boost_1_57_0',
			'../../deps/websocketpp-master',
			'../../deps/protobuf3/src',
			'../../deps/json2pb-master',
			'../../deps/glog/src',
			'../src/proto',
			'../src/proto/custommsg',
			'../src/util',
		],
		'sources': [ 
			'../src/wscli/wscli.cc',
			'../src/wscli/wscli.h',
			'../src/wscli/wsclimain.cc',
			'../src/wscli/wsclimain.h',
			'../src/wscli/main.cc',
			'../src/util/codec.cc',
			'../src/util/codec.h',
			'../src/util/codecinmsg.cc',
			'../src/util/codecinmsg.h',
			'../src/util/glog.cc',
			'../src/util/glog.h',
			'../src/util/nodeexception.h',
			'../src/proto/packmsg.pb.cc',
			'../src/proto/packmsg.pb.h',
			'../src/proto/custommsg/xkcrm.pb.cc',
			'../src/proto/custommsg/xkcrm.pb.h',
			'../src/proto/custommsg/nosqldb.pb.cc',
			'../src/proto/custommsg/nosqldb.pb.h',
		],
		'conditions': [
			['OS=="win"', {
				'libraries': [
					'Advapi32.lib',
					'User32.lib'
				],
			}]
		],
    }],
}	