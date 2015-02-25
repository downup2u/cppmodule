{

	
'conditions': [
 ['OS!="win"', {
 'variables': {
 'config_h_dir':
 './protobuf3/xcode', # crafted for gcc/linux.
 },
 }, { # else, OS=="win"
 'variables': {
 'config_h_dir':
 './protobuf3/vsprojects', # crafted for msvc.
 },
 'target_defaults': {
 'msvs_disabled_warnings': [
 4018, # signed/unsigned mismatch in comparison
 4244, # implicit conversion, possible loss of data
 4355, # 'this' used in base member initializer list
],
'defines!': [
'WIN32_LEAN_AND_MEAN', # Protobuf defines this itself.
 ],
},
 }]
 ],


'targets': [
    # The "lite" lib is about 1/7th the size of the heavy lib,
    # but it doesn't support some of the more exotic features of
    # protobufs, like reflection.  To generate C++ code that can link
    # against the lite version of the library, add the option line:
    #
    #   option optimize_for = LITE_RUNTIME;
    #
    # to your .proto file.
    {
      'target_name': 'protobuf_lite',
      'type': 'static_library',
	  'includes':[
		'./config.gypi',
	  ],
	  'include_dirs': [
		'./protobuf3/src',
		'<(config_h_dir)',
	  ],		
      'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive','-std=c++11'],
      'sources': [
			'protobuf3/src/google/protobuf/stubs/strutil.cc',
			'protobuf3/src/google/protobuf/stubs/substitute.cc',
			'protobuf3/src/google/protobuf/stubs/structurally_valid.cc',
			'protobuf3/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc',
			'protobuf3/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc',
			'protobuf3/src/google/protobuf/descriptor.cc',
			'protobuf3/src/google/protobuf/descriptor.pb.cc',
			'protobuf3/src/google/protobuf/descriptor_database.cc',
			'protobuf3/src/google/protobuf/dynamic_message.cc',
			'protobuf3/src/google/protobuf/extension_set_heavy.cc',
			'protobuf3/src/google/protobuf/generated_message_reflection.cc',
			'protobuf3/src/google/protobuf/message.cc',
			'protobuf3/src/google/protobuf/map_field.cc',
			'protobuf3/src/google/protobuf/arenastring.cc',
			'protobuf3/src/google/protobuf/reflection_ops.cc',
			'protobuf3/src/google/protobuf/service.cc',
			'protobuf3/src/google/protobuf/text_format.cc',
			'protobuf3/src/google/protobuf/unknown_field_set.cc',
			'protobuf3/src/google/protobuf/wire_format.cc',
			'protobuf3/src/google/protobuf/arena.cc',
			'protobuf3/src/google/protobuf/io/gzip_stream.cc',
			'protobuf3/src/google/protobuf/io/printer.cc',
			'protobuf3/src/google/protobuf/io/tokenizer.cc',
			'protobuf3/src/google/protobuf/io/zero_copy_stream_impl.cc',
			'protobuf3/src/google/protobuf/io/strtod.cc',
			'protobuf3/src/google/protobuf/compiler/importer.cc',
			'protobuf3/src/google/protobuf/compiler/parser.cc',
			'<(config_h_dir)/config.h',
      ],
      # This macro must be defined to suppress the use of dynamic_cast<>,
      # which requires RTTI.
      'defines': [
        'GOOGLE_PROTOBUF_NO_RTTI',
        'GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER',
      ],

      'direct_dependent_settings': {
        'include_dirs': [
          '<(config_h_dir)',
          'src',
        ],
        'defines': [
          'GOOGLE_PROTOBUF_NO_RTTI',
          'GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER',
        ],
      },
    },
    # This is the full, heavy protobuf lib that's needed for c++ .proto's
    # that don't specify the LITE_RUNTIME option.  The protocol
    # compiler itself (protoc) falls into that category.
    #
    # DO NOT LINK AGAINST THIS TARGET IN CHROME CODE  --agl
    {
      'target_name': 'protobuf_full_do_not_use',
      'type': 'static_library',
	  'includes':[
		'./config.gypi',
	  ],
	  'include_dirs': [
		'./protobuf3/src',
	  ],	
      'cflags_cc': ['-fexceptions','-frtti','-pthread','-fpermissive','-std=c++11'],
      'sources': [
			'protobuf3/src/google/protobuf/stubs/common.cc',
			'protobuf3/src/google/protobuf/stubs/once.cc',
			'protobuf3/src/google/protobuf/stubs/stringprintf.cc',
			'protobuf3/src/google/protobuf/extension_set.cc',
			'protobuf3/src/google/protobuf/generated_message_util.cc',
			'protobuf3/src/google/protobuf/message_lite.cc',
			'protobuf3/src/google/protobuf/repeated_field.cc',
			'protobuf3/src/google/protobuf/wire_format_lite.cc',
			'protobuf3/src/google/protobuf/io/coded_stream.cc',
			'protobuf3/src/google/protobuf/io/coded_stream_inl.h',
			'protobuf3/src/google/protobuf/io/zero_copy_stream.cc',
			'protobuf3/src/google/protobuf/io/zero_copy_stream_impl_lite.cc',
			
			'protobuf3/src/google/protobuf/stubs/strutil.cc',
			'protobuf3/src/google/protobuf/stubs/substitute.cc',
			'protobuf3/src/google/protobuf/stubs/structurally_valid.cc',
			'protobuf3/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc',
			'protobuf3/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc',
			'protobuf3/src/google/protobuf/descriptor.cc',
			'protobuf3/src/google/protobuf/descriptor.pb.cc',
			'protobuf3/src/google/protobuf/descriptor_database.cc',
			'protobuf3/src/google/protobuf/dynamic_message.cc',
			'protobuf3/src/google/protobuf/extension_set_heavy.cc',
			'protobuf3/src/google/protobuf/generated_message_reflection.cc',
			'protobuf3/src/google/protobuf/message.cc',
			'protobuf3/src/google/protobuf/map_field.cc',
			'protobuf3/src/google/protobuf/arenastring.cc',
			'protobuf3/src/google/protobuf/reflection_ops.cc',
			'protobuf3/src/google/protobuf/service.cc',
			'protobuf3/src/google/protobuf/text_format.cc',
			'protobuf3/src/google/protobuf/unknown_field_set.cc',
			'protobuf3/src/google/protobuf/wire_format.cc',
			'protobuf3/src/google/protobuf/arena.cc',
			'protobuf3/src/google/protobuf/io/gzip_stream.cc',
			'protobuf3/src/google/protobuf/io/printer.cc',
			'protobuf3/src/google/protobuf/io/tokenizer.cc',
			'protobuf3/src/google/protobuf/io/zero_copy_stream_impl.cc',
			'protobuf3/src/google/protobuf/io/strtod.cc',
			'protobuf3/src/google/protobuf/compiler/importer.cc',
			'protobuf3/src/google/protobuf/compiler/parser.cc',
			'<(config_h_dir)/config.h',
		],
      'dependencies': [
        'protobuf_lite',
      ],
      'export_dependent_settings': [
        'protobuf_lite',
      ],
    }
	]
}
