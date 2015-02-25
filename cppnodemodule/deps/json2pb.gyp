{
  'variables': {
    'json2pb_home': 'json2pb-master',
    'json2pb_src_home': '<(json2pb_home)',
    'googleprotobuf_home':'protobuf3/src',
	'jasson_home': 'jansson-2.6',
  },
  'targets': [
    {
      'target_name': 'json2pb',
      'type': 'static_library',
	   'includes':[
		'osconfig.gypi',
	  ],
      'include_dirs': [
		'<(jansson_config_h_dir)',
        '<(json2pb_src_home)',
		'<(googleprotobuf_home)',
      ],
      'dependencies': [
        './jansson.gyp:jansson',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          'jansson-2.6/src',
        ],
      },
      'sources': [
        '<(json2pb_src_home)/bin2ascii.h',
        '<(json2pb_src_home)/json2pb.cc',
        '<(json2pb_src_home)/json2pb.h',        
      ],
    },
  ]
}
