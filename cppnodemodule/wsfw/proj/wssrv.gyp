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
		'target_name': "wssrv",
		'type':'executable',
		'defines': [
			'LCBUV_EMBEDDED_SOURCE=1',
		 ],	
		 'includes':[
			'../../deps/osconfig.gypi',
			'../../proj/configd.gypi',
		],
		'dependencies': [
			'../../deps/protobuf3.gyp:protobuf_full_do_not_use',
			'../../deps/json2pb.gyp:json2pb',
			'../../deps/jansson.gyp:jansson',		
			'../../deps/glog.gyp:glog',
			'../../deps/uv.gyp:libuv',
		],
		'include_dirs': [
			'../../deps/boost_1_57_0',
			'../../deps/websocketpp-master',
			'../../deps/protobuf3/src',
			'<(jansson_config_h_dir)',
			'../../deps/json2pb-master',
			'../../deps/glog/src',
			'../../deps/libuv/include',
			'../../deps/encryptstring',
			'../../deps/include',
			'../src/proto',
			'../src/proto/custommsg',
			'../src/util',
			'../src/wssrv/couchbase',
		],
		'sources': [ 
			'../src/wssrv/wssrv.cc',
			'../src/wssrv/wssrv.h',
			'../src/wssrv/wssrvmain.cc',
			'../src/wssrv/wssrvmain.h',
			'../src/wssrv/sessconn.h',
			'../src/wssrv/sessconn.cc',
			'../src/wssrv/sessdbdispatcher.h',
			'../src/wssrv/sessdbdispatcher.cc',
			'../src/wssrv/dispatcher.hpp',
			'../src/wssrv/main.cc',
			'../src/wssrv/couchbase/couchbasesrv.cc',
			'../src/wssrv/couchbase/couchbasesrv.h',
			'../src/wssrv/couchbase/couchbasesrv.pb.cc',
			'../src/wssrv/couchbase/couchbasesrv.pb.h',
			'../src/wssrv/couchbase/couchbasewrap.cc',
			'../src/wssrv/couchbase/couchbasewrap.h',
			'../src/wssrv/couchbase/libuvplugin/libuv_compat.h',
			'../src/wssrv/couchbase/libuvplugin/libuv_io_opts.h',
			'../src/wssrv/couchbase/libuvplugin/plugin-internal.h',
			'../src/wssrv/couchbase/libuvplugin/plugin-libuv.c',
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
					'User32.lib',
					'../../libs/windebug/couchbaseS_d.lib',
				],
			}]
		],
    }],
}