{
  'variables': {
    'jasson_home': 'jansson-2.6',
    'jasson_src_home': '<(jasson_home)/src',
  },

  'targets': [
    {	
	  'target_name': 'jansson',
      'type': 'static_library',
	  'defines': [
        'HAVE_STDINT_H',
      ],
	  'includes':[
		'osconfig.gypi',
	  ],
      'include_dirs': [
        '<(jasson_src_home)',
        '<(jansson_config_h_dir)',
      ],
      'all_dependent_settings': {
        'include_dirs': [
          '<(jasson_src_home)',
        ],
      },
      'sources': [
        '<(jasson_src_home)/hashtable.h',
        '<(jasson_src_home)/jansson.h',
        '<(jansson_config_h_dir)/jansson_config.h',
        '<(jasson_src_home)/jansson_private.h',
        '<(jasson_src_home)/strbuffer.h',
        '<(jasson_src_home)/utf.h',       
        '<(jasson_src_home)/dump.c',
        '<(jasson_src_home)/error.c',
        '<(jasson_src_home)/hashtable.c',
        '<(jasson_src_home)/load.c',
        '<(jasson_src_home)/memory.c',
        '<(jasson_src_home)/pack_unpack.c',
        '<(jasson_src_home)/strbuffer.c',
        '<(jasson_src_home)/strconv.c',
        '<(jasson_src_home)/utf.c',
        '<(jasson_src_home)/value.c',       
      ],
    },
  ]
}
